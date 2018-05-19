
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.TextureLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::TextureLoader( Renderer* renderer )
	: m_renderer	( renderer )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* TextureLoader::Load( const EFK_CHAR* path )
{
	FILE* fp_texture = _wfopen( (const wchar_t *)path, L"rb" );

	if( fp_texture != NULL )
	{
		IDirect3DTexture9* texture = NULL;

		fseek( fp_texture, 0, SEEK_END );
		size_t size_texture = ftell( fp_texture );
		char* data_texture = new char[size_texture];
		fseek( fp_texture, 0, SEEK_SET );
		fread( data_texture, 1, size_texture, fp_texture );
		fclose( fp_texture );

		D3DXCreateTextureFromFileInMemory( m_renderer->GetDevice(), data_texture, size_texture, &texture );

		delete [] data_texture;

		return (void*)texture;
	}

	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload( void* data )
{
	if( data != NULL )
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data;
		texture->Release();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
