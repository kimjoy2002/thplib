#ifndef H_SOUND_BUFFER_LIBRARIES_CLASS___
#define H_SOUND_BUFFER_LIBRARIES_CLASS___
// ===================================================================
// CSoundLibraries.h
//	CWaveData�����[�h����CSoundBuffer�N���X�̑匳�������Ă����N���X�E�w�b�_�[
//	CSoundPlayer��CSoundBuffer����������Duplicate�����ĉ���炷
// ===================================================================

//#include "define.h"
#include "util.h"

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include "CSoundBuffer.h"
#include "CWaveData.h"
//#include "COggDecoder.h"
#include "CWuVorbisDecoder.h"
#include <vector>

//dwCreateFlags
#define CSLIB_DEFAULT	(DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS)
#define CSLIB_USE_3D	(DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRL3D)

class CSoundLibraries
{
public:
	CSoundLibraries(LPDIRECTSOUND8 pDS);
	virtual ~CSoundLibraries();

	inline int GetCount() { return m_mapPSoundBuffer.size(); };

	void ResetDirectSound(LPDIRECTSOUND8 pDS){	m_pDS = pDS;	};

	// �o�b�t�@���擾
	// ID����������Depulicate���Ď擾
	CSoundBuffer* GetDuplicatedFromID(int id);
	// ��v���郊�\�[�X����T����Depulicate���Ď擾
	CSoundBuffer* GetDuplicatedFromResouceName(const TCHAR* pResouceName);

	// ��~�A���ʂȂǃo�b�t�@�𑀍삷�邽��
	CSoundBuffer* GetBuffer(int id)
	{
		std::map<int, CSoundBuffer* >::iterator itfind = m_mapPSoundBuffer.find(id);
		if (itfind != m_mapPSoundBuffer.end())
			return (*itfind).second;
		else						return NULL;
	};

	CSoundBuffer* GetBufferFromResouceName(const TCHAR* pResouceName);


	// Sound��ǉ�
	// �t�@�C������ǂݍ���
	int AddFromFile(const TCHAR* path, DWORD dwCreateFlags=CSLIB_DEFAULT);

	// �J���Ă���t�@�C���|�C���^����ǂݍ���
	int AddFromFileHeader(FILE* fh, DWORD size, const TCHAR* res_name=NULL, DWORD dwCreateFlags=CSLIB_DEFAULT);

	// ����������ǂݍ���
	int AddFromMemory(BYTE *pbData, DWORD dwDataSize, const TCHAR* res_name=NULL, DWORD dwCreateFlags=CSLIB_DEFAULT);

	// ���C�u�����ɒǉ������ɃT�E���h�o�b�t�@�쐬
	BOOL CreateFromMemory(CSoundBuffer** ppSoundBuffer, BYTE *pbData, DWORD dwDataSize, DWORD dwCreateFlags);

	BOOL Remove(int id);

	BOOL Clear(void);					// ���X�g�̃N���A
protected:
	LPDIRECTSOUND8	m_pDS;
	// ���j�[�NID, �o�b�t�@
	std::map<int, CSoundBuffer* >	m_mapPSoundBuffer;	// �o�b�t�@�}�b�v
	int	m_nIDCounter;
	BOOL AddNewSound(CWaveData *pNewData, DWORD dwCreateFlags, const TCHAR* pResouceName=NULL);
	// ��v���郊�\�[�X����T���ăC�e���[�^�[�擾
	std::map<int, CSoundBuffer* >::iterator GetIteratorFromResouceName(const TCHAR* pResouceName);
private:


};

#endif