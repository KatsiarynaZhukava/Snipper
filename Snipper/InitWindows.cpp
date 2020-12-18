#pragma once
#include "InitWindows.h"
#include "GlobalValues.h"

BOOL CreateBackgroundWindow(HWND* hBackWnd, HINSTANCE hInstance, HWND hParent)
{
	int initX = 0;
	int initY = 0;
	int initWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int initHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	*hBackWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST,
		TEXT("BackgroundWnd"),
		TEXT("ScreenShot"),
		WS_POPUP,
		initX, 
		initY, 
		initWidth, 
		initHeight,
		hParent,
		NULL,
		hInstance,
		NULL
	);

	if (!hBackWnd)
	{
		return FALSE;
	}

	ShowWindow(*hBackWnd, SW_HIDE);
	UpdateWindow(*hBackWnd);

	return TRUE;
}

BOOL CreateMainWindow(HWND* hMainWnd, HINSTANCE hInstance)
{
	int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);;


	*hMainWnd = CreateWindowEx(
		WS_EX_TOPMOST ,
		TEXT("MainWnd"),
		TEXT("SNIPPER"),
		WS_POPUPWINDOW | WS_CAPTION,
		(screenWidth - DEFAULT_WINDOW_WIDTH) / 2, 
		DEFAULT_Y, 
		DEFAULT_WINDOW_WIDTH, 
		DEFAULT_WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hMainWnd)
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	ShowWindow(*hMainWnd, SW_SHOW);
	UpdateWindow(*hMainWnd);

	return TRUE;
}