#ifndef H_FILER_CLASS___
#define H_FILER_CLASS___
// ===================================================================
// CFiler.h
//	データファイルを取ってくるクラス・ヘッダー
// ===================================================================

#include "util.h"

#include <stdio.h>
#include <windows.h>
#include <TCHAR.h>
#include <mmsystem.h>
#include <io.h>
#include <errno.h>
#include "../LinkageList/CLinkageList.h"

class CFiler
{
public:
	CFiler();
	virtual ~CFiler();

	virtual BOOL SetCurrentDir(WCHAR* path);

	// 指定パスのファイル一覧を得る
	// path			:ディレクトリ（カレントから先のパス）
	// in_list		:一覧追加されるもの、クリアせずそのまま追加
	// return		:追加された数
	virtual int GetFileListOnDir(WCHAR *path, CLinkageList *in_list);
	virtual FILE* GetFileHeader(WCHAR *path, size_t *size);
	virtual BOOL GetFileMemory(WCHAR *path, BYTE** pbuf, size_t *size);
	virtual BOOL VirtualAllocFileMemory(WCHAR *path, BYTE** pbuf, size_t *size, DWORD memtype=MEM_COMMIT|MEM_RESERVE, DWORD protect=PAGE_READWRITE);

protected:
	void SetLastSeparate(WCHAR *path);
	void DelLastSeparate(WCHAR *path);
	WCHAR m_szCurrentDir[_MAX_DIR];

private:

};

#endif
