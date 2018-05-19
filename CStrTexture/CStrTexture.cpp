#include "CStrTexture.h"

void CStrTexture::AddChar(WCHAR wc, RECT *rc)
{
	_ASSERT(rc);
	RECT rcChar;
	CopyRect(&rcChar, rc);
	m_mapChar.insert( std::map<WCHAR, RECT>::value_type( wc, rcChar ) );
}

void CStrTexture::Render(
	WCHAR* str,
	LPD3DXSPRITE pSprite,
	int nPosX, int nPosY,
	D3DMATRIX* mat, float fScale,
	EStrTextureRenderHorizonAlign eSTRAlign,
	D3DCOLOR color,
	EStrTextureRenderHorizonAlign eSTRHCenter,
	EStrTextureRenderVerticalAlign eSTRVCenter
	)
{
	_ASSERT(pSprite);
	switch (eSTRAlign)
	{
	case eSTRAlignLeft:	RenderAlignLeft(str, pSprite, nPosX, nPosY, mat, fScale, color, eSTRHCenter, eSTRVCenter);	break;
	case eSTRAlignRight:	RenderAlignRight(str, pSprite, nPosX, nPosY, mat, fScale, color, eSTRHCenter, eSTRVCenter);	break;
	default: break;
	}
}

void CStrTexture::RenderAlignLeft(WCHAR* str, LPD3DXSPRITE pSprite, int nPosX, int nPosY, D3DMATRIX* mat, float fScale, D3DCOLOR color, EStrTextureRenderHorizonAlign eSTRHCenter, EStrTextureRenderVerticalAlign eSTRVCenter)
{
	RECT* prc = NULL;
	int nLeft = nPosX;
	float fTop = (float)nPosY;

	D3DXVECTOR3 vCenter;
	vCenter.z = 0.0f;

	D3DXMATRIXA16 matTransform;
	D3DXMatrixScaling( &matTransform, fScale, fScale, 1.0f );
	if (!mat)	pSprite->SetTransform( &(matTransform) );
	else		    pSprite->SetTransform( &(matTransform*(*mat)) );

	for (int i=0;str[i]!=NULL;i++)
	{
		std::map<WCHAR, RECT>::iterator itfind = m_mapChar.find(str[i]);
		if (itfind == m_mapChar.end())
		{
			if (prc)
				nLeft += (int)((float)CALC_RECT_WIDTH(prc)*fScale)+m_nCharOffset;
			continue;
		}
		prc = &((*itfind).second);
		D3DXVECTOR3 vPos = D3DXVECTOR3((float)nLeft/fScale, (float)fTop/fScale, 0.0f);
		SetCenterVector(&vCenter, prc, eSTRHCenter, eSTRVCenter);
		pSprite->Draw(m_pTexture, prc, &vCenter, &vPos, color);
		nLeft += ((int)((float)CALC_RECT_WIDTH(prc)*fScale)+m_nCharOffset);
	}
}

void CStrTexture::RenderAlignRight(WCHAR* str, LPD3DXSPRITE pSprite, int nPosX, int nPosY, D3DMATRIX* mat, float fScale, D3DCOLOR color, EStrTextureRenderHorizonAlign eSTRHCenter, EStrTextureRenderVerticalAlign eSTRVCenter)
{
	RECT* prc = NULL;
	int nLeft = nPosX;
	float fTop = (float)nPosY;
	D3DXVECTOR3 vCenter;
	vCenter.z = 0.0f;
	D3DXMATRIXA16 matTransform;
	D3DXMatrixScaling( &matTransform, fScale, fScale, 1.0f );
	if (!mat)	pSprite->SetTransform( &(matTransform) );
	else		    pSprite->SetTransform( &(matTransform*(*mat)) );
	
	for (int i = wcslen(str)-1;i>=0;i--)
	{
		std::map<WCHAR, RECT>::iterator itfind = m_mapChar.find(str[i]);
		if (itfind == m_mapChar.end())
		{
			if (prc)
				nLeft -= (int)((float)CALC_RECT_WIDTH(prc)*fScale)+m_nCharOffset;
			continue;
		}
		prc = &((*itfind).second);		
		nLeft -= ((int)((float)CALC_RECT_WIDTH(prc)*fScale)+m_nCharOffset);
		D3DXVECTOR3 vPos = D3DXVECTOR3((float)nLeft/fScale, fTop/fScale, 0.0f);
		SetCenterVector(&vCenter, prc, eSTRHCenter, eSTRVCenter);
		pSprite->Draw(m_pTexture, prc, &vCenter, &vPos, color);
	}
}
