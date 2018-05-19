#ifndef H_CLASS_GRAPHIC___
#define H_CLASS_GRAPHIC___

// ===================================================================
// CGraphics.h
//	�`��Ǘ��N���X�E�w�b�_�[
//�@2D/3D�̕`����Ǘ��B3D��Game�̒��ł��B
//	Author : ue
// ===================================================================

#include "util.h"

//--------------------------------------------
//	DirectX�I�u�W�F�N�g
//--------------------------------------------

enum EBlendType {
	//eNoType,					// �ύX���Ȃ�
	eNoBlend = 0,					// �A���t�@�u�����h
	eAlphaBlend,					// SRCALPHA / INVSRCALPHA
	eABlend,						// SRCALPHA / INVSRCALPHA
	eFxBlend,						// ONE / ONE
	eInvBlend,						// INVCOLOR / INVCOLOR
	eMaxBlendType,					// ��
};

// �X�v���C�g�`����
struct TSpriteInfo
{
	LPDIRECT3DTEXTURE9 pTexture;	// Texture
	RECT		rRect;				// Rect
	D3DXVECTOR2 Scal;				// Scal
	D3DXVECTOR2 RotCenter;			// ��]���S���W
	float		Ang;				// ��]�l
	D3DXVECTOR2 Trans;				// XY���W
	BOOL		UseMatrix;			// Matrix�g�p�̐���
	D3DXMATRIX	Matrix;				// Matrix
	DWORD Color;					// 2D:�����x/3D:�����F

	TSpriteInfo *next;				// ���ւ̃|�C���^
};

// ���b�V���`����
struct TMeshInfo
{
//	CMesh*			pMesh;			// Mesh
	D3DXMATRIX		Matrix;			// Matrix
	BOOL			UseMaterial;	// Material�g�p�̐���
	D3DMATERIAL9	Material;		// Material
	LPDIRECT3DTEXTURE9 pTexture;	// Texture
//	EBlendType		eBlendType;		// BlendType

	TMeshInfo		*next;			// ���ւ̃|�C���^
};

class CGame;

class CGraphic{
public:
	CGraphic();
	~CGraphic();

	// ������
	BOOL Init(LPDIRECT3DDEVICE9 pDev);
	BOOL InitRender(HWND hWnd);

	// �A�X�y�N�g��ݒ�
	void SetAspect(float fAspect);
	// ���C�e�B���O�ݒ�
	void SetLight(D3DLIGHT9 *light);
	// �A���t�@�u�����h�ݒ�
	void SetAlphaBlend(EBlendType eBlendType);
	EBlendType GetAlphaBlendType()
	{ return m_eBlendType;	};

	// BG�ݒ�
	inline void SetBGColor(D3DCOLOR clr)
	{	BGColor = clr;	};

	// Rect: �摜�͈�, Scal:�{��, RotCenter: ��]���S�ʒu, Ang:��]�p�x, Trans:���W, argb: �����x
	BOOL RegistSpriteList(
		LPDIRECT3DTEXTURE9 pTex,
		RECT*		pRect,
		D3DXVECTOR2 Scal,
		D3DXVECTOR2 RotCenter,
		float Ang,
		D3DXVECTOR2 Trans,
		DWORD argb
		);

	// Rect: �摜�͈�, Matrix:�s��, Color: �����F
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

	// pMesh: Mesh, Matrix: Matrix, eBlendType: �A���t�@�u�����h
	BOOL RegistMeshList(
//		CMesh*			pMesh,
		D3DXMATRIX*		pMatrix,		// Matrix
		D3DMATERIAL9	*pMaterial,		// Material
		LPDIRECT3DTEXTURE9 pTexture,	// Texture
		EBlendType		eBlendType		// BlendType
	);


	BOOL CALLBACK Draw(void);					// �`��

	D3DFORMAT			DisplayFormat;	// �t�H�[�}�b�g

	LPDIRECT3DDEVICE9 GetDevice()		// �f�o�C�X�擾
	{return m_pD3DDevice;};

	void ClearDrawList(void)			// �`��\��폜
	{
		ClearSpriteList();
		ClearMeshList();
	};

protected:
	BOOL ClearSpriteList(void);			// �o�^���ꂽ�X�v���C�g���N���A
	BOOL RenderSprite();			// �X�v���C�g��`��

	BOOL ClearMeshList(void);
	BOOL RenderMesh(void);				// ���b�V����`��

	LPDIRECT3DDEVICE9	m_pD3DDevice;		// �f�o�C�X

	LPD3DXSPRITE		m_pSprite;		// D3DSprite

	TSpriteInfo	*SpriteTop;
	TSpriteInfo *SpriteIndex;

	TMeshInfo	*MeshTop[eMaxBlendType];
	TMeshInfo	*MeshIndex[eMaxBlendType];

	D3DCOLOR	BGColor;
	EBlendType	m_eBlendType;
};

#endif