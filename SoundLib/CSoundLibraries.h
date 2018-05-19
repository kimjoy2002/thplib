#ifndef H_SOUND_BUFFER_LIBRARIES_CLASS___
#define H_SOUND_BUFFER_LIBRARIES_CLASS___
// ===================================================================
// CSoundLibraries.h
//	CWaveDataをロードしてCSoundBufferクラスの大元を持っておくクラス・ヘッダー
//	CSoundPlayerはCSoundBufferをここからDuplicateさせて音を鳴らす
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

	// バッファを取得
	// IDを検索してDepulicateして取得
	CSoundBuffer* GetDuplicatedFromID(int id);
	// 一致するリソース名を探してDepulicateして取得
	CSoundBuffer* GetDuplicatedFromResouceName(const TCHAR* pResouceName);

	// 停止、音量などバッファを操作するため
	CSoundBuffer* GetBuffer(int id)
	{
		std::map<int, CSoundBuffer* >::iterator itfind = m_mapPSoundBuffer.find(id);
		if (itfind != m_mapPSoundBuffer.end())
			return (*itfind).second;
		else						return NULL;
	};

	CSoundBuffer* GetBufferFromResouceName(const TCHAR* pResouceName);


	// Soundを追加
	// ファイルから読み込み
	int AddFromFile(const TCHAR* path, DWORD dwCreateFlags=CSLIB_DEFAULT);

	// 開いているファイルポインタから読み込み
	int AddFromFileHeader(FILE* fh, DWORD size, const TCHAR* res_name=NULL, DWORD dwCreateFlags=CSLIB_DEFAULT);

	// メモリから読み込み
	int AddFromMemory(BYTE *pbData, DWORD dwDataSize, const TCHAR* res_name=NULL, DWORD dwCreateFlags=CSLIB_DEFAULT);

	// ライブラリに追加せずにサウンドバッファ作成
	BOOL CreateFromMemory(CSoundBuffer** ppSoundBuffer, BYTE *pbData, DWORD dwDataSize, DWORD dwCreateFlags);

	BOOL Remove(int id);

	BOOL Clear(void);					// リストのクリア
protected:
	LPDIRECTSOUND8	m_pDS;
	// ユニークID, バッファ
	std::map<int, CSoundBuffer* >	m_mapPSoundBuffer;	// バッファマップ
	int	m_nIDCounter;
	BOOL AddNewSound(CWaveData *pNewData, DWORD dwCreateFlags, const TCHAR* pResouceName=NULL);
	// 一致するリソース名を探してイテレーター取得
	std::map<int, CSoundBuffer* >::iterator GetIteratorFromResouceName(const TCHAR* pResouceName);
private:


};

#endif