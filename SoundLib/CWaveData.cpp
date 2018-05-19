#include "CWaveData.h"

// ����
CWaveData::CWaveData()
{
	// �t�H�[�}�b�g
	ZeroMemory(&m_tWaveFormat, sizeof(WAVEFORMATEX));
	// �ǂݍ��񂾃f�[�^
	m_pMemory = NULL;
	// �f�[�^�T�C�Y
	m_dwDataSize = 0;
}

// �j��
CWaveData::~CWaveData()
{
	Close();
}

// �ǂݍ��񂾃�������j��
BOOL CWaveData::Close()
{
	SafeDeleteArray(m_pMemory);
	// �t�H�[�}�b�g
	ZeroMemory(&m_tWaveFormat, sizeof(WAVEFORMATEX));
	// �f�[�^�T�C�Y
	m_dwDataSize = 0;		
	
	return TRUE;
}

// �t�@�C������ǂݍ���
BOOL CWaveData::LoadFromFile(const TCHAR* path)
{
	// �O�̃�������j��
	Close();

	FILE *fh = NULL;
#ifdef _UNICODE
	errno_t err = _wfopen_s(&fh, path, L"rb");
#else
	errno_t err = fopen_s(&fh, path, "rb");
#endif
	if (err != 0) return FALSE;
//	if (!fh)	return FALSE;

	// �T�C�Y�擾
	DWORD fsize = _filelength(_fileno(fh));

	// Open����
	BOOL ret = Load(fh, fsize);
	// �t�@�C�������
	fclose(fh);

	return ret;
}


// �J���Ă���t�@�C���|�C���^����ǂݍ���
BOOL CWaveData::Load(FILE* fh, DWORD size)
{
	// �O�̃�������j��
	Close();

	if (!fh || !size)
		return FALSE;

	//> �T�C�Y�̌���
	// ���݈ʒu
	long fcur = ftell(fh);
	fseek(fh, 0, SEEK_END);
	// �ŏI�ʒu
	long fsize = _filelength(_fileno(fh));

	// ���̈ʒu�ɖ߂�
	fseek(fh, fcur, SEEK_SET);

	// �����̃T�C�Y���t�@�C���T�C�Y���傫���ꍇ
	if ((long)size > (fsize-fcur))
		return DXTRACE_ERR( TEXT("size > (fsize-fcur)"), FALSE );

	// �������쐬
	BYTE *pmem = new BYTE[size];
	if (!pmem)
		return FALSE;

	// �t�@�C�����[�h
	DWORD readsize = fread(pmem, 1, size, fh);

	// ���̈ʒu�ɖ߂�
	fseek(fh, fcur, SEEK_SET);
	// �ǂݍ��ݎ��s
	if (readsize < size)
	{
		SafeDeleteArray(pmem);
		return FALSE;
	}

	// ����������ǂݍ���
	BOOL ret = LoadFromMemory(pmem, size);

	SafeDeleteArray(pmem);
	return ret;
}


// ����������ǂݍ���
// pbData		- ���������WAVE�t�@�C��
// dwDataSize	- �t�@�C���T�C�Y
BOOL CWaveData::LoadFromMemory(BYTE *pbData, DWORD dwDataSize)
{
	// �O�̃�������j��
	Close();

	if (!pbData)
		return FALSE;
	if (!dwDataSize)
		return FALSE;

	TWAVEHEADER WaveHeader;

	// �w�b�_�R�s�[
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

	// "data"��T��
	while ( strncmp(pOffset, "data", 4) != 0)
	{
		// �T�C�Y�`�F�b�N
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

	// �R�s�[
	if (!SafeMemCopy(m_pMemory, pbData+nDataOffset, m_dwDataSize, m_dwDataSize))
		return FALSE;

	return TRUE;
}


// �������R�s�[
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