
#include "CWuVorbisDecoder.h"

/*static */unsigned int CWuVorbisDecoder::WuVorbisInitialize(const char *dll_file_name_can_be_null, __int32 *cputype)
{
	int ret = 3;
	if(ret = WuVorbisInit(dll_file_name_can_be_null))
		return ret;
	unsigned __int32 ct = wu_DetectCPU();
	wu_SetCPUType(ct);
	if (cputype)
		*cputype = ct;
	return ret;
}

/*static */int CWuVorbisDecoder::WuVorbisFinalize()
{
	// WuVorbis ���I��
	// �ʏ� Windows �̓A�v���P?�V�����I�����Ɏ����I�� DLL ���������̂�
	// ����͕K�v�Ȃ������B�B�B
	return WuVorbisUninit();
}

CWuVorbisDecoder::CWuVorbisDecoder():CWaveData()
{
	// �t�H??�b�g
	ZeroMemory(&m_tWaveFormat, sizeof(WAVEFORMATEX));
	// �ǂݍ��񂾃f??
	m_pMemory = NULL;
	// �f??�T�C�Y
	m_dwDataSize = 0;	
	m_dwCursor = 0;
}

CWuVorbisDecoder::~CWuVorbisDecoder()
{
    Close();
}
// ���g��?�C��?
CWuVorbisDecoder* CWuVorbisDecoder::GetThis( void* stream )
{
	CWuVorbisDecoder *p = NULL;
	SafeMemCopy( &p, stream, sizeof( CWuVorbisDecoder*), sizeof( CWuVorbisDecoder*));
	return p;
}


// ����������ǂݍ���
size_t __cdecl CWuVorbisDecoder::cb_read_func( void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE*)datasource);
}

// �������V?�N
int __cdecl CWuVorbisDecoder::cb_seek_func( void *datasource, wu_ogg_int64_t offset, int whence)
{
	return fseek((FILE*)datasource, (long)offset, whence);
}

// �������N��?�Y
int __cdecl CWuVorbisDecoder::cb_close_func( void *datasource)
{
	return fclose((FILE*)datasource);
}

// �������ʒu
long __cdecl CWuVorbisDecoder::cb_tell_func( void *datasource)
{
	return ftell((FILE*)datasource);
}

// �ǂݍ��񂾃�������j��
BOOL CWuVorbisDecoder::Close()
{
	m_dwCursor = 0;
	return CWaveData::Close();
}

// �J���Ă���t?�C��?�C��?����ǂݍ���
BOOL CWuVorbisDecoder::Load(FILE* fh, DWORD size)
{
	// �O�̃�������j��
	Close();

	if (!fh || !size)
		return FALSE;

	// wu_og_open_callbacks ���g�p���ĊJ��
	wu_OggVorbis_File ovfile;

	WuVorbisInit(NULL);
	// �R?���o�b�N�֐��̃Z�b�g�A�b�v
	wu_ov_callbacks callbacks = {cb_read_func, cb_seek_func, cb_close_func, cb_tell_func};

	if(wu_ov_open_callbacks((void*)fh, &ovfile, NULL, 0, callbacks) < 0)
	{
		// �t?�C���𐳏�ɊJ���Ȃ�
		fclose(fh);
		WuVorbisUninit();
		MessageBox(NULL, _T("vorbis file decode error"),_T("Cannot open (in ov_open_callbacks)\n"), MB_OK);
		return FALSE;
	}

	// ����?��
	wu_vorbis_info *info = wu_ov_info(&ovfile, -1);

	if(!info)
	{
		wu_ov_clear(&ovfile);
		WuVorbisUninit();
		MessageBox(NULL, _T("vorbis file decode error"), _T("ov_info failed\n"), MB_OK);
		return FALSE;
	}
	MakeWaveFormat(&m_tWaveFormat, info);

	int nPCMBytes = (int)wu_ov_pcm_total(&ovfile, -1);

	// �f�R?�h���Ă݂�
	// �f�R?�h�f??�������쐬
	int	nWaveSize = nPCMBytes;
	void *pwave = (void*)malloc(nPCMBytes);
	void *ptemp;
	char *ppos = (char*)pwave;
	int	nDecSize = 0;
	int nDecTotal = 0;
	// ���̃e�X�g�v���O��?���o�͂���f�R?�h���ʂ� raw pcm �Ȃ̂Œ���
	int section = -1;
	BOOL ret = TRUE;
	while(true)
	{
		const int buf_size = 4096;
		char buf[buf_size];
		// wu_ov_read �� bigendianp �� 0�A
		// word �� 2(16bit signed integer) �� 4 (32bit float) �A
		// signed �� 1 �̕K�v������
		int res = wu_ov_read(&ovfile, buf, buf_size, 0, 2, 1, &section);
		// wu_ov_read �͗v�����ꂽ�T�C�Y�𖞂������ɋA��ꍇ������̂Œ��ӁB
		// 0 ���߂�΃f�R?�h�I���A���̐����߂��
		// �f�R?�h����(�߂�l�͏������܂ꂽ�o�C�g��)�A
		// ���̐����߂����ꍇ�̓p�P�b�g�s���Ńf�R?�h�ł��Ȃ����B
		//  (�l�b�g��?�N����̃X�g��??���O�Ƃ��łȂ��Ă����̏󋵂͔�������)
		// ���̐����߂����ꍇ�́A�ǂݎ���悤�ɂȂ�܂�
		// ���̂܂� wu_ov_read ���J��Ԃ��΂悢

		if(res == 0) break; // �f�R?�h�I��
		if(res > 0)
		{
			// �̈�g��
			if (nWaveSize < nDecSize+res)
			{
				nWaveSize = nDecSize+(4096*100);
				if (!(ptemp = realloc(pwave, nWaveSize)))
				{
					ret = FALSE;
					break;
				}
				pwave = ptemp;
				ppos = (char*)pwave;
				ppos += nDecSize;
			}
			SafeMemCopy(ppos, buf, res , buf_size);
			
			ppos += res;
			nDecSize += res;
		}
	}

	// wu_ov_clear �� ovfile ?���̂��N���A
	wu_ov_clear(&ovfile);

	if (ret)
	{
		m_pMemory = new BYTE[nDecSize];
		SafeMemCopy(m_pMemory, pwave, nDecSize, nDecSize);

		m_dwDataSize = nDecSize;
		m_dwCursor = 0;
	}
	SafeFree(pwave);

	return ret;
}

// ����������ǂݍ���
BOOL CWuVorbisDecoder::LoadFromMemory(BYTE *pbData, DWORD dwDataSize)
{
	return DXTRACE_ERR( TEXT("CWuVorbisDecoder::LoadFromMemory�͎g���܂���"), FALSE );
#if 0
	vorbis_info		*vi;
	// �O�̃�������j��
	Close();

	if (!pbData)
		return FALSE;
	if (!dwDataSize)
		return FALSE;

	// �������쐬 (���g��?�C��?+�t?�C��)
	
//	BYTE *pmem = new BYTE[/*dwDataSize+*/sizeof(COggDecoder)];
/*
	if (!pmem)
		return FALSE;
	ZeroMemory(pmem, sizeof(COggDecoder));
*/
	m_pMemory = new BYTE[dwDataSize];

	m_dwDataSize = dwDataSize;

	// ���������R�s?
	if (!SafeMemCopy((char*)m_pMemory, pbData, dwDataSize, dwDataSize))
		return FALSE;

	OggVorbis_File tOggFile;
	
	// �R?���o�b�N�p�ϐ�
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

	// �f�R?�h�f??�T�C�Y
//	if (!dwWaveSize)
//		return FALSE;

	// �f�R?�h�f??�������쐬
	void *pwave = (void*)malloc(dwDataSize);
	void *ptemp;
	char *ppos = (char*)pwave;
	char dec_data[4096];
	int current_section;
	int	nWaveSize = dwDataSize;
	int	nDecSize = 0;
	int nDecTotal = 0;

	// �f�R?�h
	do
	{
		ret = ov_read(&tOggFile, dec_data, 4096, 0,2,1, &current_section);

		// �̈�g��
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
//  ��ov_close���ŌĂяo��

	m_pMemory = new BYTE[dwWaveSize];
	SafeMemCopy(m_pMemory, pwave, dwWaveSize, dwWaveSize);

	SafeFree(pwave);
	m_dwDataSize = dwWaveSize;
	m_dwCursor = 0;

	return TRUE;
#endif
}

// WaveFormat���쐬
BOOL CWuVorbisDecoder::MakeWaveFormat(WAVEFORMATEX *wave_fmt, wu_vorbis_info* vi)
{
	if (!wave_fmt || !vi)
		return DXTRACE_ERR( TEXT("MakeWaveFormat:NULL vorbis_info"), FALSE );

//	m_tWaveFormat
/*	fprintf(stderr,
		"version   : %d\n"
		"channels  : %d\n"
		"rate      : %d\n"
		"bitrate_upper   : %d\n"
		"bitrate_nominal : %d\n"
		"bitrate_lower   : %d\n"
		"bitrate_window  : %d\n",
		info->version, info->channels, info->rate, info->bitrate_upper,
		info->bitrate_nominal, info->bitrate_lower, info->bitrate_window);
*/	
	wave_fmt->wFormatTag = WAVE_FORMAT_PCM;
	wave_fmt->nChannels = vi->channels;
	wave_fmt->nSamplesPerSec = vi->rate;
	wave_fmt->wBitsPerSample = 16;
	wave_fmt->nBlockAlign = wave_fmt->nChannels * 2;//wh.wBitsPerSample/8;
	wave_fmt->nAvgBytesPerSec = wave_fmt->nBlockAlign*vi->rate;
	wave_fmt->cbSize = 0;
	return TRUE;
}
