#ifndef H_FILER_CLASS___
#define H_FILER_CLASS___
// ===================================================================
// CFiler.h
//	�f�[�^�t�@�C��������Ă���N���X�E�w�b�_�[
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

	// �w��p�X�̃t�@�C���ꗗ�𓾂�
	// path			:�f�B���N�g���i�J�����g�����̃p�X�j
	// in_list		:�ꗗ�ǉ��������́A�N���A�������̂܂ܒǉ�
	// return		:�ǉ����ꂽ��
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
