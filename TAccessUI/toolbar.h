#pragma once

class CTAToolbar
{
public:
	CTAToolbar();
	~CTAToolbar();

private:
	HWND m_hWndToolbar{ nullptr };

	HWND m_hMainWnd{ nullptr };
	HINSTANCE m_hInst{ nullptr };

	HIMAGELIST m_hImageList{ nullptr };  //32*32

	void OnInitialUpdate();

public:
	BOOLEAN Create(HWND hWnd, HINSTANCE hInst, DWORD_PTR id = 0xE800);
	HWND GetHwnd() const { return m_hWndToolbar; }

	void AddButton(int idIcon/*iBitmap*/, int idCommad, BOOLEAN bEnabled = TRUE, int fsStyle = TBSTYLE_BUTTON, int iImageSize= 32);
	void AddSeperator();

	LRESULT ToolbarWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class CTARebar
{
public:
	CTARebar();
	~CTARebar();

private:
	HWND m_hWndRebar{ nullptr };

	HWND m_hMainWnd{ nullptr };
	HINSTANCE m_hInst{ nullptr };

	void OnInitialUpdate();

public:
	BOOLEAN Create(HWND hWnd, HINSTANCE hInst, DWORD_PTR id = 0xE801);
	HWND GetHwnd() const { return m_hWndRebar; }

	HRESULT AddBand(HWND hBand, UINT id, BOOLEAN bBreak = FALSE);
	HRESULT AddToolbar(HWND hToolbar, UINT id, BOOLEAN bBreak=FALSE);

	LRESULT ToolbarWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class CTAStatusBar
{
public:
	CTAStatusBar();
	~CTAStatusBar();

private:
	HWND m_hWnd{ nullptr };

	void OnInitialUpdate();

public:
	BOOLEAN Create(HWND hWnd, HINSTANCE hInst, DWORD_PTR id = 0xE802);
	HWND GetHwnd() const { return m_hWnd; }

	void SetText(int nPart, TCHAR* pszText);

};

