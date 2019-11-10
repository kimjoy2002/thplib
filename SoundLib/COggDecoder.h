#ifndef H_OGG_DECORDER_CLASS___
#define H_OGG_DECORDER_CLASS___
// ===================================================================
// COggDecoder.h
//	OGG�f�R?�h�N���X�E�w�b??
// ===================================================================

#include "util.h"
#include "CWaveData.h"
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "vorbis/vorbisfile.h"
#include <dsound.h>

const DWORD c_dwRequestSize = 4096;	// �ǂݍ��ݒP��

class COggDecoder : public CWaveData
{
public:
	COggDecoder();
	~COggDecoder();

	// ���g��?�C��?
	static COggDecoder* __cdecl GetThis( void* stream );
	// ����������ǂݍ���
	static size_t __cdecl ogg_read( void* buffer, size_t size, size_t maxCount, void* stream );
	// �������s0��
	static int __cdecl ogg_seek( void* buffer, ogg_int64_t offset, int flag );
	// �������N��?�Y
	static int __cdecl ogg_close( void* buffer );
	// �������ʒu
	static long __cdecl ogg_tell( void* buffer );

//	// �t?�C������ǂݍ���
	BOOL LoadFromFile(const TCHAR* path);

//	// �J���Ă���t?�C��?�C��?����ǂݍ���
	BOOL Load(FILE* fh, DWORD size);

	// �����s����
	// ����������ǂݍ���
	BOOL LoadFromMemory(BYTE *pbData, DWORD dwDataSize);

	// �ǂݍ��񂾃�������j��
	BOOL Close();
	
	// �������R�s?
	BOOL MemoryCopy(DWORD dwSrcOffset, void* pDest, DWORD *dwMaxSize);
protected:
//	BYTE*	m_pMemory;				// �ǂݍ��񂾃f??
//	DWORD	m_dwDataSize;			// �f??�T�C�Y
//	WAVEFORMATEX m_tWaveFormat;		// �t�H??�b�g

	BOOL MakeWaveFormat(WAVEFORMATEX *wave_fmt, vorbis_info* vi);

	DWORD	m_dwCursor;
};

/* �e
//-----------------------------------------------------------------------------
// Name: class CWaveData
// Desc: WAV�t?�C����ǂݍ��݁A��������ێ����Ă����N���X
//-----------------------------------------------------------------------------
class CWaveData
{
public:
	CWaveData();
	~CWaveData();

	// �t?�C������ǂݍ���
	BOOL FileOpen(const char* path);

	// �J���Ă���t?�C��?�C��?����ǂݍ���
	BOOL Open(FILE* fh, DWORD size);

	// ����������ǂݍ���
	BOOL OpenFromMemory(BYTE *pbData, DWORD dwDataSize);

	// ����������R�s?
	BOOL OpenFromMemoryCopy(BYTE *pbData, DWORD dwDataSize);

	// �ǂݍ��񂾃�������j��
	BOOL Close();



protected:
	WAVEFORMATEX m_tWaveFormat;		// �t�H??�b�g
	BYTE*	m_pMemory;				// �ǂݍ��񂾃f??
	DWORD	m_dwDataSize;			// �f??�T�C�Y

};
*/

#endif