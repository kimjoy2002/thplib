#ifndef H_CLASS_GRAPHIC___
#define H_CLASS_GRAPHIC___

// ===================================================================
// CGraphics.h
//	描画管理クラス・ヘッダー
//　2D/3Dの描画を管理。3DはGameの中でも可。
//	Author : ue
// ===================================================================

#include "util.h"

//--------------------------------------------
//	DirectXオブジェクト
//--------------------------------------------

enum EBlendType {
	//eNoType,					// 変更しない
	eNoBlend = 0,					// アルファブレンド
	eAlphaBlend,					// SRCALPHA / INVSRCALPHA
	eABlend,						// SRCALPHA / INVSRCALPHA
	eFxBlend,						// ONE / ONE
	eInvBlend,						// INVCOLOR / INVCOLOR
	eMaxBlendType,					// 数
};

// スプライト描画情報
struct TSpriteInfo
{
	LPDIRECT3DTEXTURE9 pTexture;	// Texture
	RECT		rRect;				// Rect
	D3DXVECTOR2 Scal;				// Scal
	D3DXVECTOR2 RotCenter;			// 回転中心座標
	float		Ang;				// 回転値
	D3DXVECTOR2 Trans;				// XY座標
	BOOL		UseMatrix;			// Matrix使用の是非
	D3DXMATRIX	Matrix;				// Matrix
	DWORD Color;					// 2D:透明度/3D:透明色

	TSpriteInfo *next;				// 次へのポインタ
};

// メッシュ描画情報
struct TMeshInfo
{
//	CMesh*			pMesh;			// Mesh
	D3DXMATRIX		Matrix;			// Matrix
	BOOL			UseMaterial;	// Material使用の是非
	D3DMATERIAL9	Material;		// Material
	LPDIRECT3DTEXTURE9 pTexture;	// Texture
//	EBlendType		eBlendType;		// BlendType

	TMeshInfo		*next;			// 次へのポインタ
};

class CGame;

class CGraphic{
public:
	CGraphic();
	~CGraphic();

	// 初期化
	BOOL Init(LPDIRECT3DDEVICE9 pDev);
	BOOL InitRender(HWND hWnd);

	// アスペクト比設定
	void SetAspect(float fAspect);
	// ライティング設定
	void SetLight(D3DLIGHT9 *light);
	// アルファブレンド設定
	void SetAlphaBlend(EBlendType eBlendType);
	EBlendType GetAlphaBlendType()
	{ return m_eBlendType;	};

	// BG設定
	inline void SetBGColor(D3DCOLOR clr)
	{	BGColor = clr;	};

	// Rect: 画像範囲, Scal:倍率, RotCenter: 回転中心位置, Ang:回転角度, Trans:座標, argb: 透明度
	BOOL RegistSpriteList(
		LPDIRECT3DTEXTURE9 pTex,
		RECT*		pRect,
		D3DXVECTOR2 Scal,
		D3DXVECTOR2 RotCenter,
		float Ang,
		D3DXVECTOR2 Trans,
		DWORD argb
		);

	// Rect: 画像範囲, Matrix:行列, Color: 透明色
	BOOL RegistSpriteList2(
		LPDIRECT3DTEXTURE9	pTex,
		RECT*				pRect,
		D3DXMATRIX			*pMatrix,
		DWORD				Color);

	BOOL SpriteRenderTransform(
		LPDIRECT3DTEXTURE9 pTex,
		CONST RECT* pRect,
		CONST D3DXMATRIX* pTransform,
		D3DCOLOR Color);

	// pMesh: Mesh, Matrix: Matrix, eBlendType: アルファブレンド
	BOOL RegistMeshList(
//		CMesh*			pMesh,
		D3DXMATRIX*		pMatrix,		// Matrix
		D3DMATERIAL9	*pMaterial,		// Material
		LPDIRECT3DTEXTURE9 pTexture,	// Texture
		EBlendType		eBlendType		// BlendType
	);


	BOOL CALLBACK Draw(void);					// 描画

	D3DFORMAT			DisplayFormat;	// フォーマット

	LPDIRECT3DDEVICE9 GetDevice()		// デバイス取得
	{return m_pD3DDevice;};

	void ClearDrawList(void)			// 描画予約削除
	{
		ClearSpriteList();
		ClearMeshList();
	};

protected:
	BOOL ClearSpriteList(void);			// 登録されたスプライトをクリア
	BOOL RenderSprite();			// スプライトを描画

	BOOL ClearMeshList(void);
	BOOL RenderMesh(void);				// メッシュを描画

	LPDIRECT3DDEVICE9	m_pD3DDevice;		// デバイス

	LPD3DXSPRITE		m_pSprite;		// D3DSprite

	TSpriteInfo	*SpriteTop;
	TSpriteInfo *SpriteIndex;

	TMeshInfo	*MeshTop[eMaxBlendType];
	TMeshInfo	*MeshIndex[eMaxBlendType];

	D3DCOLOR	BGColor;
	EBlendType	m_eBlendType;
};

#endif