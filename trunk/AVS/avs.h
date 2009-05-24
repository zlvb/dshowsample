
#pragma once
#ifndef _avs_h_
#define _avs_h_

#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")


#define WM_GRAPHNOTIFY  (WM_USER+20)

namespace AVS{

class Manager
{
public:
	Manager();
	~Manager();

	void Init(HWND hwnd)
	{
		m_Window = hwnd;
	}

	BOOL ResizeVideoWindow(long inLeft, long inTop, long inWidth, long inHeight);
	void HandleEvent(WPARAM inWParam, LPARAM inLParam);

	BOOL Play(void);        
	BOOL Stop(void);
	BOOL Pause(void);
	BOOL IsPlaying(void);  
	BOOL IsStopped(void);
	BOOL IsPaused(void);

	BOOL SetFullScreen(BOOL inEnabled);
	BOOL isFullScreen(void);

	BOOL GetCurrentPosition(double * outPosition);
	BOOL GetStopPosition(double * outPosition);
	BOOL SetCurrentPosition(double inPosition);
	BOOL SetStartStopPosition(double inStart, double inStop);
	BOOL GetDuration(double * outDuration);
	BOOL SetPlaybackRate(double inRate);

	BOOL SetAudioVolume(long inVolume);
	long GetAudioVolume(void);
	
	BOOL SetAudioBalance(long inBalance);
	long GetAudioBalance(void);

	BOOL OpenMedia(const char * inFile);
	BOOL SnapshotBitmap(const char * outFile);

    void ReOpen();
    void RePlay();

    char* GetFileName(char* outFileName);

private:

	IGraphBuilder * GetGraph(void); 
	IMediaEventEx * GetEventHandle(void);

	BOOL ConnectFilters(IPin * inOutputPin, IPin * inInputPin, const AM_MEDIA_TYPE * inMediaType = 0);
	void DisconnectFilters(IPin * inOutputPin);

	void AddToObjectTable(void) ;
	void RemoveFromObjectTable(void);
	
	BOOL QueryInterfaces(void);
	BOOL SetDisplayWindow();
	BOOL SetNotifyWindow();
	virtual BOOL Create(void);
	virtual void Close(void);
	virtual BOOL Attach(IGraphBuilder * inGraphBuilder);
    char filename[300];

private:
	IGraphBuilder	*m_Graph;  
	IMediaControl	*m_MediaControl;
	IMediaEventEx	*m_Event;
	IBasicVideo		*m_BasicVideo;
	IBasicAudio		*m_BasicAudio;
	IVideoWindow	*m_VideoWindow;
	IMediaSeeking	*m_Seeking;

	DWORD				m_ObjectTableEntry; 

	HWND				m_Window;
};

}
#endif 