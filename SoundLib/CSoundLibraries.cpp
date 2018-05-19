#include "CSoundLibraries.h"


CSoundLibraries::CSoundLibraries(LPDIRECTSOUND8 pDS)
{
	m_nIDCounter = 0;
	m_pDS = pDS;
}

CSoundLibraries::~CSoundLibraries()
{
	Clear();
}

// 読み込んだサウンドを破棄
BOOL CSoundLibraries::Clear(void)
{
	BOOL ret = TRUE;
	std::vector<int>* lst = new std::vector<int>();

	for(std::map<int, CSoundBuffer* >::iterator it = m_mapPSoundBuffer.begin();
		it != m_mapPSoundBuffer.end();
		it++
		)
	{
		lst->push_back((*it).first);
	}
	for (std::vector< int >::iterator it = lst->begin();
		it != lst->end();
		it++)
	{
		if (!this->Remove(*it))
			ret = FALSE;
	}
	SafeDelete(lst);
	m_mapPSoundBuffer.clear();
	m_nIDCounter = 0;
	return ret;
}

// バッファを取得
// index: pList[index]をDepulicateして取得
CSoundBuffer* CSoundLibraries::GetDuplicatedFromID(int id)
{
	CSoundBuffer* pDuplicated = NULL;
	std::map<int, CSoundBuffer* >::iterator itfind = m_mapPSoundBuffer.find(id);

	// New Duplicate
	if (itfind != m_mapPSoundBuffer.end())
		pDuplicated = new CSoundBuffer( (*itfind).second );
	if (pDuplicated)
	{
		// DSB8取得失敗するか確認
		if (!pDuplicated->GetDirectSoundBuffer8())
		{
			SafeDelete(pDuplicated);
		}
	}
	return pDuplicated;
}


// 一致するリソース名を探してイテレーター取得
std::map<int, CSoundBuffer* >::iterator CSoundLibraries::GetIteratorFromResouceName(const TCHAR* pResouceName)
{
	CSoundBuffer* pFindBuffer = NULL;
	std::map< int, CSoundBuffer* >::iterator it = m_mapPSoundBuffer.begin();
	for (;
		it != m_mapPSoundBuffer.end();
		it++)
	{
		if ( _tcscmp((*it).second->GetResouceName(), pResouceName) == 0)
		{
			break;
		}
	}
	return it;
}

CSoundBuffer* CSoundLibraries::GetBufferFromResouceName(const TCHAR* pResouceName)
{
	CSoundBuffer* pFindBuffer = NULL;
	std::map<int, CSoundBuffer* >::iterator itfind = GetIteratorFromResouceName(pResouceName);
	if (itfind != m_mapPSoundBuffer.end())
		pFindBuffer = (*itfind).second;
	return pFindBuffer;
}

CSoundBuffer* CSoundLibraries::GetDuplicatedFromResouceName(const TCHAR* pResouceName)
{
	CSoundBuffer* pDuplicated = NULL;
	if (CSoundBuffer* pFind = GetBufferFromResouceName(pResouceName))
		pDuplicated = new CSoundBuffer(pFind);
	if (pDuplicated)
	{
		// DSB8取得失敗するか確認
		if (!pDuplicated->GetDirectSoundBuffer8())
		{
			SafeDelete(pDuplicated);
		}
	}
	
	return pDuplicated;
}

// ファイルから読み込み
int CSoundLibraries::AddFromFile(const TCHAR* path, DWORD dwCreateFlags)
{
	CWaveData* pNewData = NULL;

	// 引数チェック
	// リソース名が同じバッファの場合、追加せずにIDを返す
	if (path)
	{
		std::map<int, CSoundBuffer* >::iterator itfind = GetIteratorFromResouceName(path);
		if (itfind != m_mapPSoundBuffer.end())
			return (*itfind).first;
	}
	else
		return -1;

	TCHAR pPath[_MAX_PATH*2+1];
	GetModuleDir(pPath, _MAX_PATH*2);
	_tcscat_s(pPath, _MAX_PATH*2, path);
//	SafePrintf(pPath, _MAX_PATH*2+1, path);
	if (!IsFilePath(pPath))
		return -1;

	// ファイルタイプを拡張子でチェック
	const TCHAR*	path_ext = &path[_tcslen(path)-4];
#ifdef _UNICODE
	if (wcscmp(L".wav", path_ext) == 0)
		pNewData = new CWaveData();
	else if (wcscmp(L".ogg", path_ext) == 0)
		pNewData = new CWuVorbisDecoder();
//		pNewData = new COggDecoder();
#else
	if (strcmp(".wav", path_ext) == 0)
		pNewData = new CWaveData();
	else if (strcmp(".ogg", path_ext) == 0)
		pNewData = new CWuVorbisDecoder();
//		pNewData = new COggDecoder();
#endif

	if (!pNewData->LoadFromFile(pPath))
	{
		SafeDelete(pNewData);
		return -1;
	}
	
	if (!pNewData)	return -1;							// 該当なし、New失敗

	// 新規を挿入
	return AddNewSound(pNewData, dwCreateFlags, path);
}

// 開いているファイルポインタから読み込み
int CSoundLibraries::AddFromFileHeader(FILE* fh, DWORD size, const TCHAR* res_name, DWORD dwCreateFlags)
{
	CWaveData* pNewData = NULL;
	// 引数チェック
	if (!fh) return -1;
	
	// 現在位置を保持
	long fcur = ftell(fh);
	DWORD dwChk;
	int nRead = fread(&dwChk, 1, sizeof(DWORD), fh);
	if (nRead != 4)
		return -1;

	fseek(fh, fcur, SEEK_SET);					// FILEポインタを元の位置に戻す
	DWORD dwRIFF = 0x46464952;			// "RIFF"
	if (dwChk == dwRIFF)				// wavファイルチェック
		pNewData = new CWaveData();
	else										// oggはそれ以外で、間違ったファイルの
		pNewData = new CWuVorbisDecoder();
//		pNewData = new COggDecoder();			// 場合はデコード処理の方で分かる
		pNewData = new CWuVorbisDecoder();

	if (!pNewData->Load(fh, size))
	{
		SafeDelete(pNewData);
		return -1;
	}

	// 新規を挿入
	return AddNewSound(pNewData, dwCreateFlags, res_name);
}

// メモリから読み込み
int CSoundLibraries::AddFromMemory(BYTE *pbData, DWORD dwDataSize, const TCHAR* res_name, DWORD dwCreateFlags)
{
	CWaveData* pNewData = NULL;

	// 引数チェック
	if (!pbData || !dwDataSize) return -1;

	DWORD dwChk;
	SafeMemCopy(&dwChk, pbData, 4, 4);

	DWORD dwRIFF = 0x46464952;			// "RIFF"
	if (dwChk == dwRIFF)				// wavファイルチェック
		pNewData = new CWaveData();
	else										// oggはそれ以外で、間違ったファイルの
		return DXTRACE_ERR( TEXT("Ogg形式のAddFromMemoryは使えません"), FALSE );
//		pNewData = new COggDecoder();			// 場合はデコード処理の方で分かる

	if (!pNewData->LoadFromMemory(pbData, dwDataSize))
	{
		SafeDelete(pNewData);
		return FALSE;
	}
	// 新規を挿入
	return AddNewSound(pNewData, dwCreateFlags, res_name);
}

int CSoundLibraries::AddNewSound(CWaveData *pNewData, DWORD dwCreateFlags, const TCHAR* pResouceName)
{
	CSoundBuffer *pNewSound = new CSoundBuffer(m_pDS, pNewData, dwCreateFlags, pResouceName);

	// 作成できたか確認
	if (!pNewSound->GetDirectSoundBuffer8())	return -1;

	// バッファを作ったのでデータは削除
	SafeDelete(pNewData);
	
	if (!pNewSound)	return -1;

	if (m_mapPSoundBuffer.empty())
		m_nIDCounter = 0;
	m_nIDCounter++;

	// 重複IDチェック
	while (m_mapPSoundBuffer.find(m_nIDCounter) != m_mapPSoundBuffer.end())
	{
		m_nIDCounter++;
	}

	m_mapPSoundBuffer.insert( std::map<int, CSoundBuffer*>::value_type(m_nIDCounter,pNewSound) );
	return m_nIDCounter;
}

BOOL CSoundLibraries::CreateFromMemory(CSoundBuffer** ppSoundBuffer, BYTE *pbData, DWORD dwDataSize, DWORD dwCreateFlags)
{
	CWaveData* pNewData = NULL;
	// 引数チェック
	if (!pbData || !dwDataSize) return FALSE;
	
	DWORD dwChk;
	SafeMemCopy(&dwChk, pbData, 4, 4);

	DWORD dwRIFF = 0x46464952;			// "RIFF"
	if (dwChk == dwRIFF)				// wavファイルチェック
		pNewData = new CWaveData();
	else										// oggはそれ以外で、間違ったファイルの
		pNewData = new COggDecoder();			// 場合はデコード処理の方で分かる

	if (!pNewData->LoadFromMemory(pbData, dwDataSize))
	{
		SafeDelete(pNewData);
		return FALSE;
	}

	*ppSoundBuffer = new CSoundBuffer(m_pDS, pNewData, dwCreateFlags);
	// バッファを作ったのでデータは削除
	SafeDelete(pNewData);

	return TRUE;
}

BOOL CSoundLibraries::Remove(int id)
{
	std::map<int, CSoundBuffer* >::iterator itfind = m_mapPSoundBuffer.find(id);
	// 検索でIDが見つからなかった
	if ( itfind == m_mapPSoundBuffer.end() )	return FALSE;

	try{
		SafeDelete( (*itfind).second );
		m_mapPSoundBuffer.erase(itfind);
	} catch (char* c)
	{
		return FALSE;
	}

	return TRUE;
}
