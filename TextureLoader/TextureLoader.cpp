#include "TextureLoader.h"

BOOL TextureLoader::ClipTexture(	LPDIRECT3DTEXTURE9 *pDest, LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 pSrc, RECT* pClipRec)
{
	// 指定範囲分のテクスチャを作成
	if (FAILED(D3DXCreateTexture(pDev
		, pClipRec->right, pClipRec->bottom
		, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT
		, pDest)))
		return FALSE;

	// 元テクスチャから指定範囲を切り取る
	if (!Split(*pDest, pSrc, pClipRec))
	{
		SafeRelease( ( *pDest ));
		return FALSE;
	}
	return TRUE;
}


BOOL TextureLoader::LoadTextureFromFileInMemory(LPDIRECT3DTEXTURE9 *pDest, LPDIRECT3DDEVICE9 pDev, LPCVOID pSrc, UINT size, int *width, int *height, RECT* pClipSrcRec, UINT mipleve, DWORD usage, D3DPOOL pool)
{
	// Load
	if(FAILED(D3DXCreateTextureFromFileInMemoryEx(
		pDev,
		pSrc,
		size,
		D3DX_DEFAULT, D3DX_DEFAULT,
		mipleve,
		usage,
		D3DFMT_A8B8G8R8,
		pool,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		NULL,
		NULL,
		pDest)))
		return FALSE;

	// 元画像の範囲を指定している場合
	if (pClipSrcRec)
	{
		int ImgW,ImgH;
		GetImageInfo(pSrc, size, &ImgW, &ImgH);	
		LPDIRECT3DTEXTURE9 pTempAdx;
		RECT recSrc;
		SetRect(&recSrc, 0,0, ImgW, ImgH);
		// 指定範囲が分割範囲を超えている
		if (ImgW > pClipSrcRec->right - pClipSrcRec->left ||
			ImgH > pClipSrcRec->bottom - pClipSrcRec->top)
		{	// Release
			SafeRelease(( *pDest ));
			return FALSE;
		}
		// 指定範囲分のテクスチャを作成
		if (FAILED(D3DXCreateTexture(pDev
			, pClipSrcRec->right, pClipSrcRec->bottom
			, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT
			, &pTempAdx)))
		{	// Release
			SafeRelease(( *pDest ));
			return FALSE;
		}
		// 元テクスチャから指定範囲を切り取る
		BOOL res = Split(*pDest, pTempAdx, &recSrc);
		SafeRelease( ( *pDest ));
		if (!res)	return FALSE;
		*pDest = pTempAdx;

		// width/height
		if (((DWORD)width && (DWORD)height))
		{
			*width = pClipSrcRec->right - pClipSrcRec->left;
			*height = pClipSrcRec->bottom - pClipSrcRec->top;
		}
	}
	else
	{
		// ImageInfo
		if (((DWORD)width && (DWORD)height))
			GetImageInfo(pSrc, size, width, height);
	}

	return TRUE;
}


BOOL TextureLoader::SplitTextureFromFileInMemory(vector<LPDIRECT3DTEXTURE9> *pDest, LPDIRECT3DDEVICE9 pDev, LPCVOID pSrc, UINT size, int width, int height, int splits, RECT* pClipSrcRec)
{
	if (pDest)	return FALSE;

	int nStartIndex = pDest->size()-1;
	BOOL bres;

	if ((width + height + splits) == 0)
		return FALSE;
	else
	{
		int ImgW,ImgH;
		LPDIRECT3DTEXTURE9 adx = NULL;
		// Load
		if(FAILED(D3DXCreateTextureFromFileInMemory(pDev,pSrc, size, &adx)))
			return FALSE;
		// ImageInfo
		if (((DWORD)width && (DWORD)height))
			GetImageInfo(pSrc, size, &ImgW, &ImgH);

		// 元画像の範囲を指定している場合
		if (pClipSrcRec)
		{
			LPDIRECT3DTEXTURE9 pTempAdx;
			RECT recSrc;
			SetRect(&recSrc, 0,0, ImgW, ImgH);
			// 指定範囲が分割範囲を超えている
			if (ImgW > pClipSrcRec->right - pClipSrcRec->left ||
				ImgH > pClipSrcRec->bottom - pClipSrcRec->top)
			{	// Release
				SafeRelease(adx);
				return FALSE;
			}
			// 指定範囲分のテクスチャを作成
			if (FAILED(D3DXCreateTexture(pDev
				, pClipSrcRec->right , pClipSrcRec->bottom
				, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT
				, &pTempAdx)))
			{	// Release
				SafeRelease(adx);
				return FALSE;
			}
			// 元テクスチャから指定範囲を切り取る
			BOOL res = Split(adx, pTempAdx, &recSrc);
			SafeRelease(adx);
			if (!res)	return FALSE;
			adx = pTempAdx;
		}

		// Split
		bres = SplitTexture(pDest, pDev, adx, width, height, splits, ImgW, ImgH);
		// Release
		SafeRelease(adx);
		if(!bres)
		{
			vector<LPDIRECT3DTEXTURE9>::iterator it = pDest->begin();
			for (int i=0;it != pDest->end();i++)
			{
				if (i < nStartIndex)
					it++;
				else
				{
					SafeDelete(*it);
					it = pDest->erase(it);
				}
			}
			return FALSE;
		}
	}
	return TRUE;
}

// メモリ内のファイルから情報取得
void TextureLoader::GetImageInfo(LPCVOID pSrc, UINT size, int *width, int *height)
{
	D3DXIMAGE_INFO	imginfo;
	if (((DWORD)width && (DWORD)height))
	{
		if(FAILED(D3DXGetImageInfoFromFileInMemory(pSrc, size, &imginfo)))
		{
			*width = *height = 0;
		}
		else
		{
			*width = imginfo.Width;
			*height= imginfo.Height;
		}
	}
}

/*
	テクスチャを分割
	pSrc:分割元 / pSplits:分割したテクスチャリスト
	w:Width / h:Height / in:分割数
*/
// iw,ih		分割サイズ
// in			分割数
// sw,sh		元画像のサイズ
BOOL TextureLoader::SplitTexture(vector<LPDIRECT3DTEXTURE9> *pDest, LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 pSrc,int iw, int ih, int in, int sw, int sh)
{
	if (!pSrc) return FALSE;

	// サイズのチェック
	if (iw>sw ||ih>sh)
		return FALSE;
	// 0なら
	if (!in)
		return FALSE;

	// 0なら全体にする
	if (iw==0)
		iw = sw;
	if (ih==0)
		ih = sh;

	RECT rec;
	SetRect(&rec,0,0,0,ih);

	int i=0;
	// 分割
	for (i=0;i<in;i++)
	{
		rec.right += iw;
		rec.left= rec.right-iw;

		if (rec.right>sw)
		{
			SetRect(&rec,0,rec.top+ih, iw, rec.bottom+ih);
			// 画像の範囲を超えたら終わる
			if (rec.bottom>sh) break;
		}

		LPDIRECT3DTEXTURE9 new_tex = NULL;

		if (FAILED(D3DXCreateTexture(pDev
			, iw, ih
			, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT
			, &new_tex)))
			break;

		if (!Split(pSrc, new_tex, &rec, NULL))
			break;
		pDest->push_back(new_tex);
	}

	// 途中でループを抜けていたら失敗>>破棄
	if (i<in)
		return FALSE;

	return TRUE;
}

// 範囲を指定してテクスチャ(のサーフェース)を取得
BOOL TextureLoader::Split(LPDIRECT3DTEXTURE9 pSrc, LPDIRECT3DTEXTURE9 pDest, RECT *pSrcRec, RECT *pDestRec)
{
#if _DEBUG
	if(!pSrc || !pDest)
        return DXTRACE_ERR( TEXT("CSplitTextures::Split-in error"), FALSE );   
#else
	if(!pSrc || !pDest)
        return FALSE;   
#endif
	LPDIRECT3DSURFACE9	d_psf, s_psf;
	HRESULT hr1, hr2;

	// 転送先のサーフェース
	hr1 = pDest->GetSurfaceLevel(0, &d_psf);
	// 転送元のサーフェース
	hr2 = pSrc->GetSurfaceLevel(0, &s_psf);

	if (hr1 != D3D_OK || hr2 != D3D_OK)
	{
		SafeRelease(d_psf);
		SafeRelease(s_psf);
		return FALSE;
	}

	// 転送
	hr1 = D3DXLoadSurfaceFromSurface(
		  d_psf					// pDestSurface
		, NULL
		, pDestRec
		, s_psf
		, NULL
		, pSrcRec
		, D3DX_FILTER_NONE		// Filter
		, TEXTURE_LOADER_COLOR_KEY		// ColorKey
		);
	SafeRelease(d_psf);
	SafeRelease(s_psf);
	return (hr1 == D3D_OK);
}

// テキストからテクスチャ作成から挿入
BOOL TextureLoader::CreateSpTexsFromText(LPDIRECT3DDEVICE9 pDev, CSpTexs** ppNewTexs, const TCHAR* wd, DWORD color, LOGFONT *plf)
{
//	BOOL res = GetWordTextures(pDevice, ppNewTexs, wd, color, plf);
	// 引数チェック
	if (!pDev || !ppNewTexs || !wd || !plf)
		return FALSE;

	// テクスチャ配列クラスNew
	*ppNewTexs = new CSpTexs();
	CSpTexs *pNewTexs = *ppNewTexs;

	// フォント作成
	HFONT hFont;
	if (!(hFont = CreateFontIndirect(plf)))
	{
		SafeDelete(pNewTexs);
		return FALSE;
	}

	HDC hDC = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

	// フォント枠情報
	TEXTMETRIC TxtMet;
	GetTextMetrics(hDC, &TxtMet);

	// 文字数
	int nWordCount = GetWordNum(wd);

	// 文字数分テクスチャ作成
	if (!pNewTexs->Create(nWordCount))
	{
		SelectObject(hDC, oldFont);
		DeleteObject(hFont);
		ReleaseDC(NULL, hDC);
		SafeDelete(pNewTexs);
		return FALSE;
	}

	TCHAR *c = (TCHAR*)wd;
	UINT code;
	int nOffset=0;

	for (int i=0;i<nWordCount;i++, c+=nOffset)
	{
		// 文字コード取得
		nOffset = ChangeWordCode(c, &code);

		// 文字のビットマップ作成
		GLYPHMETRICS GlphMet;
		const MAT2 Mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hDC, code, GGO_GRAY4_BITMAP, &GlphMet, 0, NULL, &Mat);
		BYTE *ptr = new BYTE[size];
		GetGlyphOutline(hDC, code, GGO_GRAY4_BITMAP, &GlphMet, size, ptr, &Mat);

		// フォント情報
		int nOfs_x = GlphMet.gmptGlyphOrigin.x;
		int nOfs_y = TxtMet.tmAscent- GlphMet.gmptGlyphOrigin.y;
		int nBmp_w = GlphMet.gmBlackBoxX + (4-(GlphMet.gmBlackBoxX%4))%4;
		int	nBmp_h = GlphMet.gmBlackBoxY;

		// テクスチャ作成
		LPDIRECT3DTEXTURE9 pTex = NULL;

#if DIRECT3D_VERSION >= 0x0900
		if ((FAILED(pDev->CreateTexture( GlphMet.gmCellIncX, TxtMet.tmHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL)))
		&&	(FAILED(pDev->CreateTexture( GlphMet.gmCellIncX, TxtMet.tmHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex, NULL)))
		)
#else
		if ((FAILED(g_pDevice->CreateTexture( GlphMet.gmCellIncX, TxtMet.tmHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex)))
		&&	(FAILED(g_pDevice->CreateTexture( GlphMet.gmCellIncX, TxtMet.tmHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTex)))
		)
#endif // DIRECT3D_VERSION >= 0x0900
		{
			SelectObject(hDC, oldFont);
			DeleteObject(hFont);
			ReleaseDC(NULL, hDC);

			SafeDelete(ptr);
			return FALSE;
		}
		// サーフェースロック
		D3DLOCKED_RECT LockedRect;
		if ((FAILED(pTex->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD)))
		&&  (FAILED(pTex->LockRect(0, &LockedRect, NULL, 0)))
		)
		{
			SelectObject(hDC, oldFont);
			DeleteObject(hFont);
			ReleaseDC(NULL, hDC);

			SafeDelete(ptr);
			return FALSE;
		}

		FillMemory(LockedRect.pBits, LockedRect.Pitch * TxtMet.tmHeight, 0);
		if (!(color & 0xFF000000))
		{
			SelectObject(hDC, oldFont);
			DeleteObject(hFont);
			ReleaseDC(NULL, hDC);

			pTex->UnlockRect(0);
			SafeDelete(ptr);
			return FALSE;
		}
		// GetGlyphOutlineでGGO_GRAY2_BITMAP
	//	int Level = (33-1) * ((float)(color>>24) / 255);
		// GetGlyphOutlineでGGO_GRAY4_BITMAP
		int Level = (17-1) * (int)((float)(color>>24) / 255);
		// GetGlyphOutlineでGGO_GRAY8_BITMAP
	//	int Level = (5-1) * ((float)(color>>24) / 255);
		DWORD FontAlpha, Color;
		DWORD inColor = 0x00FFFFFF & color;
		int x,y;

		for (y=nOfs_y; y<nOfs_y+nBmp_h; y++)
		{
			for (x=nOfs_x; x<nOfs_x+nBmp_w; x++)
			{
				if (ptr[x-nOfs_x+nBmp_w*(y-nOfs_y)])
				{
					FontAlpha = (((ptr[x-nOfs_x+nBmp_w*(y-nOfs_y)]) * (Level)) -1);

					Color = inColor | (FontAlpha<<24);
					memcpy((BYTE*)LockedRect.pBits+LockedRect.Pitch*y+4*x, &Color, sizeof(DWORD));
				}
			}
		}
		pTex->UnlockRect(0);
		SafeDelete(ptr);
		
		pNewTexs->Set(pTex, i);
	}
	SelectObject(hDC, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hDC);
	return TRUE;
}
