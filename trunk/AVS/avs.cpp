#pragma warning(disable: 4819)

#include "avs.h"

namespace AVS{

Manager::Manager()
:m_Graph(NULL)
,m_MediaControl(NULL)
,m_Event(NULL)
,m_BasicVideo(NULL)
,m_BasicAudio(NULL)
,m_VideoWindow(NULL)
,m_Seeking(NULL)
,m_ObjectTableEntry(0)
{

}

Manager::~Manager()
{
	Close();
}

BOOL Manager::Create(void)
{
	if (!m_Graph)
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void **)&m_Graph)))
		{
			AddToObjectTable();

			return QueryInterfaces();
		}
		m_Graph = 0;
	}
	return FALSE;
}

BOOL Manager::QueryInterfaces(void)
{
	if (m_Graph)
	{
		HRESULT hr = NOERROR;
		hr |= m_Graph->QueryInterface(IID_IMediaControl, (void **)&m_MediaControl);
		hr |= m_Graph->QueryInterface(IID_IMediaEventEx, (void **)&m_Event);
		hr |= m_Graph->QueryInterface(IID_IBasicVideo, (void **)&m_BasicVideo);
		hr |= m_Graph->QueryInterface(IID_IBasicAudio, (void **)&m_BasicAudio);
		hr |= m_Graph->QueryInterface(IID_IVideoWindow, (void **)&m_VideoWindow);
		hr |= m_Graph->QueryInterface(IID_IMediaSeeking, (void **)&m_Seeking);
		if (m_Seeking)
		{
			m_Seeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
		}
		return SUCCEEDED(hr);
	}
	return FALSE;
}

void Manager::Close(void)
{
	if (m_Seeking)
	{
		m_Seeking->Release();
		m_Seeking = NULL;
	}
	if (m_MediaControl)
	{
		m_MediaControl->Release();
		m_MediaControl = NULL;
	}
	if (m_Event)
	{
		m_Event->Release();
		m_Event = NULL;
	}
	if (m_BasicVideo)
	{
		m_BasicVideo->Release();
		m_BasicVideo = NULL;
	}
	if (m_BasicAudio)
	{
		m_BasicAudio->Release();
		m_BasicAudio = NULL;
	}
	if (m_VideoWindow)
	{
		m_VideoWindow->put_Visible(OAFALSE);
		m_VideoWindow->put_MessageDrain((OAHWND)NULL);
		m_VideoWindow->put_Owner(OAHWND(0));
		m_VideoWindow->Release();
		m_VideoWindow = NULL;
	}
	RemoveFromObjectTable();
	if (m_Graph) 
	{
		m_Graph->Release(); 
		m_Graph = NULL;
	}
}

BOOL Manager::Attach(IGraphBuilder * inGraphBuilder)
{
	Close();

	if (inGraphBuilder)
	{
		inGraphBuilder->AddRef();
		m_Graph = inGraphBuilder;

		AddToObjectTable();
		return QueryInterfaces();
	}
	return TRUE;
}

IGraphBuilder * Manager::GetGraph(void)
{
	return m_Graph;
}

IMediaEventEx * Manager::GetEventHandle(void)
{
	return m_Event;
}

BOOL Manager::ConnectFilters(IPin * inOutputPin, IPin * inInputPin, 
							  const AM_MEDIA_TYPE * inMediaType)
{
	if (m_Graph && inOutputPin && inInputPin)
	{
		HRESULT hr = m_Graph->ConnectDirect(inOutputPin, inInputPin, inMediaType);
		return SUCCEEDED(hr) ? TRUE : FALSE;
	}
	return FALSE;
}

void Manager::DisconnectFilters(IPin * inOutputPin)
{
	if (m_Graph && inOutputPin)
	{
		HRESULT hr = m_Graph->Disconnect(inOutputPin);
	}
}

BOOL Manager::SetDisplayWindow()
{	
	if (m_VideoWindow)
	{
		m_VideoWindow->put_Visible(OAFALSE);
		m_VideoWindow->put_Owner((OAHWND)m_Window);

		RECT windowRect;
		::GetClientRect(m_Window, &windowRect);
		m_VideoWindow->put_Left(0);
		m_VideoWindow->put_Top(0);
		m_VideoWindow->put_Width(windowRect.right - windowRect.left);
		m_VideoWindow->put_Height(windowRect.bottom - windowRect.top);
		m_VideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

		m_VideoWindow->put_MessageDrain((OAHWND) m_Window);
		
		if (m_Window != NULL)
		{
		
			m_VideoWindow->put_Visible(OATRUE);
		}
		else
		{
			m_VideoWindow->put_Visible(OAFALSE);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL Manager::ResizeVideoWindow(long inLeft, long inTop, long inWidth, long inHeight)
{
	if (m_VideoWindow)
	{
		long lVisible = OATRUE;
		m_VideoWindow->get_Visible(&lVisible);
		
		m_VideoWindow->put_Visible(OAFALSE);

		m_VideoWindow->put_Left(inLeft);
		m_VideoWindow->put_Top(inTop);
		m_VideoWindow->put_Width(inWidth);
		m_VideoWindow->put_Height(inHeight);
	
		
		m_VideoWindow->put_Visible(lVisible);
		return TRUE;
	}
	return FALSE;
}

BOOL Manager::SetNotifyWindow()
{
	if (m_Event)
	{
		m_Event->SetNotifyWindow((OAHWND)m_Window, WM_GRAPHNOTIFY, 0);
		return TRUE;
	}
	return FALSE;
}

void Manager::HandleEvent(WPARAM inWParam, LPARAM inLParam)
{
	if (m_Event)
	{
		LONG eventCode = 0, eventParam1 = 0, eventParam2 = 0;
		while (SUCCEEDED(m_Event->GetEvent(&eventCode, &eventParam1, &eventParam2, 0)))
		{
			m_Event->FreeEventParams(eventCode, eventParam1, eventParam2);
			switch (eventCode)
			{
			case EC_COMPLETE:
				break;

			case EC_USERABORT:
			case EC_ERRORABORT:
				break;

			default:
				break;
			}
		}
	}
}

BOOL Manager::Play(void)
{
	if (m_Graph && m_MediaControl)
	{
		if (!IsPlaying())
		{
			if (SUCCEEDED(m_MediaControl->Run()))
			{
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Manager::Stop(void)
{
	if (m_Graph && m_MediaControl)
	{
		if (!IsStopped())
		{	
			if (SUCCEEDED(m_MediaControl->Stop()))
			{
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Manager::Pause(void)
{
	if (m_Graph && m_MediaControl)
	{
		if (!IsPaused())
		{	
			if (SUCCEEDED(m_MediaControl->Pause()))
			{
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Manager::IsPlaying(void)
{
	if (m_Graph && m_MediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(m_MediaControl->GetState(10, &state)))
		{
			return state == State_Running;
		}
	}
	return FALSE;
}

BOOL Manager::IsStopped(void)
{
	if (m_Graph && m_MediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(m_MediaControl->GetState(10, &state)))
		{
			return state == State_Stopped;
		}
	}
	return FALSE;
}

BOOL Manager::IsPaused(void)
{
	if (m_Graph && m_MediaControl)
	{
		OAFilterState state = State_Stopped;
		if (SUCCEEDED(m_MediaControl->GetState(10, &state)))
		{
			return state == State_Paused;
		}
	}
	return FALSE;
}

BOOL Manager::SetFullScreen(BOOL inEnabled)
{
	if (m_VideoWindow)
	{
		HRESULT hr = m_VideoWindow->put_FullScreenMode(inEnabled ? OATRUE : OAFALSE);
		return SUCCEEDED(hr);
	}
	return FALSE;
}

BOOL Manager::isFullScreen(void)
{
	if (m_VideoWindow)
	{
		long  fullScreenMode = OAFALSE;
		m_VideoWindow->get_FullScreenMode(&fullScreenMode);
		return (fullScreenMode == OATRUE);
	}
	return FALSE;
}

BOOL Manager::GetCurrentPosition(double * outPosition)
{
	if (m_Seeking)
	{
		__int64 position = 0;
		if (SUCCEEDED(m_Seeking->GetCurrentPosition(&position)))
		{
			*outPosition = ((double)position) / 10000000.;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Manager::GetStopPosition(double * outPosition)
{
	if (m_Seeking)
	{
		__int64 position = 0;
		if (SUCCEEDED(m_Seeking->GetStopPosition(&position)))
		{
			*outPosition = ((double)position) / 10000000.;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Manager::SetCurrentPosition(double inPosition)
{
	if (m_Seeking)
	{
		__int64 one = 10000000;
		__int64 position = (__int64)(one * inPosition);
		HRESULT hr = m_Seeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
			0, AM_SEEKING_NoPositioning);
		return SUCCEEDED(hr);
	}
	return FALSE;
}

BOOL Manager::SetStartStopPosition(double inStart, double inStop)
{
	if (m_Seeking)
	{
		__int64 one = 10000000;
		__int64 startPos = (__int64)(one * inStart);
		__int64 stopPos  = (__int64)(one * inStop);
		HRESULT hr = m_Seeking->SetPositions(&startPos, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
			&stopPos, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame);
		return SUCCEEDED(hr);
	}
	return FALSE;
}

BOOL Manager::GetDuration(double * outDuration)
{
	if (m_Seeking)
	{
		__int64 length = 0;
		if (SUCCEEDED(m_Seeking->GetDuration(&length)))
		{
			*outDuration = ((double)length) / 10000000.;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Manager::SetPlaybackRate(double inRate)
{
	if (m_Seeking)
	{
		if (SUCCEEDED(m_Seeking->SetRate(inRate)))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Manager::SetAudioVolume(long inVolume)
{
	if (m_BasicAudio)
	{
		HRESULT hr = m_BasicAudio->put_Volume(inVolume);
		return SUCCEEDED(hr);
	}
	return FALSE;
}

long Manager::GetAudioVolume(void)
{
	long volume = 0;
	if (m_BasicAudio)
	{
		m_BasicAudio->get_Volume(&volume);
	}
	return volume;
}


BOOL Manager::SetAudioBalance(long inBalance)
{
	if (m_BasicAudio)
	{
		HRESULT hr = m_BasicAudio->put_Balance(inBalance);
		return SUCCEEDED(hr);
	}
	return FALSE;
}

long Manager::GetAudioBalance(void)
{
	long balance = 0;
	if (m_BasicAudio)
	{
		m_BasicAudio->get_Balance(&balance);
	}
	return balance;
}

BOOL Manager::OpenMedia(const char * inFile)
{
    strcpy(filename,inFile);
	Create ();
	if (m_Graph)
	{
		WCHAR    szFilePath[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, inFile, -1, szFilePath, MAX_PATH);
		if (SUCCEEDED(m_Graph->RenderFile(szFilePath, NULL)))
		{
			SetDisplayWindow ();
			SetNotifyWindow ();
			return TRUE;
		}
	}
	return FALSE;
}


char* Manager::GetFileName(char* outFileName)
{
    strcpy(outFileName,filename);
    return outFileName;
}

void Manager::ReOpen()
{
    Stop();
    Close();
    Create();
    OpenMedia (filename);
}

void Manager::AddToObjectTable(void)
{
	IMoniker * pMoniker = 0;
    IRunningObjectTable * objectTable = 0;
    if (SUCCEEDED(GetRunningObjectTable(0, &objectTable))) 
	{
		WCHAR wsz[256];
		wsprintfW(wsz, L"FilterGraph %08p pid %08x", (DWORD_PTR)m_Graph, GetCurrentProcessId());
		HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
		if (SUCCEEDED(hr)) 
		{
			hr = objectTable->Register(0, m_Graph, pMoniker, &m_ObjectTableEntry);
			pMoniker->Release();
		}
		objectTable->Release();
	}
}

void Manager::RemoveFromObjectTable(void)
{
	IRunningObjectTable * objectTable = 0;
    if (SUCCEEDED(GetRunningObjectTable(0, &objectTable))) 
	{
        objectTable->Revoke(m_ObjectTableEntry);
        objectTable->Release();
		m_ObjectTableEntry = 0;
    }
}

void Manager::RePlay ()
{
    ReOpen ();
    Play ();
}


}