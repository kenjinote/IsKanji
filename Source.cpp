#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

BOOL IsKanji(LPWSTR c)
{
	if (c == NULL || c[0] == 0)
	{
		return FALSE;
	}
	const WCHAR c1 = c[0];
	const WCHAR c2 = c[1];
	if (c1 == 0x3005)
	{
		// 々
		return TRUE;
	}
	else if (0x3400 <= c1 && c1 <= 0x4DBF)
	{
		// CJK統合漢字拡張A
		return TRUE;
	}
	else if (0x4E00 <= c1 && c1 <= 0x9FCF)
	{
		// CJK統合漢字
		return TRUE;
	}
	else if (IS_HIGH_SURROGATE(c1) && (((0xD840 <= c1 && c1 < 0xD869) && IS_LOW_SURROGATE(c2)) || (c1 == 0xD869 && (0xDC00 <= c2 && c2 <= 0xDEDF))))
	{
		return TRUE;
	}
	else if (0xF900 <= c1 && c1 <= 0xFAFF)
	{
		// CJK互換漢字
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;
	static HWND hEdit;
	static HWND hStatic;
	switch (msg)
	{
	case WM_CREATE:
		hFont = CreateFont(28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Sugoe UI"));
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hStatic = CreateWindow(TEXT("STATIC"), 0, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, 0);
		break;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 10, 10, LOWORD(lParam) - 20, 32, TRUE);
		MoveWindow(hStatic, 10, 50, LOWORD(lParam) - 20, 32, TRUE);
		break;
	case WM_COMMAND:
		if ((HWND)lParam == hEdit && HIWORD(wParam) == EN_CHANGE)
		{
			const int nSize = GetWindowTextLengthW(hEdit);
			LPWSTR lpszText = (LPWSTR)GlobalAlloc(0, sizeof(WCHAR) * (nSize + 1));
			GetWindowText(hEdit, lpszText, nSize + 1);
			SetWindowText(hStatic, IsKanji(lpszText) ? TEXT("漢字です") : TEXT("漢字ではありません"));
			GlobalFree(lpszText);
		}
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("入力された文字列の 1 文字目が漢字かどうか判定する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
