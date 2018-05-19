#include "TextureLoader.h"

BOOL TextureLoader::ClipTexture(	LPDIRECT3DTEXTURE9 *pDest, LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 pSrc, RECT* pClipRec)
{
	// �w��͈͕��̃e�N�X�`�����쐬
	if (FAILED(D3DXCreateTexture(pDev
		, pClipRec->right, pClipRec->bottom
		, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT
		, pDest)))
		return FALSE;

	// ���e�N�X�`������w��͈͂�؂���
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

	// ���摜�͈̔͂��w�肵�Ă���ꍇ
	if (pClipSrcRec)
	{
		int ImgW,ImgH;
		GetImageInfo(pSrc, size, &ImgW, &ImgH);	
		LPDIRECT3DTEXTURE9 pTempAdx;
		RECT recSrc;
		SetRect(&recSrc, 0,0, ImgW, ImgH);
		// �w��͈͂������͈͂𒴂��Ă���
		if (ImgW > pClipSrcRec->right - pClipSrcRec->left ||
			ImgH > pClipSrcRec->bottom - pClipSrcRec->top)
		{	// Release
			SafeRelease(( *pDest ));
			return FALSE;
		}
		// �w��͈͕��̃e�N�X�`�����쐬
		if (FAILED(D3DXCreateTexture(pDev
			, pClipSrcRec->right, pClipSrcRec->bottom
			, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT
			, &pTempAdx)))
		{	// Release
			SafeRelease(( *pDest ));
			return FALSE;
		}
		// ���e�N�X�`������w��͈͂�؂���
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

		// ���摜�͈̔͂��w�肵�Ă���ꍇ
		if (pClipSrcRec)
		{
			LPDIRECT3DTEXTURE9 pTempAdx;
			RECT recSrc;
			SetRect(&recSrc, 0,0, ImgW, ImgH);
			// �w��͈͂������͈͂𒴂��Ă���
			if (ImgW > pClipSrcRec->right - pClipSrcRec->left ||
				ImgH > pClipSrcRec->bottom - pClipSrcRec->top)
			{	// Release
				SafeRelease(adx);
				return FALSE;
			}
			// �w��͈͕��̃e�N�X�`�����쐬
			if (FAILED(D3DXCreateTexture(pDev
				, pClipSrcRec->right , pClipSrcRec->bottom
				, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT
				, &pTempAdx)))
			{	// Release
				SafeRelease(adx);
				return FALSE;
			}
			// ���e�N�X�`������w��͈͂�؂���
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

// ���������̃t�@�C��������擾
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
	�e�N�X�`���𕪊�
	pSrc:������ / pSplits:���������e�N�X�`�����X�g
	w:Width / h:Height / in:������
*/
// iw,ih		�����T�C�Y
// in			������
// sw,sh		���摜�̃T�C�Y
BOOL TextureLoader::SplitTexture(vector<LPDIRECT3DTEXTURE9> *pDest, LPDIRECT3DDEVICE9 pDev, LPDIRECT3DTEXTURE9 pSrc,int iw, int ih, int in, int sw, int sh)
{
	if (!pSrc) return FALSE;

	// �T�C�Y�̃`�F�b�N
	if (iw>sw ||ih>sh)
		return FALSE;
	// 0�Ȃ�
	if (!in)
		return FALSE;

	// 0�Ȃ�S�̂ɂ���
	if (iw==0)
		iw = sw;
	if (ih==0)
		ih = sh;

	RECT rec;
	SetRect(&rec,0,0,0,ih);

	int i=0;
	// ����
	for (i=0;i<in;i++)
	{
		rec.right += iw;
		rec.left= rec.right-iw;

		if (rec.right>sw)
		{
			SetRect(&rec,0,rec.top+ih, iw, rec.bottom+ih);
			// �摜�͈̔͂𒴂�����I���
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

	// �r���Ń��[�v�𔲂��Ă����玸�s>>�j��
	if (i<in)
		return FALSE;

	return TRUE;
}

// �͈͂��w�肵�ăe�N�X�`��(�̃T�[�t�F�[�X)���擾
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

	// �]����̃T�[�t�F�[�X
	hr1 = pDest->GetSurfaceLevel(0, &d_psf);
	// �]�����̃T�[�t�F�[�X
	hr2 = pSrc->GetSurfaceLevel(0, &s_psf);

	if (hr1 != D3D_OK || hr2 != D3D_OK)
	{
		SafeRelease(d_psf);
		SafeRelease(s_psf);
		return FALSE;
	}

	// �]��
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

// �e�L�X�g����e�N�X�`���쐬����}��
BOOL TextureLoader::CreateSpTexsFromText(LPDIRECT3DDEVICE9 pDev, CSpTexs** ppNewTexs, const TCHAR* wd, DWORD color, LOGFONT *plf)
{
//	BOOL res = GetWordTextures(pDevice, ppNewTexs, wd, color, plf);
	// �����`�F�b�N
	if (!pDev || !ppNewTexs || !wd || !plf)
		return FALSE;

	// �e�N�X�`���z��N���XNew
	*ppNewTexs = new CSpTexs();
	CSpTexs *pNewTexs = *ppNewTexs;

	// �t�H���g�쐬
	HFONT hFont;
	if (!(hFont = CreateFontIndirect(plf)))
	{
		SafeDelete(pNewTexs);
		return FALSE;
	}

	HDC hDC = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hDC, hFont);

	// �t�H���g�g���
	TEXTMETRIC TxtMet;
	GetTextMetrics(hDC, &TxtMet);

	// ������
	int nWordCount = GetWordNum(wd);

	// ���������e�N�X�`���쐬
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
		// �����R�[�h�擾
		nOffset = ChangeWordCode(c, &code);

		// �����̃r�b�g�}�b�v�쐬
		GLYPHMETRICS GlphMet;
		const MAT2 Mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hDC, code, GGO_GRAY4_BITMAP, &GlphMet, 0, NULL, &Mat);
		BYTE *ptr = new BYTE[size];
		GetGlyphOutline(hDC, code, GGO_GRAY4_BITMAP, &GlphMet, size, ptr, &Mat);

		// �t�H���g���
		int nOfs_x = GlphMet.gmptGlyphOrigin.x;
		int nOfs_y = TxtMet.tmAscent- GlphMet.gmptGlyphOrigin.y;
		int nBmp_w = GlphMet.gmBlackBoxX + (4-(GlphMet.gmBlackBoxX%4))%4;
		int	nBmp_h = GlphMet.gmBlackBoxY;

		// �e�N�X�`���쐬
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
		// �T�[�t�F�[�X���b�N
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
		// GetGlyphOutline��GGO_GRAY2_BITMAP
	//	int Level = (33-1) * ((float)(color>>24) / 255);
		// GetGlyphOutline��GGO_GRAY4_BITMAP
		int Level = (17-1) * (int)((float)(color>>24) / 255);
		// GetGlyphOutline��GGO_GRAY8_BITMAP
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
