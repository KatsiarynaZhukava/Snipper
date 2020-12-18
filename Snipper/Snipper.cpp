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
//#include <afxstr.h>
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
CHOOSECOLOR chooseColor = { 0 };


// My defined functions
void initializeSaveFileDialog(HWND hWnd, OPENFILENAME &saveFileDialog);
void initializeChooseColor(HWND hWnd);
HBITMAP TakeScreenShot(HWND hWnd, RECT rect);
HBITMAP CaptureScreen(HWND hWnd, int x, int y, int width, int height);
void DrawScreen(HWND hWnd, HBITMAP hBitmap);
void SaveFile(HWND hWnd, HBITMAP hBitmap, OPENFILENAME saveFileDialog);
void GetBitmapFrame();
BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);
void DrawRect(HWND hWnd);
int CountResizeWidth(int left, int right);
int CountResizeHeight(HWND hWnd, int top, int bottom);



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

    switch (message)
    {
		case WM_CREATE:
			initializeChooseColor(hWnd);
			initializeSaveFileDialog(hWnd, saveFileDialog);
			break;
		case WM_MOVE:
			if (hBitmap)
			{
				DrawScreen(hWnd, hBitmap);
			}
			break;
		case WM_COMMAND:
			{
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
				case ID_PEN_COLOR:				
					if (ChooseColor(&chooseColor)) {
						// Действия с cc.rgbResult
					}
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:

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
	switch (message)
	{
		case WM_LBUTTONDOWN:
			startX = GET_X_LPARAM(lParam);
			startY = GET_Y_LPARAM(lParam);
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
			break;
		case WM_MOUSEMOVE:
			/*endX = GET_X_LPARAM(lParam);
			endY = GET_Y_LPARAM(lParam);
			DrawRect(hWnd);*/
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

	if (hBitmap != NULL) 
	{
		ShowWindow(hMainWnd, SW_SHOW);
		MoveWindow(hWnd, 0, 0, CountResizeWidth(rect.left, rect.right), CountResizeHeight(hWnd, rect.top, rect.bottom), true);
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
	HDC hdc, hdcMem;
	BITMAP bm;
	RECT windowRect;

	hdc = GetDC(hWnd);
	GetClientRect(hWnd, &windowRect);

	hdcMem = CreateCompatibleDC(hdc);
	GetObject(hBitmap, sizeof(bm), &bm);
	SelectObject(hdcMem, hBitmap);

	BitBlt(hdc, SCREENSHOT_DISPLAY_BORDER, SCREENSHOT_DISPLAY_BORDER, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	ReleaseDC(hWnd, hdc);

	DeleteDC(hdcMem);
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

////////////////////////////////////////////// BackGroundWindow effects /////////////////////////////////

void DrawRect(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
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
	chooseColor.lpCustColors = custColors;
}





///////////////////////////////////////////////////////////// Save file ///////////////////////////////////////////////////////////


void initializeSaveFileDialog(HWND hWnd, OPENFILENAME &saveFileDialog)
{
	char szSaveFileName[MAX_PATH];

	ZeroMemory(&saveFileDialog, sizeof(saveFileDialog));
	saveFileDialog.lStructSize = sizeof(saveFileDialog);
	saveFileDialog.hwndOwner = hWnd;
	saveFileDialog.lpstrFilter = (LPCWSTR)L"Jpg-file (*.JPG)\0*.jpg\0Gif-file (*.GIF)\0*.gif\0Png-file (*.PNG)\0*.png\0Bmp-file (*.BMP)\0*.bmp\0";
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
		//const wchar_t* str = (wchar_t*)PathStripPath(saveFileDialog.lpstrFile);
		PathStripPath(saveFileDialog.lpstrFile);
		const wchar_t* str = (const wchar_t*)saveFileDialog.lpstrFile;
		//const wchar_t* str = L"aaaaaaaaaaaaaaa.jpg";
		if (lstrcmpW(extension, (LPCWSTR)L".jpg") == 0)
		{
			image.Attach(hBitmap);
			//image.Save(str, Gdiplus::ImageFormatJPEG);
			image.Save((LPCWSTR)L"someImage.jpg", Gdiplus::ImageFormatJPEG);
		}
		else if (lstrcmpW(extension, (LPCWSTR)L".gif") == 0)
		{
			image.Attach(hBitmap);
			//image.Save((LPCTSTR)saveFileDialog.lpstrFile, Gdiplus::ImageFormatGIF);
			image.Save((LPCTSTR)L"someImage.gif", Gdiplus::ImageFormatGIF);

		}
		else if (lstrcmpW(extension, (LPCWSTR)L".png") == 0)
		{
			image.Attach(hBitmap);
			//image.Save((LPCTSTR)saveFileDialog.lpstrFile, Gdiplus::ImageFormatPNG);
			image.Save((LPCTSTR)L"someImage.png", Gdiplus::ImageFormatPNG);
		}
		else if (lstrcmpW(extension, (LPCWSTR)L".bmp") == 0)
		{
			SaveHBITMAPToFile(hBitmap, saveFileDialog.lpstrFile);
		}
	}
	//DeleteObject(image);
}



BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP Bitmap0;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal2 = NULL;
	hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap0.bmWidth;
	bi.biHeight = -Bitmap0.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 256;
	dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
		* Bitmap0.bmHeight;
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal2)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}