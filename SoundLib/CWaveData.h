#ifndef H_WAVE_DATA_CLASS___
#define H_WAVE_DATA_CLASS___
// ===================================================================
// CWaveData.h
//	Wave�f�[�^�N���X�E�w�b�_�[
// ===================================================================
#include "util.h"

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <dxerr9.h>

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
// Desc: WAV�t�@�C����ǂݍ��݁A��������ێ����Ă����N���X
//-----------------------------------------------------------------------------
class CWaveData
{
public:
	CWaveData();
	virtual ~CWaveData();

	// �t�@�C������ǂݍ���
	virtual BOOL LoadFromFile(const TCHAR* path);

	// �J���Ă���t�@�C���|�C���^����ǂݍ���
	virtual BOOL Load(FILE* fh, DWORD size);

	// ����������ǂݍ���
	virtual BOOL LoadFromMemory(BYTE *pbData, DWORD dwDataSize);

	// �ǂݍ��񂾃�������j��
	virtual BOOL Close();

	WAVEFORMATEX* GetFormat()
	{	return &m_tWaveFormat;	};

	// DataSize
	DWORD	GetDataSize()
	{	return m_dwDataSize;	};

	// �������R�s�[
	virtual BOOL MemoryCopy(DWORD dwSrcOffset, void* pDest, DWORD *dwMaxSize);

protected:
	BYTE*	m_pMemory;				// �ǂݍ��񂾃f�[�^
	DWORD	m_dwDataSize;			// �f�[�^�T�C�Y
	WAVEFORMATEX m_tWaveFormat;	// �t�H�[�}�b�g

};

#endif