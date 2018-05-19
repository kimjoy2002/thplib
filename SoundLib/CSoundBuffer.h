#ifndef H_DXSOUND_BUFFER_CLASS___
#define H_DXSOUND_BUFFER_CLASS___
// ===================================================================
// CWaveData.h
//	Waveデータクラス・ヘッダー
// ===================================================================
//#include "define.h"
#include "util.h"

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include "CWaveData.h"
#include "COggDecoder.h"
#include <string>

#define SOUNDLIB_ENDLESSLOOP 0xFFFFFFFF

//-----------------------------------------------------------------------------
// Name: class CSoundBuffer
// Desc: DXSoundBufferラッパークラス
//-----------------------------------------------------------------------------
typedef class CSoundBuffer
{
public:
	// Create
	CSoundBuffer(
		LPDIRECTSOUND8 pDS,
		CWaveData* pWaveData,
		DWORD dwCreateFlags =(DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCDEFER),
		const TCHAR* pResouceName=NULL);

	CSoundBuffer(CSoundBuffer* pSrcSoundBuffer);
	// Duplicate
	CSoundBuffer(LPDIRECTSOUND8 pDS, LPDIRECTSOUNDBUFFER8 pDSBuffer8);

	~CSoundBuffer();

	BOOL Create(CWaveData* pWaveData, DWORD dwCreateFlags);
	BOOL Duplicate(LPDIRECTSOUNDBUFFER8 pDSBuffer8);

	void Release();

//	inline LPDIRECTSOUNDBUFFER	GetDirectSoundBuffer() { return m_pDSBuffer; };
	inline LPDIRECTSOUNDBUFFER8	GetDirectSoundBuffer8() { return m_pDSBuffer8; };
	DWORD GetStatus();
	inline DWORD GetCreateFlags() { return m_dwCreateFlags; }; 

	// 永遠ループ loops = SOUNDLIB_ENDLESSLOOP
	BOOL Play(DWORD loops, int fade = 0);
	void Stop(int fade = 0);
	void SetVolume(BYTE vol);
	BYTE GetVolume();
	DWORD GetRestLoopCount(){ return m_dwLoops;	};
	void SetRestLoopCount(DWORD value){ m_dwLoops = value;	};

	HRESULT SetPosition(DWORD pos);

	inline LPDIRECTSOUND8 GetDirectSound()
	{	return m_pDS;	};
	
	// return:TRUE	= 再生終了
	BOOL Frame();

	void SetResouceName(const TCHAR* name);
	const TCHAR* GetResouceName()
	{	return m_strResouceName.data();	};
	std::basic_string<TCHAR> GetResouceString()
	{	return m_strResouceName;	};

protected:
	LPDIRECTSOUND8 m_pDS;
//	LPDIRECTSOUNDBUFFER m_pDSBuffer;
	LPDIRECTSOUNDBUFFER8 m_pDSBuffer8;
	LONG	m_lVolume;
	LONG	m_lFadeStep;
	LONG	m_lFadeTime;
	
	WORD	m_wLoops;
	DWORD m_dwCreateFlags;
	std::basic_string<TCHAR> m_strResouceName;

private:
	HRESULT Restore( BOOL* pbWasRestored );
	HRESULT FillBuffer(CWaveData* pWaveData);
	
	BOOL SetDSBuffer8(LPDIRECTSOUNDBUFFER pDSB);
	DWORD m_dwLoops;
	BOOL m_bStopFlag;

} *PSoundBuffer, **PPSoundBuffer;


#endif