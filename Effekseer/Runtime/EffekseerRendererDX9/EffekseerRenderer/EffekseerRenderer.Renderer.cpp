
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.RendererImplemented.h"

#include "EffekseerRenderer.VertexBuffer.h"
#include "EffekseerRenderer.IndexBuffer.h"
#include "EffekseerRenderer.DeviceObject.h"
#include "EffekseerRenderer.SpriteRenderer.h"
#include "EffekseerRenderer.RibbonRenderer.h"
#include "EffekseerRenderer.RingRenderer.h"
#include "EffekseerRenderer.TextureLoader.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer* Renderer::Create( LPDIRECT3DDEVICE9 device, int32_t squareMaxCount )
{
	RendererImplemented* renderer = new RendererImplemented( squareMaxCount );
	if( renderer->Initialize( device ) )
	{
		return renderer;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented( int32_t squareMaxCount )
	: m_reference	( 1 )
	, m_d3d_device	( NULL )
	, m_vertexBuffer( NULL )
	, m_indexBuffer	( NULL )
	, m_squareMaxCount	( squareMaxCount )
	, m_coordinateSystem	( ::Effekseer::COORDINATE_SYSTEM_RH )
	, m_state_vertexDeclaration	( NULL )
	, m_state_streamData ( NULL )
	, m_state_IndexData	( NULL )
	, m_state_pTexture	( NULL )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
	assert( m_reference == 0 );
	ES_SAFE_DELETE( m_vertexBuffer );
	ES_SAFE_DELETE( m_indexBuffer );
	assert( m_reference == -2 );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnLostDevice()
{
	std::set<DeviceObject*>::iterator it = m_deviceObjects.begin();
	std::set<DeviceObject*>::iterator it_end = m_deviceObjects.end();
	while( it != it_end )
	{
		(*it)->OnLostDevice();
		it++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnResetDevice()
{
	std::set<DeviceObject*>::iterator it = m_deviceObjects.begin();
	std::set<DeviceObject*>::iterator it_end = m_deviceObjects.end();
	while( it != it_end )
	{
		(*it)->OnResetDevice();
		it++;
	}
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize( LPDIRECT3DDEVICE9 device )
{
	m_d3d_device = device;

	// 頂点の生成
	{
		// 最大でfloat * 10 と仮定
		m_vertexBuffer = VertexBuffer::Create( this, sizeof(float) * 10 * m_squareMaxCount * 4, true );
		if( m_vertexBuffer == NULL ) return false;
	}

	// インデックスの生成
	{
		m_indexBuffer = IndexBuffer::Create( this, m_squareMaxCount * 6, false );
		if( m_indexBuffer == NULL ) return false;

		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect( 6 );
			buf[0] = 3 + 4 * i;
			buf[1] = 1 + 4 * i;
			buf[2] = 0 + 4 * i;
			buf[3] = 3 + 4 * i;
			buf[4] = 0 + 4 * i;
			buf[5] = 2 + 4 * i;
		}

		m_indexBuffer->Unlock();
	}

	// 参照カウントの調整
	// m_vertexBufferの参照カウンタ
	Release();
	// m_indexBufferの参照カウンタ
	Release();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::AddRef()
{
	m_reference++;
	return m_reference;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::Release()
{
	m_reference--;
	int count = m_reference;
	if ( count == 0 )
	{
		delete this;
	}
	return count;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destory()
{
	Release();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	assert( m_d3d_device != NULL );

	::Effekseer::Matrix44::Mul( m_cameraProj, m_camera, m_proj );

	// ステートを保存する
	GetDevice()->GetRenderState( D3DRS_ALPHABLENDENABLE, &m_state_D3DRS_ALPHABLENDENABLE );
	GetDevice()->GetRenderState( D3DRS_BLENDOP, &m_state_D3DRS_BLENDOP );
	GetDevice()->GetRenderState( D3DRS_DESTBLEND, &m_state_D3DRS_DESTBLEND );
	GetDevice()->GetRenderState( D3DRS_SRCBLEND, &m_state_D3DRS_SRCBLEND );
	GetDevice()->GetRenderState( D3DRS_ALPHAREF, &m_state_D3DRS_ALPHAREF );

	GetDevice()->GetRenderState( D3DRS_ZENABLE, &m_state_D3DRS_ZENABLE );
	GetDevice()->GetRenderState( D3DRS_ZWRITEENABLE, &m_state_D3DRS_ZWRITEENABLE );
	GetDevice()->GetRenderState( D3DRS_ALPHATESTENABLE, &m_state_D3DRS_ALPHATESTENABLE );
	GetDevice()->GetRenderState( D3DRS_CULLMODE, &m_state_D3DRS_CULLMODE );

	GetDevice()->GetRenderState( D3DRS_COLORVERTEX, &m_state_D3DRS_COLORVERTEX );
	GetDevice()->GetRenderState( D3DRS_LIGHTING, &m_state_D3DRS_LIGHTING );
	GetDevice()->GetRenderState( D3DRS_SHADEMODE, &m_state_D3DRS_SHADEMODE );

	GetDevice()->GetVertexDeclaration( &m_state_vertexDeclaration );
	GetDevice()->GetStreamSource( 0, &m_state_streamData, &m_state_OffsetInBytes, &m_state_pStride );
	GetDevice()->GetIndices( &m_state_IndexData );
	
	GetDevice()->GetTexture( 0, &m_state_pTexture );

	GetDevice()->GetFVF( &m_state_FVF );

	// ステート初期値設定
	GetDevice()->SetTexture( 0, NULL );
	GetDevice()->SetFVF( 0 );

	GetDevice()->SetRenderState( D3DRS_COLORVERTEX, TRUE);
	GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE);
	GetDevice()->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	assert( m_d3d_device != NULL );

	// ステートを復元する
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, m_state_D3DRS_ALPHABLENDENABLE );
	GetDevice()->SetRenderState( D3DRS_BLENDOP, m_state_D3DRS_BLENDOP );
	GetDevice()->SetRenderState( D3DRS_DESTBLEND, m_state_D3DRS_DESTBLEND );
	GetDevice()->SetRenderState( D3DRS_SRCBLEND, m_state_D3DRS_SRCBLEND );
	GetDevice()->SetRenderState( D3DRS_ALPHAREF, m_state_D3DRS_ALPHAREF );

	GetDevice()->SetRenderState( D3DRS_ZENABLE, m_state_D3DRS_ZENABLE );
	GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, m_state_D3DRS_ZWRITEENABLE );
	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, m_state_D3DRS_ALPHATESTENABLE );
	GetDevice()->SetRenderState( D3DRS_CULLMODE, m_state_D3DRS_CULLMODE );

	GetDevice()->SetRenderState( D3DRS_COLORVERTEX, m_state_D3DRS_COLORVERTEX );
	GetDevice()->SetRenderState( D3DRS_LIGHTING, m_state_D3DRS_LIGHTING );
	GetDevice()->SetRenderState( D3DRS_SHADEMODE, m_state_D3DRS_SHADEMODE );

	GetDevice()->SetVertexDeclaration( m_state_vertexDeclaration );
	ES_SAFE_RELEASE( m_state_vertexDeclaration );

	GetDevice()->SetStreamSource( 0, m_state_streamData, m_state_OffsetInBytes, m_state_pStride );
	ES_SAFE_RELEASE( m_state_streamData );

	GetDevice()->SetIndices( m_state_IndexData );
	ES_SAFE_RELEASE( m_state_IndexData );

	GetDevice()->SetTexture( 0, m_state_pTexture );
	ES_SAFE_RELEASE( m_state_pTexture );

	GetDevice()->SetFVF( m_state_FVF );

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LPDIRECT3DDEVICE9 RendererImplemented::GetDevice()
{
	return m_d3d_device;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	return m_vertexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	return m_indexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t RendererImplemented::GetSquareMaxCount() const
{
	return m_squareMaxCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetProjectionMatrix() const
{
	return m_proj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetProjectionMatrix( const ::Effekseer::Matrix44& mat )
{
	m_proj = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetCameraMatrix() const
{
	return m_camera;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetCameraMatrix( const ::Effekseer::Matrix44& mat )
{
	m_camera = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
{
	return m_cameraProj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::ApplyAlphaBlend( ::Effekseer::eAlphaBlend alphaBlend )
{
	if( alphaBlend == ::Effekseer::ALPHA_BLEND_OPACITY )
	{
		GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		GetDevice()->SetRenderState( D3DRS_BLENDOP,   D3DBLENDOP_ADD );
		GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		GetDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		GetDevice()->SetRenderState( D3DRS_ALPHAREF,  127 );
	}
	else if( alphaBlend == ::Effekseer::ALPHA_BLEND_BLEND )
	{
		GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		GetDevice()->SetRenderState( D3DRS_BLENDOP,   D3DBLENDOP_ADD );
		GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		GetDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		GetDevice()->SetRenderState( D3DRS_ALPHAREF,  0 );
	}
	else if( alphaBlend == ::Effekseer::ALPHA_BLEND_ADD )
	{
		GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		GetDevice()->SetRenderState( D3DRS_BLENDOP,   D3DBLENDOP_ADD );
		GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		GetDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		GetDevice()->SetRenderState( D3DRS_ALPHAREF,  0 );
	}
	else if( alphaBlend == ::Effekseer::ALPHA_BLEND_SUB )
	{
		GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		GetDevice()->SetRenderState( D3DRS_BLENDOP,   D3DBLENDOP_REVSUBTRACT );
		GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		GetDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		GetDevice()->SetRenderState( D3DRS_ALPHAREF,  0 );
	}
	else if( alphaBlend == ::Effekseer::ALPHA_BLEND_MUL )
	{
		GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		GetDevice()->SetRenderState( D3DRS_BLENDOP,   D3DBLENDOP_ADD );
		GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		GetDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		GetDevice()->SetRenderState( D3DRS_ALPHAREF,  0 );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return SpriteRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return RibbonRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return RingRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader()
{
	return new TextureLoader( this );
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
