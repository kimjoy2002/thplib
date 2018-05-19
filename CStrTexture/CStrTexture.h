#ifndef H_STR_TEXTURE___
#define H_STR_TEXTURE___
// ===================================================================
// CStrTexture.h
// 文字列とテクスチャを関連付ける・ヘッダー
// ===================================================================

#include <windows.h>
#include <TCHAR.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <map>
#include "util.h"

enum EStrTextureRenderHorizonAlign
{
	eSTRAlignLeft,
	eSTRAlignCenter,
	eSTRAlignRight,
};

enum EStrTextureRenderVerticalAlign
{
	eSTRAlignTop,
	eSTRAlignMiddle,
	eSTRAlignBottom,
};


class CStrTexture
{
public:
	CStrTexture(LPDIRECT3DTEXTURE9 pTexture)
	{
		m_nCharOffset = 0;
		SetTexture(pTexture);
	};
	~CStrTexture() {};
	
	void Clear()	{	m_mapChar.clear();	};
	void SetTexture(LPDIRECT3DTEXTURE9 pTexture)	{m_pTexture = pTexture;	};
	void AddChar(WCHAR wc, RECT* rc);

	void Render(WCHAR* str, LPD3DXSPRITE pSprite,  int nPosX, int nPosY, D3DMATRIX* mat=NULL, float fScale=1.0f, EStrTextureRenderHorizonAlign eSTRAlign = eSTRAlignLeft, D3DCOLOR color=0xFFFFFFFF, EStrTextureRenderHorizonAlign eSTRHCenter=eSTRAlignLeft, EStrTextureRenderVerticalAlign eSTRVCenter=eSTRAlignTop);

	void SetCharOffset(int value) { m_nCharOffset = value;	};
	int GetCharOffset() { return m_nCharOffset;	};
protected:
	void RenderAlignLeft(WCHAR* str, LPD3DXSPRITE pSprite, int nPosX, int nPosY, D3DMATRIX* mat, float fScale, D3DCOLOR color, EStrTextureRenderHorizonAlign eSTRHCenter, EStrTextureRenderVerticalAlign eSTRVCenter);
	void RenderAlignRight(WCHAR* str, LPD3DXSPRITE pSprite, int nPosX, int nPosY, D3DMATRIX* mat, float fScale, D3DCOLOR color, EStrTextureRenderHorizonAlign eSTRHCenter, EStrTextureRenderVerticalAlign eSTRVCenter);

	inline void SetCenterVector(D3DXVECTOR3* vec, RECT* prc, EStrTextureRenderHorizonAlign eSTRHCenter, EStrTextureRenderVerticalAlign eSTRVCenter)
	{
		switch (eSTRHCenter)
		{
		case eSTRAlignLeft:		vec->x = 0;		break;
		case eSTRAlignCenter:	vec->x = (float)CALC_RECT_WIDTH(prc)/2.0f;
		case eSTRAlignRight:		vec->x = (float)CALC_RECT_WIDTH(prc);
		}
		switch (eSTRVCenter)
		{
		case eSTRAlignTop:		vec->y = 0;		break;
		case eSTRAlignMiddle:	vec->y = (float)CALC_RECT_HEIGHT(prc)/2.0f;
		case eSTRAlignBottom:	vec->y = (float)CALC_RECT_HEIGHT(prc);
		}
	}
	
	int	m_nCharOffset;
	LPDIRECT3DTEXTURE9	m_pTexture;
	std::map<WCHAR, RECT>	m_mapChar;
};

#endif