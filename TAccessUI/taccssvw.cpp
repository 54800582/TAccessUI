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
    DWORD dwExStyle = LVS_EX_DOUBLEBUFFER | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT;
    //dwExStyle |= LVS_EX_CHECKBOXES;//show checkbox style
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
    m_hHeaderWnd = hHeader;

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

    if (IsElevated())
    {
        //插入特权列
    }

    //ListView_EnableGroupView(m_hWnd, TRUE);

    ShowWindow(m_hWnd, SW_SHOW);

    RefershNow();
}

void CTAccessView::RedrawItems()
{
    DWORD itemCount = 0;
    DWORD countPerPage = 0;
    DWORD idxTop = 0;

    itemCount = 10;
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
        //if (bGroup)
        //    pItem->iIndent = -1;
        //else
        //    pItem->iIndent = 1;
    }

    if ((pItem->mask & LVIF_TEXT) == LVIF_TEXT)
    {
        // then display the appropriate column
        pItem->pszText[0] = _T('\0');
        _tcscpy_s(pItem->pszText, pItem->cchTextMax, _T("Test"));
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
    dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL;
    dwStyle |= LVS_OWNERDATA | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_REPORT;
    //dwStyle |= LVS_NOSORTHEADER; //固化表头，不可单击排序

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

int CTAccessView::FindItem(int iStart, LVFINDINFO* plvfi)
{
    int nItem = 0;

    nItem = ListView_GetNextItem(m_hWnd, -1, LVIS_SELECTED);
    if (nItem == -1)
        return -1;

    int count = ListView_GetItemCount(m_hWnd);
    TCHAR szItemText[MAX_PATH];
    int cchFind = _tcslen(plvfi->psz);
    for (int idx = nItem; idx < nItem + count; idx++)
    {//从当前选择行开始搜索
        szItemText[0] = _T('\0');
        ListView_GetItemText(m_hWnd, idx % count, 0, szItemText, MAX_PATH);
        if (_tcsnicmp(szItemText, plvfi->psz, cchFind) == 0)
        {
            return (idx % count);
        }
    }
    return -1;
}

void CTAccessView::OnNMDoubleClick(NMHDR* pHdr)
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(m_hWnd, &point);

    LVHITTESTINFO testInfo;
    testInfo.pt = point;
    testInfo.flags = 0;
    testInfo.iGroup = 0;
    testInfo.iItem = 0;
    testInfo.iSubItem = 0;
    int nItem = ListView_SubItemHitTest(m_hWnd, &testInfo);

    if (nItem == -1
        || testInfo.iItem == -1)
        return;

    //处理双击表格行的事件，例如打开对应的属性Dialog
}

void CTAccessView::OnPopupMenu(WPARAM wParam, LPARAM lParam)
{
    NMHDR* phdr = (NMHDR*)lParam;
    POINT point{ 0, 0 }, mpt{ 0, 0 };
    GetCursorPos(&point);
    mpt = point;
    ScreenToClient(m_hHeaderWnd, &point);
    HDHITTESTINFO hdHitTestInfo;
    hdHitTestInfo.pt = point;
    hdHitTestInfo.flags = 0;
    hdHitTestInfo.iItem = -1;
    LRESULT result = SendMessage(m_hHeaderWnd, HDM_HITTEST, 0, (LPARAM)&hdHitTestInfo);
    if (result >= 0)
        return;

    LVHITTESTINFO lvHitTestInfo;
    lvHitTestInfo.pt = point;
    lvHitTestInfo.flags = 0;
    lvHitTestInfo.iItem = 0;
    lvHitTestInfo.iSubItem = 0;
    lvHitTestInfo.iGroup = 0;
    ListView_SubItemHitTest(m_hWnd, &lvHitTestInfo);
    if (lvHitTestInfo.iItem < 0)
        return;

    HMENU hLoadMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDM_LIST_MENU));
    HMENU hMenu = GetSubMenu(hLoadMenu, 0);

    BOOLEAN bEnabled = TRUE;
    EnableMenuItem(hMenu, IDM_PROPERTIES, MF_BYCOMMAND | (bEnabled ? MF_ENABLED : MF_DISABLED));
 

    TrackPopupMenu(hMenu, TPM_LEFTALIGN, mpt.x, mpt.y, 0, m_hMainWnd, nullptr);
    DestroyMenu(hMenu);
}

void CTAccessView::OnColumnClick(NMHDR* pNotifyStruct)
{
    NM_LISTVIEW* pnmlv = (NM_LISTVIEW*)pNotifyStruct;
    if (m_sortColumnCur == -1)
    {
        //m_sortColumnCur = 0;
        m_sortType = 1;
        m_nColumnCounter = 1;
    }
    else if (pnmlv->iSubItem == m_sortColumnCur)
    {
        //m_sortImageCur = m_sortImageCur == 0 ? 1 : 0;
        m_nColumnCounter++;
        if (m_nColumnCounter != 3)
        {
            m_sortType = m_sortType == 0 ? 1 : 0;
        }
        else
        {
            m_sortType = -1;
            m_nColumnCounter = 1;
        }
    }
    else
    {
        m_nColumnCounter = 1;
    }

    //set first column ascend image
    HDITEM hdItem;
    hdItem.mask = HDI_FORMAT;
    HWND hHeader = ListView_GetHeader(m_hWnd);
    Header_GetItem(hHeader, m_sortColumnCur, &hdItem);
    //hdItem.fmt &= ~(HDF_BITMAP_ON_RIGHT | HDF_IMAGE);
    //hdItem.iImage = 0;
    hdItem.fmt &= ~(HDF_BITMAP | HDF_SORTUP | HDF_SORTDOWN);
    Header_SetItem(hHeader, m_sortColumnCur, &hdItem);

    if (m_sortType != -1)
    {
        m_sortColumnCur = pnmlv->iSubItem;

        hdItem.mask = HDI_FORMAT;
        Header_GetItem(hHeader, m_sortColumnCur, &hdItem);
        hdItem.fmt |= ((m_sortType == 0) ? HDF_SORTDOWN : HDF_SORTUP);
        Header_SetItem(hHeader, m_sortColumnCur, &hdItem);
    }
    else {
        m_sortColumnCur = -1;
    }

    //执行排序
    //sort()

    DWORD countPerPage = 0;
    DWORD idxTop = 0;

    countPerPage = ListView_GetCountPerPage(m_hWnd);
    idxTop = ListView_GetTopIndex(m_hWnd);
    ListView_RedrawItems(m_hWnd, idxTop, idxTop + countPerPage);
}

void CTAccessView::OnItemChanged(NMHDR* pHdr)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pHdr);
    if (pNMLV->uChanged != LVIF_STATE
        || (pNMLV->uNewState & LVIS_SELECTED) == 0)
    {
        return;
    }
    int nItem = 0;

    nItem = ListView_GetNextItem(m_hWnd, -1, LVIS_SELECTED);
    if (nItem == -1)
    {
        return;
    }
}

LRESULT CTAccessView::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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
        int i = FindItem(pFindItem->iStart, &pFindItem->lvfi);
        if (pResult != NULL)
        {
            *pResult = i;
        }
        return 1;
    }

    case LVN_COLUMNCLICK:
        OnColumnClick(phdr);
        return 1;

    case NM_RCLICK:
        OnPopupMenu(wParam, lParam);
        return 1;

    case NM_DBLCLK:
        OnNMDoubleClick(phdr);
        return 1;

    case LVN_ITEMCHANGED:
        OnItemChanged(phdr);
        return 1;

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
        BOOLEAN bImagePathExisted = TRUE;
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
        else if (!bImagePathExisted)
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

INT CTAccessView::GetSelectedItem()
{
    int nItem = 0;

    nItem = ListView_GetNextItem(m_hWnd, -1, LVIS_SELECTED);

    return nItem;
}

void CTAccessView::RefershNow()
{
    //rescan_data();
    RedrawItems();
}

void CTAccessView::OnEditCopy()
{
    int nItem = 0;

    nItem = ListView_GetNextItem(m_hWnd, -1, LVIS_SELECTED);
    if (nItem == -1)
        return;

    CString szInfo(_T(""));

    TCHAR strItemText[MAX_PATH] = _T("\0");
    ListView_GetItemText(m_hWnd, nItem, 0, strItemText, MAX_PATH);
    szInfo.AppendFormat(_T("%s"), strItemText);

    for (int nColumn = 1; nColumn < Header_GetItemCount(m_hHeaderWnd); nColumn++)
    {
        strItemText[0] = 0;
        ListView_GetItemText(m_hWnd, nItem, nColumn, strItemText, MAX_PATH);
        szInfo.AppendFormat(_T(", %s"), strItemText);
    }

    CopyToClipboard(&szInfo);
}

#include "Commdlg.h"
#pragma comment(lib, "Comdlg32.lib")
void CTAccessView::OnFont()
{
    HFONT hFont = (HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0);
    LOGFONT lf;

    memset(&lf, 0, sizeof(lf));
    GetObject(hFont, sizeof LOGFONT, &lf);

    CHOOSEFONT cf;            // common dialog box structure
    //static LOGFONT lf;        // logical font structure
    //static DWORD rgbCurrent;  // current text color
    //HFONT hfont, hfontPrev;
    //DWORD rgbPrev;

    // Initialize CHOOSEFONT
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = m_hWnd;
    //cf.hwndOwner = GetActiveWindow();
    cf.lpLogFont = &lf;
    cf.rgbColors = 0;
    //cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
    cf.Flags = CF_NOVERTFONTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;

    if (ChooseFont(&cf) == TRUE)
    {
        hFont = CreateFontIndirect(cf.lpLogFont);
        SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, 1);
    }
}