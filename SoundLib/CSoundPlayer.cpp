// =====================================================================
// サウンド再生クラス
// =====================================================================

#include "CSoundPlayer.h"

CSoundPlayer::CSoundPlayer()
{
	m_bCreated = FALSE;
    m_pDS = NULL;
	m_dwIDCounter = 0;
	m_bytDefaultVolueme = 100;
}

CSoundPlayer::~CSoundPlayer()
{
	Release();
}

void CSoundPlayer::Release()
{
	ClearPlayingList();
	SafeRelease(m_pDS);
	m_bCreated = FALSE;
}

void CSoundPlayer::ClearPlayingList()
{
	for (std::map< DWORD, PSoundBuffer >::iterator it = m_mapPlayingList.begin();
		it != m_mapPlayingList.end();
		it++)
	{
		(*it).second->Stop();
		SafeDelete( (*it).second );
//		it = m_mapPlayingList.erase(it);
	}
	m_mapPlayingList.clear();
	m_dwIDCounter = 0;
}

//-----------------------------------------------------------------------------
// Name: CSoundPlayer::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//		 DSSCL_EXCLUSIVE		: 入力フォーカスがあるときだけ鳴る
//		 DSSCL_NORMAL			: 標準
//		 DSSCL_PRIORITY			: 優先
//-----------------------------------------------------------------------------
HRESULT CSoundPlayer::Initialize(HWND hWnd, DWORD dwCoopLevel)
{
    HRESULT             hr;

    SafeRelease( m_pDS );

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
        return DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
        return DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );   

	ClearPlayingList();

	m_bCreated = TRUE;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CSoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       !WARNING! - Setting the primary buffer format and then using this
//                   same dsound object for DirectMusic messes up DirectMusic! 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   default	Channels = 2
//							Freq     = 22050, 
//							BitRate  = 16
//-----------------------------------------------------------------------------
HRESULT CSoundPlayer::SetPrimaryBufferFormat( EDirectSoundBufferFormat fmt )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	DWORD				dwCh	= 2;
	DWORD				dwFreq	= 22050;
	DWORD				dwRate	= 16;
/*
	DWORD				dwCh	= 2;
	DWORD				dwFreq	= 44100;
	DWORD				dwRate	= 16;
*/

	switch (fmt)
	{
	case DSBufFmt_High:
						dwCh	= 2;
						dwFreq	= 44100;
						dwRate	= 16;
						break;
	case DSBufFmt_Default:
	default:
		break;
	}

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwCh; 
    wfx.nSamplesPerSec  = (DWORD) dwFreq; 
    wfx.wBitsPerSample  = (WORD) dwRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR( TEXT("SetFormat"), hr );

    SafeRelease( pDSBPrimary );

    return S_OK;

}

// PlaySoundBuffer
// 戻り値：バッファID
INT CSoundPlayer::PlaySoundBuffer(PSoundBuffer pSoundBuffer, BYTE bytVolume, DWORD dwLoopCount, int fade)
{
	if (!pSoundBuffer)	return 0;

	// カウンタ増加
	if (m_mapPlayingList.empty())
		m_dwIDCounter = 0;
	else
		m_dwIDCounter++;
	// 重複IDチェック
	while (m_mapPlayingList.find(m_dwIDCounter) != m_mapPlayingList.end())
		m_dwIDCounter++;

	m_mapPlayingList.insert( std::map<DWORD, PSoundBuffer>::value_type(m_dwIDCounter,pSoundBuffer) );

	if (bytVolume == 0xFF)
		pSoundBuffer->SetVolume(m_bytDefaultVolueme);
	else
		pSoundBuffer->SetVolume(bytVolume);

	if (!pSoundBuffer->Play(dwLoopCount, fade))
		return 0;

	return m_dwIDCounter;
}

// 再生中のリストから停止(引数のpSoundBuffer自体で直接stopしても大丈夫)
BOOL CSoundPlayer::StopSoundBuffer(PSoundBuffer pSoundBuffer, int fade)
{
	if (!pSoundBuffer) return FALSE;

	for (std::map<DWORD, PSoundBuffer>::iterator it = m_mapPlayingList.begin();
		it != m_mapPlayingList.end();
		it++)
	{
		if (pSoundBuffer == (*it).second)
		{
			pSoundBuffer->Stop(fade);
			return TRUE;
		}
	}
	return FALSE;
}

// ID番号からストップ
BOOL CSoundPlayer::StopSoundBufferFromID(INT id, int fade)
{
	std::map<DWORD, PSoundBuffer>::iterator it = m_mapPlayingList.find(id);
	if (it != m_mapPlayingList.end())
	{
		(*it).second->Stop(fade);
		return TRUE;
	}
	return FALSE;
}

// ID番号からサウンドバッファ取得
const PSoundBuffer CSoundPlayer::GetSoundBufferFromID(INT id)
{
	std::map<DWORD, PSoundBuffer>::iterator it = m_mapPlayingList.find(id);
	if (it != m_mapPlayingList.end())
	{
		return (*it).second;
	}
	return NULL;
}


// 1フレームごとに呼び出される
void CSoundPlayer::Frame()
{
#if (SOUND_FFPS>1)
	m_nFrame = (++m_nFrame)%SOUND_FFPS;
	if (m_nFrame)
		return;
#endif

	if (m_mapPlayingList.empty())
		return;

	CSoundBuffer	*pSoundBuffer = NULL;
	DWORD dwStatus = 0;

	for (std::map<DWORD, PSoundBuffer>::iterator it = m_mapPlayingList.begin();
		it != m_mapPlayingList.end();
		)
	{
		pSoundBuffer = (*it).second;
		dwStatus = pSoundBuffer->GetStatus();
		// 再生終了 = TRUE
		if (pSoundBuffer->Frame())
		{
			SafeDelete(pSoundBuffer);
			it = m_mapPlayingList.erase(it);
		}
		else
			it++;
	}
}
