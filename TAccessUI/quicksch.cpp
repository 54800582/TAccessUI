#include "framework.h"

#include "resource.h"
#include "quicksch.h"

CDlgQuickSearch::CDlgQuickSearch()
{

}

CDlgQuickSearch::~CDlgQuickSearch()
{

}

LRESULT CALLBACK CDlgQuickSearch::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        CDlgQuickSearch* pDlg = (CDlgQuickSearch*)lParam;
        pDlg->m_hWnd = hWnd;

        //SendMessage(GetDlgItem(hWnd, IDC_EDT_QUICKSEARCH), EM_SETLIMITTEXT, UINT_MAX, 0);
        Edit_SetCueBannerText(GetDlgItem(hWnd, IDC_EDT_QUICKSEARCH), _T("Quick Filter"));
        //DrawIconEx()

        return TRUE;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wNotifyCode = HIWORD(wParam);  //https://learn.microsoft.com/zh-cn/windows/win32/menurc/wm-command?redirectedfrom=MSDN&devlangs=cpp&f1url=%3FappId%3DDev16IDEF1%26l%3DZH-CN%26k%3Dk(WINUSER%252FWM_COMMAND)%3Bk(WM_COMMAND)%3Bk(DevLang-C%252B%252B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
        if (wNotifyCode == EN_UPDATE)
        {
            TCHAR szText[100];
            GetDlgItemText(hWnd, IDC_EDT_QUICKSEARCH, szText, 100);
            return TRUE;
        }
    }
    break;

    case WM_SETFONT:
        break;

    case WM_CLOSE:
        break;

    case WM_NOTIFY:
    {
        NMHDR* pNMHDR = (NMHDR*)lParam;
        
        break;
    }

    case WM_TIMER:
    {
        break;
    }

    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    {
        return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
    }
    break;

    case WM_DPICHANGED:
        break;
    default:
        break;
    }
    return 0;
}