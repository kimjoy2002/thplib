#ifndef H_CLASS_TEXTTEXTURE___
#define H_CLASS_TEXTTEXTURE___
// ==================================================================
// TextTexture.h
//	TextTexture �N���X�E�w�b�_�[ Version 1.0
//	Author : ue
// ==================================================================

#include "windows.h"
#include "TCHAR.h"
#include "TextureLoader.h"
#include "util.h"

#define MAX_TEXT_TEXTURE_SIZE	(256)

// IniFile�̎�舵���⏕�N���X
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