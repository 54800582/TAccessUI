#pragma once
#include "basedlg.h"

#define WINDOW_COPYRIGHT_HEIGHT  120
class CDlgCopyright :
    public CBaseDlg
{
public:
    CDlgCopyright();
    ~CDlgCopyright();

private:
    BOOLEAN m_bHideCopyrigth{ FALSE };

    HFONT m_hDpiFont{ NULL };  //为版本信息准备字体

    HFONT CreateFont(HWND hWnd);

protected:
    void OnInitialUpdate(); // WM_INITDIALOG消息里调用来初始话对话框里的控件
    LRESULT OnReceiveMessage(UINT msg, WPARAM wparam, LPARAM lparam);

    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void OnSize(WPARAM wParam, LPARAM lParam);

public:
    BOOLEAN CopyrightHidded() { return m_bHideCopyrigth; }

};

