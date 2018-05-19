// ==================================================================
// CIniFile.h
//	CIniFile クラス・ヘッダー
// ==================================================================
#include "CIniFile.h"
//引数：ファイル名(m_pszProfileName)

CIniFile::CIniFile()
{
	TCHAR pDrive[(_MAX_DRIVE*2)+1];
	TCHAR pDir[(_MAX_DIR*2)+1];
	TCHAR pFName[(_MAX_FNAME*2)+1];
	TCHAR* pExt = _T(".ini");

	m_szFolderPath[0] = NULL;
	m_szFilePath[0] = NULL;
	// exeのある位置で同ファイル名を初期値にする
	TCHAR path[_MAX_PATH*2+1];
	::GetModuleFileName(NULL,path,sizeof(path));
	//exeファイルのパスを分解
	_tsplitpath(path,pDrive,pDir,pFName,NULL);
	//引数のファイル名をexeファイルのパスでフルパス化
	_tmakepath(m_szFolderPath,pDrive,pDir,NULL,NULL);
	_tmakepath(m_szFilePath,pDrive,pDir,pFName,pExt);
	SetFilePath(m_szFilePath);
}

// ファイルパスセット
#ifdef _UNICODE
BOOL CIniFile::SetFilePathW(const WCHAR* path)
{
	if (!path)
		return m_bExisting = FALSE;
	const DWORD dwFileBit = (_S_IFREG | _S_IREAD | _S_IWRITE);
	struct _stat stt;
	int ret = _wstat( path, &stt );

	if (ret == 0)
	{
		if ((stt.st_mode & dwFileBit) == dwFileBit )
			return m_bExisting = TRUE;
	}
	return m_bExisting = FALSE;
}
#else
// ファイルパスセット
BOOL CIniFile::SetFilePathA(const char* path)
{
	if (!path)
		return m_bExisting = FALSE;
	const DWORD dwFileBit = (_S_IFREG | _S_IREAD | _S_IWRITE);
	struct _stat stt;
	int ret = _stat( path, &stt );

	if (ret == 0)
	{
		if ((stt.st_mode & dwFileBit) == dwFileBit )
		{
			strncpy_s(m_szFilePath, _MAX_PATH*2, path, _MAX_PATH*2);
			return m_bExisting = TRUE;
		}
	}
	return m_bExisting = FALSE;
}
#endif

#ifdef _UNICODE
BOOL CIniFile::ReadBoolW(const WCHAR *Section,const WCHAR *Item, const BOOL Default)
{
	if (!NullCheckW(Section, Item)) return Default;
	return GetPrivateProfileIntW(Section, Item, Default, m_szFilePath);
}
#else
BOOL CIniFile::ReadBoolA(const char *Section,const char *Item, const BOOL Default)
{
	if (!NullCheckA(Section, Item)) return Default;
	return GetPrivateProfileIntA(Section, Item, Default, m_szFilePath);
}
#endif

#ifdef _UNICODE
void CIniFile::WriteBoolW(const WCHAR *Section, const WCHAR *Item, const BOOL Value)
{
	if (!NullCheckW(Section, Item)) return;
	if (Value)	WritePrivateProfileStringW(Section, Item, L"1", m_szFilePath);
	else		WritePrivateProfileStringW(Section, Item, L"0", m_szFilePath);
}
#else
void CIniFile::WriteBoolA(const char *Section, const char *Item, const BOOL Value)
{
	if (!NullCheckA(Section, Item)) return;
	if (Value)	WritePrivateProfileStringA(Section, Item, "1", m_szFilePath);
	else		WritePrivateProfileStringA(Section, Item, "0", m_szFilePath);
}
#endif

#ifdef _UNICODE
int CIniFile::ReadIntW(const WCHAR *Section,const WCHAR *Item, const int Default)
{
	if (!NullCheckW(Section, Item)) return Default;
	return GetPrivateProfileIntW(Section, Item, Default, m_szFilePath);
}
#else
int CIniFile::ReadIntA(const char *Section,const char *Item, const int Default)
{
	if (!NullCheckA(Section, Item)) return Default;
	return GetPrivateProfileIntA(Section, Item, Default, m_szFilePath);
}
#endif

#ifdef _UNICODE
void CIniFile::WriteIntW(const WCHAR *Section, const WCHAR *Item, const int Value)
{
	if (!NullCheckW(Section, Item)) return;
	WCHAR szValue[MAX_KETA];
	_itow(Value, szValue, 10);
	WritePrivateProfileStringW(Section, Item, szValue, m_szFilePath);
}
#else
void CIniFile::WriteIntA(const char *Section, const char *Item, const int Value)
{
	if (!NullCheckA(Section, Item)) return;
	char szValue[MAX_KETA];
	_itoa(Value, szValue, 10);
	WritePrivateProfileStringA(Section, Item, szValue, m_szFilePath);
}
#endif


#ifdef _UNICODE
float CIniFile::ReadFloatW(const WCHAR *Section,const WCHAR *Item, const float Default)
{
	if (!NullCheckW(Section, Item)) return Default;
	WCHAR szValue[MAX_KETA], szRet[MAX_KETA];
	swprintf_s(szValue, MAX_KETA, L"%f", Default);
	/*int nLen = */GetPrivateProfileStringW(Section, Item, szValue, szRet, MAX_KETA-1, m_szFilePath);
	return (float) _wtof(szRet); 
}
#else
float CIniFile::ReadFloatA(const char *Section,const char *Item, const float Default)
{
	if (!NullCheckA(Section, Item)) return Default;
	char szValue[MAX_KETA], szRet[MAX_KETA];
	sprintf_s(szValue, MAX_KETA, "%f", Default);
	/*int nLen = */GetPrivateProfileStringA(Section, Item, szValue, szRet, MAX_KETA-1, m_szFilePath);
	return (float) atof(szRet); 
}
#endif

#ifdef _UNICODE
void CIniFile::WriteFloatW(const WCHAR *Section, const WCHAR *Item, const float Value)
{
	if (!NullCheckW(Section, Item)) return;
	WCHAR szValue[MAX_KETA];
	swprintf_s(szValue, MAX_KETA, L"%f", Value);
	WritePrivateProfileStringW(Section, Item, szValue, m_szFilePath);
}
#else
void CIniFile::WriteFloatA(const char *Section, const char *Item, const float Value)
{
	if (!NullCheckA(Section, Item)) return;
	char szValue[MAX_KETA];
	sprintf_s(szValue, MAX_KETA, "%f", Value);
	WritePrivateProfileStringA(Section, Item, szValue, m_szFilePath);
}
#endif


#ifdef _UNICODE
int CIniFile::ReadStringW(const WCHAR *Section,const WCHAR *Item, const WCHAR* Default, WCHAR* Value, int MaxSize)
{
	if (!NullCheckW(Section, Item)) return 0;
	int nLen = 0;
	if (MaxSize)
		nLen = GetPrivateProfileStringW(Section, Item, Default, Value, MaxSize, m_szFilePath);
	else
		nLen = GetPrivateProfileStringW(Section, Item, Default, Value, 255-1, m_szFilePath);
	return nLen;
}
#else
int CIniFile::ReadStringA(const char *Section,const char *Item, const char* Default, char* Value, int MaxSize)
{
	if (!NullCheckA(Section, Item)) return 0;
	int nLen = 0;
	if (MaxSize)
		nLen = GetPrivateProfileStringA(Section, Item, Default, Value, MaxSize, m_szFilePath);
	else
		nLen = GetPrivateProfileStringA(Section, Item, Default, Value, 255-1, m_szFilePath);
	return nLen;
}
#endif

#ifdef _UNICODE
void CIniFile::WriteStringW(const WCHAR *Section, const WCHAR *Item, const WCHAR* Value)
{
	if (!NullCheckW(Section, Item)) return;
	WritePrivateProfileStringW(Section, Item, Value, m_szFilePath);
}
#else
void CIniFile::WriteStringA(const char *Section, const char *Item, const char* Value)
{
	if (!NullCheckA(Section, Item)) return;
	WritePrivateProfileStringA(Section, Item, Value, m_szFilePath);
}
#endif


// Window情報
#ifdef _UNICODE
void CIniFile::ReadWinW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
{
	if (!NullCheckW(Section, Item)) return;
	const int c_nMaxItem = 255;
	WCHAR szItem[c_nMaxItem];
	WINDOWPLACEMENT wp;

	wp.length = sizeof(WINDOWPLACEMENT);	
	if (!GetWindowPlacement(hWnd, &wp)) return;
	// flags
	swprintf_s(szItem, c_nMaxItem, L"%s.Flags", Item);
	wp.flags = GetPrivateProfileIntW(Section, szItem, wp.flags, m_szFilePath);
	// ShowCmd
	swprintf_s(szItem, c_nMaxItem, L"%s.ShowCmd", Item);
	wp.showCmd = GetPrivateProfileIntW(Section, szItem, wp.showCmd, m_szFilePath);
	// ptMaxPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxX", Item);
	wp.ptMaxPosition.x = GetPrivateProfileIntW(Section, szItem, wp.ptMaxPosition.x, m_szFilePath);
	// ptMaxPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxY", Item);
	wp.ptMaxPosition.y = GetPrivateProfileIntW(Section, szItem, wp.ptMaxPosition.y, m_szFilePath);
	// ptMinPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MinX", Item);
	wp.ptMinPosition.x = GetPrivateProfileIntW(Section, szItem, wp.ptMinPosition.x, m_szFilePath);
	// ptMinPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MinY", Item);
	wp.ptMinPosition.y = GetPrivateProfileIntW(Section, szItem, wp.ptMinPosition.y, m_szFilePath);
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	wp.rcNormalPosition.left = GetPrivateProfileIntW(Section, szItem, wp.rcNormalPosition.left, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	wp.rcNormalPosition.top = GetPrivateProfileIntW(Section, szItem, wp.rcNormalPosition.top, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	wp.rcNormalPosition.right = GetPrivateProfileIntW(Section, szItem, wp.rcNormalPosition.right, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	wp.rcNormalPosition.bottom = GetPrivateProfileIntW(Section, szItem, wp.rcNormalPosition.bottom, m_szFilePath);
	// SetWindowPlacement
	SetWindowPlacement(hWnd, &wp);
}
#else
void CIniFile::ReadWinA(const char *Section,const char *Item,const HWND hWnd)
{
	if (!NullCheckA(Section, Item)) return;
	const int c_nMaxItem = 255;
	char szItem[c_nMaxItem];
	WINDOWPLACEMENT wp;

	wp.length = sizeof(WINDOWPLACEMENT);	
	if (!GetWindowPlacement(hWnd, &wp)) return;
	// flags
	sprintf_s(szItem, c_nMaxItem, "%s.Flags", Item);
	wp.flags = GetPrivateProfileIntA(Section, szItem, wp.flags, m_szFilePath);
	// ShowCmd
	sprintf_s(szItem, c_nMaxItem, "%s.ShowCmd", Item);
	wp.showCmd = GetPrivateProfileIntA(Section, szItem, wp.showCmd, m_szFilePath);
	// ptMaxPosition.x
	sprintf_s(szItem, c_nMaxItem, "%s.MaxX", Item);
	wp.ptMaxPosition.x = GetPrivateProfileIntA(Section, szItem, wp.ptMaxPosition.x, m_szFilePath);
	// ptMaxPosition.y
	sprintf_s(szItem, c_nMaxItem, "%s.MaxY", Item);
	wp.ptMaxPosition.y = GetPrivateProfileIntA(Section, szItem, wp.ptMaxPosition.y, m_szFilePath);
	// ptMinPosition.x
	sprintf_s(szItem, c_nMaxItem, "%s.MinX", Item);
	wp.ptMinPosition.x = GetPrivateProfileIntA(Section, szItem, wp.ptMinPosition.x, m_szFilePath);
	// ptMinPosition.y
	sprintf_s(szItem, c_nMaxItem, "%s.MinY", Item);
	wp.ptMinPosition.y = GetPrivateProfileIntA(Section, szItem, wp.ptMinPosition.y, m_szFilePath);
	// Left
	sprintf_s(szItem, c_nMaxItem, "%s.Left", Item);
	wp.rcNormalPosition.left = GetPrivateProfileIntA(Section, szItem, wp.rcNormalPosition.left, m_szFilePath);
	// Top
	sprintf_s(szItem, c_nMaxItem, "%s.Top", Item);
	wp.rcNormalPosition.top = GetPrivateProfileIntA(Section, szItem, wp.rcNormalPosition.top, m_szFilePath);
	// Right
	sprintf_s(szItem, c_nMaxItem, "%s.Right", Item);
	wp.rcNormalPosition.right = GetPrivateProfileIntA(Section, szItem, wp.rcNormalPosition.right, m_szFilePath);
	// Bottom
	sprintf_s(szItem, c_nMaxItem, "%s.Bottom", Item);
	wp.rcNormalPosition.bottom = GetPrivateProfileIntA(Section, szItem, wp.rcNormalPosition.bottom, m_szFilePath);
	// SetWindowPlacement
	SetWindowPlacement(hWnd, &wp);
}
#endif

#ifdef _UNICODE
void CIniFile::WriteWinW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	WCHAR szItem[c_nMaxItem];
	WCHAR szValue[MAX_KETA];
	WINDOWPLACEMENT wp;

	wp.length = sizeof(WINDOWPLACEMENT);	
	if (!GetWindowPlacement(hWnd, &wp)) return;

	// flags
	swprintf_s(szItem, c_nMaxItem, L"%s.Flags", Item);
	_itow(wp.flags, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// ShowCmd
	swprintf_s(szItem, c_nMaxItem, L"%s.ShowCmd", Item);
	_itow(wp.showCmd, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// ptMaxPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxX", Item);
	_itow(wp.ptMaxPosition.x, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// ptMaxPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxY", Item);
	_itow(wp.ptMaxPosition.y, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// ptMinPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MinX", Item);
	_itow(wp.ptMinPosition.x, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// ptMinPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MinY", Item);
	_itow(wp.ptMinPosition.y, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	_itow(wp.rcNormalPosition.left, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	_itow(wp.rcNormalPosition.top, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	_itow(wp.rcNormalPosition.right, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	_itow(wp.rcNormalPosition.bottom, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
}
#else
void CIniFile::WriteWinA(const char *Section,const char *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	char szItem[c_nMaxItem];
	char szValue[MAX_KETA];
	WINDOWPLACEMENT wp;

	wp.length = sizeof(WINDOWPLACEMENT);	
	if (!GetWindowPlacement(hWnd, &wp)) return;

	// flags
	sprintf_s(szItem, c_nMaxItem, "%s.Flags", Item);
	_itoa(wp.flags, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// ShowCmd
	sprintf_s(szItem, c_nMaxItem, "%s.ShowCmd", Item);
	_itoa(wp.showCmd, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// ptMaxPosition.x
	sprintf_s(szItem, c_nMaxItem, "%s.MaxX", Item);
	_itoa(wp.ptMaxPosition.x, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// ptMaxPosition.y
	sprintf_s(szItem, c_nMaxItem, "%s.MaxY", Item);
	_itoa(wp.ptMaxPosition.y, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// ptMinPosition.x
	sprintf_s(szItem, c_nMaxItem, "%s.MinX", Item);
	_itoa(wp.ptMinPosition.x, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// ptMinPosition.y
	sprintf_s(szItem, c_nMaxItem, "%s.MinY", Item);
	_itoa(wp.ptMinPosition.y, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// Left
	sprintf_s(szItem, c_nMaxItem, "%s.Left", Item);
	_itoa(wp.rcNormalPosition.left, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// Top
	sprintf_s(szItem, c_nMaxItem, "%s.Top", Item);
	_itoa(wp.rcNormalPosition.top, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// Right
	sprintf_s(szItem, c_nMaxItem, "%s.Right", Item);
	_itoa(wp.rcNormalPosition.right, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// Bottom
	sprintf_s(szItem, c_nMaxItem, "%s.Bottom", Item);
	_itoa(wp.rcNormalPosition.bottom, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
}
#endif


// Window位置/サイズ
#ifdef _UNICODE
void CIniFile::ReadWinRectW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	WCHAR szItem[c_nMaxItem];
	RECT rWin;

	if (!GetWindowRect(hWnd, &rWin)) return;
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	rWin.left = GetPrivateProfileIntW(Section, szItem, rWin.left, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	rWin.top = GetPrivateProfileIntW(Section, szItem, rWin.top, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	rWin.right = GetPrivateProfileIntW(Section, szItem, rWin.right, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	rWin.bottom = GetPrivateProfileIntW(Section, szItem, rWin.bottom, m_szFilePath);
	
	SetWindowPos(hWnd, HWND_NOTOPMOST, rWin.left, rWin.top, rWin.right-rWin.left, rWin.bottom-rWin.top, SWP_NOZORDER);
}
#else
void CIniFile::ReadWinRectA(const char *Section,const char *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	char szItem[c_nMaxItem];
	RECT rWin;

	if (!GetWindowRect(hWnd, &rWin)) return;
	// Left
	sprintf_s(szItem, c_nMaxItem, "%s.Left", Item);
	rWin.left = GetPrivateProfileIntA(Section, szItem, rWin.left, m_szFilePath);
	// Top
	sprintf_s(szItem, c_nMaxItem, "%s.Top", Item);
	rWin.top = GetPrivateProfileIntA(Section, szItem, rWin.top, m_szFilePath);
	// Right
	sprintf_s(szItem, c_nMaxItem, "%s.Right", Item);
	rWin.right = GetPrivateProfileIntA(Section, szItem, rWin.right, m_szFilePath);
	// Bottom
	sprintf_s(szItem, c_nMaxItem, "%s.Bottom", Item);
	rWin.bottom = GetPrivateProfileIntA(Section, szItem, rWin.bottom, m_szFilePath);
	
	SetWindowPos(hWnd, HWND_NOTOPMOST, rWin.left, rWin.top, rWin.right-rWin.left, rWin.bottom-rWin.top, SWP_NOZORDER);
}
#endif

#ifdef _UNICODE
void CIniFile::WriteWinRectW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	WCHAR szItem[c_nMaxItem];
	WCHAR szValue[MAX_KETA];
	RECT rWin;

	if (!GetWindowRect(hWnd, &rWin)) return;
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	_itow(rWin.left, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	_itow(rWin.top, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	_itow(rWin.right, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	_itow(rWin.bottom, szValue, 10);
	WritePrivateProfileStringW(Section, szItem, szValue, m_szFilePath);
}
#else
void CIniFile::WriteWinRectA(const char *Section,const char *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	char szItem[c_nMaxItem];
	char szValue[MAX_KETA];
	RECT rWin;

	if (!GetWindowRect(hWnd, &rWin)) return;
	// Left
	sprintf_s(szItem, c_nMaxItem, "%s.Left", Item);
	_itoa(rWin.left, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// Top
	sprintf_s(szItem, c_nMaxItem, "%s.Top", Item);
	_itoa(rWin.top, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// Right
	sprintf_s(szItem, c_nMaxItem, "%s.Right", Item);
	_itoa(rWin.right, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
	// Bottom
	sprintf_s(szItem, c_nMaxItem, "%s.Bottom", Item);
	_itoa(rWin.bottom, szValue, 10);
	WritePrivateProfileStringA(Section, szItem, szValue, m_szFilePath);
}
#endif