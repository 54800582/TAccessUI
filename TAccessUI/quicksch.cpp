#include "framework.h"

#include "resource.h"
#include "quicksch.h"

const static TCHAR s_szCTAQuickSearchProp[] = _T("CTAQuickSearchProp_This");

CDlgQuickSearch::CDlgQuickSearch()
{

}

CDlgQuickSearch::~CDlgQuickSearch()
{

}

LRESULT CALLBACK CDlgQuickSearch::EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CDlgQuickSearch* pThis = nullptr;
    pThis = (CDlgQuickSearch*) GetProp(hWnd, s_szCTAQuickSearchProp);
    if (pThis == nullptr)
        return 0;
    switch (message)
    {
    case WM_TIMER:
    {
        int evtId = (int)(wParam);
        if (evtId == 1)
        {//快速搜索Timer，发送自定义消息来执行快速搜索
            KillTimer(hWnd, evtId);
            int nId = GetWindowLongPtr(hWnd, GWLP_ID);
            GetParent(hWnd);
            NMHDR hdr;
            hdr.hwndFrom = hWnd;
            hdr.code = TA_QUICKSEARCH;
            hdr.idFrom = nId;
            SendMessage(GetParent(hWnd), WM_NOTIFY,(WPARAM) nId, (LPARAM) & hdr);
        }
        return 1;
    }
    case WM_KEYDOWN:
    {
        //触发Timer，延迟300ms以等待用户输入更多的字符后在Timer中执行快速搜索
        SetTimer(hWnd, 1, 300, nullptr);
        int vk = (int)wParam;
        if (vk == VK_ESCAPE)
        {
            SetWindowText(hWnd, _T(""));
        }
        else if(vk == VK_TAB)
        {
            SetFocus(GetParent(hWnd));
        }
    }
    case WM_KILLFOCUS:
    case WM_SETFOCUS:
    case WM_ERASEBKGND:
    case WM_PAINT:
    default:
        break;
    }
    return pThis->m_QuickSearchOrigWndProc(hWnd, message, wParam, lParam);
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
        Edit_SetCueBannerText(GetDlgItem(hWnd, IDC_EDT_QUICKSEARCH), _T("Search"));
        if (SetProp(GetDlgItem(hWnd, IDC_EDT_QUICKSEARCH), s_szCTAQuickSearchProp, pDlg))
        {
            pDlg->m_QuickSearchOrigWndProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(hWnd, IDC_EDT_QUICKSEARCH), GWLP_WNDPROC, (LONG_PTR)EditWndProc);

        }//DrawIconEx()

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
            //处理快速搜索
            return TRUE;
        }
    }
    break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd, IDC_EDT_QUICKSEARCH));
        break;

    case WM_SETFONT:
        break;

    case WM_CLOSE:
        break;

    case WM_NOTIFY:
    {
        NMHDR* pNMHDR = (NMHDR*)lParam;
        if (pNMHDR->code == TA_QUICKSEARCH)
        {
            TCHAR szText[MAX_PATH];
            GetDlgItemText(hWnd, IDC_EDT_QUICKSEARCH, szText, MAX_PATH);
            _tcsupr_s(szText);
            
            //执行快速搜索
            //TextQuickSearch(szText);
            //SendMessage(GetActiveWindow(), WM_COMMAND, IDM_REFRESH, 0);
        }
        
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