
#include "basedlg.h"

type_pGetDpiForWindow		pfnGetDpiForWindow = nullptr;
type_pSystemParametersInfoForDpi pfnSystemParametersInfoForDpi = nullptr;

BOOLEAN m_bInitDlgMap{ FALSE };
CRITICAL_SECTION m_CriticalSection;
std::map<HWND, CBaseDlg*> m_DlgInstancesMap;

void InitBaseDlgQueue()
{
	if (m_bInitDlgMap)
		return;
    m_bInitDlgMap = TRUE;
	InitializeCriticalSectionEx(&m_CriticalSection, 0, 0);
	m_DlgInstancesMap.clear();
}

CBaseDlg::CBaseDlg()
{

}

// WM_INITDIALOG消息里调用来初始话对话框里的控件
void CBaseDlg::OnInitialUpdate()
{

}

LRESULT CALLBACK CBaseDlg::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CBaseDlg* pDlg = nullptr;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        pDlg = (CBaseDlg*)lParam;
        pDlg->m_hWnd = hWnd;

        pDlg->OnInitialUpdate();

        EnterCriticalSection(&m_CriticalSection);
        m_DlgInstancesMap.insert(std::map<HWND, CBaseDlg*>::value_type(hWnd, pDlg));
        LeaveCriticalSection(&m_CriticalSection);
        break;
    }
    default:
    {
        EnterCriticalSection(&m_CriticalSection);
        std::map<HWND, CBaseDlg*>::iterator iter = m_DlgInstancesMap.find(hWnd);
        if (iter != m_DlgInstancesMap.end())
        {
            pDlg = (*iter).second;
        }
        LeaveCriticalSection(&m_CriticalSection);
        break;
    }
    }

    if (pDlg != nullptr)
        return pDlg->OnReceiveMessage(message, wParam, lParam);
    return 0;
}

LRESULT CBaseDlg::OnReceiveMessage(UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
    {
        EnterCriticalSection(&m_CriticalSection);
        m_DlgInstancesMap.erase(GetHwnd());
        LeaveCriticalSection(&m_CriticalSection);
        break;
    }
    default:
        break;
    }
	return S_OK;
}
