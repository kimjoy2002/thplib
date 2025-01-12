#ifndef H_WAVE_DATA_CLASS___
#define H_WAVE_DATA_CLASS___
// ===================================================================
// CWaveData.h
//	Waveデ??クラス・ヘッ??
// ===================================================================
#include "util.h"

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <dxerr.h>

struct TWAVEHEADER
{
	// RIFF header
	char	cRIFFHeader[4];
	DWORD	dwFileSize;

	// data type
	char	cWAVEHeader[4];

	// wave format
	char	cFormatHeader[4];
	DWORD	dwFormatBytes;

	WORD	wFormatTag;
	WORD	wChannels;
	DWORD	dwSamplesPerSec;
	DWORD	nAvgBytesPerSec;
	WORD	wBlockAlign;
	WORD	wBitsPerSample;

	// data chunk header
	char	cDataHeader[4];
	DWORD	nDataBytes;
};

//-----------------------------------------------------------------------------
// Name: class CWaveData
// Desc: WAVフ?イルを読み込み、メモリを保持しておくクラス
//-----------------------------------------------------------------------------
class CWaveData
{
public:
	CWaveData();
	virtual ~CWaveData();

	// フ?イルから読み込み
	virtual BOOL LoadFromFile(const TCHAR* path);

	// 開いているフ?イル?イン?から読み込み
	virtual BOOL Load(FILE* fh, DWORD size);

	// メモリから読み込み
	virtual BOOL LoadFromMemory(BYTE *pbData, DWORD dwDataSize);

	// 読み込んだメモリを破棄
	virtual BOOL Close();

	WAVEFORMATEX* GetFormat()
	{	return &m_tWaveFormat;	};

	// DataSize
	DWORD	GetDataSize()
	{	return m_dwDataSize;	};

	// メモリコピ?
	virtual BOOL MemoryCopy(DWORD dwSrcOffset, void* pDest, DWORD *dwMaxSize);

protected:
	BYTE*	m_pMemory;				// 読み込んだデ??
	DWORD	m_dwDataSize;			// デ??サイズ
	WAVEFORMATEX m_tWaveFormat;	// フォ??ット

};

#endif