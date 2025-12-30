#pragma once

const COLORREF rgbRed = 0x000040FF;
const COLORREF rgbGreen = 0x000A000;
const COLORREF rgbBlue = 0x00FF0000;
const COLORREF rgbBlack = 0x00000000;
const COLORREF rgbWhite = 0x00FFFFFF;
const COLORREF rgbYellow = 0x008080;

const COLORREF rgbYellow_L = 0x00F0F0;
const COLORREF rgbGreen_L = 0x000C000;

const COLORREF rgbGroup = 0xFFD0D0;

#define ID_EVENT_SCHEDULE_UPDATE  10801

class CTAccessView
{
public:
	CTAccessView();
	~CTAccessView();

private:

	HWND m_hWnd{ nullptr };
	HWND m_hHeaderWnd{ nullptr };

	HWND m_hMainWnd{ nullptr };
	HINSTANCE m_hInst{ nullptr };

	HIMAGELIST m_hImageList24{ nullptr };
	HIMAGELIST m_hImageList32{ nullptr };

	void OnInitialUpdate();
	void Resize();

	void RedrawItems();

	void GetDispInfo(LVITEM* pItem);
	int FindItem(int /*iStart*/, LVFINDINFO* /*plvfi*/);
	void OnNMDoubleClick(NMHDR* pHdr);
	void OnColumnClick(NMHDR* pNotifyStruct);
	void OnPopupMenu(WPARAM wParam, LPARAM lParam);
	void OnItemChanged(NMHDR* pHdr);

private:
	int m_sortColumnCur{ -1 };
	int m_nColumnCounter{ 0 };  //同一列单击逢三取消排序  
	int m_sortType;  //0:descend 1: ascend

public:
	BOOLEAN Create(HWND hWnd, HINSTANCE hInst);
	HWND GetHwnd() const { return m_hWnd; }

	INT GetSelectedItem();

	LRESULT OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	LRESULT OnCustomDraw(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void RefershNow();
	void OnEditCopy();
	void OnFont();
};

