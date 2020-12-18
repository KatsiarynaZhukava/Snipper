#pragma once

#include "framework.h"
#include "Snipper.h"
#include "RegisterWindows.h"
#include "InitWindows.h"
#include "windowsx.h"
#include "commdlg.h"
#include "GlobalValues.h"
#include "dwmapi.h"
#include "shlwapi.h"
#include <string>
#include <atlstr.h>
#include <atlimage.h>


HINSTANCE   hInstance;                 

HWND			hMainWnd;						
HWND			hBackWnd;						

WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];   

LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    BackWndProc(HWND, UINT, WPARAM, LPARAM);

// My types

enum screenMode 
{ 
	ENTIRE_SCREEN, 
	WINDOW, 
	RECTANGLE, 
	ARBITRARY_AREA,
	NONE
};



// My global variables
HBITMAP hBitmap;
HWND hScreenWnd;
RECT rect;
OPENFILENAME saveFileDialog;
int startX = 0, startY = 0, endX = 0, endY = 0;
enum screenMode screenMode;
CHOOSECOLOR chooseColor;
HPEN pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));


// My defined functions
void initializeSaveFileDialog(HWND hWnd, OPENFILENAME &saveFileDialog);
void initializeChooseColor(HWND hWnd);
HBITMAP TakeScreenShot(HWND hWnd, RECT rect);
HBITMAP CaptureScreen(HWND hWnd, int x, int y, int width, int height);
void DrawScreen(HWND hWnd, HBITMAP hBitmap);
void SaveFile(HWND hWnd, HBITMAP hBitmap, OPENFILENAME saveFileDialog);
void GetBitmapFrame();
void DrawRect(HWND hWnd);
int CountResizeWidth(int left, int right);
int CountResizeHeight(HWND hWnd, int top, int bottom);
void ClearWindow(HWND hwnd);
void DrawLine(HWND hWnd, int startX, int startY, int endX, int endY);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SNIPPER, szWindowClass, MAX_LOADSTRING);

	//Register window classes
	MainClass(hInstance, MainWndProc);
	BackgroundClass(hInstance, BackWndProc);

	if (!CreateMainWindow(&hMainWnd, hInstance)) return FALSE;
	if (!CreateBackgroundWindow(&hBackWnd, hInstance, hMainWnd)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SNIPPER));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}




LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int timeout;
	static RECT currentWindowRect;
	static bool isLineDrawing;
	static POINT previousPoint;
	static HDC hdc;
	static bool lButtonPressed;
    switch (message)
    {
		case WM_CREATE:
			isLineDrawing = false;
			lButtonPressed = true;
			initializeChooseColor(hWnd);
			initializeSaveFileDialog(hWnd, saveFileDialog);
			break;
		case WM_MOVE:
			if (hBitmap)
			{
				DrawScreen(hWnd, hBitmap);
			}
			break;
		case WM_HOTKEY:
			switch (wParam)
			{
				case COPY_TO_CLIPBOARD_ID:

					break;
			}
			break;
		case WM_COMMAND:
			{
				isLineDrawing = false;
				int wmId = LOWORD(wParam);
				switch (wmId)
				{
				case IDM_ENTIRE_SCREEN:
					if (timeout)
					{
						Sleep(timeout);
						timeout = 0;
					}

					SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
					ShowWindow(hMainWnd, SW_HIDE);
					hBitmap = TakeScreenShot(hMainWnd, rect);
					break;
				case ID_SCREENAREA_WINDOW:
					if (timeout)
					{
						Sleep(timeout);
						timeout = 0;
					}
					screenMode = WINDOW;

					ShowWindow(hMainWnd, SW_HIDE);
					ShowWindow(hBackWnd, SW_SHOW);
					SetLayeredWindowAttributes(hBackWnd, RGB(0, 0, 0), BACKWND_TRANSPARENCY, LWA_ALPHA);
					break;
				case IDM_RECTANGLE:
					if (timeout)
					{
						Sleep(timeout);
						timeout = 0;
					}
					screenMode = RECTANGLE;

					ShowWindow(hMainWnd, SW_HIDE);
					ShowWindow(hBackWnd, SW_SHOW);
					SetLayeredWindowAttributes(hBackWnd, RGB(0, 0, 0), BACKWND_TRANSPARENCY, LWA_ALPHA);
					break;
				case ID_SCREENAREA_ARBITRARYAREA:
					break;
				case ID_TIMER_1SECOND:
					timeout = 1000;
					break;
				case ID_TIMER_2SECONDS:
					timeout = 2000;
					break;
				case ID_TIMER_3SECONDS:
					timeout = 3000;
					break;
				case ID_TIMER_4SECONDS:
					timeout = 4000;
					break;
				case ID_TIMER_5SECONDS:
					timeout = 5000;
					break;
				case ID_FILE_SAVEAS:
					SaveFile(hMainWnd, hBitmap, saveFileDialog);
					break;
				case ID_ACTIONS_CLEARALL:
					hBitmap = NULL;
					ClearWindow(hWnd);
					break;
				case ID_ACTIONS_COPYSCREENTOCLIPBOARD:
					break;
				case ID_PEN_COLOR:		
					isLineDrawing = true;
					lButtonPressed = false;
					/*if (ChooseColor(&chooseColor) == TRUE) {
						DeleteObject(pen);
						pen = CreatePen(PS_SOLID, 3, chooseColor.rgbResult);
					}*/
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		case WM_LBUTTONDBLCLK:
			isLineDrawing = false;
			break;
		case WM_LBUTTONDOWN:
			if (isLineDrawing)
			{
				previousPoint.x = LOWORD(lParam);
				previousPoint.y = HIWORD(lParam);
				lButtonPressed = true;
			}
			break;
		case WM_LBUTTONUP:
			if (isLineDrawing)
			{
				hdc = GetDC(hWnd);
				MoveToEx(hdc, previousPoint.x, previousPoint.y, NULL);
				LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
				ReleaseDC(hWnd, hdc);
				lButtonPressed = false;
			}
			break;
		case WM_MOUSEMOVE:
			if (isLineDrawing && lButtonPressed)
			{
				hdc = GetDC(hWnd);
				MoveToEx(hdc, previousPoint.x, previousPoint.y, NULL);
				LineTo(hdc, previousPoint.x = LOWORD(lParam),previousPoint.y = HIWORD(lParam));
				ReleaseDC(hWnd, hdc);
			}
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:

			return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK BackWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool startedDrawing;
	switch (message)
	{
		case WM_LBUTTONDOWN:
			startX = GET_X_LPARAM(lParam);
			startY = GET_Y_LPARAM(lParam);
			startedDrawing = true;
			break;
		case WM_LBUTTONUP:
			switch (screenMode)
			{
			case WINDOW:
				POINT mousePosition;
				mousePosition.x = startX;
				mousePosition.y = startY;

				ShowWindow(hBackWnd, SW_HIDE);
				hScreenWnd = WindowFromPoint(mousePosition);
				DwmGetWindowAttribute(hScreenWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));				
				hBitmap = TakeScreenShot(hMainWnd, rect);
				break;
			case RECTANGLE:
				endX = GET_X_LPARAM(lParam);
				endY = GET_Y_LPARAM(lParam);

				ShowWindow(hBackWnd, SW_HIDE);
				GetBitmapFrame();
				hBitmap = TakeScreenShot(hMainWnd, rect);
				break;
			}
			startedDrawing = false;
			break;
		case WM_MOUSEMOVE:
			if ((screenMode == RECTANGLE) && startedDrawing)
			{
				endX = GET_X_LPARAM(lParam);
				endY = GET_Y_LPARAM(lParam);
				DrawRect(hWnd);
			}
			break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



void GetBitmapFrame()
{
	if (startX <= endX)
	{
		rect.left = startX;
		rect.right = endX;
	}
	else
	{
		rect.left = endX;
		rect.right = startX;
	}

	if (startY <= endY)
	{
		rect.top = startY;
		rect.bottom = endY;
	}
	else
	{
		rect.top = endY;
		rect.bottom = startY;
	}
}


HBITMAP TakeScreenShot(HWND hWnd, RECT rect)
{
	RECT windowRect;
	Sleep(TIMEOUT_WINDOW_DISAPPEAR);
	HBITMAP hBitmap = CaptureScreen(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	GetWindowRect(hWnd, &windowRect);

	UpdateWindow(hWnd);

	if (hBitmap != NULL) 
	{
		ShowWindow(hMainWnd, SW_SHOW);
		MoveWindow(hWnd, windowRect.left, windowRect.top, CountResizeWidth(rect.left, rect.right), CountResizeHeight(hWnd, rect.top, rect.bottom), true);
		DrawScreen(hWnd, hBitmap);
	}
	else
	{
		MessageBox(hWnd, L"No screen has been captured", L"Failed", MB_OK);
	}
	return hBitmap;
}

HBITMAP CaptureScreen(HWND hWnd, int startX, int startY, int width, int height)
{
	HDC hdcScreen, hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	hdcScreen = GetDC(NULL);
	hdcMemDC = CreateCompatibleDC(hdcScreen);

	if (!hdcMemDC)
	{
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		goto cleanUp;
	}

	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);

	if (!hbmScreen)
	{
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		goto cleanUp;
	}

	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, startX, startY, SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		goto cleanUp;
	}

	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

cleanUp:
	DeleteObject(hdcMemDC);
	ReleaseDC(hWnd, hdcScreen);

	return hbmScreen;
}

void DrawScreen(HWND hWnd, HBITMAP hBitmap)
{
	HDC hdcMain, hdcMemory, hdcWithBitmap;
	HBITMAP hbmMem;
	BITMAP bm;
	RECT windowRect;

	hdcMain = GetDC(hWnd);

	GetClientRect(hWnd, &windowRect);

	hdcWithBitmap = CreateCompatibleDC(hdcMain);
	GetObject(hBitmap, sizeof(bm), &bm);
	SelectObject(hdcWithBitmap, hBitmap);                // Создали hdc с битмапом

	hdcMemory = CreateCompatibleDC(hdcMain);			// Создали дополнительный hdc для двойной буферизации
	hbmMem = CreateCompatibleBitmap(hdcMain, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
	SelectObject(hdcMemory, hbmMem);

	BitBlt(hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, hdcWithBitmap, 0, 0, SRCCOPY);			// Сначала переводим в память
	BitBlt(hdcMain, SCREENSHOT_DISPLAY_BORDER, SCREENSHOT_DISPLAY_BORDER, bm.bmWidth, bm.bmHeight, hdcMemory, 0, 0, SRCCOPY); // Оттуда на экран

	ReleaseDC(hWnd, hdcMain);

	DeleteDC(hdcMemory);
	DeleteDC(hdcWithBitmap);
	DeleteObject(hbmMem);
	//DeleteObject(hBitmap);
}


int CountResizeWidth(int left, int right)
{
	if ((right - left + 2 * SCREENSHOT_DISPLAY_BORDER) < MAX_SCREEN_WIDTH)
	{
		return right - left + 2 * SCREENSHOT_DISPLAY_BORDER;
	}
	return MAX_SCREEN_WIDTH;
}

int CountResizeHeight(HWND hWnd, int top, int bottom)
{
	int taskBarHeight;
	RECT windowRect;

	GetWindowRect(hWnd, &windowRect);
	taskBarHeight = windowRect.bottom - windowRect.top;
	GetClientRect(hWnd, &windowRect);
	taskBarHeight -= windowRect.bottom - windowRect.top;

	if ((bottom - top + 2 * SCREENSHOT_DISPLAY_BORDER) < MAX_SCREEN_HEIGHT)
	{
		return bottom - top + 2 * SCREENSHOT_DISPLAY_BORDER + taskBarHeight;
	}
	return MAX_SCREEN_HEIGHT;
}


void ClearWindow(HWND hWnd)
{
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	MoveWindow(hWnd, windowRect.left, windowRect.top, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, true);
}


void DrawLine(HWND hWnd, int startX, int startY, int endX, int endY)
{
	HDC hdcMain, hdcWithBitmap;
	BITMAP bm;

	hdcMain = GetDC(hWnd);

	hdcWithBitmap = CreateCompatibleDC(hdcMain);
	GetObject(hBitmap, sizeof(bm), &bm);
	SelectObject(hdcWithBitmap, hBitmap);                // Создали hdc с битмапом

	MoveToEx(hdcWithBitmap, startX, startY, NULL);
	LineTo(hdcWithBitmap, endX, endX);

	BitBlt(hdcMain, 0, 0, bm.bmWidth, bm.bmHeight, hdcWithBitmap, 0, 0, SRCCOPY);			// Сначала переводим в память

	/*hdcMemory = CreateCompatibleDC(hdcMain);			// Создали дополнительный hdc для двойной буферизации
	hbmMem = CreateCompatibleBitmap(hdcMain, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
	SelectObject(hdcMemory, hbmMem);

	BitBlt(hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, hdcWithBitmap, 0, 0, SRCCOPY);			// Сначала переводим в память
	BitBlt(hdcMain, SCREENSHOT_DISPLAY_BORDER, SCREENSHOT_DISPLAY_BORDER, bm.bmWidth, bm.bmHeight, hdcMemory, 0, 0, SRCCOPY); // Оттуда на экран*/

	ReleaseDC(hWnd, hdcMain);

	/*DeleteDC(hdcMemory);
	DeleteDC(hdcWithBitmap);
	DeleteObject(hbmMem);*/

	ReleaseDC(hWnd, hdcMain);
}


////////////////////////////////////////////// BackGroundWindow effects ////////////////////////////

void DrawRect(HWND hWnd)
{
	HDC hdc;
	HBITMAP hbmMem, hbmOld;
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);

	hdc = GetDC(hWnd);

	InvalidateRect(hWnd, &windowRect, TRUE);
	UpdateWindow(hWnd);

	Rectangle(hdc, startX, startY, endX, endY);
	ReleaseDC(hWnd, hdc);
}




/////////////////////////////////////////////////////////////

void initializeChooseColor(HWND hWnd)
{
	COLORREF    custColors[16];
	ZeroMemory(&chooseColor, sizeof(CHOOSECOLOR));

	chooseColor.lStructSize = sizeof(CHOOSECOLOR);

	chooseColor.hwndOwner = hWnd;
	chooseColor.rgbResult = RGB(255, 0, 0);
	chooseColor.lpCustColors = NULL;
	chooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;
	chooseColor.lpCustColors = (LPDWORD)custColors;
}

///////////////////////////////////////////////////////////// Save file ///////////////////////////////////////////////////////////


void initializeSaveFileDialog(HWND hWnd, OPENFILENAME &saveFileDialog)
{
	char szSaveFileName[MAX_PATH];

	ZeroMemory(&saveFileDialog, sizeof(saveFileDialog));
	saveFileDialog.lStructSize = sizeof(saveFileDialog);
	saveFileDialog.hwndOwner = hWnd;
	saveFileDialog.lpstrFilter = (LPCWSTR)L"Файл JPG (*.JPG)\0*.jpg\0Файл GIF (*.GIF)\0*.gif\0Файл PNG (*.PNG)\0*.png\0Файл BMP (*.BMP)\0*.bmp\0";
	saveFileDialog.lpstrFile = (LPWSTR)szSaveFileName;
	saveFileDialog.nMaxFile = MAX_PATH;
	saveFileDialog.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	saveFileDialog.lpstrDefExt = (LPCWSTR)L"jpg";
}

void SaveFile(HWND hWnd, HBITMAP hBitmap, OPENFILENAME saveFileDialog)
{
	CImage image;

	if (GetSaveFileName(&saveFileDialog)) {

		LPCWSTR extension = PathFindExtensionW(saveFileDialog.lpstrFile);
		PathStripPath(saveFileDialog.lpstrFile);
		const wchar_t* str = (const wchar_t*)saveFileDialog.lpstrFile;
		if (lstrcmpW(extension, (LPCWSTR)L".jpg") == 0)
		{
			image.Attach(hBitmap);
			image.Save((LPCTSTR)L"someImg.jpg", Gdiplus::ImageFormatJPEG);
			image.Detach();
		}
		else if (lstrcmpW(extension, (LPCWSTR)L".gif") == 0)
		{
			image.Attach(hBitmap);
			image.Save(saveFileDialog.lpstrFile, Gdiplus::ImageFormatGIF);
			image.Detach();
		}
		else if (lstrcmpW(extension, (LPCWSTR)L".png") == 0)
		{
			image.Attach(hBitmap);
			image.Save((LPCTSTR)str, Gdiplus::ImageFormatPNG);
			image.Detach();
		}
		else if (lstrcmpW(extension, (LPCWSTR)L".bmp") == 0)
		{
			image.Attach(hBitmap);
			image.Save((LPCTSTR)str, Gdiplus::ImageFormatBMP);
			image.Detach();
		}
	}
	DeleteObject(image);
}