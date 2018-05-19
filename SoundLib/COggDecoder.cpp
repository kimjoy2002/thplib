
#pragma comment(lib, "winmm.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment ( lib, "libogg_static_d.lib" )
#pragma comment ( lib, "libvorbis_static_d.lib" )
#pragma comment ( lib, "libvorbisfile_static_d.lib" )
#else
#pragma comment ( lib, "libogg_static.lib" )
#pragma comment ( lib, "libvorbis_static.lib" )
#pragma comment ( lib, "libvorbisfile_static.lib" )
#endif


#include "COggDecoder.h"


COggDecoder::COggDecoder():CWaveData()
{
	// フォーマット
	ZeroMemory(&m_tWaveFormat, sizeof(WAVEFORMATEX));
	// 読み込んだデータ
	m_pMemory = NULL;
	// データサイズ
	m_dwDataSize = 0;	
	m_dwCursor = 0;
}

COggDecoder::~COggDecoder()
{
    Close();
}
// 自身のポインタ
COggDecoder* COggDecoder::GetThis( void* stream )
{
	COggDecoder *p = NULL;

	SafeMemCopy( &p, stream, sizeof( COggDecoder*), sizeof( COggDecoder*));
	return p;
}


// メモリから読み込み
size_t COggDecoder::ogg_read( void* buffer, size_t size, size_t maxCount, void* stream )
{

	// this
	COggDecoder *pthis = (COggDecoder *)stream;//GetThis( stream );

	// カーソル～最後までのサイズ
	DWORD restSize = pthis->m_dwDataSize - pthis->m_dwCursor;
	// 残りカウント

	DWORD restCount = (restSize / size);
	if (restCount > maxCount)
		restCount = maxCount;

	SafeMemCopy(buffer , &pthis->m_pMemory[pthis->m_dwCursor], size*restCount , size*restCount);

	pthis->m_dwCursor += size*restCount;

	return restCount;
}

// メモリシーク
int COggDecoder::ogg_seek( void* buffer, ogg_int64_t offset, int flag )
{
	// this
	COggDecoder *pthis = (COggDecoder *)buffer;//GetThis( stream );

	switch (flag)
	{
	case SEEK_CUR:
		pthis->m_dwCursor += (DWORD)offset;
		break;
	case SEEK_END:
		pthis->m_dwCursor = pthis->m_dwDataSize-(DWORD)offset;
		break;
	case SEEK_SET:
		pthis->m_dwCursor = (DWORD)offset;
		break;
	default:
		return -1;
	}

	if ( pthis->m_dwCursor > pthis->m_dwDataSize )
	{
		pthis->m_dwCursor = pthis->m_dwDataSize;
		return -1;
	}
	else if ( pthis->m_dwCursor < 0)
	{
		pthis->m_dwCursor = 0;
		return -1;
	}

	return 0;
}

// メモリクローズ
int COggDecoder::ogg_close( void* buffer )
{
	COggDecoder *pthis = (COggDecoder *)buffer;
	SafeDeleteArray(pthis->m_pMemory);

	return 0;
}

// メモリ位置
long COggDecoder::ogg_tell( void* buffer )
{
	COggDecoder *pthis = (COggDecoder *)buffer;//GetThis( stream );
	return pthis->m_dwCursor;
}

// 読み込んだメモリを破棄
BOOL COggDecoder::Close()
{
	m_dwCursor = 0;
	return CWaveData::Close();
}

// ファイルから読み込み
BOOL COggDecoder::LoadFromFile(const TCHAR* path)
{
	// 前のメモリを破棄
	Close();

	FILE *fh = NULL;
#ifdef _UNICODE
	errno_t err = _wfopen_s(&fh, path, L"rb");
#else
	errno_t err = fopen_s(&fh, path, "rb");
#endif

	// サイズ取得
	fseek(fh, 0, SEEK_END);
	DWORD fsize = (DWORD)ftell(fh);
	rewind(fh);
	// Open処理
	BOOL ret = Load(fh, fsize);
	// ファイルを閉じる
	fclose(fh);

	return ret;
}


// 開いているファイルポインタから読み込み
BOOL COggDecoder::Load(FILE* fh, DWORD size)
{
	OggVorbis_File	tOggFile;
	vorbis_info		*vi;
	// 前のメモリを破棄
	Close();

	if (!fh || !size)
		return FALSE;

	//> サイズの検証
	// 現在位置
	long fcur = ftell(fh);
	fseek(fh, 0, SEEK_END);
	// 最終位置
	long fsize = (DWORD)ftell(fh);

	// 保存していた元の位置に戻す
	fseek(fh, fcur, SEEK_SET);

	// 引数のサイズがファイルサイズより大きい場合
	if ((long)size > (fsize-fcur))
		return DXTRACE_ERR( TEXT("size > (fsize-fcur)"), FALSE );

	int ret = ov_open(fh, &tOggFile, NULL, 0);
	vi = ov_info(&tOggFile, -1);

	if (ret < 0)	return 0;

	char dec_data[4096];
	int current_section;
	m_dwDataSize = (DWORD)(vi->channels * 2 * ov_pcm_total(&tOggFile, -1));
	m_pMemory = new BYTE[m_dwDataSize];

	if (!MakeWaveFormat(&m_tWaveFormat, vi))
		return FALSE;
	BYTE *pmem = m_pMemory;
	// デコード
	do
	{
		ret = ov_read(&tOggFile, dec_data, 4096, 0,2,1, &current_section);
		SafeMemCopy(pmem, dec_data, ret, ret);
		pmem += ret;
	} while (ret != 0);

	ret = ov_clear(&tOggFile);
	if (ret == 0)
	{
		// 保存していた元の位置に戻す
		fseek(fh, fcur, SEEK_SET);
	}

	return (ret == 0);
}

// メモリから読み込み
BOOL COggDecoder::LoadFromMemory(BYTE *pbData, DWORD dwDataSize)
{
	vorbis_info		*vi;
	// 前のメモリを破棄
	Close();

	if (!pbData)
		return FALSE;
	if (!dwDataSize)
		return FALSE;

	// メモリ作成 (自身のポインタ+ファイル)
	
//	BYTE *pmem = new BYTE[/*dwDataSize+*/sizeof(COggDecoder)];
/*
	if (!pmem)
		return FALSE;
	ZeroMemory(pmem, sizeof(COggDecoder));
*/
	m_pMemory = new BYTE[dwDataSize];

	m_dwDataSize = dwDataSize;

	// メモリをコピー
	if (!SafeMemCopy((char*)m_pMemory, pbData, dwDataSize, dwDataSize))
		return FALSE;

	OggVorbis_File tOggFile;
	
	// コールバック用変数
	ov_callbacks callbacks;
	callbacks.read_func = COggDecoder::ogg_read;
	callbacks.seek_func = COggDecoder::ogg_seek;
	callbacks.close_func = COggDecoder::ogg_close;
	callbacks.tell_func = COggDecoder::ogg_tell;

	int ret = ov_open_callbacks( this, &tOggFile, 0 ,0, callbacks );
	
	switch (ret)
	{
	case OV_EREAD:		SafeDeleteArray(m_pMemory);	return DXTRACE_ERR( TEXT("OV_EREAD"), FALSE );
	case OV_ENOTVORBIS:	SafeDeleteArray(m_pMemory);	return DXTRACE_ERR( TEXT("OV_ENOTVORBIS"), FALSE );
	case OV_EVERSION:	SafeDeleteArray(m_pMemory);	return DXTRACE_ERR( TEXT("OV_EVERSION"), FALSE );
	case OV_EBADHEADER:	SafeDeleteArray(m_pMemory);	return DXTRACE_ERR( TEXT("OV_EBADHEADER"), FALSE );
	case OV_EFAULT:		SafeDeleteArray(m_pMemory);	return DXTRACE_ERR( TEXT("OV_EFAULT"), FALSE );
	}
	
	vi = ov_info(&tOggFile,-1);

	// デコードデータサイズ
//	if (!dwWaveSize)
//		return FALSE;

	// デコードデータメモリ作成
	void *pwave = (void*)malloc(dwDataSize);
	void *ptemp;
	char *ppos = (char*)pwave;
	char dec_data[4096];
	int current_section;
	int	nWaveSize = dwDataSize;
	int	nDecSize = 0;
	int nDecTotal = 0;

	// デコード
	do
	{
		ret = ov_read(&tOggFile, dec_data, 4096, 0,2,1, &current_section);

		// 領域拡張
		if (nWaveSize < nDecSize+ret)
		{
			nWaveSize = nDecSize+(4096*100);
			if (!(ptemp = realloc(pwave, nWaveSize)))
			{
				ov_clear(&tOggFile);
				return FALSE;
			}
			pwave = ptemp;
			ppos = (char*)pwave;
			ppos += nDecSize;
		}
		SafeMemCopy(ppos, dec_data, ret , 4096);
		
		ppos += ret;
		nDecSize += ret;

	} while (ret != 0);

	switch (ret)
	{
	case OV_HOLE:		SafeDelete(pwave);	return DXTRACE_ERR( TEXT("ogg decode:OV_HOLE"), FALSE );
	case OV_EBADLINK:	SafeDelete(pwave);	return DXTRACE_ERR( TEXT("ogg decode:OV_EBADLINK"), FALSE );
	}

	DWORD dwWaveSize = (DWORD)(vi->channels * 2 * ov_pcm_total(&tOggFile, -1));

	if (!MakeWaveFormat(&m_tWaveFormat, vi))
	{
		SafeDeleteArray(m_pMemory);
		return FALSE;
	}

	ov_clear(&tOggFile);
//	SafeDeleteArray(m_pMemory);
//  ↑ov_close内で呼び出し

	m_pMemory = new BYTE[dwWaveSize];
	SafeMemCopy(m_pMemory, pwave, dwWaveSize, dwWaveSize);

	SafeFree(pwave);
	m_dwDataSize = dwWaveSize;
	m_dwCursor = 0;

	return TRUE;
}

// WaveFormatを作成
BOOL COggDecoder::MakeWaveFormat(WAVEFORMATEX *wave_fmt, vorbis_info* vi)
{
	if (!wave_fmt || !vi)
		return DXTRACE_ERR( TEXT("MakeWaveFormat:NULL vorbis_info"), FALSE );

	wave_fmt->wFormatTag = WAVE_FORMAT_PCM;
	wave_fmt->nChannels = vi->channels;
	wave_fmt->nSamplesPerSec = vi->rate;
	wave_fmt->nAvgBytesPerSec = vi->channels*vi->rate*2;
	wave_fmt->nBlockAlign = vi->channels*2;
	wave_fmt->wBitsPerSample = 16;
	wave_fmt->cbSize = 0;
	return TRUE;
}

// メモリコピー
BOOL COggDecoder::MemoryCopy(DWORD dwSrcOffset, void* pDest, DWORD *dwMaxSize)
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
