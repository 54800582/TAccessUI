#include "framework.h"

#include "resource.h"
#include "tawindow.h"

#include "utils.h"
#include "dlgrewards.h"

CTAcccessWindow::CTAcccessWindow(HINSTANCE hInst)
{
	m_hInstance = hInst;
    InitBaseDlgQueue();
}

CTAcccessWindow::~CTAcccessWindow()
{

}

#define IDM_RUNAS 0x801F
#define IDS_RUNAS_NAME _T("以管理员方式运行")
#define IDS_RUNAS_NAME_EN _T("Run As &Administrator")
void CTAcccessWindow::AddMenuRunas()
{
    if (m_hMenu == nullptr)
    {
        m_hMenu = GetMenu(m_hWnd);
    }
    if (m_hMenu == nullptr)
        return;
    if (IsElevated())
        return;

    HMENU hMenu = GetSubMenu(m_hMenu, 0);
    InsertMenu(hMenu, 0, MF_BYPOSITION, IDM_RUNAS, IDS_RUNAS_NAME);
    InsertMenu(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
}

void CTAcccessWindow::OnInitMenuPopup(WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu = (HMENU)wParam;
    if (hMenu == nullptr)
        return;

    int nMenu = LOWORD(lParam);
    switch (nMenu)
    {
    case 2:
    {
        int nItem = m_TaccessView.GetSelectedItem();
        EnableMenuItem(hMenu, IDM_COPY, MF_BYCOMMAND | ((nItem > 0) ? MF_ENABLED : MF_DISABLED));
        EnableMenuItem(hMenu, IDM_PROPERTIES, MF_BYCOMMAND | ((nItem > 0) ? MF_ENABLED : MF_DISABLED));
        break;
    }
    case 3:
        CheckMenuItem(hMenu, IDM_TOOGLE_PAUSE, (m_dwUpdateSpeed != 0) ? MF_CHECKED : MF_UNCHECKED);
        UINT  check;
        if (m_dwUpdateSpeed == 1000)
            check = IDM_UPDATE_1;
        else if (m_dwUpdateSpeed == 2000)
            check = IDM_UPDATE_2;
        else if (m_dwUpdateSpeed == 5000)
            check = IDM_UPDATE_5;
        else
            check = IDM_UPDATE_PAUSE;
        CheckMenuRadioItem(hMenu, IDM_UPDATE_1, IDM_UPDATE_PAUSE, check, MF_BYCOMMAND);
        break;
    case 4:
        CheckMenuItem(hMenu, IDM_ALWAYSONTOP, m_bAlwaysOnTop ? MF_CHECKED : MF_UNCHECKED);
        break;
    default:
        break;
    }
}

void CTAcccessWindow::OnPauseResume()
{
    if (m_dwUpdateSpeed != 0)
    {
        m_dwLastUpdateSpeed = m_dwUpdateSpeed;
        m_dwUpdateSpeed = 0;
    }
    else
    {
        m_dwUpdateSpeed = m_dwLastUpdateSpeed;
    }
    KillTimer(m_hWnd, ID_EVENT_SCHEDULE_UPDATE);

    if (m_dwUpdateSpeed != 0)
    {
        TCHAR fmtStr[64];
        _stprintf_s(fmtStr, _T("刷新速度: %d 秒"), m_dwUpdateSpeed / 1000);
        CTAStatusBar::Instance().SetPaneText(6, fmtStr);
        SetTimer(m_hWnd, ID_EVENT_SCHEDULE_UPDATE, m_dwUpdateSpeed, NULL);
    }
    else
    {
        CTAStatusBar::Instance().SetPaneText(6, (TCHAR*)_T("暂停"));
    }
    m_TaToolbar.SetCheck(IDM_TOOGLE_PAUSE, m_dwUpdateSpeed != 0);
}

void CTAcccessWindow::OnUpdateSpeed(UINT nCommandID)
{
    switch (nCommandID)
    {
    case IDM_UPDATE_1:
        m_dwUpdateSpeed = 1000;
        break;

    case IDM_UPDATE_2:
        m_dwUpdateSpeed = 2000;
        break;

    case IDM_UPDATE_5:
        m_dwUpdateSpeed = 5000;
        break;

    case IDM_UPDATE_PAUSE:
        m_dwLastUpdateSpeed = m_dwUpdateSpeed;
        m_dwUpdateSpeed = 0;
        break;

    default:
        break;
    }

    KillTimer(m_hWnd, ID_EVENT_SCHEDULE_UPDATE);
    if (m_dwUpdateSpeed != 0)
    {
        TCHAR fmtStr[64];
        _stprintf_s(fmtStr, _T("刷新速度: %d 秒"), m_dwUpdateSpeed / 1000);
        CTAStatusBar::Instance().SetPaneText(6, fmtStr);
        SetTimer(m_hWnd, ID_EVENT_SCHEDULE_UPDATE, m_dwUpdateSpeed, NULL);
    }
    else
    {
        CTAStatusBar::Instance().SetPaneText(6, (TCHAR*)_T("暂停"));
    }
    m_TaToolbar.SetCheck(IDM_TOOGLE_PAUSE, m_dwUpdateSpeed != 0);
}

LRESULT CTAcccessWindow::OnReceiveMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        OnCreate();
        break;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            OnSize();
        }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, OnAbout);
            break;
        case IDM_EXIT:
            DestroyWindow(m_hWnd);
            break;
        case IDM_RUNAS:
            OnRunAs();
            break;
        case IDM_QUICKFIND:
            SendMessage(m_DlgQuickSearch.GetHwnd(), WM_SETFOCUS, 0, 0);
            break;
        case IDM_COPY:
            m_TaccessView.OnEditCopy();
            break;
        case IDM_REFRESH:
        {
            m_TaccessView.RefershNow();
            break;
        }
        case IDM_PROPERTIES:
        {
            CDlgRewards dlgRewards;
            DialogBoxParam(nullptr, MAKEINTRESOURCE(IDD_DLG_REWARD), m_hWnd, CBaseDlg::DlgProc, (LPARAM)&dlgRewards);
            break;
        }

        case IDM_FONT:
        {
            m_TaccessView.OnFont();
            break;
        }

        case IDM_ALWAYSONTOP:
        {
            m_bAlwaysOnTop = m_bAlwaysOnTop ? FALSE : TRUE;
            ::SetWindowPos(m_hWnd,
                m_bAlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            break;
        }
        case IDM_TOOGLE_PAUSE:
            OnPauseResume();
            break;
        case IDM_UPDATE_1:
        case IDM_UPDATE_2:
        case IDM_UPDATE_5:
        case IDM_UPDATE_PAUSE:
            OnUpdateSpeed(wmId);
            break;

        default:
            return DefWindowProc(m_hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        OnPaint();
    }
    break;
    case WM_NOTIFY:
    {
        LPNMHDR pNMHDR = (LPNMHDR)lParam;
        if (m_TaToolbar.ToolbarWndProc(message, wParam, lParam) != 0)
        {
        }
        else
        {
            LRESULT result = 0;
            if (m_TaccessView.OnChildNotify(message, wParam, lParam, &result) == 0)
            {
                result = 0;
                if (m_TaccessView.OnCustomDraw(message, wParam, lParam, &result) != 0)
                {
                    return result;
                }
                return DefWindowProc(m_hWnd, message, wParam, lParam);
            }
            else
            {
                return result;
            }
        }
        break;
    }
    case WM_INITMENUPOPUP:
    {
        OnInitMenuPopup(wParam, lParam);
        break;
    }
    case WM_TIMER:
    {
        int evtId = (int)(wParam);
        if (evtId == ID_EVENT_SCHEDULE_UPDATE)
        {
            //KillTimer(m_hWnd, evtId);
            m_TaccessView.RefershNow();
            //SetTimer(m_hWnd, ID_EVENT_SCHEDULE_UPDATE, m_dwUpdateSpeed, nullptr);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return CBaseWindow::OnReceiveMessage(message, wParam, lParam);
    }
    return S_OK;
}

//////////////////////////////////////////////msg
HRESULT CTAcccessWindow::OnCreate()
{
    HRESULT hr = S_OK;

    AddMenuRunas();
    if (IsElevated())
    {
        TCHAR szAccountName[100];
        TCHAR szDomainName[100];
        TCHAR szWindowText[MAX_PATH];
        hr = QueryProcessAccountInfo(LongToHandle(GetCurrentProcessId()), szAccountName, 100, szDomainName, 100);
        if (hr == 0)
        {
            _stprintf_s(szWindowText, _T("TAccessUI - T.Access: https://github.com/54800582/wintools (Administrator) [%s\\%s]"),
                szAccountName, szDomainName);
            SetWindowText(m_hWnd, szWindowText);
        }
        else
            SetWindowText(m_hWnd, _T("TAccessUI - T.Access: https://github.com/54800582/wintools (Administrator)"));
    }
    else
    {
        SetWindowText(m_hWnd, _T("TAccessUI - T.Access: https://github.com/54800582/wintools"));
    }

    m_TaStatusbar.Create(m_hWnd, m_hInstance);

    m_TaRebar.Create(m_hWnd, m_hInstance);

    m_TaToolbar.Create(m_hWnd, m_hInstance);

    TBBUTTON tbBtn;
    RtlZeroMemory(&tbBtn, sizeof TBBUTTON);
    TBBUTTON tbButtons[] = {
        {IDI_RUNAS, IDM_RUNAS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {0, 0, 0, BTNS_SEP, 0, 0, 0},
        {IDI_UPDATE, IDM_TOOGLE_PAUSE, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_BUTTON | BTNS_CHECK, 0, 0, 0},
        {0, 0, 0, BTNS_SEP, 0, 0, 0},
        {IDI_REFRESH, IDM_REFRESH, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {IDI_COPY, IDM_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {IDI_FIND, IDM_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {IDI_PROPERTIES, IDM_PROPERTIES, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {-1, -1, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
    };
    int idx = 0;
    if (IsElevated())
    {
        idx = 2;
    }
    for (;; idx++)
    {
        if (tbButtons[idx].idCommand == -1)
            break;
        m_TaToolbar.AddButton(&tbButtons[idx]);
    }

    m_TaRebar.AddToolbar(m_TaToolbar.GetHwnd(), 0, TRUE);

    m_TaccessView.Create(m_hWnd, m_hInstance);

    CreateDialogParam(m_hInstance, MAKEINTRESOURCE(IDD_QUICKSEARCH), m_hWnd,
        m_DlgQuickSearch.WndProc, (LPARAM)&m_DlgQuickSearch);
    ShowWindow(m_DlgQuickSearch.GetHwnd(), SW_NORMAL);

    m_TaRebar.AddBand(m_DlgQuickSearch.GetHwnd(), IDD_QUICKSEARCH);

    CreateDialogParam(m_hInstance, MAKEINTRESOURCE(IDD_DLG_COPYRIGHT), m_hWnd,
        m_DlgCopyright.DlgProc, (LPARAM)&m_DlgCopyright);
    ShowWindow(m_DlgCopyright.GetHwnd(), SW_NORMAL);
    
    SetFocus(m_TaccessView.GetHwnd());

    OnUpdateSpeed(IDM_UPDATE_2);

    return hr;
}

void CTAcccessWindow::OnSize()
{
    UINT width = 0;
    UINT height = 0;

    // resize the toolbar
    SendMessage(m_TaToolbar.GetHwnd(), WM_SIZE, 0, 0); // TB_AUTOSIZE
    SendMessage(m_TaRebar.GetHwnd(), WM_SIZE, 0, 0);
    SendMessage(m_TaStatusbar.GetHwnd(), WM_SIZE, 0, 0);
    SendMessage(m_TaccessView.GetHwnd(), WM_SIZE, 0, 0);
    SendMessage(m_DlgCopyright.GetHwnd(), WM_SIZE, 0, 0);

    RECT rcWindow;
    RECT rcControl;

    //// Find the client area of the application.
    GetClientRect(m_hWnd, &rcWindow);
    width = rcWindow.right - rcWindow.left - 4;

    //// Subtract the area of the rebar control.
    GetClientRect(m_TaToolbar.GetHwnd(), &rcControl);
    SubtractRect(&rcWindow, &rcWindow, &rcControl);

    if (!m_DlgCopyright.CopyrightHidded())
    {
        MoveWindow(m_DlgCopyright.GetHwnd(), rcWindow.left + 2, rcWindow.top + 1,
            width, m_DlgCopyright.Height(), true);

        GetClientRect(m_DlgCopyright.GetHwnd(), &rcControl);
        rcWindow.top += rcControl.bottom;
    }
    else
    {
        rcWindow.top += 1;
    }

    GetClientRect(m_TaStatusbar.GetHwnd(), &rcControl);
    width = rcWindow.right - rcWindow.left - 4;
    height = rcWindow.bottom - rcWindow.top;
    height -= (rcControl.bottom - rcControl.top) + 8;
    //SubtractRect(&rcWindow, &rcWindow, &rcControl);

   MoveWindow(m_TaccessView.GetHwnd(), rcWindow.left + 2, rcWindow.top+6,
        width, height, true);
}

void CTAcccessWindow::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    // TODO: 在此处添加使用 hdc 的任何绘图代码...
    EndPaint(m_hWnd, &ps);
}

void CTAcccessWindow::OnRunAs()
{
    PostMessage(m_hWnd, WM_CLOSE, 0, 0);

    RunAsAdmin();
}

void CTAcccessWindow::OnWmNotify(const NMHDR* pHdr)
{
    switch (pHdr->code)
    {
    case TTN_GETDISPINFO:
        // Display tool tips
        
        break;

    default:
       
        break;
    }
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK CTAcccessWindow::OnAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_STATIC_RLSDATE, __DATE__ _T(" Release"));
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}