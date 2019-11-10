#ifndef H_SOUND_PLAYER_CLASS___
#define H_SOUND_PLAYER_CLASS___
// ===================================================================
// CSoundAdministrator.h
//	音管理クラス・ヘッ??
// ===================================================================

#include <windows.h>
#include <stdio.h>
#include <STDLIB.H>
#include "math.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <vector>
#include "DXUTsound.h"

#include <Dxerr.h>

//#include "define.h"
#include "util.h"

#include "CSoundBuffer.h"


//(FPS*SOUND_FFPS)

enum EDirectSoundBufferFormat
{
	DSBufFmt_Default,
	DSBufFmt_High,

};

class CSoundPlayer
{
public:
	CSoundPlayer();
	virtual ~CSoundPlayer();

    inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
	virtual HRESULT Initialize( HWND hWnd, DWORD dwCoopLevel=DSSCL_PRIORITY );
	virtual void Release();

	virtual BOOL IsCreated() { return m_bCreated;	};

	// DirectX help参照
	virtual HRESULT SetPrimaryBufferFormat( EDirectSoundBufferFormat fmt=DSBufFmt_Default );
	/**
		Play 永遠ル?プ：dwLoopCount=SOUNDLIB_ENDLESSLOOP
	*/
	virtual INT PlaySoundBuffer(PSoundBuffer pSoundBuffer,BYTE bytVolume=0xFF , DWORD dwLoopCount=0, int fade=0);

	inline int GetPlayingCount()
	{	return m_mapPlayingList.size();	};

	// 再生中の音の監視
	virtual void Frame();

	// Stop
	virtual BOOL StopSoundBuffer(PSoundBuffer pSoundBuffer, int fade=0);
	virtual BOOL StopSoundBufferFromID(INT id, int fade=0);

	// DefaultVolume
	inline BYTE GetDefaultSoundVolume(){	return m_bytDefaultVolueme;	};
	inline void SetDefaultSoundVolume(BYTE value){	m_bytDefaultVolueme = value;	};
	// Get
	virtual const PSoundBuffer GetSoundBufferFromID(INT id);

	virtual void ClearPlayingList();
protected:

	DWORD				m_dwIDCounter;		// ユニ?クなID番号を得るための数値
										// ※フロ?は考えない
										// ※0番は使用しない
	LPDIRECTSOUND8	m_pDS;
//	CLinkageList*	m_pPlayingList;
	std::map< DWORD, PSoundBuffer > m_mapPlayingList;
	int				m_nFrame;
	BOOL	m_bCreated;
	BYTE	m_bytDefaultVolueme;
//private:



};

#endif