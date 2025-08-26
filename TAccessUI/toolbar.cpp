#include "framework.h"

#include "resource.h"
#include "toolbar.h"

CTAToolbar::CTAToolbar()
{

}

CTAToolbar::~CTAToolbar()
{
    if (m_hImageList != nullptr)
        ImageList_Destroy(m_hImageList);
    m_hImageList = nullptr;
}

void CTAToolbar::OnInitialUpdate()
{
    DWORD imageSize = 32;
    m_hImageList = ImageList_Create(imageSize, imageSize, ILC_COLOR32, 8, 4);
    SendMessage(m_hWndToolbar, TB_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)m_hImageList);

    //LRESULT result;
    TBBUTTON tbBtn;
    RtlZeroMemory(&tbBtn, sizeof TBBUTTON);
    SendMessage(m_hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    /*
    TBBUTTON tbButtons[] = {
        {IDI_REFRESH, IDM_REFRESH, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {IDI_COPY, IDM_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {IDI_FIND, IDM_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {IDI_PROPERTIES, IDM_PROPERTIES, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
        {-1, -1, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0},
    };
    for (int idx = 0;;idx++)
    {
        if (tbButtons[idx].idCommand == -1)
            break;

        RtlZeroMemory(&tbBtn, sizeof TBBUTTON);
        HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(tbButtons[idx].iBitmap), IMAGE_ICON, imageSize, imageSize, 0);
        int nImage = ImageList_ReplaceIcon(m_hImageList, -1, hIcon);

        tbBtn.iBitmap = MAKELONG(nImage, 0);
        tbBtn.idCommand = tbButtons[idx].idCommand;
        tbBtn.fsState = tbButtons[idx].fsState;
        tbBtn.fsStyle = tbButtons[idx].fsStyle;// | BTNS_AUTOSIZE;
        //tbBtn.iString = (INT_PTR)_T("TEXT");
        result = SendMessageW(m_hWndToolbar, TB_INSERTBUTTON, -1, (LPARAM)&tbBtn);
    }
    */
    SendMessage(m_hWndToolbar, TB_AUTOSIZE, 0, 0);
}

void CTAToolbar::AddButton(int idIcon/*iBitmap*/, int idCommand, BOOLEAN bEnabled, int fsStyle, int iImageSize)
{
    LRESULT result;
    TBBUTTON tbBtn;

    RtlZeroMemory(&tbBtn, sizeof TBBUTTON);

    if (idCommand != 0)
    {
        HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(idIcon), IMAGE_ICON, iImageSize, iImageSize, 0);
        int nImage = ImageList_ReplaceIcon(m_hImageList, -1, hIcon);

        tbBtn.iBitmap = MAKELONG(nImage, 0);
        tbBtn.idCommand = idCommand;
        tbBtn.fsState = bEnabled ? TBSTATE_ENABLED : 0;
        tbBtn.fsStyle = fsStyle;// | BTNS_AUTOSIZE;
        //tbBtn.iString = (INT_PTR)_T("TEXT");
    }
    else
    {
        tbBtn.fsStyle = BTNS_SEP;
    }
    result = SendMessageW(m_hWndToolbar, TB_INSERTBUTTON, -1, (LPARAM)&tbBtn);
}

void CTAToolbar::AddSeperator()
{
    LRESULT result;
    TBBUTTON tbBtn;

    RtlZeroMemory(&tbBtn, sizeof TBBUTTON);
    
    tbBtn.fsStyle = BTNS_SEP;
    result = SendMessageW(m_hWndToolbar, TB_INSERTBUTTON, -1, (LPARAM)&tbBtn);
}

BOOLEAN CTAToolbar::Create(HWND hWndParent, HINSTANCE hInst, DWORD_PTR id )
{
    m_hMainWnd = hWndParent;
    m_hInst = hInst;

    //5600094Ch
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NODIVIDER;
    dwStyle |= TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
    //dwStyle |= TBSTYLE_LIST; //按钮右侧会有文字占位符，需要BTNS_AUTOSIZE来消除
    //dwStyle |= TBSTYLE_DROPDOWN | TBSTYLE_GROUP;使用这两个风格，不会自动显示Toolbar

    HIMAGELIST hImageList = nullptr;
    HWND hWnd = nullptr;

    
    hWnd = CreateWindowEx(0/*WS_EX_CLIENTEDGE*/, 
        TOOLBARCLASSNAME,//_T("ToolbarWindow32"),
        nullptr,
        dwStyle,
        CW_USEDEFAULT, 0, 200, 0,
        hWndParent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        hInst, 0);

    //SendMessage(hWnd, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_MIXEDBUTTONS);

    m_hWndToolbar = hWnd;

    OnInitialUpdate();

    SendMessage(hWnd, WM_SIZE, 0, 0);
    //ShowWindow(hWnd, SW_SHOW);

    return TRUE;
}

LRESULT CTAToolbar::ToolbarWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    LPTOOLTIPTEXT lpToolTipText = NULL;
    lpToolTipText = (LPTOOLTIPTEXT)lParam;
    if (lpToolTipText->hdr.code == TTN_NEEDTEXT)
    {
        if ((UINT)wParam == 0
            || (lpToolTipText->uFlags & TTF_IDISHWND) != 0)
        {
            return 0;
        }

        TCHAR szTooltip[0x100];
        RtlZeroMemory(szTooltip, 0x100);
        if (LoadString(m_hInst, (UINT)wParam, szTooltip, 0x100) >0)
        {
            TCHAR* pEnter = _tcschr(szTooltip, _T('\n'));
            if(pEnter != nullptr)
                _tcscpy_s(lpToolTipText->szText, pEnter + 1);
            else 
                _tcscpy_s(lpToolTipText->szText, szTooltip);
        }
        lpToolTipText->uFlags = TTF_DI_SETITEM;
        return 1;
    }

    return 0;
}


///////////////////////////////////////////////////
//// Rebar

CTARebar::CTARebar()
{

}

CTARebar::~CTARebar()
{

}

void CTARebar::OnInitialUpdate()
{
    REBARINFO rbi;
    rbi.cbSize = sizeof(REBARINFO);
    rbi.fMask = 0;

    SendMessage(m_hWndRebar, RB_SETBARINFO, 0, (LPARAM) & rbi);

}

BOOLEAN CTARebar::Create(HWND hParent, HINSTANCE hInstance, DWORD_PTR id)
{
    DWORD dwError = 0;
    DWORD dwStyle = 0;

    //56002640h
    dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER;
    dwStyle |= RBS_AUTOSIZE | RBS_BANDBORDERS | RBS_VARHEIGHT;
    m_hWndRebar = CreateWindowEx(0, 
        REBARCLASSNAME,
        (LPTSTR)NULL,
        dwStyle,
        0, 0, 100, 100, 
        hParent, 
        (HMENU)id, 
        hInstance, NULL);

    if (m_hWndRebar == 0)
    {
        dwError =__HRESULT_FROM_WIN32(GetLastError());
        return FALSE;
    }
    
    OnInitialUpdate();

    //5600184Ch
    //dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NODIVIDER;
    //dwStyle |= TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
    //dwStyle |= TBSTYLE_LIST; //按钮右侧会有文字占位符，需要BTNS_AUTOSIZE来消除
    //dwStyle |= TBSTYLE_DROPDOWN | TBSTYLE_GROUP; //使用这两个风格，不会自动显示Toolbar

    //HWND hWndPad = nullptr;

    //hWndPad = CreateWindowEx(0/*WS_EX_CLIENTEDGE*/,
    //    TOOLBARCLASSNAME,//_T("ToolbarWindow32"),
    //    nullptr,
    //    dwStyle,
    //    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
    //    hParent,
    //    reinterpret_cast<HMENU>(static_cast<INT_PTR>(id+100)),
    //    hInstance, 0);

    //AddToolbar(hWndPad, id + 100);

    ShowWindow(m_hWndRebar, SW_SHOW);
    return TRUE;
}

HRESULT CTARebar::AddToolbar(HWND hToolbar, UINT id, BOOLEAN bBreak)
{
    int nBtnCount = 0;
    nBtnCount = (int)::SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
    
    // Set band info structure
    REBARBANDINFO rbBand;
    ZeroMemory(&rbBand, sizeof(rbBand));

    rbBand.cbSize = sizeof(rbBand);// REBARBANDINFO_V6_SIZE;

    rbBand.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE | RBBIM_IDEALSIZE;
    rbBand.fStyle = RBBS_CHILDEDGE;
    if (nBtnCount > 0)
    {
        // add chevron style for toolbar with buttons
        rbBand.fStyle |= RBBS_USECHEVRON;
    }
    if (bBreak)
    {
        rbBand.fStyle |= RBBS_BREAK;
    }
    rbBand.hwndChild = hToolbar;

    if (id == 0)
    {
        id = SendMessage(m_hWndRebar, RB_GETBANDCOUNT, 0, 0);
        id += 0xEB00;
    }
    rbBand.wID = id;

    // Calculate the size of the band
    BOOL bRet = FALSE;
    RECT rcTmp = { 0, 0, 0, 0 };

    if (nBtnCount > 0)
    {
        bRet = (BOOL)::SendMessage(hToolbar, TB_GETITEMRECT, nBtnCount - 1, (LPARAM)&rcTmp);

        rbBand.cx = nBtnCount * (rcTmp.right - rcTmp.left); // length of band
        rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;  // minimum height 
        rbBand.cxMinChild = rbBand.cx;  // minimum width
    }
    else	// no buttons, either not a toolbar or really has no buttons
    {
        bRet = ::GetWindowRect(hToolbar, &rcTmp);
        rbBand.cx = rcTmp.right - rcTmp.left;
        rbBand.cxMinChild = rbBand.cx;
        rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
    }
    rbBand.cxIdeal = rbBand.cx;  // ?? Not sure this is right

    // Add the band
    LRESULT lRes = SendMessage(m_hWndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
    if (lRes == 0)
    {
        return E_FAIL;
    }

    DWORD dwExStyle = (DWORD)::SendMessage(hToolbar, TB_GETEXTENDEDSTYLE, 0, 0L);
    ::SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0, dwExStyle | TBSTYLE_EX_HIDECLIPPEDBUTTONS);

    return S_OK;
}

HRESULT CTARebar::AddBand(HWND hBand, UINT id, BOOLEAN bBreak)
{
    BOOL bIsToolbar = FALSE;
    int nBtnCount = 0;

    const DWORD STRING_LEN = 32;
    WCHAR szClassName[STRING_LEN];

    if (0 == GetClassName(hBand, szClassName, STRING_LEN))
    {
        return __HRESULT_FROM_WIN32(GetLastError());
    }

    // Note: per MSDN, string returned by GetClassName is always null terminated (but may be truncated)
    if (wcscmp(szClassName, TOOLBARCLASSNAME) == 0)
    {
        bIsToolbar = TRUE;
    }

    if (bIsToolbar)
    {
        // Get number of buttons on the toolbar
        nBtnCount = (int)::SendMessage(hBand, TB_BUTTONCOUNT, 0, 0);
    }

    // Set band info structure
    REBARBANDINFO rbBand;
    ZeroMemory(&rbBand, sizeof(rbBand));

    rbBand.cbSize = sizeof(rbBand);//REBARBANDINFO_V6_SIZE;

    rbBand.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE | RBBIM_IDEALSIZE;
    rbBand.fStyle = RBBS_CHILDEDGE;
    if (nBtnCount > 0)
    {
        // add chevron style for toolbar with buttons
        rbBand.fStyle |= RBBS_USECHEVRON;
    }
    if (bBreak)
    {
        rbBand.fStyle |= RBBS_BREAK;
    }

    rbBand.hwndChild = hBand;

    if (id == 0)
    {
        id = SendMessage(m_hWndRebar, RB_GETBANDCOUNT, 0, 0);
        id += 0xEB00;
    }
    rbBand.wID = id;

    // Calculate the size of the band
    BOOL bRet = FALSE;
    RECT rcTmp = { 0, 0, 0, 0 };

    if (nBtnCount > 0)
    {
        bRet = (BOOL)::SendMessage(hBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM)&rcTmp);

        rbBand.cx = nBtnCount * (rcTmp.right - rcTmp.left); // length of band
        rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;  // minimum height 
        rbBand.cxMinChild = rbBand.cx;  // minimum width
    }
    else	// no buttons, either not a toolbar or really has no buttons
    {
        bRet = ::GetWindowRect(hBand, &rcTmp);
        rbBand.cx = rcTmp.right - rcTmp.left;
        rbBand.cxMinChild = rbBand.cx;
        rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
    }
    rbBand.cxIdeal = rbBand.cx;  // ?? Not sure this is right

    // Add the band
    LRESULT lRes = SendMessage(m_hWndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
    if (lRes == 0)
    {
        return E_FAIL;
    }

    DWORD dwExStyle = (DWORD)::SendMessage(hBand, TB_GETEXTENDEDSTYLE, 0, 0L);
    ::SendMessage(hBand, TB_SETEXTENDEDSTYLE, 0, dwExStyle | TBSTYLE_EX_HIDECLIPPEDBUTTONS);

    return S_OK;
}

LRESULT CTARebar::ToolbarWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    return S_OK;
}


///////////////////////////////////////////
/////STATUS BAR

CTAStatusBar::CTAStatusBar()
{

}
CTAStatusBar::~CTAStatusBar()
{

}

void CTAStatusBar::OnInitialUpdate()
{
#define PANEL_NUM 3
    int array[PANEL_NUM] = { 120,120 * 2,-1 };
    SendMessage(m_hWnd, SB_SETPARTS, (WPARAM)PANEL_NUM, (LPARAM)array); //设置面板个数
    SendMessage(m_hWnd, SB_SETTEXT, (LPARAM)1, (WPARAM)TEXT("")); //设置第二个面板内容
    SendMessage(m_hWnd, SB_SETTEXT, (LPARAM)2, (WPARAM)TEXT("")); //设置第三个面板内容
}

BOOLEAN CTAStatusBar::Create(HWND hWnd, HINSTANCE hInst, DWORD_PTR id)
{
    DWORD dwStyle = 0;

    //0x56000900
    dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    dwStyle |= SBARS_SIZEGRIP | SBARS_TOOLTIPS;
    m_hWnd = CreateStatusWindow(dwStyle,_T("就绪"), hWnd, id);
    //SetWindowLongPtr(m_hWnd);
    OnInitialUpdate();

    return TRUE;
}

void CTAStatusBar::SetText(int nPart, TCHAR* pszText)
{
    SendMessage(m_hWnd, SB_SETTEXT, nPart, (WPARAM)pszText);
}