#ifndef H_OGG_DECORDER_CLASS___
#define H_OGG_DECORDER_CLASS___
// ===================================================================
// COggDecoder.h
//	OGGデコードクラス・ヘッダー
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

	// 自身のポインタ
	static COggDecoder* __cdecl GetThis( void* stream );
	// メモリから読み込み
	static size_t __cdecl ogg_read( void* buffer, size_t size, size_t maxCount, void* stream );
	// メモリ市0区
	static int __cdecl ogg_seek( void* buffer, ogg_int64_t offset, int flag );
	// メモリクローズ
	static int __cdecl ogg_close( void* buffer );
	// メモリ位置
	static long __cdecl ogg_tell( void* buffer );

//	// ファイルから読み込み
	BOOL LoadFromFile(const TCHAR* path);

//	// 開いているファイルポインタから読み込み
	BOOL Load(FILE* fh, DWORD size);

	// ↓失敗する
	// メモリから読み込み
	BOOL LoadFromMemory(BYTE *pbData, DWORD dwDataSize);

	// 読み込んだメモリを破棄
	BOOL Close();
	
	// メモリコピー
	BOOL MemoryCopy(DWORD dwSrcOffset, void* pDest, DWORD *dwMaxSize);
protected:
//	BYTE*	m_pMemory;				// 読み込んだデータ
//	DWORD	m_dwDataSize;			// データサイズ
//	WAVEFORMATEX m_tWaveFormat;		// フォーマット

	BOOL MakeWaveFormat(WAVEFORMATEX *wave_fmt, vorbis_info* vi);

	DWORD	m_dwCursor;
};

/* 親
//-----------------------------------------------------------------------------
// Name: class CWaveData
// Desc: WAVファイルを読み込み、メモリを保持しておくクラス
//-----------------------------------------------------------------------------
class CWaveData
{
public:
	CWaveData();
	~CWaveData();

	// ファイルから読み込み
	BOOL FileOpen(const char* path);

	// 開いているファイルポインタから読み込み
	BOOL Open(FILE* fh, DWORD size);

	// メモリから読み込み
	BOOL OpenFromMemory(BYTE *pbData, DWORD dwDataSize);

	// メモリからコピー
	BOOL OpenFromMemoryCopy(BYTE *pbData, DWORD dwDataSize);

	// 読み込んだメモリを破棄
	BOOL Close();



protected:
	WAVEFORMATEX m_tWaveFormat;		// フォーマット
	BYTE*	m_pMemory;				// 読み込んだデータ
	DWORD	m_dwDataSize;			// データサイズ

};
*/

#endif