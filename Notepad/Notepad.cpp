// Notepad.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Notepad.h"
#include <windowsX.h>
#include <Commdlg.h>
#include <winuser.h>
#include <commctrl.h>
#include <fstream>
#include <cstdio>
#include <codecvt>
#include <sstream>
#include <algorithm>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
#define MAX_LOADSTRING 100

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnPaint(HWND hwnd);
void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
std::wstring ReplaceAll(std::wstring str, const std::wstring& from, const std::wstring& to);

HWND hWndMain;


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NOTEPAD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEPAD));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (IsDialogMessage(hWndMain, &msg)){
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//}
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
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_NOTEPAD);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2));
	return RegisterClassEx(&wcex);
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
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	hWndMain = hWnd;

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
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
HWND tbMain = NULL;
HWND statusBar;
LOGFONT lf;
BOOL CheckedStatusBar = FALSE;
CHOOSEFONT fntTextBox;
DWORD rgbCurrent = NULL; // current text color
OPENFILENAME fileOpen;
BOOL IsSave = TRUE;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	//HDC hdc;
	int Start;
	int End;
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
	case WM_CLOSE:
		if (IsSave == FALSE){
			int size = GetWindowTextLength(hWnd);
			WCHAR* text = new WCHAR[size + 1];
			GetWindowText(hWnd, text, size + 1);
			std::wstring str(text);
			str = str.substr(0, str.length() - 10);
			str = L"Do you want to save changes to " + str + L"?";
			int check = MessageBox(hWnd, str.c_str(), L"Notepad", MB_YESNOCANCEL);
			delete text;
			if (check == 6){
				SendMessage(hWnd, WM_COMMAND, ID_FILE_SAVE, 0);
			}
			else if (check == 7){
				PostQuitMessage(0);
			}
		}
		else{
			PostQuitMessage(0);
		}
	case WM_CTLCOLOREDIT:
	{
		if ((HWND)lParam == GetDlgItem(hWnd, IDI_TEXTBOX))
		{
			SetTextColor((HDC)wParam, rgbCurrent);
		}
		return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
	}
	case WM_MENUSELECT:
		if (SendMessage(tbMain, EM_CANUNDO, 0, 0) == 0){
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_UNDO, MF_DISABLED | MF_GRAYED);
		}
		else{
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_UNDO, MF_ENABLED);
		}
		SendMessage(tbMain, EM_GETSEL, (WPARAM)&Start, (LPARAM)&End);
		if (End == Start){
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_COPY, MF_DISABLED | MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_CUT, MF_DISABLED | MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_DELETE, MF_DISABLED | MF_GRAYED);
		}
		else{
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_COPY, MF_ENABLED);
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_CUT, MF_ENABLED);
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_DELETE, MF_ENABLED);
		}
		if (!IsClipboardFormatAvailable(CF_TEXT)){
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_PASTE, MF_DISABLED | MF_GRAYED);
		}
		else{
			EnableMenuItem(GetMenu(hWnd), ID_EDIT_PASTE, MF_ENABLED);
		}
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

WNDPROC pEditProc;

LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int startpos = 0;
	int endpos = 0;
	int posLine = 0;
	int ai;
	int li;
	int posCur;
	WCHAR* buffer = new WCHAR[255];
	switch (msg)
	{
	case WM_LBUTTONUP:
		SendMessage(hwnd, EM_GETSEL, (WPARAM)&startpos, (LPARAM)&endpos);
		posLine = (int)SendMessage(hwnd, EM_LINEFROMCHAR, startpos, 0);
		li = (int)SendMessage(hwnd, EM_LINEINDEX, -1, 0);
		ai = startpos & 0xffff;
		posCur = ai - li;
		if (posCur < 0){
			posLine = (int)SendMessage(hwnd, EM_LINEFROMCHAR, endpos, 0);
			li = (int)SendMessage(hwnd, EM_LINEINDEX, -1, 0);
			ai = endpos & 0xffff;
			posCur = ai - li;
		}
		wsprintf(buffer, L"  Ln %d, Col %d", posLine + 1, posCur + 1);
		SendMessage(statusBar, SB_SETTEXT, 1, (LPARAM)buffer);
		//SetWindowText(statusBar, buffer);
		break;
	case WM_KEYDOWN:
		if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A') {
			SendMessage(hwnd, EM_SETSEL, 0, -1);
		}
		else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'S') {
			SendMessage(hWndMain, WM_COMMAND, ID_FILE_SAVE, 0);
		}
		else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'N') {
			SendMessage(hWndMain, WM_COMMAND, ID_FILE_NEW, 0);
		}
		else if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'O') {
			SendMessage(hWndMain, WM_COMMAND, ID_FILE_OPEN, 0);
		}
		else if (!(GetKeyState(VK_CONTROL) & 0x8000)) {
				IsSave = FALSE;
		}
		SendMessage(hwnd, EM_GETSEL, 0, (LPARAM)&endpos);
		posLine = (int)SendMessage(hwnd, EM_LINEFROMCHAR, endpos, 0);
		ai = endpos & 0xffff;
		li = (int)SendMessage(hwnd, EM_LINEINDEX, -1, 0);
		posCur = ai - li;
		wsprintf(buffer, L"  Ln %d, Col %d", posLine + 1, posCur + 1);
		SendMessage(statusBar, SB_SETTEXT, 1, (LPARAM)buffer);
		break;
	default:
		break;
	}
	delete buffer;
	return CallWindowProc(pEditProc, hwnd, msg, wParam, lParam);
}

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct){
	//statusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, _T("Welcome to SpyWindows"), hWnd, 9000);

	statusBar = CreateWindow(STATUSCLASSNAME, NULL,
		WS_CHILD | SBARS_SIZEGRIP,
		0, 0, 0, 0, hWnd, NULL, hInst, NULL);
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int parts[2] = { width * 0.7, width };

	SendMessage(statusBar, SB_SETPARTS, 2, (LPARAM)&parts);

	tbMain = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, 0, 0, 100, 100, hWnd, (HMENU)IDI_TEXTBOX, hInst, NULL);
	//Ctrl + A
	if (!(pEditProc = (WNDPROC)SetWindowLong(tbMain, GWL_WNDPROC, (LONG)&EditProc)))
	{
		return false;
	}
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWidth,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	SendMessage(tbMain, WM_SETFONT, WPARAM(hFont), TRUE);

	SetWindowText(hWnd, L"Untitled - Notepad");
	return true;
}

void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify){
	HMENU hMenu = GetMenu(hWnd);
	HFONT hFont = NULL;
	HDC hdc = GetDC(tbMain);
	SelectObject(hdc, hFont);
	RECT rect;
	WCHAR filename[MAX_PATH] = { '\0' };
	int sizeTextBox; 
	int sizeTitle;
	WCHAR* textTextBox;
	WCHAR* textTitle;
	std::wstring pathMain;
	ReleaseDC(tbMain, hdc);
	switch (id)
	{

	case ID_VIEW_STATUSBAR:
		if (CheckedStatusBar){
			CheckMenuItem(hMenu, ID_VIEW_STATUSBAR, MF_UNCHECKED);
			CheckedStatusBar = FALSE;
			if (GetClientRect(hWnd, &rect))
			{
				SetWindowPos(tbMain, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, NULL);
				ShowWindow(statusBar, FALSE);
				SetWindowPos(statusBar, NULL, 0, 0, rect.right - rect.left, 24, NULL);
			}

		}
		else{
			CheckMenuItem(hMenu, ID_VIEW_STATUSBAR, MF_CHECKED);
			CheckedStatusBar = TRUE;

			if (GetClientRect(hWnd, &rect))
			{
				SetWindowPos(tbMain, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top - 24, NULL);
				ShowWindow(statusBar, TRUE);
				SetWindowPos(statusBar, NULL, 0, 0, rect.right - rect.left, 24, NULL);
			}
		}
		break;
	case ID_FORMAT_FONT:
		fntTextBox.lStructSize = sizeof(CHOOSEFONT);
		fntTextBox.hwndOwner = tbMain;
		fntTextBox.lpLogFont = &lf;
		fntTextBox.rgbColors = rgbCurrent;
		fntTextBox.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
		if (ChooseFont(&fntTextBox) == TRUE){
			hFont = CreateFontIndirect(fntTextBox.lpLogFont);
			rgbCurrent = fntTextBox.rgbColors;
			SendMessage(tbMain, WM_SETFONT, WPARAM(hFont), TRUE);
		}
		break;
	case ID_FILE_SAVE:
		ZeroMemory(&fileOpen, sizeof(fileOpen));
		fileOpen.lStructSize = sizeof(OPENFILENAME);
		fileOpen.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
		fileOpen.lpstrFilter = (LPCWSTR)L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
		fileOpen.lpstrFile = filename;
		fileOpen.nMaxFile = MAX_PATH;
		fileOpen.lpstrTitle = L"Save As";
		fileOpen.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
		fileOpen.lpstrDefExt = (LPCWSTR)L"txt";

		sizeTitle = GetWindowTextLength(hWnd);
		textTitle = new WCHAR[sizeTitle + 1];
		GetWindowText(hWnd, textTitle, sizeTitle + 1);
		pathMain = textTitle;
		pathMain = pathMain.substr(0, pathMain.length() - 10);

		if (pathMain == L"Untitled"){
			if (GetSaveFileName(&fileOpen) == TRUE){
				std::wstring path(fileOpen.lpstrFile);
				SetWindowText(hWnd, (path + L" - Notepad").c_str());
				IsSave = TRUE;
				//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
				sizeTextBox = GetWindowTextLength(tbMain);
				textTextBox = new WCHAR[sizeTextBox + 1];
				GetWindowText(tbMain, textTextBox, sizeTextBox + 1);
				std::wofstream  wif(path);
				wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
				std::wstring str(textTextBox);
				str = ReplaceAll(str, L"\r\n", L"\n");
				wif << str;
				
			}
		}
		else{
			std::wstring path(pathMain);
			SetWindowText(hWnd, (path + L" - Notepad").c_str());
			IsSave = TRUE;
			//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
			sizeTextBox = GetWindowTextLength(tbMain);
			textTextBox = new WCHAR[sizeTextBox + 1];
			GetWindowText(tbMain, textTextBox, sizeTextBox + 1);
			std::wofstream  wif(path);
			wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
			std::wstring str(textTextBox);
			str = ReplaceAll(str, L"\r\n", L"\n");
			wif << str;
		}
		delete textTextBox;
		delete textTitle;
		break;
	case ID_FILE_OPEN:
		
		// Display the Open dialog box. 
		if (IsSave == FALSE){
			sizeTitle= GetWindowTextLength(hWnd);
			textTitle = new WCHAR[sizeTitle + 1];
			GetWindowText(hWnd, textTitle, sizeTitle + 1);
			std::wstring str(textTitle);
			str = str.substr(0, str.length() - 10);
			std::wstring message = L"Do you want to save changes to " + str + L"?";
			int check = MessageBox(hWnd, message.c_str(), L"Notepad", MB_YESNOCANCEL);
			delete textTitle;
			if (check == 6){
				SendMessage(hWnd, WM_COMMAND, ID_FILE_SAVE, 0);
			}
			else if (check != 7){
				break;
			}
		}
		// Initialize OPENFILENAME
		ZeroMemory(&fileOpen, sizeof(fileOpen));
		fileOpen.lStructSize = sizeof(fileOpen);
		fileOpen.hwndOwner = NULL;
		fileOpen.lpstrFile = filename;
		//
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		//
		fileOpen.lpstrFile[0] = '\0';
		fileOpen.nMaxFile = MAX_PATH;
		fileOpen.lpstrFilter = L"Text Documents(*.txt)\0*.txt\0All File\0*.*\0";
		fileOpen.nFilterIndex = 1;
		fileOpen.lpstrTitle = L"Open";
		fileOpen.lpstrFileTitle = NULL;
		fileOpen.nMaxFileTitle = 0;
		fileOpen.lpstrInitialDir = NULL;
		fileOpen.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&fileOpen) == TRUE){
			std::wifstream wif(fileOpen.lpstrFile);
			std::wstring path(fileOpen.lpstrFile);
			SetWindowText(hWnd, (path + L" - Notepad").c_str());
			IsSave = TRUE;
			wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
			std::wstringstream wss;
			wss << wif.rdbuf();
			std::wstring str = wss.str();
			//str = ReplaceAll(str, L"\n", L"\n");
			str = ReplaceAll(str, L"\n", L"\r\n");
			SetWindowText(tbMain, str.c_str());
		}
		break;
	case ID_FILE_NEW:
		if (IsSave == FALSE){
			sizeTitle = GetWindowTextLength(hWnd);
			textTitle = new WCHAR[sizeTitle + 1];
			GetWindowText(hWnd, textTitle, sizeTitle + 1);
			std::wstring str(textTitle);
			str = str.substr(0, str.length() - 10);
			std::wstring message = L"Do you want to save changes to " + str + L"?";
			int check = MessageBox(hWnd, message.c_str(), L"Notepad", MB_YESNOCANCEL);
			delete textTitle;
			if (check == 6){
				SendMessage(hWnd, WM_COMMAND, ID_FILE_SAVE, 0);
			}
			else if (check != 7){
				break;
			}
		}
		SetWindowText(hWnd, L"Untitled - Notepad");
		SetWindowText(tbMain, L"");
		break;
	case ID_EDIT_SELECTALL:
		SendMessage(tbMain, EM_SETSEL, 0, -1);
		break;
	case ID_EDIT_DELETE:
		SendMessage(tbMain, WM_KEYDOWN, VK_DELETE, 0x001C0001);
		//Not Save
		IsSave = FALSE;
		break;
	case ID_EDIT_PASTE:
		SendMessage(tbMain, WM_PASTE, 0, 0);
		//Not Save
		IsSave = FALSE;
		break;
	case ID_EDIT_COPY:
		SendMessage(tbMain, WM_COPY, 0, 0);
		break;
	case ID_EDIT_CUT:
		SendMessage(tbMain, WM_CUT, 0, 0);
		//Not Save
		IsSave = FALSE;
		break;
	case ID_EDIT_UNDO:
		SendMessage(tbMain, EM_UNDO, 0, 0);
		//Not Save
		IsSave = FALSE;
		break;
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}
}
void OnSize(HWND hwnd, UINT state, int cx, int cy){
	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int parts[2] = { width * 0.7, width };
	SendMessage(statusBar, SB_SETPARTS, 2, (LPARAM)&parts);
	if (CheckedStatusBar){
		SetWindowPos(tbMain, NULL, 0, 0, cx, cy - 24, NULL);
		//ShowWindow(GetDlgItem(hwnd, ID_VIEW_STATUSBAR), SW_HIDE);
		SetWindowPos(statusBar, NULL, 0, 0, cx, 24, NULL);
	}
	else{
		SetWindowPos(tbMain, NULL, 0, 0, cx, cy, NULL);
		//ShowWindow(GetDlgItem(hwnd, ID_VIEW_STATUSBAR), SW_HIDE);
		SetWindowPos(statusBar, NULL, 0, 0, cx, 24, NULL);
	}

	//int b = SetWindowPos(tbMain, NULL, 100, 20, cx + 2, cy, TRUE);
}
void OnPaint(HWND hWnd){
	BITMAP bmJPY, bmSGD;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);


	DeleteDC(hdcMem);
	EndPaint(hWnd, &ps);
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}


std::wstring ReplaceAll(std::wstring str, const std::wstring& from, const std::wstring& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}