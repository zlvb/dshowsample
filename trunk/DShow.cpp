
#include "avs/avs.h"
#include <stdio.h>
#include <windows.h>

#define FILENAME "jianti.wmv"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND MakeWindow(char *title);

int main()
{
	printf("Video System\nThe high quality video playback engine.");

	HWND	hWnd = MakeWindow(FILENAME);	// �������Ŵ���
	::CoInitialize (NULL);					// ��ʼ��COM
	//////////////////////////////////////////////////////////////////////////
	AVS::Manager mgr;						// ������Ƶ������
	mgr.Init(hWnd);							// ��ʼ����Ƶ������
	mgr.OpenMedia(FILENAME);				// ���ļ�
	mgr.Play();								// ����
	//////////////////////////////////////////////////////////////////////////
	// ��Ϣѭ��
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, (HWND) NULL, 0, 0)) != 0 && bRet != -1) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	} 
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND MakeWindow(char *title)
{
	HINSTANCE hInstance = GetModuleHandle(0);
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= "Video System Test";
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);
	HWND	hWnd = 0;
	hWnd = CreateWindow( wcex.lpszClassName, title, WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
		400, 300,	NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return hWnd;
}