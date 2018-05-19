
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRenderer.Renderer.h>
#include <EffekseerRenderer/EffekseerRenderer.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRenderer.IndexBuffer.h>
#include "EffekseerTool.Grid.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Shader_
{
static
#include "EffekseerTool.Grid.Shader.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid::Grid( RendererImplemented* renderer, ID3DXEffect* shader )
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, m_shader		( shader )
	, m_vertexDeclaration	( NULL )
	, m_lineCount		( 0 )
{
	// ���W(3) �F(4)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0,	12,	D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		D3DDECL_END()
	};

	renderer->GetDevice()->CreateVertexDeclaration( decl, &m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid::~Grid()
{
	ES_SAFE_RELEASE( m_shader );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid* Grid::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	HRESULT hr;
	ID3DXBuffer* buf = NULL;

	ID3DXEffect* shader = NULL;

	hr = D3DXCreateEffect(
		renderer->GetDevice(),
		Shader_::g_effect,
		sizeof(Shader_::g_effect),
		NULL,
		NULL,
		0,
		NULL,
		&shader,
		&buf);

	if( FAILED(hr) )
	{
		printf( "Grid Error\n");
		if( buf != NULL )
		{
			printf( (char*)buf->GetBufferPointer() );
			printf("\n");
			buf->Release();
		}
		else
		{
			printf( "Unknown Error\n" );
		}

		return NULL;
	}

	return new Grid( renderer, shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::OnLostDevice()
{
	m_shader->OnLostDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::OnResetDevice()
{
	m_shader->OnResetDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::Rendering()
{
	m_lineCount = 0;

	m_renderer->GetVertexBuffer()->Lock();

	
	for( int i = -5; i <= 5; i++ )
	{
		DrawLine( 
			::Effekseer::Vector3D( i * 2.0f, 0.0f, 10.0f ),
			::Effekseer::Vector3D( i * 2.0f, 0.0f, -10.0f ),
			::Effekseer::Color( 255, 255, 255, 255 ) );

		DrawLine( 
			::Effekseer::Vector3D( 10.0f, 0.0f, i * 2.0f ),
			::Effekseer::Vector3D( -10.0f, 0.0f, i * 2.0f ),
			::Effekseer::Color( 255, 255, 255, 255 ) );
	}

	DrawLine( 
			::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ),
			::Effekseer::Vector3D( 2.0f, 0.0f, 0.0f ),
			::Effekseer::Color( 255, 0, 0, 255 ) );

	DrawLine( 
			::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ),
			::Effekseer::Vector3D( 0.0f, 2.0f, 0.0f ),
			::Effekseer::Color( 0, 255, 0, 255 ) );

	DrawLine( 
			::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ),
			::Effekseer::Vector3D( 0.0f, 0.0f, 2.0f ),
			::Effekseer::Color( 0, 0, 255, 255 ) );

	m_renderer->GetVertexBuffer()->Unlock();

	if( m_lineCount == 0 ) return;

	m_renderer->GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	uint32_t pass;

	ID3DXEffect* shader = NULL;
	shader = m_shader;

	shader->SetTechnique( "Texhnique" );
	shader->Begin( &pass, 0 );

	assert( pass == 1 );

	shader->BeginPass(0);

	shader->SetMatrix( "mCameraProj", (const D3DXMATRIX *)&m_renderer->GetCameraProjectionMatrix() );

	m_renderer->ApplyAlphaBlend( ::Effekseer::ALPHA_BLEND_BLEND );

	shader->CommitChanges();

	m_renderer->GetDevice()->SetVertexDeclaration( m_vertexDeclaration );
	m_renderer->GetDevice()->SetStreamSource( 0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex) );
	m_renderer->GetDevice()->SetIndices( NULL );
	m_renderer->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, m_lineCount );

	shader->EndPass();
	shader->End();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::DrawLine( const ::Effekseer::Vector3D& pos1, const ::Effekseer::Vector3D& pos2, const ::Effekseer::Color& col )
{
	Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 2 );

	verteies[0].Pos = pos1;
	verteies[1].Pos = pos2;
	
	for( int i = 0; i < 2; i++ )
	{
		verteies[i].Col[0] = (float)col.R / 255.0f;
		verteies[i].Col[1] = (float)col.G / 255.0f;
		verteies[i].Col[2] = (float)col.B / 255.0f;
		verteies[i].Col[3] = (float)col.A / 255.0f;
	}

	m_lineCount++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
