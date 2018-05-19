// =====================================================================
// グラフィック描画クラス
// =====================================================================

#include "CGraphic.h"

// 生成--------------------------------
CGraphic::CGraphic(/*CGame* gm*/)
{
	m_pD3DDevice	= NULL;
	m_pSprite		= NULL;
	SpriteTop	= new TSpriteInfo;
	SpriteTop->next	= NULL;
	SpriteIndex = SpriteTop;

	for (int i=0;i<eMaxBlendType;i++)
	{
		MeshTop[i] = new TMeshInfo;
		MeshTop[i]->next = NULL;
		MeshIndex[i] = MeshTop[i];
	}

	m_eBlendType = eNoBlend;
	BGColor = 0x00000000;
}

// 破棄--------------------------------
CGraphic::~CGraphic(void)
{
	ClearSpriteList();
	SafeRelease(m_pSprite);
	SafeDelete(SpriteTop);
	for (int i=0;i<eMaxBlendType;i++)
		SafeDelete(MeshTop[i]);
}

// Device初期化------------------------
BOOL CGraphic::Init(LPDIRECT3DDEVICE9 pDev)
{
	if(!pDev)	return FALSE;

	m_pD3DDevice = pDev;

	//スプライト作成
	if (FAILED(D3DXCreateSprite(m_pD3DDevice, &m_pSprite)))
		return FALSE;
///////////////////////////////////////////
	return TRUE;
}

// 描画初期設定------------------------
BOOL CGraphic::InitRender(HWND hWnd)
{	
	// レンダーステートを設定
	m_pD3DDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,	  TRUE );
#if DIRECT3D_VERSION >= 0x0900
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
#else
	m_pD3DDevice->SetTextureStageState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDevice->SetTextureStageState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
#endif

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1	, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP	, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2	, D3DTA_DIFFUSE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1	, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP	, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2	, D3DTA_DIFFUSE);

	m_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP	, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	// ライトの情報を設定
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.a = 1.0f; light.Ambient.a = 0.0f; light.Specular.a = 0.75f;
	light.Diffuse.r = 1.0f; light.Ambient.r = 0.0f; light.Specular.r = 0.75f;
	light.Diffuse.g = 1.0f; light.Ambient.g = 0.0f; light.Specular.g = 0.75f;
	light.Diffuse.b = 1.0f; light.Ambient.b = 0.0f; light.Specular.b = 0.75f;
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3(-0.4f, 0.4f, 0.8f) );
	SetLight(&light);

//	pD3DDevice->SetRenderState( D3DRS_AMBIENT, 0x00101010);
	RECT rect;
	GetClientRect(hWnd, &rect);
	float fWidth = (float)(rect.right-rect.left);
	float fHeight = (float)(rect.bottom-rect.top);

	// 投影マトリックスを設定する
	D3DXMATRIX matProj;
	FLOAT	   fAspect = fWidth / fHeight;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 1000.0f );
	m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	// カリングモード

	return TRUE;
}


// アスペクト比設定
void CGraphic::SetAspect(float fAspect)
{
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 1000.0f );
	m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

// ライト設定
void CGraphic::SetLight(D3DLIGHT9 *light)
{
	if (light)
	{
		m_pD3DDevice->SetLight(0, light);			// ライトを設定
		m_pD3DDevice->LightEnable(0, TRUE);
		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
	}
	else
	{
		m_pD3DDevice->SetLight(0, NULL);			// ライトを設定
		m_pD3DDevice->LightEnable(0, FALSE);
		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
	}
}

// アルファブレンド設定
void CGraphic::SetAlphaBlend(EBlendType eBlendType)
{
	if (m_eBlendType == eBlendType)
		return;
	m_eBlendType = eBlendType;
	switch (eBlendType)
	{
	case eNoBlend:
		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,	  TRUE );
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		return;
	case eABlend:
		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,	  TRUE );
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		return;
	case eAlphaBlend:
		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,	  D3DZB_FALSE );
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
//		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		return;
	case eFxBlend:
		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,	  TRUE );
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		return;
	case eInvBlend:
		m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,	  TRUE );
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCCOLOR);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);
//	case eNoType:
	default:
		return;
	}
}

// 描画
BOOL CALLBACK CGraphic::Draw(void)
{		
	BOOL bRender = FALSE;
	for (int i=0;i<eMaxBlendType;i++)
	{
		if (bRender = (MeshTop[i]->next != NULL))
			break;
	}
	if (bRender)
		RenderMesh();

	if (SpriteTop->next)
		RenderSprite();

	ClearMeshList();
	ClearSpriteList();

	return TRUE;
}

// スプライト描画
BOOL CGraphic::RenderSprite()
{
	static TSpriteInfo *ptemp;
	RECT*	r;
	ptemp = SpriteTop->next;

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	while(ptemp)
	{
		if (IsRectEmpty(r = &ptemp->rRect))
			r = NULL;
		if (ptemp->UseMatrix)
		{
/*
			m_pSprite->DrawTransform(
				ptemp->pTexture,
				r,
				&ptemp->Matrix,
				ptemp->Color);
*/
		}
		else
		{
#if DIRECT3D_VERSION >= 0x0900
//			D3DXVECTOR3 s = D3DXVECTOR3(ptemp->Trans.x,ptemp->Trans.y, 0.0f);
			D3DXVECTOR3 c = D3DXVECTOR3(ptemp->RotCenter.x,ptemp->RotCenter.y, 0.0f);
			D3DXVECTOR3 t = D3DXVECTOR3(ptemp->Trans.x,ptemp->Trans.y, 0.0f);
//			D3DXMATRIX mat,matTemp;
//			D3DXMatrixIdentity(&mat);
//			D3DXMatrixTranslation(&matTemp, ptemp->Trans.x,ptemp->Trans.y, 0.0f);
//			mat *= matTemp;
//			D3DXMatrixRotationZ(&matTemp, ptemp->Ang);
//			mat *= matTemp;
//			D3DXMatrixScaling(&matTemp, ptemp->Scal.x,ptemp->Scal.y, 1.0f);
//			mat *= matTemp;

			m_pSprite->Draw(
				ptemp->pTexture,
				r,
				&c,
				&t,
				ptemp->Color//D3DCOLOR_ARGB(ptemp->Color,0xff,0xff,0xff)
				);
#else
			m_pSprite->Draw(
				ptemp->pTexture,
				r,
				&ptemp->Scal,
				&ptemp->RotCenter,
				ptemp->Ang,
				&ptemp->Trans,
				ptemp->Color//D3DCOLOR_ARGB(ptemp->Color,0xff,0xff,0xff)
				);
#endif
		}
		ptemp = ptemp->next;
	}
	m_pSprite->End();

	return TRUE;
}

BOOL CGraphic::SpriteRenderTransform(
	LPDIRECT3DTEXTURE9 pTex,
	CONST RECT* pRect,
	CONST D3DXMATRIX* pTransform,
	D3DCOLOR Color)
{
	if (!pTex) return FALSE;

// DX8
//	HRESULT hr = m_pSprite->DrawTransform(pTex,pRect,pTransform,Color);
// DX9
	m_pSprite->SetTransform(pTransform);
	HRESULT hr = m_pSprite->Draw(pTex,pRect,NULL,NULL,Color);
	return hr;
}

//////////////////////////////////////////
// ---描画登録---
// Rect: 画像範囲
// Scal:倍率
// RotCenter: 回転中心位置
// Ang:回転角度
// Trans:座標
// argb: 透明度
///////////////////////////////////////////
BOOL CGraphic::RegistSpriteList(
	LPDIRECT3DTEXTURE9 pTex,
	RECT*		pRect,
	D3DXVECTOR2 Scal,
	D3DXVECTOR2 RotCenter,
	float Ang,
	D3DXVECTOR2 Trans,
	DWORD argb)
{
	if (!pTex) return FALSE;
	TSpriteInfo *ptemp = NULL;

	if(!(ptemp = new TSpriteInfo))
		return FALSE;
	
	ptemp->pTexture = pTex;
	if (pRect)
		CopyRect(&ptemp->rRect, pRect);
	else
		SetRectEmpty(&ptemp->rRect);
//	ptemp->pRect =pRect;
	ptemp->Scal		= Scal;
	ptemp->RotCenter= RotCenter;
	ptemp->Ang		= Ang;
	ptemp->Trans	= Trans;
	ptemp->UseMatrix= FALSE;
	ptemp->Color	= argb;
	ptemp->next		= NULL;
	
	SpriteIndex->next = ptemp;
	SpriteIndex = ptemp;

	return TRUE;
}


//////////////////////////////////////////
// ---描画登録---
// pRect	: 画像範囲
// pMatrix	: 座標
// Color	: 透明度
///////////////////////////////////////////
BOOL CGraphic::RegistSpriteList2(
		LPDIRECT3DTEXTURE9	pTex,
		RECT*				pRect,
		D3DXMATRIX			*pMatrix,
		DWORD				Color)
{
	if (!pTex) return FALSE;
	TSpriteInfo *ptemp = NULL;

	if(!(ptemp = new TSpriteInfo))
		return FALSE;
	
	ptemp->pTexture = pTex;

	if (pRect)
		CopyRect(&ptemp->rRect, pRect);
	else
		SetRectEmpty(&ptemp->rRect);
//	ptemp->pRect	= pRect;
	ptemp->UseMatrix= TRUE;
	ptemp->Matrix	= *pMatrix;
	ptemp->Color	= Color;
	ptemp->next		= NULL;
	
	SpriteIndex->next = ptemp;
	SpriteIndex = ptemp;

	return TRUE;
}


BOOL CGraphic::ClearSpriteList(void)
{
	static TSpriteInfo *ptemp = NULL;
	static TSpriteInfo *p = NULL;

	ptemp = SpriteTop->next;
	while (ptemp)
	{
		p = ptemp;
		ptemp = ptemp->next;
		delete p;
		p = NULL;
	}
	SpriteIndex = SpriteTop;
	SpriteTop->next = NULL;

	return TRUE;
}

//////////////////////////////////////////
// ---Mesh描画登録---
// pMesh		: CMesh
// pMatrix		: 座標
// EBlendType	: AlphaBlendType
///////////////////////////////////////////
BOOL CGraphic::RegistMeshList(
//		CMesh			*pMesh,
		D3DXMATRIX		*pMatrix,		// Matrix
		D3DMATERIAL9	*pMaterial,		// Material
		LPDIRECT3DTEXTURE9 pTexture,	// Texture
		EBlendType		eBlendType		// BlendType
	)
{
//	if (!pMesh) return FALSE;
	TMeshInfo *ptemp = NULL;

	if(!(ptemp = new TMeshInfo))
		return FALSE;
	
//	ptemp->pMesh = pMesh;

	if (pMatrix)
		ptemp->Matrix	= *pMatrix;
	else
		D3DXMatrixIdentity(&ptemp->Matrix);
	
	if (pMaterial)
		ptemp->Material = *pMaterial;
	else
		ptemp->UseMaterial = FALSE;

	ptemp->pTexture = pTexture;
//	ptemp->eBlendType = eBlendType;

	ptemp->next		= NULL;
	
	MeshIndex[eBlendType]->next = ptemp;
	MeshIndex[eBlendType] = ptemp;

	return TRUE;
}

BOOL CGraphic::ClearMeshList(void)
{
	static TMeshInfo *ptemp = NULL;
	static TMeshInfo *p = NULL;

	for (int i=0;i<eMaxBlendType;i++)
	{
		ptemp = MeshTop[i]->next;
		while (ptemp)
		{
			p = ptemp;
			ptemp = ptemp->next;
			delete p;
			p = NULL;
		}
		MeshIndex[i] = MeshTop[i];
		MeshTop[i]->next = NULL;
	}

	return TRUE;
}

// メッシュ描画
BOOL CGraphic::RenderMesh()
{
	static TMeshInfo *ptemp;
	for (int i=0;i<eMaxBlendType;i++)
	{
		ptemp = MeshTop[i]->next;
		if (ptemp)
			SetAlphaBlend((EBlendType)i);

		while(ptemp)
		{
			m_pD3DDevice->SetTransform(D3DTS_WORLD, &ptemp->Matrix);
/*
			if (ptemp->UseMaterial)
			{
				if (ptemp->pTexture)
					ptemp->pMesh->Render(&ptemp->Material, ptemp->pTexture);
				else
					ptemp->pMesh->Render(&ptemp->Material);
			}
			else if (ptemp->pTexture)
				ptemp->pMesh->Render(ptemp->pTexture);
			else
				ptemp->pMesh->Render();
*/
			ptemp = ptemp->next;
		}
		MeshIndex[i] = MeshTop[i];
		MeshTop[i]->next = NULL;
	}
	//m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	//m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	return TRUE;
}
