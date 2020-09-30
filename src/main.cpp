// ClipboardOCR2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include <ClipBoard.h>
#include <FileFunctions.h>
#include <iostream>

#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
#ifdef _DEBUG
	//RedirectIOToConsole();
#endif
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIPBOARDOCR2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIPBOARDOCR2));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIPBOARDOCR2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIPBOARDOCR2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 400, 350, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

std::string output = "HELLO\n";

void extractTextFromImage()
{
	output = "";

	// Save a BMP image from the clipboard: 
	int r = ClipBoard::getFromClipBoard("D:\\dev\\temp\\image.bmp");
	output += "Result of saving BMP from clipboard: " + r + std::string("\n");


	// Convert the BMP image into a PNG:
	r = FileFunctions::convertBMPtoPNG("D:\\dev\\temp\\image.bmp", "D:\\dev\\temp\\image.png");
	output += "Result of converting BMP image to PNG: " + r + std::string("\n");


	// Execute tesseract on the image:
	r = FileFunctions::execTesseract("D:\\dev\\temp\\image.png", "D:\\dev\\temp\\image");
	output += "Result of converting the PNG to text: " + r + std::string("\n");


	// Read the file contents:
	std::string temp = FileFunctions::readFileContents("D:\\dev\\temp\\image.txt");
	output += "Result of converting reading the text image: " + temp + "haha\n";

	output = temp;
	// Copy the text extracted from the image to the clipboard
	ClipBoard::copyToClipBoard(temp.c_str());

	std::cout << "Done!\n\n";
}

HWND hwndNextViewer;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{



	switch (message)
	{
	case WM_CREATE:
	{
		std::cout << "Created message recieved" << std::endl;
		output += "WINDOW CREATED\n";

		hwndNextViewer = SetClipboardViewer(hWnd);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DRAWCLIPBOARD:
	{
		extractTextFromImage();
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetClientRect(hWnd, &rect);

		//DrawTextA(hdc, output.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
		DrawTextA(hdc, "Running!", -1, &rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_SINGLELINE);

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_CHANGECBCHAIN:
	{
		// If the next window is closing, repair the chain. 

		if ((HWND)wParam == hwndNextViewer)
			hwndNextViewer = (HWND)lParam;

		// Otherwise, pass the message to the next link. 

		else if (hwndNextViewer != NULL)
			SendMessage(hwndNextViewer, message, wParam, lParam);
	}
	break;
	case WM_DESTROY:
		ChangeClipboardChain(hWnd, hwndNextViewer);
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
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
