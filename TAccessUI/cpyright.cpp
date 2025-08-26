#include "cpyright.h"

CDlgCopyright::CDlgCopyright()
{

}

CDlgCopyright::~CDlgCopyright()
{
    if (m_hDpiFont != NULL)
    {
        DeleteObject(m_hDpiFont);
        m_hDpiFont = NULL;
    }
}

HFONT CDlgCopyright::CreateFont(HWND hWnd)
{
    //UINT dpi = 0;// GetDpiForWindow(hWnd);
    NONCLIENTMETRICS ncMetrics;
    LOGFONT logFont;


    if (pfnSystemParametersInfoForDpi == nullptr)
        pfnSystemParametersInfoForDpi = (type_pSystemParametersInfoForDpi)GetProcAddress(LoadLibrary(L"User32.dll"),
            "SystemParametersInfoForDpi");
    if (pfnGetDpiForWindow == nullptr)
        pfnGetDpiForWindow = (type_pGetDpiForWindow)GetProcAddress(LoadLibrary(L"User32.dll"),
            "GetDpiForWindow");

    if (m_hDpiFont != NULL)
    {
        DeleteObject(m_hDpiFont);
        m_hDpiFont = NULL;
    }

    memset(&logFont, 0, sizeof(logFont));
    RtlZeroMemory(&ncMetrics, sizeof ncMetrics);
    ncMetrics.cbSize = sizeof ncMetrics;
    if (pfnSystemParametersInfoForDpi != nullptr
        && pfnGetDpiForWindow != nullptr
        && pfnSystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof ncMetrics, &ncMetrics, 0, pfnGetDpiForWindow(hWnd))
        )
    {
        logFont = ncMetrics.lfMessageFont;
    }
    else
    {
        HGDIOBJ hGdiObj = GetStockObject(DEFAULT_GUI_FONT);
        GetObject(hGdiObj, sizeof logFont, &logFont);
    }
    logFont.lfWeight = FW_BOLD;// 0x2BC;
    HFONT hFont = CreateFontIndirect(&logFont);
    return hFont;
}

// WM_INITDIALOG消息里调用来初始话对话框里的控件
void CDlgCopyright::OnInitialUpdate()
{
    HICON hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_TALOGO));
    SendMessage(GetDlgItem(GetHwnd(), IDC_TA_ICON), STM_SETICON, (WPARAM)hIcon, 0);

    //hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CLOSE));
    //SendMessage(GetDlgItem(GetHwnd(), IDC_BTN_X), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

    //HBITMAP hBitmap = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_CLOSE));
    //SendMessage(GetDlgItem(GetHwnd(), IDC_BTN_X), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

    SetDlgItemText(GetHwnd(), IDC_TA_PUBLISHER, _T("Copyright © 2025 T.Access"));
    SetDlgItemText(GetHwnd(), IDC_TA_PRODUCTNAME, _T("TAcess通用Window UI框架 v1.0"));
    SetDlgItemText(GetHwnd(), IDC_TA_HLINK,
        _T("<a HREF=\"https://github.com/54800582/wintools\">T.Access - https://github.com/54800582/wintools</a>"));

    if (m_hDpiFont == NULL)
    {
        m_hDpiFont = CreateFont(GetHwnd());
    }
}

LRESULT CDlgCopyright::OnReceiveMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wNotifyCode = HIWORD(wParam);  //https://learn.microsoft.com/zh-cn/windows/win32/menurc/wm-command?redirectedfrom=MSDN&devlangs=cpp&f1url=%3FappId%3DDev16IDEF1%26l%3DZH-CN%26k%3Dk(WINUSER%252FWM_COMMAND)%3Bk(WM_COMMAND)%3Bk(DevLang-C%252B%252B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
        if (wNotifyCode == BN_CLICKED)
        {
            switch (wmId)
            {
            case IDC_BTN_X:
                m_bHideCopyrigth = TRUE;
                ShowWindow(GetHwnd(), SW_HIDE);
                InvalidateRect(GetParent(GetHwnd()), nullptr, TRUE);
                SendMessage(GetParent(GetHwnd()), WM_SIZE, 0, 0);
                return TRUE;
            default:
                break;
            }
        }
        break;
    }
    case WM_SIZE:
    {
        OnSize(wParam, lParam);
        break;
    }
    case WM_NOTIFY:
        //NMHDR* pNMHDR = (NMHDR*)lParam;
        OnNotify(GetHwnd(), wParam, lParam);
        break;

    case WM_CTLCOLORSTATIC:
    {
        HWND hItem = GetDlgItem(GetHwnd(), IDC_TA_PRODUCTNAME);
        if (hItem == (HWND)lParam
            )
        {
            SetBkMode((HDC)wParam, TRANSPARENT);
            SelectObject((HDC)wParam, m_hDpiFont);//版本信息使用醒目字体
            //不返回画笔，使用静态控件缺省的背景画笔
            //HBRUSH hbrBkgnd = GetSysColorBrush(COLOR_BTNFACE);
            //return PtrToLong(hbrBkgnd);
        }
    }
    break;

    case WM_DPICHANGED:
        m_hDpiFont = CreateFont(GetHwnd());
        break;

    default:
        break;
    }
    return CBaseDlg::OnReceiveMessage(msg, wParam, lParam);
}


////////message handler

void CDlgCopyright::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    NMHDR* lpNMHDR = (NMHDR*)lParam;
    int code = lpNMHDR->code;
    if (lpNMHDR->idFrom == IDC_TA_HLINK
        && code == NM_CLICK)
    {
        NMLINK* pNMLink = (NMLINK*)lParam;
        ShellExecute(hWnd, _TEXT("open"), pNMLink->item.szUrl, NULL, NULL, 1);
    }
}

void CDlgCopyright::OnSize(WPARAM wParam, LPARAM lParam)
{
    RECT rcWindow;
    RECT rcControl;

    if ((int)wParam == SIZE_MINIMIZED)
        return;

    //// Find the client area of the application.
    GetClientRect(GetHwnd(), &rcWindow);

    GetWindowRect(GetDlgItem(GetHwnd(), IDC_BTN_X), &rcControl);
    ScreenToClient(GetHwnd(), (LPPOINT)&rcControl.left);  //转换为相对父控件的坐标
    ScreenToClient(GetHwnd(), (LPPOINT)&rcControl.right);

    //rcControl.right = rcWindow.right - 20;
    UINT width = rcControl.right - rcControl.left;
    UINT height = rcControl.bottom - rcControl.top;

    SetWindowPos(GetDlgItem(GetHwnd(), IDC_BTN_X), nullptr, rcWindow.right - width -8, rcControl.top,
           width, height, SWP_NOZORDER);

    //MoveWindow(GetDlgItem(GetHwnd(), IDC_EDT_EVTINFO), rcControl.left, rcControl.top,
      //  width, height, true);
}