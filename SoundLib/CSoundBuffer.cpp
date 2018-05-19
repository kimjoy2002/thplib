#include "CSoundBuffer.h"

// New & Create
CSoundBuffer::CSoundBuffer(LPDIRECTSOUND8 pDS, CWaveData* pWaveData, DWORD dwCreateFlags, const TCHAR* pResouceName)
{
	m_pDS = pDS;
//	m_pDSBuffer = NULL;
	m_pDSBuffer8 = NULL;
	m_dwCreateFlags = 0;
	m_lFadeStep = 0;
	m_lVolume = 0;
	m_dwLoops = 0;
	m_lFadeTime = 0;
	m_bStopFlag = FALSE;

	if (pResouceName)
		SetResouceName(pResouceName);
	Create(pWaveData, dwCreateFlags);
}

// New Copy & Duplicate
CSoundBuffer::CSoundBuffer(CSoundBuffer* pSrcSoundBuffer)
{
	m_pDS = pSrcSoundBuffer->GetDirectSound();
	m_pDSBuffer8 = NULL;
	m_lFadeStep = 0;
	m_lVolume = 0;
	m_dwLoops = 0;
	m_lFadeTime = 0;
	m_bStopFlag = FALSE;

	SetResouceName(pSrcSoundBuffer->GetResouceName());

	Duplicate(pSrcSoundBuffer->GetDirectSoundBuffer8());
}

// Delete & Release
CSoundBuffer::~CSoundBuffer()
{
	Release();
}

// Release
void CSoundBuffer::Release()
{
	m_dwCreateFlags = 0;

//	SafeRelease(m_pDSBuffer);
	SafeRelease(m_pDSBuffer8);
	m_pDS = NULL;
}

// Create Sound Buffer
BOOL CSoundBuffer::Create(CWaveData* pWaveData,
						  DWORD dwCreateFlags =(DSBCAPS_GLOBALFOCUS
											  | DSBCAPS_GETCURRENTPOSITION2
											  | DSBCAPS_LOCDEFER)
						 )
{
	DSBUFFERDESC dsbd;

	// 引数チェック
	if (!pWaveData)		return FALSE;
	// 作成済みなら破棄しておく
	if (m_pDSBuffer8)
		Release();

	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize          = sizeof(DSBUFFERDESC);
	dsbd.dwFlags         = dwCreateFlags; //DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCDEFER;
	dsbd.dwBufferBytes   = pWaveData->GetDataSize();
	dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;
	dsbd.lpwfxFormat     = pWaveData->GetFormat();

	// フラグを記憶
	m_dwCreateFlags = dsbd.dwFlags;

	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	// CreateSoundBuffer
	if( FAILED( m_pDS->CreateSoundBuffer( &dsbd, &pDSBuffer, NULL ) ) )
		return DXTRACE_ERR( TEXT("CreateSoundBuffer"), FALSE );

	BOOL ret = SetDSBuffer8(pDSBuffer);
	SafeRelease(pDSBuffer);

	if ( FAILED( FillBuffer(pWaveData) ) )
	{
		SafeRelease(pDSBuffer);
		m_pDSBuffer8 = NULL;
		return DXTRACE_ERR( TEXT("FillBuffer"), FALSE );
	}

	return ret;
}

// Duplicate Sound Buffer
BOOL CSoundBuffer::Duplicate(LPDIRECTSOUNDBUFFER8 pDSBuffer8)
{
	// 引数チェック
	if (!pDSBuffer8)		return FALSE;
	// 作成済みなら破棄しておく
	if (m_pDSBuffer8)	Release();
	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	// new
	if( FAILED( m_pDS->DuplicateSoundBuffer( pDSBuffer8, &pDSBuffer) ) )
		return DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), FALSE );

	BOOL ret = SetDSBuffer8(pDSBuffer);
	SafeRelease(pDSBuffer);

	return ret;
}


//-----------------------------------------------------------------------------
// Name: CSoundBuffer::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also NULL if the information is not needed.
//-----------------------------------------------------------------------------
HRESULT CSoundBuffer::Restore( BOOL* pbWasRestored )
{
	HRESULT hr;

	if ( !m_pDSBuffer8 )
		return FALSE;

	if( pbWasRestored )
		*pbWasRestored = FALSE;

	DWORD dwStatus;
	if( FAILED( hr = m_pDSBuffer8->GetStatus( &dwStatus ) ) )
		return DXTRACE_ERR( TEXT("GetStatus"), hr );

	if( dwStatus & DSBSTATUS_BUFFERLOST )
	{
		// Since the app could have just been activated, then
		// DirectSound may not be giving us control yet, so 
		// the restoring the buffer may fail.  
		// If it does, sleep until DirectSound gives us control.
		do 
		{
			hr = m_pDSBuffer8->Restore();
			if( hr == DSERR_BUFFERLOST )
				Sleep( 1 );
		}
		while( ( hr = m_pDSBuffer8->Restore() ) == DSERR_BUFFERLOST );

		if( pbWasRestored != NULL )
			*pbWasRestored = TRUE;

		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
	return S_OK;
}

// create時にバッファを埋める
HRESULT CSoundBuffer::FillBuffer(CWaveData* pWaveData)
{
	HRESULT	hr;
	VOID*	pDSLockedPtr1	= NULL;
	DWORD	dwDSLockedSize1	= 0;
	VOID*	pDSLockedPtr2	= NULL;
	DWORD	dwDSLockedSize2	= 0;
/*
	// Make sure we have focus, and we didn't just switch in from
	// an app which had a DirectSound device
	if( FAILED( Restore( NULL ) ) ) 
		return DXTRACE_ERR( TEXT("RestoreBuffer"), S_FAIL );
*/
    // Lock the buffer down
    hr = m_pDSBuffer8->Lock( 0, pWaveData->GetDataSize(), 
                                 &pDSLockedPtr1, &dwDSLockedSize1, 
                                 &pDSLockedPtr2, &dwDSLockedSize2, 0L );
	if (hr  == DSERR_BUFFERLOST)
	{
		if( FAILED( hr = m_pDSBuffer8->Restore() ) )
			return DXTRACE_ERR( TEXT("Lock and Restore"), hr );
	}
	
	DWORD dwCopySize = dwDSLockedSize1;
	pWaveData->MemoryCopy(0, pDSLockedPtr1, &dwCopySize);
	// 検証
	if (dwCopySize != dwDSLockedSize1)
		return DXTRACE_ERR( TEXT("pWaveData->MemoryCopy 1"), hr );

	// ring buffer
	if (dwDSLockedSize2 != 0 )
	{
		dwCopySize = dwDSLockedSize2;
		pWaveData->MemoryCopy(dwDSLockedSize1, pDSLockedPtr2, &dwCopySize);
		// 検証
		if (dwCopySize != dwDSLockedSize1)
			return DXTRACE_ERR( TEXT("pWaveData->MemoryCopy 2"), hr );
	}

    // Unlock the buffer, we don't need it anymore.
    m_pDSBuffer8->Unlock( pDSLockedPtr1, dwDSLockedSize1, pDSLockedPtr2, dwDSLockedSize2 );

	return S_OK;
}


DWORD CSoundBuffer::GetStatus()
{
	DWORD ret = 0;
	if (!m_pDSBuffer8)
		return 0;
	if (FAILED(m_pDSBuffer8->GetStatus(&ret)))
		return 0;

	return ret;
}

BOOL CSoundBuffer::SetDSBuffer8(LPDIRECTSOUNDBUFFER pDSB)
{
	if (pDSB)
	{
		if (FAILED(pDSB->QueryInterface( IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8 )))
		{
			m_pDSBuffer8 = NULL;
			return DXTRACE_ERR( TEXT("SetDSBuffer8::QueryInterface failed"), FALSE );
		}
	}

	if (!m_pDSBuffer8)	return FALSE;

	// Position 0
	if ( FAILED(m_pDSBuffer8->SetCurrentPosition(0)) )
		return DXTRACE_ERR( TEXT("SetDSBuffer8::SetCurrentPosition failed"), FALSE );

	return TRUE;
}

HRESULT CSoundBuffer::SetPosition(DWORD pos)
{
	if (m_pDSBuffer8)
		return m_pDSBuffer8->SetCurrentPosition(0);
	return S_FALSE;
}


BOOL CSoundBuffer::Play(DWORD loops, int fade)
{
	if (!m_pDSBuffer8)
		return FALSE;
	DWORD	dwFlag = 0;
	if (loops == SOUNDLIB_ENDLESSLOOP)
		dwFlag = DSBPLAY_LOOPING;

	m_dwLoops = loops;
	m_lFadeTime = fade;
	m_bStopFlag = FALSE;

	m_lFadeStep = 0;
	if (FAILED(m_pDSBuffer8->GetVolume(&m_lVolume)))
		return FALSE;

	if (fade)
	{
		m_lFadeStep = (m_lVolume - DSBVOLUME_MIN) / fade;
		m_pDSBuffer8->SetVolume(DSBVOLUME_MIN);
	}

	if (FAILED(m_pDSBuffer8->Play(0,0,dwFlag)))
		return FALSE;

	return TRUE;
}

void CSoundBuffer::Stop(int fade)
{
	if (!m_pDSBuffer8)
		return;
	m_bStopFlag = TRUE;
	if (!fade)
	{
//		if (GetStatus() & DSBSTATUS_PLAYING)
			m_pDSBuffer8->Stop();
		return;
	}
	if (FAILED(m_pDSBuffer8->GetVolume(&m_lVolume)))
	{
		if (GetStatus() & DSBSTATUS_PLAYING)
			m_pDSBuffer8->Stop();
		return;
	}
	m_lFadeStep = (DSBVOLUME_MIN - m_lVolume) / fade;
	
	return;
}

// 0～100に補正
void CSoundBuffer::SetVolume(BYTE vol)
{
	if (!m_pDSBuffer8)
		return;

	const LONG lStep = DSBVOLUME_MIN / 100;
//	int nMax = (DSBVOLUME_MAX-DSBVOLUME_MIN);
	if (vol != 100)
		m_pDSBuffer8->SetVolume(lStep*(100-(vol%100)));
	else
		m_pDSBuffer8->SetVolume(DSBVOLUME_MAX);
}

// 0～100に補正
BYTE CSoundBuffer::GetVolume()
{
	LONG ret;
	const LONG lStep = DSBVOLUME_MIN / 100;

	if (!m_pDSBuffer8)
		return 0;

	if (FAILED(m_pDSBuffer8->GetVolume(&ret)))
		return 0;
	
	return (BYTE)(ret / lStep);
}

// 周期処理
BOOL CSoundBuffer::Frame()
{
	DWORD dwStatus = GetStatus();
	// バッファロストは終了
	if (dwStatus & DSBSTATUS_BUFFERLOST)	return TRUE;
	// 再生中でないならループ確認
	if (!(dwStatus & DSBSTATUS_PLAYING))
	{
		// 停止フラグ確認
		if (m_bStopFlag)	return TRUE;

		if (m_dwLoops)
		{
			if (m_dwLoops != SOUNDLIB_ENDLESSLOOP)
				m_dwLoops--;
			Play(m_dwLoops, m_lFadeTime);
			return FALSE;
		}
		else
			return TRUE;
	}

	// フェード処理のみ
	if ((m_lFadeStep==0))
		return FALSE;

	LONG lTemp;
	if (FAILED(m_pDSBuffer8->GetVolume(&lTemp)))
		return FALSE;

	LONG lStep = 0;
#if (SOUND_FFPS>0)
	lStep += m_lFadeStep*SOUND_FFPS;
#else
	lStep += m_lFadeStep;
#endif

	LONG lSetVolume = lStep+lTemp;

	if (lStep>0)									// Fade In
	{
		if (lSetVolume > m_lVolume)
		{
			lSetVolume = m_lVolume;
			m_lFadeStep = 0;
		}
		m_pDSBuffer8->SetVolume(lSetVolume);
	}
	else if (lStep<0)								// Fade In
	{
		if (lSetVolume <= DSBVOLUME_MIN)
		{
			m_lVolume = DSBVOLUME_MIN;
			m_lFadeStep = 0;
			Stop();
		}
		else
		{
			m_pDSBuffer8->SetVolume(lSetVolume);
		}
	}
	else
	{
		m_lFadeStep = 0;
	}

	return FALSE;
}

void CSoundBuffer::SetResouceName(const TCHAR* name)
{
	m_strResouceName.clear();
	if (name)
		m_strResouceName = name;
}
