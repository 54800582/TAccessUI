#pragma once

//WM_NOTIFY iCode
#define TA_QUICKSEARCH  0x1600

class CDlgQuickSearch
{
public:

	CDlgQuickSearch();
	~CDlgQuickSearch();

private:
	HWND m_hWnd{ nullptr };
	WNDPROC m_QuickSearchOrigWndProc{ nullptr };

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetHwnd() { return m_hWnd; }
};

