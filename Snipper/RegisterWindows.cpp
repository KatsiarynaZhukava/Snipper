#pragma once
#include "RegisterWindows.h"

ATOM MainClass(HINSTANCE hInstance, WNDPROC MainWndProc)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCWSTR)132);	//MAKEINTRESOURCE(IDI_SNIPPER)
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCWSTR)109;				//MAKEINTRESOURCE(IDC_SNIPPER)
	wcex.lpszClassName = TEXT("MainWnd");
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

ATOM BackgroundClass(HINSTANCE hInstance, WNDPROC BackWndProc)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hInstance = hInstance;
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpszClassName = TEXT("BackgroundWnd");
	wcex.lpszMenuName = NULL;
	wcex.lpfnWndProc = (WNDPROC)BackWndProc;

	return RegisterClassEx(&wcex);
}