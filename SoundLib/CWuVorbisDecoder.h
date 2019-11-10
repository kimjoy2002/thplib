#ifndef H_WU_VORBIS_DECORDER_CLASS___
#define H_WU_VORBIS_DECORDER_CLASS___
// ===================================================================
// CWuVorbisDecoder.h
//	OGG�f�R?�h(wuvorbis.dll)�N���X�E�w�b??
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

	/*�߂�l
	  0 : ����
	  1 : DLL ��������Ȃ����ǂݍ��߂Ȃ�
	  2 : DLL �͓ǂݍ��߂����֐��̃C��??�g�Ɏ��s�����A���邢�͓ǂݍ��� DLL ��
		  wuvorbis.dll �ł͂Ȃ�
	  3 : DLL �� wuvorbisfile �̊Ԃŕs��������������
	*/
	static unsigned int WuVorbisInitialize(const TCHAR *dll_file_name_can_be_null=NULL, __int32 *cputype=NULL);
	static int WuVorbisFinalize();

	// ���g��?�C��?
	static CWuVorbisDecoder* __cdecl GetThis( void* stream );
	// ����������ǂݍ���
	static size_t __cdecl cb_read_func( void *ptr, size_t size, size_t nmemb, void *datasource);
	// �������s0��
	static int __cdecl cb_seek_func( void *datasource, wu_ogg_int64_t offset, int whence);
	// �������N��?�Y
	static int __cdecl cb_close_func( void *datasource);
	// �������ʒu
	static long __cdecl cb_tell_func( void *datasource);

//	// �t?�C������ǂݍ�
//	virtual BOOL LoadFromFile(const TCHAR* path);

//	// �J���Ă���t?�C��?�C��?����ǂݍ���
	virtual BOOL Load(FILE* fh, DWORD size);

	// �����s����
	// ����������ǂݍ���
	BOOL LoadFromMemory(BYTE *pbData, DWORD dwDataSize);

	// �ǂݍ��񂾃�������j��
	BOOL Close();
	
protected:
//	BYTE*	m_pMemory;				// �ǂݍ��񂾃f??
//	DWORD	m_dwDataSize;			// �f??�T�C�Y
//	WAVEFORMATEX m_tWaveFormat;		// �t�H??�b�g

	BOOL MakeWaveFormat(WAVEFORMATEX *wave_fmt, wu_vorbis_info* vi);

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