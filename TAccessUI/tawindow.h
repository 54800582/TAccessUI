#pragma once

#include "basewind.h"
#include "toolbar.h"
#include "taccssvw.h"
#include "quicksch.h"
#include "cpyright.h"

class CTAcccessWindow: public CBaseWindow
{
public:
	CTAcccessWindow(HINSTANCE hInst = nullptr);
	~CTAcccessWindow();

private:

	CTAccessView m_TaccessView;
	CTAToolbar m_TaToolbar;
	CTARebar m_TaRebar;
	CTAStatusBar& m_TaStatusbar{ CTAStatusBar::Instance() };
	CDlgQuickSearch m_DlgQuickSearch;
	CDlgCopyright m_DlgCopyright;

	BOOLEAN m_bAlwaysOnTop{ FALSE };
	DWORD m_dwUpdateSpeed{ 2000 };
	DWORD m_dwLastUpdateSpeed{ 2000 };

private:
	HMENU m_hMenu{ nullptr };
	void AddMenuRunas();
	void OnInitMenuPopup(WPARAM wParam, LPARAM lParam);
	void OnPauseResume();
	void OnUpdateSpeed(UINT nCommandID);

	// Message handlers
private:
	HRESULT OnCreate();
	void	OnSize();
	void	OnPaint();
	void    OnWmNotify(const NMHDR* pHdr);
	void	OnRunAs();

public:
	
	LRESULT OnReceiveMessage(UINT msg, WPARAM wparam, LPARAM lparam);

	static INT_PTR CALLBACK OnAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

