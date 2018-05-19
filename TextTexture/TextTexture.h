#ifndef H_CLASS_TEXTTEXTURE___
#define H_CLASS_TEXTTEXTURE___
// ==================================================================
// TextTexture.h
//	TextTexture クラス・ヘッダー Version 1.0
//	Author : ue
// ==================================================================

#include "windows.h"
#include "TCHAR.h"
#include "TextureLoader.h"
#include "util.h"

#define MAX_TEXT_TEXTURE_SIZE	(256)

// IniFileの取り扱い補助クラス
class TextTexture {
public:
	TextTexture();
	virtual ~TextTexture()
	{ };

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    LPCWSTR GetText() { return m_strText; }
    HRESULT SetText( LPCWSTR strText );

	virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
    virtual bool GetVisible() { return m_bVisible; }

private:
	
	WCHAR m_szText[MAX_TEXT_TEXTURE_SIZE+1];

};

#endif