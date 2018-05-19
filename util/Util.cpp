//#include "Inc.h"
#include "util.h"

/*
	pSource	: 元文字列
	pDist	: NULLで渡す
	return	: 文字サイズ
*/
BOOL ConvertUTF8toSJIS(BYTE* pSource, BYTE* pDist, int* pSize )
{
  *pSize = 0;
 
   //UTF-8からUTF-16へ変換
   const int nSize = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pSource, -1, NULL, 0 );
	
   BYTE* buffUtf16;
   DEBUG_NEW(buffUtf16, BYTE[ nSize * 2 + 2 ], _T("ConvertUTF8toSJIS") );
   ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pSource, -1, (LPWSTR)buffUtf16, nSize );
   std::wstring ws;
  
//   CHECK_DEBUG_MEMORY_LEAK;

   //UTF-16からShift-JISへ変換
   const int nSizeSJis = ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL );
   if( !pDist ){
       *pSize = nSizeSJis;
       DEBUG_DELETE_ARRAY( buffUtf16, _T("ConvertUTF8toSJIS") );
       return TRUE;
   }
 
   BYTE* buffSJis;
   DEBUG_NEW(buffSJis, BYTE[ nSizeSJis * 2 ], _T("ConvertUTF8toSJIS"));
   ZeroMemory( buffSJis, nSizeSJis * 2 );
   ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffSJis, nSizeSJis, NULL, NULL );
 
   *pSize = _tcslen( (TCHAR*)buffSJis );
   memcpy( pDist, buffSJis, *pSize );
 
   DEBUG_DELETE_ARRAY( buffUtf16, _T("ConvertUTF8toSJIS") );
   DEBUG_DELETE_ARRAY( buffSJis, _T("ConvertUTF8toSJIS") );
 
   return TRUE;
}

BOOL ConvertSJIStoUTF8( BYTE* pSource, BYTE* pDist, int* pSize )
{
    *pSize = 0;
 
   //ShiftJISからUTF-16へ変換
   const int nSize = ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)
pSource, -1, NULL, 0 );
 
	BYTE* buffUtf16;
	DEBUG_NEW(buffUtf16, BYTE[ nSize * 2 + 2 ], _T("ConvertSJIStoUTF8"));
	::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pSource, -1, (LPWSTR)
		buffUtf16, nSize );
 
   //UTF-16からShift-JISへ変換
   const int nSizeUtf8 = ::WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)
buffUtf16, -1, NULL, 0, NULL, NULL );
   if( !pDist ){
       *pSize = nSizeUtf8;
		DEBUG_DELETE_ARRAY( buffUtf16, _T("ConvertSJIStoUTF8") );
		return TRUE;
   }
 
	BYTE* buffUtf8;
	DEBUG_NEW(buffUtf8, BYTE[ nSizeUtf8 * 2 ], _T("ConvertSJIStoUTF8") );
	ZeroMemory( buffUtf8, nSizeUtf8 * 2 );
	::WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)
										buffUtf8, nSizeUtf8, NULL, NULL );

	*pSize = _tcslen( (TCHAR*)buffUtf8 );
	memcpy( pDist, buffUtf8, *pSize );

	DEBUG_DELETE_ARRAY( buffUtf16, _T("ConvertSJIStoUTF8") );
	DEBUG_DELETE_ARRAY( buffUtf8, _T("ConvertSJIStoUTF8") );

   return TRUE;
}

int NewSafePrintf(LPTSTR &str_new, size_t MaxBuf, LPCTSTR pszFormat, ...)
{
    va_list argList;
    HRESULT hr;
	UINT nLength = _tcslen( str_new);
	if (nLength > MaxBuf)
	{
		return 0;
	}
	DEBUG_NEW(str_new, TCHAR[nLength], _T("NewSafePrintf") );

    va_start(argList, pszFormat);
	hr = _vsntprintf_s(str_new, MaxBuf, _TRUNCATE, pszFormat, argList);
	va_end(argList);

	nLength = _tcslen( str_new);

	return (int)nLength;
}


// data:NULL無しで、頭1Byteにサイズ情報のあるデータから文字列をコピーする(NULLあり)
int DecodeSizeStrFromRaw(LPWSTR str, BYTE* src_data, int maxstr, int maxsrc)
{
	if (!src_data) return 0;

	int size = (int)src_data[0];

	if (maxstr < size+1)
		return 0;
	if (maxsrc < size)
		return 0;
	
	memcpy((void*)str, &src_data[1], size);
	str[size/sizeof(WCHAR)] = '\0';

	return size;

}

//------------------------------------------------------------------
//	当たり判定
//-------------------------------------------------------------------
int HitJuge(D3DXVECTOR3 Pos1, D3DXVECTOR3 Pos2, int HitLenge)
{
	D3DXVECTOR3 Mem = Pos1 - Pos2;
	float Lenge = (Mem.x * Mem.x) + (Mem.y * Mem.y) + (Mem.z * Mem.z);
	if(Lenge < HitLenge)
		return 1;
	return 0;

}

// IO
CTextFile::CTextFile(LPTSTR fn, LPTSTR rmode)
{
//	errno_t err;
	f = NULL;
	_set_fmode(_O_BINARY);
	
#ifdef _UNICODE
#ifdef _DEBUG
	f = _wfsopen(fn, rmode, _SH_DENYNO);
#else
	f = _wfsopen(fn, rmode, _SH_DENYNO);
#endif
#else
		char* pchar;
		BOOL ret = LPTSTRToPCHAR(&pchar, fn);
		if (ret)
		{
			const char* cpchar = pchar;
			if (*rmode == 'r')
#ifdef _DEBUG
			{
				if (fopen_s( &f, cpchar, "r") != 0)
				{
					MessageBox(NULL, "fopen_s()", "error", MB_OK);
					return;
				}
			}
#else
			{
				if (fopen_s( &f, cpchar, "r") != 0)
					return;
			}
#endif
			else
#ifdef _DEBUG
			{
				if (fopen_s( &f, cpchar, rmode) != 0)
				{
					MessageBox(NULL, "fopen_s()", "error", MB_OK);
					return;
				}
			}
#else
			{
				if (fopen_s( &f, cpchar, rmode) != 0)
					return;
			}
#endif
			SafeDeleteArray(pchar);
		}
#endif

//	if (err != 0)	return;

	if (!f) return;

	fh = _fileno(f);
}

CTextFile::~CTextFile()
{
	if (f) fclose(f);
}

BOOL CTextFile::ReadLine(LPTSTR p)
{
	if (!f || !p) return FALSE;

	LPTSTR buf;
	buf = new TCHAR[MAX_CHAR+1];
	DWORD rpos = ftell(f);

	ZeroMemory(buf, MAX_CHAR+1);

	fread(buf, 1, MAX_CHAR, f);
	// 読むデータが無い
	if (feof(f) && !buf[0])
	{
		fsize =ftell(f);
		p[0] = NULL;
		SafeDeleteArray(buf);
		return FALSE;
	}
	// 改行検索
	buf[MAX_CHAR] = NULL;
	TCHAR *line = _tcschr(buf, 13);
	if (!line)
	{
		if (!feof(f))
		{
			ErrMsg(_T("1行が長すぎます。"));
			p=NULL;
			SafeDeleteArray(buf);
			return FALSE;
		}
		
		fsize = ftell(f);

		_tcscpy_s(p, _tcslen(buf), buf);
		p[_tcslen(buf)] = NULL;
		SafeDeleteArray(buf);
		return FALSE;
	}

	*line = NULL;
	int ln = _tcslen(buf);

	// 改行ポイント+2まで戻る
	fseek(f, rpos+ln+2, SEEK_SET);

	_tcscpy_s(p, ln, buf);
	p[ln] = NULL;

	SafeDeleteArray(buf);
	return TRUE;
}

BOOL CTextFile::ReadLineA(char* p)
{
	if (!f || !p) return FALSE;

	char* buf;
	buf = new char[MAX_CHAR+1];
	DWORD rpos = ftell(f);

	ZeroMemory(buf, MAX_CHAR+1);

	fread(buf, 1, MAX_CHAR, f);
	// 読むデータが無い
	if (feof(f) && !buf[0])
	{
		fsize =ftell(f);
		p[0] = NULL;
		SafeDeleteArray(buf);
		return FALSE;
	}

	// 改行検索
	buf[MAX_CHAR] = NULL;
	char *line = strchr(buf, 13);
	if (!line)
	{
		if (!feof(f))
		{
			ErrMsg(_T("1行が長すぎます。"));
			p=NULL;
			SafeDeleteArray(buf);
			return FALSE;
		}
		
		fsize = ftell(f);
		strcpy_s(p, strlen(buf), buf);
		p[strlen(buf)] = NULL;
		SafeDeleteArray(buf);
		return FALSE;
	}

	*line = NULL;
	int ln = strlen(buf);

	// 改行ポイント+2まで戻る
	fseek(f, rpos+ln+2, SEEK_SET);

//	strcpy_s(p, ln, buf);
	SafePrintfA(p, ln+1, "%s", buf);
	p[ln] = NULL;

	SafeDeleteArray(buf);
	return TRUE;
}

BOOL CTextFile::WriteLine(LPTSTR p)
{
	if (!f || !p) return FALSE;
	TCHAR pd[MAX_CHAR+1];

	int n = SafePrintf(pd, MAX_CHAR, _T("%s\r\n"), p);
	fwrite(pd, sizeof(pd[0]), n, f);
	fflush(f);
	return TRUE;
}

BOOL CTextFile::Write(LPTSTR p, int size)
{
	if (!f || !p) return FALSE;
	fwrite(p, sizeof(TCHAR), size, f);
	return TRUE;
}

//Code ////////////////////////////////////////////
BOOL CCodeTextFile::ReadLine(LPTSTR p, BOOL dec)
{
	if (!f || !p) return FALSE;
	BOOL res = CTextFile::ReadLine(p);
	if (res && dec)
		CodeXOR(p);
	return res;
}

BOOL CCodeTextFile::WriteLine(LPTSTR p, BOOL enc)
{
	if (!f || !p) return FALSE;
	if (enc)
		CodeXOR(p);
	return CTextFile::WriteLine(p);
}

void CCodeTextFile::CodeXOR(LPTSTR &p)
{
	DWORD *pd = (DWORD*)p;
	int n = _tcslen(p);
	n = n / sizeof(DWORD);
	for (int i=0;i<n;i++,pd++)
		*pd ^= (DWORD)0xFFFFFFFF;
	BYTE *pc = (BYTE*)pd;
	while (*pc)
	{
		*pc ^= (BYTE)0xff;
		pc++;
	}
}


// ディレクトリ有無
BOOL IsDirPathA(const char* path)
{
	if (!path) return FALSE;

	struct _stat stt;

	int ret = _stat( path, &stt );

	if (ret == 0)
	{
		if (stt.st_mode & _S_IFDIR)
			return TRUE;
	}
//#if defined(DEBUG) | defined(_DEBUG)
//	return DXTRACE_ERR( _T("DirectoryNotHound"), FALSE );
//#endif
	return FALSE;
}

BOOL IsDirPathW(const WCHAR* path)
{
	if (!path) return FALSE;
	struct _stat stt;

	int ret = _wstat( path, &stt );

	if (ret == 0)
	{
		if (stt.st_mode & _S_IFDIR)
			return TRUE;
	}
	return FALSE;
}

BOOL IsFilePathA(const char* path)
{
	if (!path) return FALSE;
	const DWORD dwFileBit = (_S_IFREG | _S_IREAD | _S_IWRITE);
	struct _stat stt;
	int ret = _stat( path, &stt );

	if (ret == 0)
	{
		if ((stt.st_mode & dwFileBit) == dwFileBit )
			return TRUE;
	}
	return FALSE;
}

BOOL IsFilePathW(const WCHAR* path)
{
	if (!path) return FALSE;
	const DWORD dwFileBit = (_S_IFREG | _S_IREAD | _S_IWRITE);
	struct _stat stt;
	int ret = _wstat( path, &stt );

	if (ret == 0)
	{
		if ((stt.st_mode & dwFileBit) == dwFileBit )
			return TRUE;
	}
	return FALSE;
}

// 文字コード変換、文字のサイズ(Byte)を返す
int ChangeWordCodeW(LPCTSTR wd, UINT* code)
{
	TCHAR *c = (TCHAR*)wd;
	*code = 0;
	int bytes = sizeof(TCHAR)*2;
	if (code)
		*code = (UINT)c;
	return bytes;
}
// 文字コード変換、文字のサイズ(Byte)を返す
int ChangeWordCodeA(const char* wd, UINT* code)
{
	char *c = (char*)wd;
	*code = 0;
	int bytes = sizeof(BYTE);
	if (IsDBCSLeadByte(*c))
	{
		bytes += sizeof(BYTE);	
		if (code)
			*code = (BYTE)c[0]<<8 | (BYTE)c[1];
	}
	else
		*code = c[0];
	return bytes;
}

// 文字数を返す（マルチバイトのため）
int GetWordNumW(LPCTSTR wd)
{
	int num = 0;
	TCHAR *c = (TCHAR*)wd;
	while (*c)
	{
		num++;
		c++;
	}
	return num;
}
// 文字数を返す（マルチバイトのため）
int GetWordNumA(const char* wd)
{
	int num = 0;
	char *c = (char*)wd;
	while (*c)
	{
		num++;
		if (IsDBCSLeadByte(*c))
			c += sizeof(BYTE)*2;
		else
			c++;
	}
	return num;
}

BOOL HexStrToIntW(LPTSTR src, int len, int *dest)
{
	TCHAR *p = src;

	if (!p) return FALSE;
	if (len < 3) return FALSE;

	if (*p != (TCHAR)(*_T("0"))) return FALSE;
	p++;
	if (*p != (TCHAR)(*_T("x"))) return FALSE;
	p++;

	int plen = _tcslen(p)-1;
	if (plen > len)
		plen = len;

	int x = 1;
	const int base = 16;
	int nTotal = 0;

	for (int i=plen;i>=0;i--)
	{
		if (isdigit(p[i]))
		{
			nTotal += (p[i] - (int)_T("0")) * x;
		}
		else
		{
			TCHAR upper = p[i];
			if ( islower(p[i]) )
				upper = _toupper(p[i]);

			if ( upper < (int)(*_T("A")) || upper > (int)(*_T("F")))
				return FALSE;

			nTotal += ((upper - (int)(*_T("A"))) + 10)*x;
		}
		x *= base;
	}
	
	*dest = nTotal;
	return TRUE;
}

BOOL HexStrToIntA(char* src, int len, int *dest)
{
	char *p = src;

	if (!p) return FALSE;
	if (len < 3) return FALSE;

	if (*p != '0') return FALSE;
	p++;
	if (*p != 'x') return FALSE;
	p++;

	int plen = strlen(p)-1;
	if (plen > len)
		plen = len;

	int x = 1;
	const int base = 16;
	int nTotal = 0;

	for (int i=plen;i>=0;i--)
	{
		if (isdigit(p[i]))
		{
			nTotal += ((p[i] - '0') * x);
		}
		else
		{
			TCHAR upper = p[i];
			if ( islower(p[i]) )
				upper = _toupper(p[i]);

			if ( (upper < 'A') || (upper > 'F'))
				return FALSE;

			nTotal += ((upper - 'A"') + 10)*x;
		}
		x *= base;
	}
	
	*dest = nTotal;
	return TRUE;
}

BOOL LPTSTRToPCHARW(char** ppszDestChar, WCHAR* pszSrcTchar)
{
	char* pszChar = NULL;
	int		nLen;

	//charに必要な文字数の取得
	nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,pszSrcTchar,-1,NULL,0,NULL,NULL);
	DEBUG_NEW(pszChar, char[nLen], L"LPTSTRToPCHARW" );
	if(pszChar)
	{
		//変換
		nLen = ::WideCharToMultiByte(CP_THREAD_ACP,0,pszSrcTchar,(int)::wcslen(pszSrcTchar)+1,pszChar,nLen,NULL,NULL);
		if(nLen == 0)
		{
			DEBUG_DELETE_ARRAY( pszChar, _T("LPTSTRToPCHARW") );
			pszChar = NULL;
		}
	}
	if (pszChar)
	{
		*ppszDestChar = pszChar;
		return TRUE;
	}
	return FALSE;
}

BOOL LPTSTRToPCHARA(char** ppszDestChar, char* pszSrcTchar)
{
	char* pszChar = NULL;
	size_t	nLen;

	nLen = ::strlen(pszSrcTchar) + 1;
	DEBUG_NEW(pszChar, char[nLen], _T("LPTSTRToPCHARA") );
	if(pszChar)
		::strcpy_s(pszChar,nLen * sizeof(char), (char*)pszSrcTchar);
	if (pszChar)
	{
		*ppszDestChar = pszChar;
		return TRUE;
	}
	return FALSE;
}

// char* -> WCHAR*  count = 文字数(否バイト数)

int CStringToWString(WCHAR* dest_wstr, char* src_str, int dest_size)
{
	if (dest_size)
	{
		int len = strlen(src_str);
		if (dest_wstr)
		{
			len = MultiByteToWideChar(CP_THREAD_ACP, 0, src_str, len, dest_wstr, dest_size);
			if (len > 0)
			{
//			dest_wstr[len] = NULL;
				return len;
			}
		}
	}
	else
	{
		int len = MultiByteToWideChar(CP_THREAD_ACP, 0, src_str, -1, NULL,0);
		if (dest_wstr)
			MultiByteToWideChar(CP_THREAD_ACP, 0, src_str, strlen(src_str)+1, dest_wstr, len+1);
		return len;
	}

	return -1;
};

void GetModuleDirA(char* path, int nSize)
{
	char exepath[_MAX_PATH*2+1];
	char pDrive[_MAX_DRIVE+1];
	char pDir[_MAX_DIR+1];
	GetModuleFileNameA(NULL, exepath, _MAX_PATH*2);
	_splitpath_s(exepath, pDrive, _MAX_DRIVE, pDir, _MAX_DIR, NULL, 0, NULL, 0);
	int nLen = strlen(pDir);
	if (!(pDir[nLen-1] == L'/' || pDir[nLen-1] == L'\\'))
	{
		pDir[nLen-1] = L'\\';
		pDir[nLen] = NULL;
	}
	_makepath_s(path, nSize, pDrive, pDir, NULL, NULL);
}

void GetModuleDirW(WCHAR* path, int nSize)
{
	WCHAR exepath[_MAX_PATH*2+1];
	WCHAR pDrive[_MAX_DRIVE+1];
	WCHAR pDir[_MAX_DIR+1];
	GetModuleFileNameW(NULL, exepath, _MAX_PATH*2);
	_wsplitpath_s(exepath, pDrive, _MAX_DRIVE, pDir, _MAX_DIR, NULL, 0, NULL, 0);
	int nLen = wcslen(pDir);
	if (!(pDir[nLen-1] == L'/' || pDir[nLen-1] == L'\\'))
	{
		pDir[nLen-1] = L'\\';
		pDir[nLen] = NULL;
	}
	_wmakepath_s(path, nSize, pDrive, pDir, NULL, NULL);
}


BOOL WCHARToMB(char** ppDest, int* pLen, WCHAR* src)
{
	*pLen = 0;
	int nLen = WideCharToMultiByte(CP_THREAD_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	if (!nLen) return FALSE;
	*ppDest = new char[nLen+1];
	char* pDest = *ppDest;
	ZeroMemory(pDest, sizeof(char)*nLen+1);
	if (WideCharToMultiByte(CP_THREAD_ACP, 0, src, -1, pDest, nLen, NULL, NULL) == 0)
	{
		MessageBoxW(NULL, L"WCharToMB convert error", src, MB_OK);
		SafeDeleteArray(pDest);
		return FALSE;
	}
	*pLen = nLen;
	return TRUE;
}
