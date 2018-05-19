#ifndef _CLASS_ARCIVER_H___
#define _CLASS_ARCIVER_H___

#include <assert.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <TCHAR.h>
#include <process.h>
#include "util.h"
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <io.h>
#include <fcntl.h>

struct TFileInfo
{
	unsigned long index;
	unsigned long size;
	unsigned char* buffer;
	FILE*			f;
};

struct TArcInfo
{
	unsigned long index;
	unsigned long size;
	WCHAR	path[_MAX_PATH+1];
};

enum ELoadPrimary
{
	eLoadPrimaryArc,
	eLoadPrimaryFile,
};
const ELoadPrimary c_eLoadPrimary = eLoadPrimaryArc;

class CArchiver
{
public:
	static CArchiver* Instance();
	static void Create();
	static void Destroy();
	
	void Open(const WCHAR* path);								// �A�[�J�C�u�t�@�C����ǉ�
	BOOL Load(const WCHAR* path);								// �A�[�J�C�u������t�@�C�����[�h�J�n
	BOOL Unload(const WCHAR* path);							// ���[�h���ꂽ�t�@�C����j��
	const TFileInfo* GetFileInfo(const WCHAR* path);		// ���[�h���t�@�C�����̏����擾
	void ClearLoadedBuffer();											// �S�Ă��A�����[�h
	
	BOOL LoadOn(const WCHAR* path, TFileInfo* pFileInfo);	// �����Ƀt�@�C�������[�h

	inline void EnterLoadCriticalSection()		{	EnterCriticalSection(&m_pInstance->m_csLoad);	};
	inline void LeaveLoadCriticalSection()		{	LeaveCriticalSection(&m_pInstance->m_csLoad);	};
	BOOL IsLoading()
	{
		EnterCriticalSection(&m_pInstance->m_csLoading);
		BOOL ret = m_bLoading;
		LeaveCriticalSection(&m_pInstance->m_csLoading);
		return ret;
	};
	BOOL IsFinished()
	{
		EnterCriticalSection(&m_pInstance->m_csLoading);
		BOOL ret = !m_bLoading;
		LeaveCriticalSection(&m_pInstance->m_csLoading);
		return ret;
	};


	static unsigned int __stdcall Loading(void* param)
	{
		if (!param) return 0;
		CArchiver* self = reinterpret_cast< CArchiver* >( param );
		self->Loading();
		return 0;
	}
	void Loading();

	BOOL LoadOnFile(const WCHAR* path, TFileInfo* pfi);	// �t�@�C���̓ǂݍ���
	
private:
	CArchiver();
	CArchiver( const CArchiver& );
	void operator= ( const CArchiver &	);
	virtual ~CArchiver();

	BOOL UpdateIsLoading()
	{
		EnterLoadCriticalSection();
		EnterCriticalSection(&m_pInstance->m_csLoading);
		BOOL ret = !m_qReqLoad.empty();
		m_bLoading = ret;
		LeaveCriticalSection(&m_pInstance->m_csLoading);
		LeaveLoadCriticalSection();
		return ret;
	};

	void LoadBuffer(const WCHAR* path);
	BOOL LoadBufferFromArc(TFileInfo* pfi);
	BOOL LoadBufferFromFile(const WCHAR* path, TFileInfo* pfi);	// �t�@�C���̓ǂݍ���

	BOOL ListArcInfo(const WCHAR* path, FILE* pFile);
	BOOL ListFileInfo(const WCHAR* path);
	BOOL ListDir(const WCHAR* path, int nBaseLen);
	std::map< std::wstring, TFileInfo > m_mapFileInfo;
	std::vector< FILE* >	m_vecFilePtr;
	WCHAR	m_wsBaseDirectory[_MAX_PATH+1];

	HANDLE	m_hMyEvent;
	HANDLE m_hThread; //�X���b�h�n���h��
	CRITICAL_SECTION	m_csLoad;
	CRITICAL_SECTION	m_csLoading;
	BOOL m_bReqThreadEnd;
	BOOL m_bLoading;
	std::queue< const WCHAR* > m_qReqLoad;
	static CArchiver*	m_pInstance;
};

#endif