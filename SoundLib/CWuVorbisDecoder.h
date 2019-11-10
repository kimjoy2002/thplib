#ifndef H_WU_VORBIS_DECORDER_CLASS___
#define H_WU_VORBIS_DECORDER_CLASS___
// ===================================================================
// CWuVorbisDecoder.h
//	OGGデコ?ド(wuvorbis.dll)クラス・ヘッ??
// ===================================================================

#include "util.h"
#include "CWaveData.h"
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "wuvorbisfile.h"
#include <dsound.h>

class CWuVorbisDecoder : public CWaveData
{
public:
	CWuVorbisDecoder();
	~CWuVorbisDecoder();

	/*戻り値
	  0 : 正常
	  1 : DLL が見つからないか読み込めない
	  2 : DLL は読み込めたが関数のイン??トに失敗した、あるいは読み込んだ DLL は
		  wuvorbis.dll ではない
	  3 : DLL と wuvorbisfile の間で不整合が発生した
	*/
	static unsigned int WuVorbisInitialize(const TCHAR *dll_file_name_can_be_null=NULL, __int32 *cputype=NULL);
	static int WuVorbisFinalize();

	// 自身の?イン?
	static CWuVorbisDecoder* __cdecl GetThis( void* stream );
	// メモリから読み込み
	static size_t __cdecl cb_read_func( void *ptr, size_t size, size_t nmemb, void *datasource);
	// メモリ市0区
	static int __cdecl cb_seek_func( void *datasource, wu_ogg_int64_t offset, int whence);
	// メモリクロ?ズ
	static int __cdecl cb_close_func( void *datasource);
	// メモリ位置
	static long __cdecl cb_tell_func( void *datasource);

//	// フ?イルから読み込
//	virtual BOOL LoadFromFile(const TCHAR* path);

//	// 開いているフ?イル?イン?から読み込み
	virtual BOOL Load(FILE* fh, DWORD size);

	// ↓失敗する
	// メモリから読み込み
	BOOL LoadFromMemory(BYTE *pbData, DWORD dwDataSize);

	// 読み込んだメモリを破棄
	BOOL Close();
	
protected:
//	BYTE*	m_pMemory;				// 読み込んだデ??
//	DWORD	m_dwDataSize;			// デ??サイズ
//	WAVEFORMATEX m_tWaveFormat;		// フォ??ット

	BOOL MakeWaveFormat(WAVEFORMATEX *wave_fmt, wu_vorbis_info* vi);

	DWORD	m_dwCursor;
};

/* 親
//-----------------------------------------------------------------------------
// Name: class CWaveData
// Desc: WAVフ?イルを読み込み、メモリを保持しておくクラス
//-----------------------------------------------------------------------------
class CWaveData
{
public:
	CWaveData();
	~CWaveData();

	// フ?イルから読み込み
	BOOL FileOpen(const char* path);

	// 開いているフ?イル?イン?から読み込み
	BOOL Open(FILE* fh, DWORD size);

	// メモリから読み込み
	BOOL OpenFromMemory(BYTE *pbData, DWORD dwDataSize);

	// メモリからコピ?
	BOOL OpenFromMemoryCopy(BYTE *pbData, DWORD dwDataSize);

	// 読み込んだメモリを破棄
	BOOL Close();



protected:
	WAVEFORMATEX m_tWaveFormat;		// フォ??ット
	BYTE*	m_pMemory;				// 読み込んだデ??
	DWORD	m_dwDataSize;			// デ??サイズ

};
*/

#endif