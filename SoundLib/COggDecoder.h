#ifndef H_OGG_DECORDER_CLASS___
#define H_OGG_DECORDER_CLASS___
// ===================================================================
// COggDecoder.h
//	OGGデコ?ドクラス・ヘッ??
// ===================================================================

#include "util.h"
#include "CWaveData.h"
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "vorbis/vorbisfile.h"
#include <dsound.h>

const DWORD c_dwRequestSize = 4096;	// 読み込み単位

class COggDecoder : public CWaveData
{
public:
	COggDecoder();
	~COggDecoder();

	// 自身の?イン?
	static COggDecoder* __cdecl GetThis( void* stream );
	// メモリから読み込み
	static size_t __cdecl ogg_read( void* buffer, size_t size, size_t maxCount, void* stream );
	// メモリ市0区
	static int __cdecl ogg_seek( void* buffer, ogg_int64_t offset, int flag );
	// メモリクロ?ズ
	static int __cdecl ogg_close( void* buffer );
	// メモリ位置
	static long __cdecl ogg_tell( void* buffer );

//	// フ?イルから読み込み
	BOOL LoadFromFile(const TCHAR* path);

//	// 開いているフ?イル?イン?から読み込み
	BOOL Load(FILE* fh, DWORD size);

	// ↓失敗する
	// メモリから読み込み
	BOOL LoadFromMemory(BYTE *pbData, DWORD dwDataSize);

	// 読み込んだメモリを破棄
	BOOL Close();
	
	// メモリコピ?
	BOOL MemoryCopy(DWORD dwSrcOffset, void* pDest, DWORD *dwMaxSize);
protected:
//	BYTE*	m_pMemory;				// 読み込んだデ??
//	DWORD	m_dwDataSize;			// デ??サイズ
//	WAVEFORMATEX m_tWaveFormat;		// フォ??ット

	BOOL MakeWaveFormat(WAVEFORMATEX *wave_fmt, vorbis_info* vi);

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