#include "framework.h"

#include "Resource.h"
#include "taccssvw.h"
#include "utils.h"

CTAccessView::CTAccessView()
{
    LOGFONT lf;

    // specify the font to use (stored in a LOGFONT structure)
    lf.lfHeight = 20;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_DONTCARE;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;
    _tcsncpy_s(lf.lfFaceName, sizeof(lf.lfFaceName) / sizeof(TCHAR), _T("MS Shell Dlg"), 12);
}

CTAccessView::~CTAccessView()
{
    if (m_hImageList24 != nullptr)
    {
        ImageList_Destroy(m_hImageList24);
    }
    if (m_hImageList32 != nullptr)
    {
        ImageList_Destroy(m_hImageList32);
    }
}

void CTAccessView::OnInitialUpdate()
{
    DWORD dwExStyle = LVS_EX_DOUBLEBUFFER | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES;//0x10424
    ListView_SetExtendedListViewStyle(m_hWnd, dwExStyle);

    
    m_hImageList24 = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 16, 8);
    HICON hIcon = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_APPLICATION));
    ImageList_ReplaceIcon(m_hImageList24, -1, hIcon);

    m_hImageList32 = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 16, 8);
    hIcon = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_APPLICATION));
    ImageList_ReplaceIcon(m_hImageList32, -1, hIcon);

    TCHAR szSystemDirectory[MAX_PATH];
    if (!GetSystemDirectory(szSystemDirectory, MAX_PATH))
        return;

    TCHAR szFile[MAX_PATH];
    _stprintf_s(szFile, _T("%s\\kernel32.dll"), szSystemDirectory);
    WORD iIcon = 0;
    hIcon = ExtractAssociatedIcon(m_hInst, szFile, &iIcon);
    ImageList_ReplaceIcon(m_hImageList24, -1, hIcon);

    iIcon = 0;
    hIcon = ExtractAssociatedIcon(m_hInst, szFile, &iIcon);
    ImageList_ReplaceIcon(m_hImageList32, -1, hIcon);

    _stprintf_s(szFile, _T("%s\\drivers\\acpi.sys"), szSystemDirectory);
    iIcon = 0;
    hIcon = ExtractAssociatedIcon(m_hInst, szFile, &iIcon);
    ImageList_ReplaceIcon(m_hImageList24, -1, hIcon);

    iIcon = 0;
    hIcon = ExtractAssociatedIcon(m_hInst, szFile, &iIcon);
    ImageList_ReplaceIcon(m_hImageList32, -1, hIcon);

    hIcon = LoadShellIcon(SHSTOCKICONID::SIID_FOLDEROPEN, 0);
    ImageList_ReplaceIcon(m_hImageList24, -1, hIcon);

    hIcon = LoadShellIcon(SHSTOCKICONID::SIID_FOLDEROPEN, 1);
    ImageList_ReplaceIcon(m_hImageList32, -1, hIcon);

    hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_WMI), IMAGE_ICON, 24, 24, 0);
    ImageList_ReplaceIcon(m_hImageList24, -1, hIcon);

    hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_WMI), IMAGE_ICON, 32, 32, 0);
    ImageList_ReplaceIcon(m_hImageList32, -1, hIcon);

    hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_REG), IMAGE_ICON, 24, 24, 0);
    ImageList_ReplaceIcon(m_hImageList24, -1, hIcon);

    hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_REG), IMAGE_ICON, 32, 32, 0);
    ImageList_ReplaceIcon(m_hImageList32, -1, hIcon);

    SendMessage(m_hWnd, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)m_hImageList24);

    LV_COLUMN lvc;
    RtlZeroMemory(&lvc, sizeof LV_COLUMN);

    HWND hHeader = ListView_GetHeader(m_hWnd);

    lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    lvc.fmt = LVCFMT_LEFT;
    lvc.pszText = (PWCHAR)_T("Column 1"); //_T("Autoruns Entry");  //应用名称
    lvc.cx = 0x190 * 1.5;
    ListView_InsertColumn(m_hWnd, Header_GetItemCount(hHeader), &lvc);

    lvc.pszText = (PWCHAR)_T("Column 2");// _T("Description");
    lvc.cx = 0xF0 * 1.5;
    ListView_InsertColumn(m_hWnd, Header_GetItemCount(hHeader), &lvc);

    lvc.pszText = (PWCHAR)_T("Column 3");// _T("Publisher");
    lvc.cx = 0xC8 * 1.5;
    ListView_InsertColumn(m_hWnd, Header_GetItemCount(hHeader), &lvc);

    lvc.pszText = (PWCHAR)_T("Column 4");//_T("Image Path");
    lvc.cx = 0x190 * 1.5;
    ListView_InsertColumn(m_hWnd, Header_GetItemCount(hHeader), &lvc);

    lvc.pszText = (PWCHAR)_T("Column 5");// _T("Timestamp");
    lvc.cx = 0x96 * 1.5;
    ListView_InsertColumn(m_hWnd, Header_GetItemCount(hHeader), &lvc);

    lvc.pszText = (PWCHAR)_T("Column 6");
    lvc.cx = 0x78 * 1.5;
    ListView_InsertColumn(m_hWnd, Header_GetItemCount(hHeader), &lvc);

    //ListView_EnableGroupView(m_hWnd, TRUE);

    ShowWindow(m_hWnd, SW_SHOW);

    RefershNow();
}

void CTAccessView::RedrawItems()
{
    DWORD itemCount = 0;
    DWORD countPerPage = 0;
    DWORD idxTop = 0;

    ListView_SetItemCountEx(m_hWnd, itemCount, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
    //itemCount =  ListView_GetItemCount(GetListCtrl().GetSafeHwnd());
    countPerPage = ListView_GetCountPerPage(m_hWnd);
    idxTop = ListView_GetTopIndex(m_hWnd);
    ListView_RedrawItems(m_hWnd, idxTop, idxTop + countPerPage);
}

void CTAccessView::Resize()
{
    RECT  rc;

    GetClientRect(m_hMainWnd, &rc);

    MoveWindow(m_hWnd,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        TRUE);

    //only call this if we want the LVS_NOSCROLL style
    //PositionHeader(hwndListView);
}

void CTAccessView::GetDispInfo(LVITEM* pItem)
{
    // called when the listview needs to display data
    // first, move to the appropriate row in the database

    if ((pItem->mask & LVIF_PARAM) == LVIF_PARAM)
    {
        pItem->mask |= LVIF_PARAM;
        pItem->lParam = (LPARAM)nullptr;
    }

    if ((pItem->mask & LVIF_IMAGE) == LVIF_IMAGE)
    {
        pItem->iImage = 0;
    }

    BOOLEAN bGroup = FALSE;
    if ((pItem->mask & LVIF_INDENT) == LVIF_INDENT)
    {
        if (bGroup)
            pItem->iIndent = -1;
        else
            pItem->iIndent = 1;
    }

    if ((pItem->mask & LVIF_TEXT) == LVIF_TEXT)
    {
        // then display the appropriate column
        pItem->pszText[0] = _T('\0');
        switch (pItem->iSubItem)
        {
        case 0:
            //_tcscpy_s(pItem->pszText, pItem->cchTextMax, pEntry->m_szAppName.GetString());
            break;
        case 1:

            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;

        default:
            break;
        }
    }

    DWORD dwExStyle = 0;
    dwExStyle = ListView_GetExtendedListViewStyle(m_hWnd);
    if (dwExStyle & LVS_EX_CHECKBOXES)
    {
        if (pItem->iSubItem == 0
            && (pItem->mask & LVIF_STATE) == LVIF_STATE)
        {
            //ListView_SetCheckState(m_hWnd, pItem->iItem, TRUE);
            pItem->state = INDEXTOSTATEIMAGEMASK(2);
            pItem->stateMask = LVIS_STATEIMAGEMASK;

            DWORD dwSel = ListView_GetNextItem(m_hWnd, -1, LVIS_SELECTED);
            if (dwSel == pItem->iItem)
            {
                pItem->state |= LVIS_SELECTED;
                pItem->stateMask = LVIS_STATEIMAGEMASK | LVIS_SELECTED;
            }
        }
    }
}

BOOLEAN CTAccessView::Create(HWND hWnd, HINSTANCE hInst)
{
    m_hMainWnd = hWnd;
    m_hInst = hInst;

    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_NOTIFY;

    //0x5600904D
    dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_NOSORTHEADER | WS_VSCROLL | WS_HSCROLL
        | LVS_OWNERDATA | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_REPORT;

    HWND hWndLv = CreateWindowEx(0/*WS_EX_CLIENTEDGE*/, 
        WC_LISTVIEW, //_T("SysListView32")
        nullptr,
        dwStyle,//0x5600100D,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
        hWnd,
        nullptr,//reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_LISTBOX)),
        hInst, 0);
    if (hWndLv == nullptr)
        return FALSE;
    m_hWnd = hWndLv;

    //Resize();
    OnInitialUpdate();

    return TRUE;
}

LRESULT CTAccessView::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message != WM_NOTIFY)
    {
        return 0;
    }
    NMHDR* phdr = (NMHDR*)lParam;

    if (phdr->hwndFrom != m_hWnd)
        return 0;

    // these 3 notifications are only sent by virtual listviews
    switch (phdr->code)
    {
    case LVN_GETDISPINFO:
    {
        NMLVDISPINFO* pLvdi;

        pLvdi = (NMLVDISPINFO*)lParam;
        GetDispInfo(&pLvdi->item);
        return 1;
    }
    break;

    case LVN_ODFINDITEM:
    {
        NMLVFINDITEM* pFindItem = (NMLVFINDITEM*)lParam;
            
        return 1;
    }

    default:
        break;
    }

    return 0;
}

LRESULT CTAccessView::OnCustomDraw(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if (message != WM_NOTIFY)
    {
        return 0;
    }
    NMHDR* phdr = (NMHDR*)lParam;
    if (phdr->hwndFrom != m_hWnd)
        return 0;
    if (phdr->code != NM_CUSTOMDRAW)
    {
        return 0;
    }

    LPNMLVCUSTOMDRAW pNMLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(phdr);

    *pResult = CDRF_DODEFAULT;
    if (CDDS_PREPAINT == pNMLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pNMLVCD->nmcd.dwDrawStage)
    {
        //*pResult = CDRF_NOTIFYSUBITEMDRAW;
        BOOLEAN bGroup = FALSE;
        BOOLEAN bImagePahtExisted = FALSE;
        BOOLEAN bVerifyCodeSignatures = FALSE;
        if (bGroup)
        {
            //组条目，将子项区域合并，直接画图标和文字
            pNMLVCD->clrTextBk = rgbGroup;

            RECT rcDst{ 0, 0, 0, 0 };
            CopyRect(&rcDst, &pNMLVCD->nmcd.rc);
            if (IsRectEmpty(&rcDst))
            {
                *pResult = CDRF_SKIPDEFAULT;
                return 1;
            }
            COLORREF rgb = rgbGroup;
            BOOLEAN bSel = ListView_GetItemState(m_hWnd, pNMLVCD->nmcd.dwItemSpec, LVIS_SELECTED);
            if(bSel)
            {
                rgb = GetSysColor(COLOR_HIGHLIGHT);
            }
            rgb = SetBkColor(pNMLVCD->nmcd.hdc, rgb);
            if (rgb != -1)
            {
                ExtTextOut(pNMLVCD->nmcd.hdc, 0, 0, ETO_OPAQUE, &rcDst, nullptr, 0, nullptr);
                SetBkColor(pNMLVCD->nmcd.hdc, rgb);
            }
            SetBkMode(pNMLVCD->nmcd.hdc, TRANSPARENT);
            if (bSel)
            {
                rgb = GetSysColor(COLOR_HIGHLIGHTTEXT);
            }
            else
            {
                rgb = GetSysColor(COLOR_WINDOWTEXT);
            }
            SetTextColor(pNMLVCD->nmcd.hdc, rgb);
            OffsetRect(&rcDst, 2, 2);

            //icon rect
            rcDst.right = rcDst.left + 24;
            rcDst.bottom = rcDst.top + 24;

            //text rect
            pNMLVCD->nmcd.rc.left = rcDst.left + 24 + 4;

            ImageList_DrawEx(m_hImageList24, 5, pNMLVCD->nmcd.hdc, 
                rcDst.left, rcDst.top, rcDst.right - rcDst.left, rcDst.bottom - rcDst.top, 
                CLR_NONE, CLR_NONE, ILD_NORMAL);
            //DrawText(pNMLVCD->nmcd.hdc, pEntry->m_szAppName.GetString(), pEntry->m_szAppName.GetLength(), 
            //    &pNMLVCD->nmcd.rc, DT_SINGLELINE | DT_VCENTER);

            //if (pEntry->m_szTimestamp.GetLength() > 0)
            //{
            //    if (ListView_GetSubItemRect(m_hWnd, pNMLVCD->nmcd.dwItemSpec, 4, LVIR_BOUNDS, &rcDst))
            //    {
            //        rcDst.left += 4;
            //        DrawText(pNMLVCD->nmcd.hdc, pEntry->m_szTimestamp.GetString(), pEntry->m_szTimestamp.GetLength(),
            //            &rcDst, DT_SINGLELINE | DT_VCENTER);
            //    }
            //}

            *pResult = CDRF_SKIPDEFAULT;  //将不会触发LVN_GETDISPINFO消息
            return 1;
        }
        else if (!bImagePahtExisted)
        {
            pNMLVCD->clrTextBk = rgbYellow_L;
        }
        else if (bVerifyCodeSignatures)
        {
            pNMLVCD->clrTextBk = 0xD0D0FF;
        }
        *pResult = CDRF_DODEFAULT;
    }
    return 1;
}

void CTAccessView::RefershNow()
{
    RedrawItems();
}