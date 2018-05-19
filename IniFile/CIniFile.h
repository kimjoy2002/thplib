#ifndef H_CLASS_INIFILE___
#define H_CLASS_INIFILE___
// ==================================================================
// CIniFile.h
//	CIniFile クラス・ヘッダー
// ==================================================================
#include <windows.h>
#include <winbase.h>
#include <stdio.h>

#include "util.h"

#define MAX_KETA	(32)

// IniFileの取り扱い補助クラス
class CIniFile {
public:
	CIniFile();
	virtual ~CIniFile() { };

	// アクセス
	// BOOL
	/* UNICODEと分けたりする*/
#ifdef _UNICODE
#define ReadBool	ReadBoolW
#define WriteBool	WriteBoolW
#define ReadInt	ReadIntW
#define WriteInt	WriteIntW
#define ReadFloat		ReadFloatW
#define WriteFloat		WriteFloatW
#define ReadString	ReadStringW
#define WriteString	WriteStringW
#define ReadWin		ReadWinW
#define WriteWin		WriteWinW
#define ReadWinRect	ReadWinRectW
#define WriteWinRect	WriteWinRectW
#define SetFilePath	SetFilePathW
#else
#define ReadBool	ReadBoolA
#define WriteBool	WriteBoolA
#define ReadInt	ReadIntA
#define WriteInt	WriteIntA
#define ReadFloat		ReadFloatA
#define WriteFloat		WriteFloatA
#define ReadString	ReadStringA
#define WriteString	WriteStringA
#define ReadWin		ReadWinA
#define WriteWin		WriteWinA
#define ReadWinRect	ReadWinRectA
#define WriteWinRect	WriteWinRectA
#define SetFilePath	SetFilePathA
#endif
#ifdef _UNICODE
	BOOL ReadBoolW(const WCHAR *Section,const WCHAR *Item, const BOOL Default);
#else
	BOOL ReadBoolA(const char *Section,const char *Item, const BOOL Default);
#endif

#ifdef _UNICODE
	void WriteBoolW(const WCHAR *Section, const WCHAR *Item, const BOOL Value);
#else
	void WriteBoolA(const char *Section, const char *Item, const BOOL Value);
#endif
	// int
#ifdef _UNICODE
	int ReadIntW(const WCHAR *Section,const WCHAR *Item, const int Default);
#else
	int ReadIntA(const char *Section,const char *Item, const int Default);
#endif

#ifdef _UNICODE
	void WriteIntW(const WCHAR *Section, const WCHAR *Item, const int Value);
#else
	void WriteIntA(const char *Section, const char *Item, const int Value);
#endif

	// float
#ifdef _UNICODE
	float ReadFloatW(const WCHAR *Section,const WCHAR *Item, const float Default);
#else
	float ReadFloatA(const char *Section,const char *Item, const float Default);
#endif

#ifdef _UNICODE
	void WriteFloatW(const WCHAR *Section, const WCHAR *Item, const float Value);
#else
	void WriteFloatA(const char *Section, const char *Item, const float Value);
#endif

	// str
#ifdef _UNICODE
	int ReadStringW(const WCHAR *Section,const WCHAR *Item, const WCHAR* Default, WCHAR* Value, int MaxSize=0);
#else
	int ReadStringA(const char *Section,const char *Item, const char* Default, char* Value, int MaxSize=0);
#endif

#ifdef _UNICODE
	void WriteStringW(const WCHAR *Section, const WCHAR *Item, const WCHAR* Value);
#else
	void WriteStringA(const char *Section, const char *Item, const char* Value);
#endif
	// Window情報
#ifdef _UNICODE
	void ReadWinW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd);
#else
	void ReadWinA(const char *Section,const char *Item,const HWND hWnd);
#endif

#ifdef _UNICODE
	void WriteWinW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd);
#else
	void WriteWinA(const char *Section,const char *Item,const HWND hWnd);
#endif

	// Window位置/サイズ
#ifdef _UNICODE
	void ReadWinRectW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd);
#else
	void ReadWinRectA(const char *Section,const char *Item,const HWND hWnd);
#endif

#ifdef _UNICODE
	void WriteWinRectW(const WCHAR *Section,const WCHAR *Item,const HWND hWnd);
#else
	void WriteWinRectA(const char *Section,const char *Item,const HWND hWnd);
#endif

	// ファイルパスセット
#ifdef _UNICODE
	BOOL SetFilePathW(const WCHAR* path);
#else
	BOOL SetFilePathA(const char* path);
#endif
	LPTSTR GetFolderPath(){ return m_szFolderPath;	};
	LPTSTR GetFilePath() { return m_szFilePath;	};
private:
//	char m_szAppName[_MAX_PATH];	// アプリケーション名

	// セクション、アイテムのチェック
#ifdef _UNICODE
	BOOL NullCheckW(const WCHAR *Section, const WCHAR *Item)
	{
		if (!Section || !Item) return FALSE;
		return TRUE;
	}
#else
	BOOL NullCheckA(const char *Section, const char *Item)
	{
		if (!Section || !Item) return FALSE;
		return TRUE;
	}
#endif
	BOOL m_bExisting;				// 存在フラグ

	TCHAR m_szFilePath[_MAX_PATH*2+1];	// ファイルパス
	TCHAR m_szFolderPath[_MAX_DIR*2+1];	// フォルダパス
};

#endif