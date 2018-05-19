#include "WebpLoader.h"

// PngLoader���g���ă���������e�N�X�`�����쐬
// pDev					: device
// src_buff				: filer
// size					: �o�b�t�@�T�C�Y
// width					: out png_width
// height				: out png_height
// ppCreatedTexture: out image data
BOOL WebpLoader::CreateTextureFromFileInMemory(
	LPDIRECT3DDEVICE9 pDev, BYTE* data, UINT data_size, int* width, int* height, LPDIRECT3DTEXTURE9 *ppCreatedTexture, D3DPOOL pool)
{
	int nWidth, nHeight;
	if (!ppCreatedTexture)	return FALSE;

	if (!WebPGetInfo(data, data_size, &nWidth, &nHeight))
		return FALSE;
	uint8_t* out_data = WebPDecodeBGRA(data, data_size, &nWidth, &nHeight);

	if (!out_data) return FALSE;

	// �e�N�X�`���T�C�Y���v�Z
	int nTexWidth = 1;
	while (nTexWidth < nWidth)
		nTexWidth *= 2;
	if (width)
		*width = nTexWidth;

	int nTexHeight = 1;
	while (nTexHeight < nHeight)
		nTexHeight *= 2;
	if (height)
		*height = nTexHeight;

	// �e�N�X�`���쐬
	HRESULT hr = pDev->CreateTexture(nTexWidth, nTexHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, pool, ppCreatedTexture, NULL);
	if (FAILED(hr))
	{
		delete [] out_data;
		return FALSE;
	}

	LPDIRECT3DTEXTURE9 pCreatedTexture = *ppCreatedTexture;

	// �T�[�t�F�[�X���b�N
	D3DLOCKED_RECT LockedRect;
	hr = pCreatedTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD);
	if (hr != D3D_OK)
	{
		SafeRelease(pCreatedTexture);
		delete [] out_data;
		MessageBox(NULL, L"�X�e�[�W�摜�̑���Ɏ��s���܂���", L"error", MB_OK);
		return FALSE;
	}

	DWORD* pDest = (DWORD*)LockedRect.pBits;
	DWORD* pSrc = (DWORD*)out_data;

	// �摜�T�C�Y���Ⴄ�Ȃ��[0x0�œh��ׂ�
	if (nTexHeight != nHeight && nTexWidth != nWidth)
	{
		memset(pDest, 0x0, nTexHeight*nTexWidth*sizeof(DWORD));
		// 1�s���R�s�[
		for (int i=0;i<nHeight;i++)
		{
			memcpy(pDest, pSrc, nWidth*sizeof(DWORD));
			pDest += nTexWidth;
			pSrc += nWidth;
		}
	}
	else
	{
		memset(pDest, 0x0, nTexHeight*nTexWidth*sizeof(DWORD));
		memcpy(pDest, pSrc, nHeight*nWidth*sizeof(DWORD));
	}
	hr = pCreatedTexture->UnlockRect(0);

	delete [] out_data;

	return TRUE;
}
