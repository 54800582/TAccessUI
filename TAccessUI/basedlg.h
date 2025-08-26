#pragma once
#include "framework.h"
#include "resource.h"

typedef UINT(__stdcall* type_pGetDpiForWindow)(
    HWND hwnd
    );
typedef BOOL(__stdcall* type_pSystemParametersInfoForDpi)(
    UINT  uiAction,
    UINT  uiParam,
    PVOID pvParam,
    UINT  fWinIni,
    UINT  dpi
    );

extern type_pGetDpiForWindow		pfnGetDpiForWindow;
extern type_pSystemParametersInfoForDpi pfnSystemParametersInfoForDpi;

class CBaseDlg
{
public:
	CBaseDlg();
	virtual ~CBaseDlg() {};

private:
	HWND m_hWnd{ nullptr };


protected:
	virtual void OnInitialUpdate(); // WM_INITDIALOG消息里调用来初始话对话框里的控件
	virtual LRESULT OnReceiveMessage(UINT msg, WPARAM wparam, LPARAM lparam);

public:
	static LRESULT CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetHwnd() { return m_hWnd; }
};

//首次使用CBaseDlg基类之前调用该API来初始化CBaseDlg队列
void InitBaseDlgQueue();

