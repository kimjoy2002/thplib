#include "CFiler.h"

CFiler::CFiler()
{
	WCHAR pDrive[_MAX_DRIVE];
	WCHAR pDir[_MAX_DIR];
	WCHAR pTemp[_MAX_DIR];

	// exeのあるフォルダをカレントディレクトリにする
	_wsplitpath_s(__wargv[0], pDrive, _MAX_DRIVE, pDir, _MAX_DIR, NULL, NULL, NULL, NULL);
	SafePrintf(pTemp, _MAX_DIR, L"%s%s", pDrive, pDir);
//	DelLastSeparate(pTemp);
	SetCurrentDir(pTemp);
}

CFiler::~CFiler()
{

}

BOOL CFiler::SetCurrentDir(WCHAR* path)
{
	WCHAR pDrive[_MAX_DRIVE];
	WCHAR pDir[_MAX_DIR];
	
	DelLastSeparate(path);
	BOOL ret = FALSE;
	if (ret = IsDirPath(path))
	{
		SetLastSeparate(path);
		_wsplitpath_s(path, pDrive, _MAX_DRIVE, pDir, _MAX_DIR, NULL, NULL, NULL, NULL);
		SafePrintf(m_szCurrentDir, _MAX_DIR, L"%s%s", pDrive, pDir);
	}
	return ret;
}

int CFiler::GetFileListOnDir(WCHAR *path, CLinkageList *in_list)
{
	int ret = 0;
	WCHAR pFindDir[_MAX_DIR];
	WCHAR pFindPath[_MAX_PATH];
	WCHAR *ppath = NULL;
	long hFind = -1L;
	struct _wfinddata_t pFindinfo;
	WCHAR *pFindName = 0;

	// 引数確認
	if (!path || !in_list)
		return FALSE;

	// カレントパス使用か
	switch (path[0])
	{
	case L'.':
		ppath = &path[2];	break;
	case L'/':
	case L'\\':
		ppath = &path[1];	break;
	default:
		ppath = &path[0];	break;
	}

	// 結合
	SafePrintf(pFindDir, _MAX_DIR, L"%s%s", m_szCurrentDir, ppath);
	// スラ追加
	SetLastSeparate(pFindDir);
	SafePrintf(pFindPath, _MAX_DIR, L"%s*.*", pFindDir);
	
	// 一覧を追加
	if( (hFind = _wfindfirst( pFindPath, &pFindinfo ) ) == -1L )
    {
		do
		{
			NewSafePrintf( pFindName, _MAX_PATH, L"%s%s", pFindDir, pFindinfo.name);
			if (pFindName)
			{
				if (in_list->AddLast(pFindName, _tcslen(pFindName)*sizeof(WCHAR)+1 ))
					ret++;
			}
		}
		while (_wfindnext(hFind, &pFindinfo));
		// 検索終了
		_findclose(hFind);
	}

	return ret;
}

void CFiler::SetLastSeparate(WCHAR *path)
{
	int nLen = wcslen(path);
	if (nLen > 0)
	{
		switch (path[nLen-1])
		{
		case L'\\':
		case L'/':
			return;
		default:
			path[nLen] = L'\\';
			path[nLen+1] = NULL;
			break;
		}
	}
}


void CFiler::DelLastSeparate(WCHAR *path)
{
	int nLen = wcslen(path);
	if (nLen > 0)
	{
		switch (path[nLen-1])
		{
		case L'\\':
		case L'/':
			path[nLen-1] = NULL;
			break;
		default:
			return;
		}
	}
}

FILE* CFiler::GetFileHeader(WCHAR *path, size_t *size)
{
	FILE *fres = NULL;
	// 引数チェック
	if (!path || !size)	return fres;

	WCHAR pAllPath[_MAX_PATH];

	SafePrintf(pAllPath, _MAX_PATH, L"%s%s", m_szCurrentDir, path);

	// ファイル有無チェック
	if (!IsFilePath(pAllPath))
		return fres;

	errno_t err;
	err = _wfopen_s(&fres, pAllPath, L"rb");
	if (err > 0)	return fres;
	*size = _filelength( _fileno( fres ) );

	return fres;
}

BOOL CFiler::GetFileMemory(WCHAR *path, BYTE** pbuf, size_t *size)
{
	FILE *fh = NULL;
	// 引数チェック
	if (!path || !size)	return FALSE;

	WCHAR pAllPath[_MAX_PATH];

	SafePrintf(pAllPath, _MAX_PATH, L"%s%s", m_szCurrentDir, path);

	// ファイル有無チェック
	if (!IsFilePath(pAllPath))
		return FALSE;

	errno_t err = _wfopen_s(&fh, pAllPath, L"rb");
	if (!fh)	return FALSE;
	*size = _filelength(_fileno(fh));
	*pbuf = new BYTE[*size];
	if (!pbuf)	return FALSE;
	size_t rsize = fread(*pbuf, 1, *size, fh);
	fclose(fh);

	// 読みこんだサイズチェック
	if (rsize == *size)
		return TRUE;

	SafeDeleteArray(*pbuf);
	return FALSE;
}

BOOL CFiler::VirtualAllocFileMemory(WCHAR *path, BYTE** pbuf, size_t *size, DWORD memtype, DWORD protect)
{
	FILE *fh = NULL;
	// 引数チェック
	if (!path || !size)	return FALSE;

	WCHAR pAllPath[_MAX_PATH];

	SafePrintf(pAllPath, _MAX_PATH, L"%s%s", m_szCurrentDir, path);

	// ファイル有無チェック
	if (!IsFilePath(pAllPath))
		return FALSE;

	errno_t err = _wfopen_s(&fh, pAllPath, L"rb");
	if (!fh)	return FALSE;
	*size = _filelength(_fileno(fh));
	*pbuf = (BYTE*)VirtualAlloc(0, *size, memtype, protect);
	if (!pbuf)	return FALSE;
	size_t rsize = fread(*pbuf, 1, *size, fh);
	fclose(fh);

	// 読みこんだサイズチェック
	if (rsize == *size)
		return TRUE;

	if (pbuf)
		VirtualFree(pbuf, 0, MEM_RELEASE);
	return FALSE;
}
