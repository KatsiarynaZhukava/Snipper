#pragma once

#include "framework.h"
#include "Snipper.h"
#include "RegisterWindows.h"
#include "InitWindows.h"
#include "windowsx.h""
#include "commdlg.h"
#include "GlobalValues.h"


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
	ARBITRARY_AREA 
};



// My global variables
HBITMAP hBitmap;
HWND hScreenWnd;
RECT rect;
OPENFILENAME saveFileDialog;
int startX = 0, startY = 0, endX = 0, endY = 0;
enum screenMode screenMode;


// My defined functions
void initializeSaveFileDialog(HWND hWnd, OPENFILENAME &saveFileDialog);
HBITMAP TakeScreenShot(HWND hWnd, RECT rect);
HBITMAP CaptureScreen(HWND hWnd, int x, int y, int width, int height);
void DrawScreen(HWND hWnd, HBITMAP hBitmap);
void SaveFile(HWND hWnd, HBITMAP hBitmap, OPENFILENAME saveFileDialog);
void GetBitmapFrame();




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

	int currentPos, wheelDelta, step = 0;
	static int timeout;

    switch (message)
    {
	case WM_CREATE:
		initializeSaveFileDialog(hWnd, saveFileDialog);
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
				ShowWindow(hMainWnd, SW_HIDE);
				ShowWindow(hBackWnd, SW_SHOW);

				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);                         
				hBitmap = TakeScreenShot(hMainWnd, rect);

				//ShowWindow(hBackWnd, SW_HIDE);
				//ShowWindow(hMainWnd, SW_SHOW);
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
			GetWindowRect(hScreenWnd, &rect);
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
	HBITMAP hBitmap = CaptureScreen(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	GetWindowRect(hWnd, &rect);
	if (hBitmap != NULL)
	{
		ShowWindow(hMainWnd, SW_SHOW);
		MoveWindow(hWnd, rect.left, rect.top, DEFAULT_WINDOW_EXTENDED_WIDTH, DEFAULT_WINDOW_EXTENDED_HEIGHT, true);
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
	HWND hDesktopWindow;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen, bm;

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
	//SetLayeredWindowAttributes(hWnd, NULL, 255, LWA_ALPHA);	

cleanUp:
	DeleteObject(hdcMemDC);
	ReleaseDC(hWnd, hdcScreen);

	return hbmScreen;
}

void DrawScreen(HWND hWnd, HBITMAP hBitmap)
{
	HDC hdc, hdcMem;
	BITMAP bm;

	hdc = GetDC(hWnd);

	hdcMem = CreateCompatibleDC(hdc);
	GetObject(hBitmap, sizeof(bm), &bm);
	SelectObject(hdcMem, hBitmap);

	BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	ReleaseDC(hWnd, hdc);

	DeleteDC(hdcMem);
	//DeleteObject(hBitmap);
}




















/////////////////////////////////////////////////////////////


void initializeSaveFileDialog(HWND hWnd, OPENFILENAME &saveFileDialog)
{
	char szSaveFileName[MAX_PATH];

	ZeroMemory(&saveFileDialog, sizeof(saveFileDialog));
	saveFileDialog.lStructSize = sizeof(saveFileDialog);
	saveFileDialog.hwndOwner = hWnd;
	saveFileDialog.lpstrFilter = (LPCWSTR)L"Jpg-file (*.JPG)\0*.jpg\0Gif-file (*.GIF)\0*.gif\0Png-file (*.PNG)\0*.png\0";
	saveFileDialog.lpstrFile = (LPWSTR)szSaveFileName;
	saveFileDialog.nMaxFile = MAX_PATH;
	//saveFileDialog.lpstrFileTitle = (LPWSTR)L"Screen";
	saveFileDialog.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	//saveFileDialog.lpstrDefExt = "Dtxt";
}

void SaveFile(HWND hWnd, HBITMAP hBitmap, OPENFILENAME saveFileDialog)
{
	if (GetSaveFileName(&saveFileDialog)) {

	}
	else
	{
		MessageBox(hWnd, L"Saving failed", L"Failed", MB_OK);
	}
}


/*private void btnSave_Click(object sender, EventArgs e)
{
    saveDialog.FileName = txtModelName.Text;
    saveDialog.DefaultExt = "jpg";
    saveDialog.Filter = "JPG images (*.jpg)|*.jpg";    

    if (saveDialog.ShowDialog() == DialogResult.OK)
    {
        Bitmap bmp = new Bitmap(pnlDraw.Width, pnlDraw.Height);

        pnlDraw.DrawToBitmap(bmp, new Rectangle(0, 0,
            pnlDraw.Width, pnlDraw.Height));

        var fileName = saveDialog.FileName;
        if(!System.IO.Path.HasExtension(fileName) || System.IO.Path.GetExtension(fileName) != "jpg")
            fileName = fileName + ".jpg";

        bmp.Save(fileName, System.Drawing.Imaging.ImageFormat.Jpeg);
    }
}*/