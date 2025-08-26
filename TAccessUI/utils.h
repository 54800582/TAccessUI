#pragma once

/////////////////////////////////////////////////
///file version help functions
///
typedef struct LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
} LANGANDCODEPAGE, *PLANGANDCODEPAGE;

PVOID GetFileVersionInfoBlock(TCHAR* szFilename);
BOOLEAN QueryFileVersionInfo(PVOID pBlock, TCHAR* pVerName, TCHAR* pszVerValue, DWORD cchVerValue);
BOOLEAN QueryVerFileDescription(PVOID pBlock, TCHAR* pszFileDescription, DWORD cch);
BOOLEAN QueryVerCompanyName(PVOID pBlock, TCHAR* pszCompanyName, DWORD cch);
BOOLEAN QueryVerFileVersion(PVOID pBlock, TCHAR* pszFileVersion, DWORD cch);

BOOLEAN IsWindowsOsOrLater(DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD wServicePackMajor);
BOOLEAN IsWindowsX86_64();

BOOL IsElevated();
void CopyToClipboard(CString* data);

void FormatDateTime(FILETIME DateTime, _TCHAR* szTimestampStr, DWORD cchTimestampStr);

void QueryRegKeyLastWriteTime(HKEY hKey, FILETIME* FileTime);

HICON LoadShellIcon(DWORD id, DWORD uFlags);

BOOLEAN RunAsAdmin();

DWORD QueryProcessAccountInfo(IN HANDLE dwProcessId,
	OUT TCHAR* szAccount,
	IN ULONG cchAccount,
	OUT TCHAR* szDomain,
	IN ULONG cchDomain);

BOOL AdjustProcessPrivilege(TCHAR* szPrivilegeName, BOOL bEnabled);
