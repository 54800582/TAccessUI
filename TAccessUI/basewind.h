#pragma once

class CBaseWindow
{
public:

	CBaseWindow();
	virtual ~CBaseWindow() {}

	static LRESULT CALLBACK	WindowProc(HWND, UINT, WPARAM, LPARAM);

	virtual LRESULT OnReceiveMessage(UINT msg, WPARAM wparam, LPARAM lparam);

protected:

	virtual void OnPaint() = 0;


	HWND		m_hWnd{ nullptr };
	HINSTANCE	m_hInstance{ nullptr };
};

