#include "framework.h"

#include "dlgrewards.h"

CDlgRewards::CDlgRewards()
{

}

CDlgRewards::~CDlgRewards()
{

}

HFONT CDlgRewards::CreateFont(HWND hWnd)
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
void CDlgRewards::OnInitialUpdate()
{
    if (m_hDpiFont == NULL)
    {
        m_hDpiFont = CreateFont(GetHwnd());
    }
}

LRESULT CDlgRewards::OnReceiveMessage(UINT msg, WPARAM wParam, LPARAM lParam)
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
            case IDOK:
                EndDialog(GetHwnd(), LOWORD(wParam));
                return TRUE;
            default:
                break;
            }
        }
        break;
    }
    case WM_SIZE:
    {
        break;
    }
    case WM_NOTIFY:
        //NMHDR* pNMHDR = (NMHDR*)lParam;
        break;

    case WM_CTLCOLORSTATIC:
    {
        HWND hItem = GetDlgItem(GetHwnd(), IDC_REWARDS_INFO);
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

