#include "framework.h"

#include "resource.h"
#include "basewind.h"

//--------------------------------------------------------------------------------------
// BaseWindow constructor.
//--------------------------------------------------------------------------------------

CBaseWindow::CBaseWindow() : m_hWnd(NULL), m_hInstance(NULL)
{
}


//--------------------------------------------------------------------------------------
// CBaseWindow::WindowProc
// Description: Window procedure.
//--------------------------------------------------------------------------------------

LRESULT CALLBACK CBaseWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBaseWindow* pWin = NULL;

	if (uMsg == WM_NCCREATE)
	{
		// When we create the window, we pass in a pointer to this class 
		// as part of the CREATESTRUCT structure.
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
		pWin = (CBaseWindow*)lpcs->lpCreateParams;

		// Set the window handle.
		pWin->m_hWnd = hwnd;

		// Set the pointer to the class as user data.

		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pWin);
	}
	else
	{
		// Get the pointer to the class.
		pWin = (CBaseWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	if (pWin)
	{
		return pWin->OnReceiveMessage(uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

//--------------------------------------------------------------------------------------
// CBaseWindow::OnReceiveMessage
// Description: Handle window messages other than WM_NCCREATE.
//--------------------------------------------------------------------------------------

LRESULT CBaseWindow::OnReceiveMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCDESTROY:
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);

	case WM_PAINT:
		OnPaint();
		return 0;

	}
	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

