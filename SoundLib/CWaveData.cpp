#include "CWaveData.h"

// 生成
CWaveData::CWaveData()
{
	// フォーマット
	ZeroMemory(&m_tWaveFormat, sizeof(WAVEFORMATEX));
	// 読み込んだデータ
	m_pMemory = NULL;
	// データサイズ
	m_dwDataSize = 0;
}

// 破棄
CWaveData::~CWaveData()
{
	Close();
}

// 読み込んだメモリを破棄
BOOL CWaveData::Close()
{
	SafeDeleteArray(m_pMemory);
	// フォーマット
	ZeroMemory(&m_tWaveFormat, sizeof(WAVEFORMATEX));
	// データサイズ
	m_dwDataSize = 0;		
	
	return TRUE;
}

// ファイルから読み込み
BOOL CWaveData::LoadFromFile(const TCHAR* path)
{
	// 前のメモリを破棄
	Close();

	FILE *fh = NULL;
#ifdef _UNICODE
	errno_t err = _wfopen_s(&fh, path, L"rb");
#else
	errno_t err = fopen_s(&fh, path, "rb");
#endif
	if (err != 0) return FALSE;
//	if (!fh)	return FALSE;

	// サイズ取得
	DWORD fsize = _filelength(_fileno(fh));

	// Open処理
	BOOL ret = Load(fh, fsize);
	// ファイルを閉じる
	fclose(fh);

	return ret;
}


// 開いているファイルポインタから読み込み
BOOL CWaveData::Load(FILE* fh, DWORD size)
{
	// 前のメモリを破棄
	Close();

	if (!fh || !size)
		return FALSE;

	//> サイズの検証
	// 現在位置
	long fcur = ftell(fh);
	fseek(fh, 0, SEEK_END);
	// 最終位置
	long fsize = _filelength(_fileno(fh));

	// 元の位置に戻す
	fseek(fh, fcur, SEEK_SET);

	// 引数のサイズがファイルサイズより大きい場合
	if ((long)size > (fsize-fcur))
		return DXTRACE_ERR( TEXT("size > (fsize-fcur)"), FALSE );

	// メモリ作成
	BYTE *pmem = new BYTE[size];
	if (!pmem)
		return FALSE;

	// ファイルリード
	DWORD readsize = fread(pmem, 1, size, fh);

	// 元の位置に戻す
	fseek(fh, fcur, SEEK_SET);
	// 読み込み失敗
	if (readsize < size)
	{
		SafeDeleteArray(pmem);
		return FALSE;
	}

	// メモリから読み込み
	BOOL ret = LoadFromMemory(pmem, size);

	SafeDeleteArray(pmem);
	return ret;
}


// メモリから読み込み
// pbData		- メモリ上のWAVEファイル
// dwDataSize	- ファイルサイズ
BOOL CWaveData::LoadFromMemory(BYTE *pbData, DWORD dwDataSize)
{
	// 前のメモリを破棄
	Close();

	if (!pbData)
		return FALSE;
	if (!dwDataSize)
		return FALSE;

	TWAVEHEADER WaveHeader;

	// ヘッダコピー
	SafeMemCopy(&WaveHeader, pbData, sizeof(TWAVEHEADER), sizeof(TWAVEHEADER));
	m_tWaveFormat.cbSize = (WORD)WaveHeader.dwFormatBytes;
	m_tWaveFormat.nChannels = WaveHeader.wChannels;
	m_tWaveFormat.wFormatTag = WaveHeader.wFormatTag;
	m_tWaveFormat.nBlockAlign = WaveHeader.wBlockAlign;
	m_tWaveFormat.nAvgBytesPerSec = WaveHeader.nAvgBytesPerSec;
	m_tWaveFormat.nSamplesPerSec = WaveHeader.dwSamplesPerSec;
	m_tWaveFormat.wBitsPerSample = WaveHeader.wBitsPerSample;
	
	if (m_tWaveFormat.wFormatTag != WAVE_FORMAT_PCM)
		return FALSE;

	if (dwDataSize < (WaveHeader.dwFileSize+8) )
		return DXTRACE_ERR( TEXT("wave file dwDataSize Over"), FALSE );
	
//	DWORD dwDATAChunk = 0x61746164;
	int nDataOffset = ((int)WaveHeader.dwFormatBytes) + 20;
	char* pOffset = (char*)&pbData[nDataOffset];
	DWORD dwTemp = 0;

	// "data"を探す
	while ( strncmp(pOffset, "data", 4) != 0)
	{
		// サイズチェック
		if (nDataOffset+8 >= (int)dwDataSize)
			return FALSE;
		// Offset
		SafeMemCopy(&dwTemp, &pOffset[4], 4, 4);
		pOffset += (8+dwTemp);
		nDataOffset += (8+dwTemp);
	}
	nDataOffset += 8;
	SafeMemCopy(&m_dwDataSize, &pOffset[4], 4, 4);

	m_pMemory = new BYTE[m_dwDataSize];
	if (!m_pMemory)
		return FALSE;

	// コピー
	if (!SafeMemCopy(m_pMemory, pbData+nDataOffset, m_dwDataSize, m_dwDataSize))
		return FALSE;

	return TRUE;
}


// メモリコピー
BOOL CWaveData::MemoryCopy(DWORD dwSrcOffset, void* pDest, DWORD *dwMaxSize)
{
	if (!pDest || !m_pMemory || !dwMaxSize)
		return FALSE;

	if (m_dwDataSize < dwSrcOffset)
		return FALSE;
	if (*dwMaxSize > (m_dwDataSize - dwSrcOffset))
		*dwMaxSize = (m_dwDataSize - dwSrcOffset);

	SafeMemCopy(pDest, &m_pMemory[dwSrcOffset], *dwMaxSize, *dwMaxSize);
	return (pDest != NULL);
}