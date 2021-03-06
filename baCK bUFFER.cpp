// baCK bUFFER.cpp : Defines the entry point for the application.
//



#include "stdafx.h"
#include "mmsystem.h"
#include <stdio.h>
#include <CommCtrl.h>
#include "Windowsx.h"
#include "Winuser.h"
#include "math.h"
#include "resource.h"
#include "baCK bUFFER.h"
#include "../../LIB/Graphics/Graphics.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HDC hBkDC = NULL;
HWND hMainWnd = NULL;
HWND hStatusBar = NULL;
int palletidx = 0;
unsigned int autotime, maxidle = 1000;
int currentobj = 0, gridnumber = 14, lastrad = 250;
double  noiseminx = .5, noisemaxx = .2;
bool inprogress = false, BkChanged = false;
CPoint startpoint, mouseposition, lastpoint;

enum
{
	SHAPE_SPIRAL_IDX,
	SHAPE_LINE_IDX,
	SHAPE_MAX_IDX = SHAPE_LINE_IDX
};


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void AddSpiral(int x, int y, int size, double noisemin = .5, double noisemax = .2)
{
	CSpiralInfo Si;
			
	int r, g, b;
	switch (palletidx)
	{
	case 0:
		r = GetRand(50, 100);
		g = GetRand(50, 100);
		b = GetRand(50, 100);
		break;
	case 1:
		r = GetRand(200, 255);
		g = GetRand(60, 80);
		b = GetRand(60, 80);
		break;
	case 2:
		r = GetRand(60, 80);
		g = GetRand(200, 225);
		b = GetRand(60, 80);
		break;
	case 3:
		r = GetRand(60, 80);
		g = GetRand(60, 80);
		b = GetRand(200, 255);
		break;
	default:
		r = GetRand(70, 185),
		g = GetRand(70, 185),
		b = GetRand(70, 185);
		break;
	}

	Si.color = RGB(r, g, b);
	Si.InitRad = size/3 * rand() / RAND_MAX;
	Si.StopRad = size;
	Si.NumRev = size/GetRand(18, 25);
	Si.x = x;
	Si.y = y;
	if (noisemin <= 0 && noisemax <= 0)
		Si.NoiseAmp = 0;
	else
		Si.NoiseAmp = GetRand(noisemin * 1000, noisemax * 1000) / 1000.0;
	MakeSpiral(hBkDC, Si);
	BkChanged = true;
	InvalidateRect(hMainWnd, NULL, FALSE);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BACKBUFFER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BACKBUFFER));

    MSG msg;

	autotime = timeGetTime() + maxidle;

	// Main message loop:
	for (;;)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (timeGetTime() > autotime)
			{
				RECT rectangle;
				GetClientRect(hMainWnd, &rectangle);
				AddSpiral(rectangle.right * rand() / RAND_MAX, rectangle.bottom * rand() / RAND_MAX, 250 * rand()/RAND_MAX);
				autotime = timeGetTime() + maxidle;
			}
			else
			{
				Sleep(0);
			}
		}
	}
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BACKBUFFER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BACKBUFFER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

void MakeBackground()
{

	RECT FR;
	FR.top = 0;
	FR.left = 0;
	FR.bottom = GetSystemMetrics(SM_CYSCREEN);
	FR.right = GetSystemMetrics(SM_CXSCREEN);
	Rectangle(hBkDC, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	HGDIOBJ originalbrush = SelectObject(hBkDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hBkDC, RGB(0, 250, 0));
	HBRUSH FillBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hBkDC, &FR, FillBrush);
	BkChanged = true;
	InvalidateRect(hMainWnd, NULL, FALSE);
	SelectObject(hBkDC, originalbrush);
		

}

void MakeGrid()
{
	HGDIOBJ originalpen = SelectObject(hBkDC, GetStockObject(DC_PEN));
	if (originalpen == NULL)
		return;

	SetDCPenColor(hBkDC, RGB(100, 100, 100));
	// SetDCBrushColor(hBkDC, RGB(0, 0, 0));
	//Rectangle(hBkDC, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	MakeVertLines(hBkDC, CPoint(0, 0), CPoint(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)), GetSystemMetrics(SM_CXSCREEN) / gridnumber);
	MakeHorLines(hBkDC, CPoint(0, 0), CPoint(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)), GetSystemMetrics(SM_CYSCREEN) / gridnumber);
	BkChanged = true;
	InvalidateRect(hMainWnd, NULL, FALSE);

	if (SelectObject(hBkDC, originalpen) == NULL)
		return;
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

	hMainWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


   if (!hMainWnd)
      return FALSE;

   InitCommonControls();

 //Creates a status bar, has no prectical use at the moment
   hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hMainWnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);


   // Creating back buffer//
   HDC hDC = GetDC(hMainWnd);
   if (hDC == NULL)
	   return FALSE;
   HBITMAP hDDB = CreateCompatibleBitmap(hDC, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
   if (hDDB == NULL)
   {
	   ReleaseDC(hMainWnd, hDC);
	   return FALSE;
   }
   hBkDC = CreateCompatibleDC(hDC);
   if (hBkDC == NULL)
   {
	   ReleaseDC(hMainWnd, hDC);
	   DeleteObject(hDDB);
	   return FALSE;
   }
   ReleaseDC(hMainWnd, hDC);
   hDC = NULL;
   if (SelectObject(hBkDC, hDDB) == NULL)
   {
	   DeleteObject(hDDB);
	   return FALSE;
   }

   HGDIOBJ originalpen = SelectObject(hBkDC, GetStockObject(DC_PEN));
   if (originalpen == NULL)
	   return FALSE;
   SetDCPenColor(hBkDC, RGB(100, 100, 100));

   HGDIOBJ originalbrush = SelectObject(hBkDC, GetStockObject(DC_BRUSH));
   SetDCBrushColor(hBkDC, RGB(0, 250, 0));
   RECT FR;
   FR.top = 0;
   FR.left = 0;
   FR.bottom = GetSystemMetrics(SM_CYSCREEN);
   FR.right = GetSystemMetrics(SM_CXSCREEN);
   HBRUSH FillBrush = CreateSolidBrush(RGB(0, 0, 0));
   FillRect(hBkDC, &FR, FillBrush);
   DeleteObject(FillBrush);


   if (SelectObject(hBkDC, originalbrush) == NULL)
	   return FALSE;
   if (SelectObject(hBkDC, originalpen) == NULL)
	   return FALSE;
   
   MakeGrid();

   ShowWindow(hMainWnd, nCmdShow);
   UpdateWindow(hMainWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x1, y1;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				InvalidateRect(hWnd, NULL, FALSE);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
			RECT rectangle, statbar;
			GetClientRect(hWnd, &rectangle);
			GetClientRect(hStatusBar, &statbar);
			rectangle.bottom -= statbar.bottom - statbar.top;
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Draw background
		//	if (BkChanged)
			{
				BitBlt(hdc, 0, 0, rectangle.right, rectangle.bottom, hBkDC, 0, 0, SRCCOPY);
				BkChanged = false;
			}
			
			//Draw foreground
			HGDIOBJ originalpen;
			originalpen = SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, RGB(0, 255, 0));
			MakeLine(hdc, CPoint(startpoint.x - 10, startpoint.y), CPoint(startpoint.x + 10, startpoint.y));
			MakeLine(hdc, CPoint(startpoint.x, startpoint.y - 10), CPoint(startpoint.x , startpoint.y + 10));
			//Ellipse(hdc, startpoint.x - 5, startpoint.y - 5, startpoint.x + 5, startpoint.y + 5);

			COLORREF bckcolor = RGB(0, 0, 0);
			COLORREF txtcolor = RGB(0, 255, 0);
			SetTextColor(hdc, txtcolor);
			SetBkColor(hdc, bckcolor);
			char sig[200];
			SIZE txtsize;
			sprintf_s(sig, sizeof(sig), "%i, %i", mouseposition.x, mouseposition.y);
			GetTextExtentPoint32A(hdc, sig, strlen(sig), &txtsize);
			TextOutA(hdc, 10, rectangle.bottom - txtsize.cy - 10, sig, strlen(sig));
			if (inprogress)
			{
				sprintf_s(sig, sizeof(sig), "%i, %i", startpoint.x, startpoint.y);
				GetTextExtentPoint32A(hdc, sig, strlen(sig), &txtsize);
				TextOutA(hdc, 10, rectangle.bottom - 2 * txtsize.cy - 15 , sig, strlen(sig));

				if(currentobj == SHAPE_LINE_IDX)
				{
					SetDCPenColor(hdc, RGB(75, 75, 70));
					MakeLine(hdc, lastpoint, mouseposition);
				}

			}
			SelectObject(hdc, originalpen);
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_LBUTTONDOWN:
		x1 = GET_X_LPARAM(lParam),
		y1 = GET_Y_LPARAM(lParam);
		Beep(1500, 50);
		autotime = timeGetTime() + maxidle;
		if (wParam & MK_SHIFT || !inprogress)
		{
			startpoint.Set(x1, y1);
			inprogress = true;
			if(currentobj == SHAPE_LINE_IDX)
			{

				//MoveToEx(hBkDC, x1, y1, NULL);
				Ellipse(hBkDC, x1 - 5, y1 - 5, x1 + 5, y1 + 5);
				lastpoint.Set(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				BkChanged = true;
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONUP:
	{
		CPoint temp;
		if (wParam & MK_CONTROL)
			temp = startpoint;
		else
			temp.Set(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if(currentobj == SHAPE_LINE_IDX)
		{
			HGDIOBJ originalpen;
			originalpen = SelectObject(hBkDC, GetStockObject(DC_PEN));
			if (originalpen == NULL)
				return FALSE;
			SetDCPenColor(hBkDC, RGB(100, 100, 100));
			Ellipse(hBkDC, temp.x - 5, temp.y - 5, temp.x + 5, temp.y + 5);
			MakeLine(hBkDC, lastpoint, temp);
			if (SelectObject(hBkDC, originalpen) == NULL)
				return FALSE;
			BkChanged = true;
			InvalidateRect(hWnd, NULL, FALSE);
			lastpoint = temp;
		}
		else if (currentobj == SHAPE_SPIRAL_IDX)
		{
			int
				dx = temp.x - startpoint.x,
				dy = temp.y - startpoint.y,
				d = int(sqrt(dx * dx + dy * dy));
			if (d > 30)
				lastrad = d;
			AddSpiral(startpoint.x, startpoint.y, lastrad, noiseminx, noisemaxx);
			inprogress = false;
		}
		break;
	}
	case WM_RBUTTONDOWN:
		inprogress = false;

		if (currentobj >= SHAPE_MAX_IDX)
		{
			currentobj = 0;
			SetWindowTextA(hStatusBar, "Spiral - Drag & Release to change diameter, and Click for Spiral");
		}
		else
		{
			SetWindowTextA(hStatusBar, "Line - Drag & Release for Line(SHIFT - LMB for new line)(Ctrl - LMB to connect to starting line)");
			currentobj++;
		}

		break;

	case WM_MOUSEMOVE:
		mouseposition.Set (GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		InvalidateRect(hWnd, NULL, FALSE);
		autotime = timeGetTime() + maxidle;
		break;

	case WM_SIZE:
		SendMessage(hStatusBar, WM_SIZE, NULL, NULL);
		break;

/*	case WM_KEYDOWN:

		autotime = timeGetTime() + maxidle;

		switch (wParam)
		{
		case VK_LEFT:

			// Process the LEFT ARROW key. 
			r = GetRand(50, 100);
			g = GetRand(50, 100);
			b = GetRand(50, 100);
			break;

		case VK_RIGHT:

			// Process the RIGHT ARROW key. 
			r = GetRand(200, 255);
			g = GetRand(60, 80);
			b = GetRand(60, 80);
			break;

		case VK_UP:

			// Process the UP ARROW key. 
			r = GetRand(60, 80);
			g = GetRand(200, 225);
			b = GetRand(60, 80);
			break;

		case VK_DOWN:

			// Process the DOWN ARROW key. 
			r = GetRand(60, 80);
			g = GetRand(60, 80);
			b = GetRand(200, 255);
			break;
		}*/
		break;
	case WM_KEYDOWN:
		autotime = timeGetTime() + maxidle;
		break;

	case WM_CHAR:
		autotime = timeGetTime() + maxidle;
		switch (wParam)
		{
		case 'q':
			palletidx = 0;
			break;
		case 'w':
			palletidx = 1;
			break;
		case 'e':
			palletidx = 2;
			break;
		case 'r':
			palletidx = 3;
			break;
		case 't':
			palletidx = 4;
			break;
		case 'g':
			inprogress = false;
			MakeGrid();
			break;
		case 'b':
			inprogress = false;
			MakeBackground();
			break;
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
	{
		char buff[10];
		HWND Ctrl;
		Ctrl = GetDlgItem(hDlg, IDC_IDLETIME);
		if (Ctrl == NULL)
		{
			MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			return FALSE;
		}
		else
		{
			_itoa_s(maxidle, buff, 10, 10);
			SetWindowTextA(Ctrl, buff);
		}

		Ctrl = GetDlgItem(hDlg, IDC_GRIDLINES);
		if (Ctrl == NULL)
		{
			MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			return FALSE;
		}
		else
		{
			_itoa_s(gridnumber, buff, 10, 10);
			SetWindowTextA(Ctrl, buff);
		}

		Ctrl = GetDlgItem(hDlg, IDC_NOISEMIN);
		if (Ctrl == NULL)
		{
			MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			return FALSE;
		}
		else
		{
			sprintf_s(buff, 10, "%g", noiseminx);
			SetWindowTextA(Ctrl, buff);
		}

		Ctrl = GetDlgItem(hDlg, IDC_NOISEMAX);
		if (Ctrl == NULL)
		{
			MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			return FALSE;
		}
		else
		{
			sprintf_s(buff, 10, "%g", noisemaxx);
			SetWindowTextA(Ctrl, buff);
		}
		return (INT_PTR)TRUE;
	}

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
			char buff[10];
			HWND Ctrl = GetDlgItem(hDlg, IDC_IDLETIME);
			if (Ctrl == NULL)
				MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			else
			{
				GetWindowTextA(Ctrl, buff, sizeof(buff));
				maxidle = atoi(buff);
			}

			Ctrl = GetDlgItem(hDlg, IDC_GRIDLINES);
			if (Ctrl == NULL)
				MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			else
			{
				GetWindowTextA(Ctrl, buff, sizeof(buff));
				int n = atoi(buff);
				if (n <= 0)
				{
					MessageBoxA(hDlg, "INPUT A NUMBER GREATHER THAN ZERO", NULL, MB_OK);
					return FALSE;
				}
				gridnumber = atoi(buff);
			}

			Ctrl = GetDlgItem(hDlg, IDC_NOISEMIN);
			if (Ctrl == NULL)
				MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			else
			{
				GetWindowTextA(Ctrl, buff, sizeof(buff));
				double n = atof(buff);
				if (n < 0)
				{
					MessageBoxA(hDlg, "INPUT A NUMBER GREATHER THAN ZERO", NULL, MB_OK);
					return FALSE;
				}
				noiseminx = atof(buff);
			}

			Ctrl = GetDlgItem(hDlg, IDC_NOISEMAX);
			if (Ctrl == NULL)
				MessageBoxA(hDlg, "CANT", NULL, MB_OK);
			else
			{
				GetWindowTextA(Ctrl, buff, sizeof(buff));
				double n = atof(buff);
				if (n < 0)
				{
					MessageBoxA(hDlg, "INPUT A NUMBER GREATHER THAN ZERO", NULL, MB_OK);
					return FALSE;
				}
				noisemaxx = atof(buff);
			}
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
