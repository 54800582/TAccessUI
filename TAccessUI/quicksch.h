#pragma once

class CDlgQuickSearch
{
public:

	CDlgQuickSearch();
	~CDlgQuickSearch();

private:
	HWND m_hWnd{ nullptr };

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetHwnd() { return m_hWnd; }
};

