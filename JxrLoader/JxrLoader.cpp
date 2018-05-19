#include "JxrLoader.h"

// PngLoaderを使ってメモリからテクスチャを作成
// pDev					: device
// src_buff				: filer
// size					: バッファサイズ
// width					: out png_width
// height				: out png_height
// ppCreatedTexture: out image data
BOOL JxrLoader::CreateTextureFromFileHandle
	(LPDIRECT3DDEVICE9 pDev, FILE* fd, UINT size, int* width, int* height, LPDIRECT3DTEXTURE9 *ppCreatedTexture, D3DPOOL pool)
{
	if (!ppCreatedTexture)	return FALSE;
	
	// read jxr file
    unsigned int alphaCodedImagePresent = 0;
    DWORD dwTell = ftell(fd);
    unsigned long off = 0;
    jxr_container_t ifile = jxr_create_container();
    int rc = jxr_read_image_container(ifile, fd);

    if (rc >= 0) {
		_ASSERT(rc >= 0);        
# if defined(DETAILED_DEBUG)
        printf("Detected wdp image container\n");
        printf("XXXX Bytes of contained image: %ld\n", jxrc_image_bytecount(ifile, 0));
#endif
        off = jxrc_image_offset(ifile, 0);
#if defined(DETAILED_DEBUG)
        printf("XXXX Offset of contained image: %ld\n", off);
#endif
        rc = fseek(fd, off, SEEK_SET);
        _ASSERT(rc >= 0);
        if(jxrc_alpha_offset(ifile, 0))
        {
            alphaCodedImagePresent = 1;
# if defined(DETAILED_DEBUG)        
            printf("XXXX Bytes of alpha image: %ld\n", jxrc_alpha_bytecount(ifile, 0));
            printf("XXXX Offset of contained image: %ld\n", jxrc_alpha_offset(ifile, 0));
#endif
        }        
    } else {        
#if defined(DETAILED_DEBUG)
        printf("No container found, assuming unwrapped bistream with no alpha coded image\n");
#endif
        rc = fseek(fd, dwTell, SEEK_SET);
        _ASSERT(rc >= 0);
    }

	int nWidth = (int)ceil(jxrc_width_resolution(ifile, 0));
	int nHeight = (int)ceil(jxrc_height_resolution(ifile, 0));

	// decompress image
	DWORD dwByteCount = jxrc_image_bytecount(ifile, 0)+jxrc_alpha_bytecount(ifile, 0);
	BYTE* pJxrData = malloc(dwByteCount);


	// テクスチャサイズを計算
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

	// テクスチャ作成
	HRESULT hr = pDev->CreateTexture(nTexWidth, nTexHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, pool, ppCreatedTexture, NULL);
	if (FAILED(hr))
	{
		return FALSE;
	}

	LPDIRECT3DTEXTURE9 pCreatedTexture = *ppCreatedTexture;

	// サーフェースロック
	D3DLOCKED_RECT LockedRect;
	hr = pCreatedTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD);
	if (hr != D3D_OK)
	{
		SafeRelease(pCreatedTexture);
		MessageBox(NULL, L"ステージ画像の操作に失敗しました", L"error", MB_OK);
		return FALSE;
	}

	DWORD* pDest = (DWORD*)LockedRect.pBits;

	// 画像サイズが違うなら一端0x0で塗り潰す
	if (nTexHeight != nHeight && nTexWidth != nWidth)
	{
		memset(pDest, 0x0, nTexHeight*nTexWidth*sizeof(DWORD));
		/*
		// 1行ずつコピー
		for (int i=0;i<nHeight;i++)
		{
			memcpy(pDest, pSrc, nWidth*sizeof(DWORD));
			pDest += nTexWidth;
			pSrc += nWidth;
		}
		*/
	}
	else
	{
		/*
		memcpy(pDest, pSrc, nHeight*nWidth*sizeof(DWORD));
		*/
	}
	hr = pCreatedTexture->UnlockRect(0);

	return TRUE;
}
