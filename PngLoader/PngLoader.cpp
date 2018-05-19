#include "PngLoader.h"

void PngLoader::png_memread_func(png_structp png_ptr, png_bytep buf, png_size_t size)
{
	TPngBuffer *png_buff = (TPngBuffer *)png_get_io_ptr(png_ptr);
	if (png_buff->data_offset + size <= png_buff->data_len)
	{
		memcpy(buf, png_buff->data + png_buff->data_offset, size);
		png_buff->data_offset += size;
	}
	else
	{
		png_error(png_ptr,"png_mem_read_func failed");
	}
}

void PngLoader::png_data_read(png_structp png_ptr, TPngBuffer *png_buff)
{
    png_set_read_fn(png_ptr, (png_voidp) png_buff,
                    (png_rw_ptr)png_memread_func);
}

// メモリからオープン
// png_ptr				: out image data
// pbuffer				: buffer
// nbuffersize			: buffer_size
// width					: out png_width
// height				: out png_height
/*static void PngLoader::PngOpenFromFileInMemory(png_structp png_ptr, BYTE* pbuffer, int nbuffersize, int* width, int* height);*/

// メモリからオープン
// dest_data			: out image data
// dest_data_lines	: out image data lines pointer
// src_buff				: buffer
// size					: buffer_size
// width					: out png_width
// height				: out png_height
/*static*/ BOOL PngLoader::PngLoadFromFileInMemory(DWORD** dest_data, DWORD*** dest_data_lines, BYTE* src_buff, UINT size, int* width, int* height)
{
	if (!dest_data || !dest_data_lines || !src_buff || !size)
	{
#ifdef _DEBUG
		MessageBox(NULL, L"PngOpenFromFileInMemory_引数が不正", L"error", MB_OK);
#endif
		return FALSE;
	}
	TPngBuffer tPngBuff;
	ZeroMemory(&tPngBuff, sizeof(TPngBuffer));
	DWORD dwPngWidth;
	DWORD dwPngHeight;
	BOOL bpp;
	int color_type;

	tPngBuff.data = src_buff;
	tPngBuff.data_len = size;
    if ( !png_check_sig((png_bytep)tPngBuff.data, 8))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"PngOpenFromFileInMemory_PNGヘッダ不正", L"error", MB_OK);
#endif
		return FALSE;
	}

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
    if (! png_ptr) {
#ifdef _DEBUG
		MessageBox(NULL, L"PngOpenFromFileInMemory_create read_struct", L"error", MB_OK);
#endif
		png_destroy_read_struct (&png_ptr, NULL, NULL);
        return FALSE;
    }

	png_infop info_ptr = png_create_info_struct(png_ptr);
    if (! info_ptr) {
#ifdef _DEBUG
		MessageBox(NULL, L"PngOpenFromFileInMemory_create info_struct", L"error", MB_OK);
#endif
		png_destroy_read_struct (&png_ptr, NULL, NULL);
        return FALSE;
    }
	png_data_read(png_ptr, &tPngBuff);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr,
		 (png_uint_32 *)&dwPngWidth, (png_uint_32 *)&dwPngHeight,
		 &bpp, &color_type,
		 NULL, NULL, NULL);

	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);
	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);
	if(color_type == PNG_COLOR_TYPE_GRAY && bpp < 8)
		png_set_expand(png_ptr);
	if(bpp > 8)
		png_set_strip_16(png_ptr);
	if(color_type == PNG_COLOR_TYPE_GRAY)
		png_set_gray_to_rgb(png_ptr);
//	if(color_type == PNG_COLOR_TYPE_RGB_ALPHA)
//	    printf("PNG_COLOR_TYPE_RGB_ALPHA");
	if(!(color_type & PNG_COLOR_MASK_ALPHA))png_set_filler(png_ptr, 0, 1);

    png_set_bgr(png_ptr);

	if (width)	*width = dwPngWidth;
	if (height)	*height = dwPngHeight;

	BYTE*	png_buff = NULL;
	BYTE**	png_lines = NULL;
//	png_buff = (unsigned char *)VirtualAlloc(NULL, dwPngWidth* dwPngHeight* 4, MEM_RESERVE | MEM_COMMIT, protect);
//	png_lines = (unsigned char **)VirtualAlloc(NULL, sizeof(unsigned char *) * dwPngHeight, MEM_RESERVE | MEM_COMMIT, protect);
	png_buff = (unsigned char *)malloc(dwPngWidth* dwPngHeight* 4);
	png_lines = (unsigned char **)malloc(sizeof(unsigned char *) * dwPngHeight);

	if (!png_buff || !png_lines)
	{
#ifdef _DEBUG
		MessageBox(NULL, L"memory_error", L"error", MB_OK);
#endif
		png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
/*
		if (png_lines)
		{
			VirtualFree(png_lines,0,MEM_RELEASE);
			png_lines = NULL;
		}
		if (png_buff)
		{
			VirtualFree(png_buff,0,MEM_RELEASE);
			png_buff = NULL;
		}
*/
		SafeFree(png_lines);
		SafeFree(png_buff);
		return FALSE;
	}

	// アドレス設定
	for(DWORD i = 0; i < dwPngHeight; i++)png_lines[i] = png_buff + dwPngWidth * 4 * i;
	png_read_image(png_ptr, png_lines);
	*dest_data = (DWORD*)png_buff;
	*dest_data_lines = (DWORD**)png_lines;

	png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

	return TRUE;
}


// メモリからオープン
// dest_data			: out image data
// dest_data_lines	: out image data lines pointer
// pfiler					: filer
// width					: out png_width
// height				: out png_height
/*static*/ BOOL PngLoader::PngLoadFromFileInMemory(DWORD** dest_data, DWORD*** dest_data_lines, WCHAR* src_path, CFiler *pfiler, int* width, int* height)
{
	BYTE* pBuffer;
	UINT	nBufferSize;
	if (!pfiler || !pfiler->GetFileMemory(src_path, &pBuffer, &nBufferSize))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"PngOpenFromFileInMemory_引数不正", L"error", MB_OK);
#endif
        return FALSE;
	}
	BOOL ret = PngLoadFromFileInMemory(dest_data, dest_data_lines, pBuffer, nBufferSize, width, height);
	SafeDeleteArray(pBuffer);
	return ret;
}

// メモリからオープン
// dest_image		: out image data
// pfiler					: filer
// width					: out png_width
// height				: out png_height
/*static*/ BOOL PngLoader::PngLoadFromFileInMemory(TPngImage* dest_image, WCHAR* src_path, CFiler *pfiler)
{
	if (!dest_image)
		return FALSE;
	if (!PngLoadFromFileInMemory (&dest_image->data, &dest_image->lines, src_path, pfiler, (int*)&dest_image->width, (int*)&dest_image->height))
	{
		return FALSE;
	}

	return TRUE;
}


// PngLoaderを使ってメモリからテクスチャを作成
// pDev					: device
// src_buff				: filer
// size					: バッファサイズ
// width					: out png_width
// height				: out png_height
// ppCreatedTexture: out image data
BOOL PngLoader::CreateTextureFromFileInMemory(LPDIRECT3DDEVICE9 pDev, BYTE* src_buff, UINT size, int* width, int* height, LPDIRECT3DTEXTURE9 *ppCreatedTexture, D3DPOOL pool)
{
	if (!ppCreatedTexture)	return FALSE;

	// PNGロード
	TPngImage temp_image;
	if (!PngLoader::PngLoadFromFileInMemory(&temp_image.data, &temp_image.lines, src_buff, size, (int*)&temp_image.width, (int*)&temp_image.height) )
		return FALSE;
	
	// テクスチャサイズを計算
	int nTexWidth = 1;
	while (nTexWidth < temp_image.width)
		nTexWidth *= 2;
	if (width)
		*width = nTexWidth;

	int nTexHeight = 1;
	while (nTexHeight < temp_image.height)
		nTexHeight *= 2;
	if (height)
		*height = nTexHeight;

	HRESULT hr = pDev->CreateTexture(nTexWidth, nTexHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, pool, ppCreatedTexture, NULL);
	if (FAILED(hr))
	{
		SafeFree(temp_image.data);
		SafeFree(temp_image.lines);
		return FALSE;
	}

	LPDIRECT3DTEXTURE9 pCreatedTexture = *ppCreatedTexture;

	// サーフェースロック
	D3DLOCKED_RECT LockedRect;
	hr = pCreatedTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD);
	if (hr != D3D_OK)
	{
		SafeRelease(pCreatedTexture);
		SafeFree(temp_image.data);
		SafeFree(temp_image.lines);
		MessageBox(NULL, L"ステージ画像の操作に失敗しました", L"error", MB_OK);
		return FALSE;
	}

	int nPngWidth = temp_image.width;
	int nPngHeight = temp_image.height;
	DWORD* pDest = (DWORD*)LockedRect.pBits;
	DWORD* pSrc = (DWORD*)temp_image.lines[0];

	// 画像サイズが違うなら一端0x0で塗り潰す
	if (nTexHeight == nPngHeight && nTexWidth == nPngWidth)
	{
		memcpy(pDest, pSrc, nPngWidth*nPngHeight*sizeof(DWORD));
	}
	else
	{
		if (nTexHeight != nPngHeight && nTexWidth != nPngWidth)
			memset(pDest, 0x0, nTexHeight*nTexWidth*sizeof(DWORD));
		// 1行ずつコピー
		for (int i=0;i<nPngHeight;i++)
		{
			memcpy(pDest, pSrc, nPngWidth*sizeof(DWORD));
			pDest += nTexWidth;
			pSrc += nPngWidth;
		}
	}
	hr = pCreatedTexture->UnlockRect(0);
	SafeFree(temp_image.data);
	SafeFree(temp_image.lines);
	if (hr != D3D_OK)
	{
		SafeRelease(pCreatedTexture);
		MessageBox(NULL, L"ステージ画像の操作に失敗しました", L"error", MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL PngLoader::CopyToTexture(LPDIRECT3DDEVICE9 pDev, RECT* pPasteRect, int nCopyX, int nCopyY, const TPngImage* pCopyImage, LPDIRECT3DTEXTURE9 pPasteTexture)
{
	// 引数確認
	if (!pDev || !pCopyImage || !pPasteTexture)
	{
		MessageBox(NULL, L"null arg", L"CopyToTexture", MB_OK);
		return FALSE;
	}
	
	RECT* prcPasteRect = pPasteRect;
	RECT rcPasteRect,rcCopyRect,rcImageRect,rcIntersectRect;
	if (pPasteRect)
	{
		SetRect(&rcImageRect, 0,0,pCopyImage->width, pCopyImage->height);
		CopyRect(&rcPasteRect, pPasteRect);
		SetRect(&rcCopyRect, nCopyX, nCopyY, nCopyX+pPasteRect->right-pPasteRect->left, nCopyY+pPasteRect->bottom-pPasteRect->top);
		// 範囲にないならコピー不要
		if (!IntersectRect(&rcIntersectRect, &rcCopyRect, &rcImageRect))
			return TRUE;

		// 範囲修正
		if (rcIntersectRect.left != rcCopyRect.left)
			rcPasteRect.left += (rcIntersectRect.left-rcCopyRect.left);
		if (rcIntersectRect.right != rcCopyRect.right)
			rcPasteRect.right -= (rcCopyRect.right-rcIntersectRect.right);
		if (rcIntersectRect.top != rcCopyRect.top)
			rcPasteRect.top += (rcIntersectRect.top-rcCopyRect.top);
		if (rcIntersectRect.bottom != rcCopyRect.bottom)
			rcPasteRect.bottom -= (rcCopyRect.bottom-rcIntersectRect.bottom);
		// 貼り付け必要なしか確認
		if (rcPasteRect.right-rcPasteRect.left <= 0 || rcPasteRect.bottom-rcPasteRect.top <= 0)
			return TRUE;
		prcPasteRect = &rcPasteRect;
	}

	// サーフェースロック
	D3DLOCKED_RECT LockedRect;
	if (pPasteTexture->LockRect(0, &LockedRect, prcPasteRect, D3DLOCK_DISCARD) != D3D_OK)
	{
		MessageBox(NULL, L"ステージ画像の操作に失敗しました", L"error", MB_OK);
		return FALSE;
	}

	DWORD* pDest = (DWORD*)LockedRect.pBits;
	DWORD* pSrc = (DWORD*)pCopyImage->lines[0];
	int nWidth = LockedRect.Pitch/sizeof(DWORD);
	int nHeight = pCopyImage->height;
	if (pPasteRect)
	{
		nHeight = rcPasteRect.bottom-rcPasteRect.top;
		pSrc = (DWORD*)pCopyImage->lines[rcIntersectRect.top];
		pSrc += rcIntersectRect.left;
		int nPngWidth = pCopyImage->width;
		// 範囲コピー
		for (int i=0;i<nHeight;i++)
		{
			memcpy(pDest, pSrc, nWidth*sizeof(DWORD));
			pDest += nWidth;
			pSrc += nPngWidth;
		}
	}
	else
	{
		// 画像全体コピー
		memcpy(pDest, pSrc, nWidth*sizeof(DWORD)*nHeight);
	}

	if (pPasteTexture->UnlockRect(0) != D3D_OK)
	{
		MessageBox(NULL, L"ステージ画像の操作に失敗しました", L"error", MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL PngLoader::PasteFromImage(RECT* pCopyRect, int nPasteX, int nPasteY, const TPngImage* pPasteImage,const TPngImage* pCopyImage)
{
	// 引数確認
	if (!pPasteImage || !pCopyImage)
		return FALSE;

	RECT rcCopyRect;
	CopyRect(&rcCopyRect, pCopyRect);
	RECT rcPasteRect = {nPasteX,nPasteY,nPasteX+pCopyRect->right-pCopyRect->left, nPasteY+pCopyRect->bottom-pCopyRect->top};
	RECT rcImageRect = {0,0,pPasteImage->width,pPasteImage->height};
	RECT rcIntersectRect;
	// 貼り付けがイメージ範囲外確認なら処理必要なし
	if (!IntersectRect(&rcIntersectRect, &rcPasteRect, &rcImageRect))
		return TRUE;

	// 貼り付け範囲が一部画面外の場合範囲修正
	if (rcIntersectRect.left != rcPasteRect.left)
		rcCopyRect.left += (rcIntersectRect.left-rcPasteRect.left);	// 参照範囲
	if (rcIntersectRect.right != rcPasteRect.right)
		rcCopyRect.right -= (rcPasteRect.right-rcIntersectRect.right);	// 参照範囲
	if (rcIntersectRect.top != rcPasteRect.top)
		rcCopyRect.top += (rcIntersectRect.top-rcPasteRect.top);	// 参照範囲
	if (rcIntersectRect.bottom != rcPasteRect.bottom)
		rcCopyRect.bottom -= (rcPasteRect.bottom-rcIntersectRect.bottom);	// 参照範囲
	// 貼り付け必要なしか確認
	if (rcCopyRect.right-rcCopyRect.left <= 0 || rcCopyRect.bottom-rcCopyRect.top <= 0)
		return TRUE;

	DWORD* pSrc = (DWORD*)pCopyImage->lines[0];
	DWORD* pDest = (DWORD*)pPasteImage->lines[0];
	int nLineWidth = pCopyImage->width;
	int nHeight = pPasteImage->height;
	if (pCopyRect)
	{
		DWORD* pSrcLine = (DWORD*)pCopyImage->lines[rcCopyRect.top];
		pSrcLine+= rcCopyRect.left;
		nHeight = rcIntersectRect.bottom-rcIntersectRect.top;
		int nPasteWidth = rcIntersectRect.right-rcIntersectRect.left;
		// 範囲コピー
		for (int y=rcIntersectRect.top;y<rcIntersectRect.bottom;y++)
		{
			pSrc = pSrcLine;
			pDest = (DWORD*)pPasteImage->lines[y];
			pDest += rcIntersectRect.left;
			for (int x=0;x<nPasteWidth;x++)
			{
				if (*pSrc &  0xFF000000 && ((*pDest & 0xFF000000) ^ 0xFF000000))
					memcpy(pDest,pSrc,sizeof(DWORD));
				pSrc++;
				pDest++;
			}
			pSrcLine += nLineWidth;
		}
	}
	else
	{
		// 画像全体コピー
		memcpy(pDest, pSrc, nLineWidth*sizeof(DWORD)*nHeight);
	}
	return TRUE;
}

BOOL PngLoader::PasteFromTexture(LPDIRECT3DDEVICE9 pDev, RECT* pCopyRect, int nPasteX, int nPasteY, const TPngImage* pPasteImage, LPDIRECT3DTEXTURE9 pCopyTexture)
{
	// 引数確認
	if (!pDev || !pPasteImage || !pCopyTexture)
		return FALSE;

	RECT rcCopyRect;
	CopyRect(&rcCopyRect, pCopyRect);
	RECT rcPasteRect = {nPasteX,nPasteY,nPasteX+pCopyRect->right-pCopyRect->left, nPasteY+pCopyRect->bottom-pCopyRect->top};
	RECT rcImageRect = {0,0,pPasteImage->width,pPasteImage->height};
	RECT rcIntersectRect;
	// 貼り付けがイメージ範囲外確認なら処理必要なし
	if (!IntersectRect(&rcIntersectRect, &rcPasteRect, &rcImageRect))
		return TRUE;

	// 貼り付け範囲が一部画面外の場合範囲修正
	if (rcIntersectRect.left != rcPasteRect.left)
		rcCopyRect.left += (rcIntersectRect.left-rcPasteRect.left);	// 参照範囲
	if (rcIntersectRect.right != rcPasteRect.right)
		rcCopyRect.right -= (rcPasteRect.right-rcIntersectRect.right);	// 参照範囲
	if (rcIntersectRect.top != rcPasteRect.top)
		rcCopyRect.top += (rcIntersectRect.top-rcPasteRect.top);	// 参照範囲
	if (rcIntersectRect.bottom != rcPasteRect.bottom)
		rcCopyRect.bottom -= (rcPasteRect.bottom-rcIntersectRect.bottom);	// 参照範囲
	// 貼り付け必要なしか確認
	if (rcCopyRect.right-rcCopyRect.left <= 0 || rcCopyRect.bottom-rcCopyRect.top <= 0)
		return TRUE;

	// サーフェースロック
	D3DLOCKED_RECT LockedRect;
	if (pCopyTexture->LockRect(0, &LockedRect, &rcCopyRect, D3DLOCK_DISCARD|D3DLOCK_READONLY) != D3D_OK)
	{
		MessageBox(NULL, L"ステージ画像の操作に失敗しました", L"error", MB_OK);
		return FALSE;
	}

	DWORD* pSrc = (DWORD*)LockedRect.pBits;
	DWORD* pDest = (DWORD*)pPasteImage->lines[0];
	int nLineWidth = LockedRect.Pitch/sizeof(DWORD);
	int nHeight = pPasteImage->height;
	if (pCopyRect)
	{
		DWORD* pSrcLine = (DWORD*)LockedRect.pBits;
		nHeight = rcIntersectRect.bottom-rcIntersectRect.top;
		int nPasteWidth = rcIntersectRect.right-rcIntersectRect.left;
		// 範囲コピー
		for (int y=rcIntersectRect.top;y<rcIntersectRect.bottom;y++)
		{
			pSrc = pSrcLine;
			pDest = (DWORD*)pPasteImage->lines[y];
			pDest += rcIntersectRect.left;
			for (int x=0;x<nPasteWidth;x++)
			{
				if (*pSrc &  0xFF000000 && ((*pDest & 0xFF000000) ^ 0xFF000000) )
					memcpy(pDest,pSrc,sizeof(DWORD));
				pSrc++;
				pDest++;
			}
			pSrcLine += nLineWidth;
		}
	}
	else
	{
		// 画像全体コピー
		memcpy(pDest, pSrc, nLineWidth*sizeof(DWORD)*nHeight);
	}

	if (pCopyTexture->UnlockRect(0) != D3D_OK)
	{
		MessageBox(NULL, L"ステージ画像の操作に失敗しました", L"error", MB_OK);
		return FALSE;
	}
	return TRUE;
}