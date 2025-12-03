#pragma once
#include "basedlg.h"

class CDlgRewards :
    public CBaseDlg
{

public:
    CDlgRewards();
    ~CDlgRewards();

private:

    HFONT m_hDpiFont{ NULL };  //为版本信息准备字体

    HFONT CreateFont(HWND hWnd);

protected:
    void OnInitialUpdate(); // WM_INITDIALOG消息里调用来初始话对话框里的控件
    LRESULT OnReceiveMessage(UINT msg, WPARAM wparam, LPARAM lparam);
};

