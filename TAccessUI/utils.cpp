#include "framework.h"
#include "utils.h"

#pragma comment(lib, "Version.lib")

PVOID GetFileVersionInfoBlock(TCHAR* szFilename)
{
	DWORD dwHandle = 0;
	DWORD dwSize = GetFileVersionInfoSize(szFilename, &dwHandle);

	if (dwSize == 0)
	{
		dwHandle = GetLastError();
		return nullptr;
	}

	PVOID pData = malloc(dwSize);
	if (pData == nullptr)
		return nullptr;

	if (GetFileVersionInfo(szFilename, 0, dwSize, pData))
	{
		return pData;
	}
	dwHandle = GetLastError();
	free(pData);
	return nullptr;
}

BOOLEAN QueryFileVersionInfo(PVOID pBlock, TCHAR* pszVerName, TCHAR* pszVerValue, DWORD cchVerValue)
{
	PLANGANDCODEPAGE lpTranslate = nullptr;
	TCHAR* lpBuffer = nullptr;
	UINT cbLen = 0;
	HRESULT hr = S_OK;
	TCHAR szSubBlock[64];

	*pszVerValue = _T('\0');

	if (!VerQueryValue(pBlock,
		TEXT("\\VarFileInfo\\Translation"),
		(LPVOID*)&lpTranslate,
		&cbLen))
	{
		cbLen = GetLastError();
		return FALSE;
	}

	_stprintf_s(szSubBlock, 64,
		TEXT("\\StringFileInfo\\%04x%04x\\%s"),
		lpTranslate->wLanguage,
		lpTranslate->wCodePage,
		pszVerName);

	if (VerQueryValue(pBlock,
		szSubBlock,
		(LPVOID*)&lpBuffer,
		&cbLen))
	{
		_tcscpy_s(pszVerValue, cchVerValue, lpBuffer);
		if (cbLen > cchVerValue)
		{
			pszVerValue[cchVerValue-1] = _T('\0');
		}
		else
		{
			pszVerValue[cbLen] = _T('\0');
		}
		return TRUE;
	}

	cbLen = GetLastError();
	return FALSE;
}

BOOLEAN QueryVerFileDescription(PVOID pBlock, TCHAR* pszFileDescription, DWORD cch)
{
	return QueryFileVersionInfo(pBlock, (PTCHAR)_T("FileDescription"), pszFileDescription, cch);
}

BOOLEAN QueryVerCompanyName(PVOID pBlock, TCHAR* pszCompanyName, DWORD cch)
{
	return QueryFileVersionInfo(pBlock, (PTCHAR)_T("CompanyName"), pszCompanyName, cch);
}

BOOLEAN QueryVerFileVersion(PVOID pBlock, TCHAR* pszFileVersion, DWORD cch)
{
	return QueryFileVersionInfo(pBlock, (PTCHAR)_T("FileVersion"), pszFileVersion, cch);
}

BOOLEAN IsWindowsOsOrLater(DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD wServicePackMajor)
{
	OSVERSIONINFOEX osVerInfoEx;
	DWORD dwConditionMask = 0;

	RtlZeroMemory(&osVerInfoEx, sizeof osVerInfoEx);
	osVerInfoEx.dwOSVersionInfoSize = sizeof osVerInfoEx;
	osVerInfoEx.dwMajorVersion = 6;
	osVerInfoEx.dwMinorVersion = 2;
	osVerInfoEx.wServicePackMajor = 0;
	dwConditionMask = VerSetConditionMask(dwConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	dwConditionMask = VerSetConditionMask(dwConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	dwConditionMask = VerSetConditionMask(dwConditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	return VerifyVersionInfo(&osVerInfoEx, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwConditionMask);
}

BOOLEAN m_bProcessorArchitectureQueried{ FALSE };
BOOLEAN m_bProcessorArchitecture_X86_64{ FALSE };

BOOLEAN IsWindowsX86_64()
{
	if (m_bProcessorArchitectureQueried)
	{
		return m_bProcessorArchitecture_X86_64;
	}
	
	SYSTEM_INFO sysInfo;
	RtlZeroMemory(&sysInfo, sizeof SYSTEM_INFO);
	GetNativeSystemInfo(&sysInfo);
	if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
		|| sysInfo.dwProcessorType == PROCESSOR_ARCHITECTURE_ARM64)
		m_bProcessorArchitecture_X86_64 = TRUE;
	m_bProcessorArchitectureQueried = TRUE;
	return m_bProcessorArchitecture_X86_64;
}

BOOL IsElevated()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}

void CopyToClipboard(CString* data)
{
	if (!::OpenClipboard(nullptr))
	{
		return;
	}

	if (!::EmptyClipboard())
	{
		::CloseClipboard();
		return;
	}

	size_t cbStr = (data->GetLength() + 1) * sizeof(TCHAR);
	HGLOBAL hData = ::GlobalAlloc(GMEM_MOVEABLE, cbStr + 32);
	if (hData == NULL)
	{
		::CloseClipboard();
		return;        // NULL for memory failure
	}

	// now copy the strings, terminate each with CR/LF
	LPTSTR lpOut = (LPTSTR)::GlobalLock(hData);
	memcpy_s(lpOut, cbStr + 32, data->LockBuffer(), cbStr);
	//_tcscpy_s(lpOut, (cbStr + 32)/sizeof(TCHAR), data->LockBuffer());
	GlobalUnlock(hData);
	data->UnlockBuffer();

	// copy the current listbox contents to the clipboard as text
#ifndef _UNICODE
	::SetClipboardData(CF_TEXT, hData);
#else
	::SetClipboardData(CF_UNICODETEXT, hData);
#endif 

	::CloseClipboard();
}

void FormatDateTime(FILETIME DateTime,_TCHAR* szTimestampStr, DWORD cchTimestampStr)
{
	FILETIME LocalTime;
	SYSTEMTIME SystemTime;

	if (DateTime.dwHighDateTime == 0
		&& DateTime.dwLowDateTime == 0)
	{
		szTimestampStr[0] = _T('\0');
		return;
	}

	FileTimeToLocalFileTime((FILETIME*)&DateTime, &LocalTime);
	FileTimeToSystemTime(&LocalTime, &SystemTime);

	DWORD cch = GetDateFormat(LOCALE_USER_DEFAULT, 0, &SystemTime, nullptr, szTimestampStr, cchTimestampStr);
	if (cch != 0)
	{
		szTimestampStr[cch - 1] = _T(' ');
		if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, &SystemTime, nullptr, szTimestampStr + cch, cchTimestampStr - cch) != 0)
			return;
	}

	_stprintf_s(szTimestampStr, cchTimestampStr, _T("%d/%d/%d %d:%02d:%02d"),
		SystemTime.wYear, SystemTime.wDay, SystemTime.wMonth,
		SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
}

/////////////////////////////////////////////
/////Reg Help

void QueryRegKeyLastWriteTime(HKEY hKey, FILETIME* FileTime)
{
	RegQueryInfoKey(hKey, nullptr, nullptr, nullptr, 
		nullptr, nullptr, nullptr, nullptr, 
		nullptr, nullptr, nullptr,
		FileTime);
}

#include <	shellapi.h>
//#pragma comment(lib, "shellapi.lib")

HICON LoadShellIcon(DWORD id, DWORD uFlags)
{
	SHSTOCKICONINFO shIconInfo;

	RtlZeroMemory(&shIconInfo, sizeof SHSTOCKICONINFO);
	shIconInfo.cbSize = sizeof SHSTOCKICONINFO;

	uFlags ^= SHGFI_SMALLICON;
	uFlags |= SHGFI_ICON;
	HRESULT hResult = SHGetStockIconInfo((SHSTOCKICONID)id, uFlags, &shIconInfo);
	if (hResult == S_OK)
	{
		return shIconInfo.hIcon;
	}
	return nullptr;
}

BOOLEAN RunAsAdmin()
{
	TCHAR szFilename[MAX_PATH];

	GetModuleFileName(nullptr, szFilename, MAX_PATH);
	TCHAR* pszCmdLine = GetCommandLine();
	int iNumArgs = 0;
	PWSTR* szArglist = CommandLineToArgvW(pszCmdLine, &iNumArgs);

	WCHAR szCommandLine[1024];
	RtlZeroMemory(szCommandLine, sizeof szCommandLine);

	if (szArglist != nullptr)
	{
		for (int idx = 1; idx < iNumArgs; idx++)
		{
			_tcscat_s(szCommandLine, szArglist[idx]);
		}
	}

	LocalFree(szArglist);

	ShellExecute(nullptr, _T("runas"), szFilename, 
		(szCommandLine[0]!= _T('\0'))?szCommandLine :nullptr, 
		nullptr, SW_SHOWDEFAULT);

	return TRUE;
}

DWORD QueryProcessAccountInfo(IN HANDLE dwProcessId,
	OUT TCHAR* szAccount,
	IN ULONG cchAccount,
	OUT TCHAR* szDomain,
	IN ULONG cchDomain)
{
	CHAR  szTokenUser[0x80];
	PTOKEN_USER pTokenUser = (PTOKEN_USER)szTokenUser;
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	DWORD dwReturnLength = 0;
	SID_NAME_USE eUse;
	DWORD dwError = 0;

	szAccount[0] = '\0';
	szDomain[0] = '\0';
	if (dwProcessId == 0)
		return 0;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)dwProcessId);
	if (hProcess == NULL)
	{
		dwError = GetLastError();
		//_tcscpy(szAccount, _T("<unable to open process>"));

		return dwError;
	}
	
	if (!OpenProcessToken(hProcess, /*TOKEN_ADJUST_PRIVILEGES | */TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		//_tcscpy(szAccount, _T("<unable to open token>"));
		CloseHandle(hProcess);
		return dwError;
	}

	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, 0x80, &dwReturnLength))
	{
		dwError = GetLastError();
		//_tcscpy(szAccount, _T("<unable to query owner>"));
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return dwError;
	}
	if (!LookupAccountSid(_T(""), pTokenUser->User.Sid, szAccount, &cchAccount, szDomain, &cchDomain, &eUse))
	{
		dwError = GetLastError();
		//_tcscpy(szAccount, _T("<unknown owner>"));
	}
	CloseHandle(hToken);
	CloseHandle(hProcess);

	return dwError;
}

BOOL AdjustProcessPrivilege(TCHAR* szPrivilegeName, BOOL bEnabled)
{
	HANDLE hHandle;
	HANDLE tokenHandle;
	DWORD dwRet;
	char* pBuffer = NULL;
	LUID luid;
	TOKEN_PRIVILEGES tokenPrivileges;
	CString strLog;

	hHandle = GetCurrentProcess();
	dwRet = OpenProcessToken(hHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle);
	if (!dwRet) {
		dwRet = GetLastError();
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, szPrivilegeName, &luid))
	{
		CloseHandle(tokenHandle);
		return FALSE;
	}
	tokenPrivileges.PrivilegeCount = 1;
	tokenPrivileges.Privileges[0].Luid = luid;
	tokenPrivileges.Privileges[0].Attributes = bEnabled ? SE_PRIVILEGE_ENABLED : 0;// SE_PRIVILEGE_REMOVED;

	if (AdjustTokenPrivileges(tokenHandle, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES),
		NULL, NULL))
	{
		dwRet = GetLastError();
		CloseHandle(tokenHandle);
		if (dwRet == ERROR_SUCCESS)
			return TRUE;
		return FALSE;
	}
	CloseHandle(tokenHandle);
	return FALSE;
}