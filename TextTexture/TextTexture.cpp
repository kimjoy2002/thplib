// ==================================================================
// CIniFile.h
//	CIniFile クラス・ヘッダー Version 1.0
//	Author : ue
// ==================================================================
#include "CIniFile.h"

CIniFile::CIniFile()
{
	TCHAR pDrive[_MAX_DRIVE*2];
	TCHAR pDir[_MAX_DIR*2];
	TCHAR pFName[_MAX_FNAME*2];
	TCHAR pExt[_MAX_EXT*2];

	m_szFolderPath[0] = NULL;
	m_szFilePath[0] = NULL;
//	strncpy(m_szAppName, __argv[0], sizeof(char)*_MAX_PATH-1);

	// exeのある位置で同ファイル名を初期値にする
#if _UNICODE
	_wsplitpath(__wargv[0], pDrive, pDir, pFName, pExt);
	swprintf_s(m_szFolderPath, _MAX_DIR*2, L"%s%s", pDrive, pDir);
	swprintf_s(m_szFilePath, _MAX_PATH*2, L"%s%s%s.ini", pDrive, pDir, pFName);
#else
	_splitpath(__argv[0], pDrive, pDir, pFName, pExt);
	sprintf(m_szFolderPath, "%s%s", pDrive, pDir);
	sprintf(m_szFilePath, "%s%s%s.ini", pDrive, pDir, pFName);
#endif
	SetFilePath(m_szFilePath);
}

// ファイルパスセット
BOOL CIniFile::SetFilePath(const WCHAR* path)
{
	FILE *f;
	
	_wfopen_s(&f, path, L"r+");
	if (!f)
	{
		m_bExisting = FALSE;
		fclose(f);
	}
	else	m_bExisting = TRUE;
	
	return m_bExisting;
}

BOOL CIniFile::ReadBool(const WCHAR *Section,const WCHAR *Item, const BOOL Default)
{
	if (!NullCheck(Section, Item)) return Default;
	return GetPrivateProfileInt(Section, Item, Default, m_szFilePath);
}

void CIniFile::WriteBool(const WCHAR *Section, const WCHAR *Item, const BOOL Value)
{
	if (!NullCheck(Section, Item)) return;
	if (Value)	WritePrivateProfileString(Section, Item, L"1", m_szFilePath);
	else		WritePrivateProfileString(Section, Item, L"0", m_szFilePath);
}


int CIniFile::ReadInt(const WCHAR *Section,const WCHAR *Item, const int Default)
{
	if (!NullCheck(Section, Item)) return Default;
	return GetPrivateProfileInt(Section, Item, Default, m_szFilePath);
}

void CIniFile::WriteInt(const WCHAR *Section, const WCHAR *Item, const int Value)
{
	if (!NullCheck(Section, Item)) return;
	WCHAR szValue[MAX_KETA];
	_itow(Value, szValue, 10);
	WritePrivateProfileString(Section, Item, szValue, m_szFilePath);
}


float CIniFile::ReadFloat(const WCHAR *Section,const WCHAR *Item, const float Default)
{
	if (!NullCheck(Section, Item)) return Default;
	WCHAR szValue[MAX_KETA], szRet[MAX_KETA];
	swprintf_s(szValue, MAX_KETA, L"%f", Default);
	/*int nLen = */GetPrivateProfileString(Section, Item, szValue, szRet, MAX_KETA-1, m_szFilePath);
	return (float) _wtof(szRet); 
}

void CIniFile::WriteFloat(const WCHAR *Section, const WCHAR *Item, const float Value)
{
	if (!NullCheck(Section, Item)) return;
	WCHAR szValue[MAX_KETA];
	swprintf_s(szValue, MAX_KETA, L"%f", Value);
	WritePrivateProfileString(Section, Item, szValue, m_szFilePath);
}


int CIniFile::ReadString(const WCHAR *Section,const WCHAR *Item, const LPTSTR Default, LPTSTR Value)
{
	if (!NullCheck(Section, Item)) return 0;
	int nLen = GetPrivateProfileString(Section, Item, Default, Value, 255-1, m_szFilePath);
	return nLen;
}

void CIniFile::WriteString(const WCHAR *Section, const WCHAR *Item, const LPTSTR Value)
{
	if (!NullCheck(Section, Item)) return;
	WritePrivateProfileString(Section, Item, Value, m_szFilePath);
}


// Window情報
void CIniFile::ReadWin(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
{
	if (!NullCheck(Section, Item)) return;
	const int c_nMaxItem = 255;
	WCHAR szItem[c_nMaxItem];
	WINDOWPLACEMENT wp;

	wp.length = sizeof(WINDOWPLACEMENT);	
	if (!GetWindowPlacement(hWnd, &wp)) return;
	// flags
	swprintf_s(szItem, c_nMaxItem, L"%s.Flags", Item);
	wp.flags = GetPrivateProfileInt(Section, szItem, wp.flags, m_szFilePath);
	// ShowCmd
	swprintf_s(szItem, c_nMaxItem, L"%s.ShowCmd", Item);
	wp.showCmd = GetPrivateProfileInt(Section, szItem, wp.showCmd, m_szFilePath);
	// ptMaxPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxX", Item);
	wp.ptMaxPosition.x = GetPrivateProfileInt(Section, szItem, wp.ptMaxPosition.x, m_szFilePath);
	// ptMaxPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxY", Item);
	wp.ptMaxPosition.y = GetPrivateProfileInt(Section, szItem, wp.ptMaxPosition.y, m_szFilePath);
	// ptMinPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MinX", Item);
	wp.ptMinPosition.x = GetPrivateProfileInt(Section, szItem, wp.ptMinPosition.x, m_szFilePath);
	// ptMinPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MinY", Item);
	wp.ptMinPosition.y = GetPrivateProfileInt(Section, szItem, wp.ptMinPosition.y, m_szFilePath);
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	wp.rcNormalPosition.left = GetPrivateProfileInt(Section, szItem, wp.rcNormalPosition.left, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	wp.rcNormalPosition.top = GetPrivateProfileInt(Section, szItem, wp.rcNormalPosition.top, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	wp.rcNormalPosition.right = GetPrivateProfileInt(Section, szItem, wp.rcNormalPosition.right, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	wp.rcNormalPosition.bottom = GetPrivateProfileInt(Section, szItem, wp.rcNormalPosition.bottom, m_szFilePath);
	// SetWindowPlacement
	SetWindowPlacement(hWnd, &wp);
}

void CIniFile::WriteWin(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
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
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// ShowCmd
	swprintf_s(szItem, c_nMaxItem, L"%s.ShowCmd", Item);
	_itow(wp.showCmd, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// ptMaxPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxX", Item);
	_itow(wp.ptMaxPosition.x, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// ptMaxPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MaxY", Item);
	_itow(wp.ptMaxPosition.y, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// ptMinPosition.x
	swprintf_s(szItem, c_nMaxItem, L"%s.MinX", Item);
	_itow(wp.ptMinPosition.x, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// ptMinPosition.y
	swprintf_s(szItem, c_nMaxItem, L"%s.MinY", Item);
	_itow(wp.ptMinPosition.y, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	_itow(wp.rcNormalPosition.left, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	_itow(wp.rcNormalPosition.top, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	_itow(wp.rcNormalPosition.right, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	_itow(wp.rcNormalPosition.bottom, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
}


// Window位置/サイズ
void CIniFile::ReadWinRect(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	WCHAR szItem[c_nMaxItem];
	RECT rWin;

	if (!GetWindowRect(hWnd, &rWin)) return;
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	rWin.left = GetPrivateProfileInt(Section, szItem, rWin.left, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	rWin.top = GetPrivateProfileInt(Section, szItem, rWin.top, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	rWin.right = GetPrivateProfileInt(Section, szItem, rWin.right, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	rWin.bottom = GetPrivateProfileInt(Section, szItem, rWin.bottom, m_szFilePath);
	
	SetWindowPos(hWnd, HWND_NOTOPMOST, rWin.left, rWin.top, rWin.right-rWin.left, rWin.bottom-rWin.top, SWP_NOZORDER);
}

void CIniFile::WriteWinRect(const WCHAR *Section,const WCHAR *Item,const HWND hWnd)
{
	const int c_nMaxItem = 255;
	WCHAR szItem[c_nMaxItem];
	WCHAR szValue[MAX_KETA];
	RECT rWin;

	if (!GetWindowRect(hWnd, &rWin)) return;
	// Left
	swprintf_s(szItem, c_nMaxItem, L"%s.Left", Item);
	_itow(rWin.left, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// Top
	swprintf_s(szItem, c_nMaxItem, L"%s.Top", Item);
	_itow(rWin.top, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// Right
	swprintf_s(szItem, c_nMaxItem, L"%s.Right", Item);
	_itow(rWin.right, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
	// Bottom
	swprintf_s(szItem, c_nMaxItem, L"%s.Bottom", Item);
	_itow(rWin.bottom, szValue, 10);
	WritePrivateProfileString(Section, szItem, szValue, m_szFilePath);
}