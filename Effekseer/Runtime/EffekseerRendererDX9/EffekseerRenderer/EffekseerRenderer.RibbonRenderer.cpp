
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.VertexBuffer.h"
#include "EffekseerRenderer.IndexBuffer.h"
#include "EffekseerRenderer.RibbonRenderer.h"
#include "EffekseerRenderer.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Shader_Ribbon
{
static
#include "EffekseerRenderer.RibbonRenderer.Shader.h"
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderNoTexture_Ribbon
{
static
#include "EffekseerRenderer.RibbonRenderer.ShaderNoTexture.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::RibbonRenderer( RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture )
: m_renderer	( renderer )
	, m_shader		( shader )
	, m_shader_no_texture		( shader_no_texture )
	, m_vertexDeclaration	( NULL )
{
	// À•W(3) F(4) UV(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0,	12,	D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		{0,	28,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};

	renderer->GetDevice()->CreateVertexDeclaration( decl, &m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::~RibbonRenderer()
{
	ES_SAFE_DELETE( m_shader );
	ES_SAFE_DELETE( m_shader_no_texture );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer* RibbonRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	ID3DXBuffer* buf = NULL;

	Shader* shader = Shader::Create( renderer, Shader_Ribbon::g_effect, sizeof(Shader_Ribbon::g_effect), "RibbonRenderer" );
	if( shader == NULL ) return NULL;

	Shader* shader_no_texture = Shader::Create( renderer, ShaderNoTexture_Ribbon::g_effect, sizeof(ShaderNoTexture_Ribbon::g_effect), "RibbonRenderer No Texture" );
	if( shader_no_texture == NULL )
	{
		ES_SAFE_DELETE( shader_no_texture );
	}

	return new RibbonRenderer( renderer, shader, shader_no_texture );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RibbonRenderer::BeginRendering( const efkRibbonNodeParam& paramater, void* userData )
{
	m_ribbonCount = 0;
	m_renderer->GetVertexBuffer()->Lock();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RibbonRenderer::Rendering( const efkRibbonNodeParam& paramater, const efkRibbonInstanceParam& instanceParamater, void* userData )
{
	if( m_ribbonCount - 1 == m_renderer->GetSquareMaxCount() ) return;
	if( instanceParamater.InstanceCount < 2 ) return;

	bool isFirst = instanceParamater.InstanceIndex == 0;
	bool isLast = instanceParamater.InstanceIndex == (instanceParamater.InstanceCount - 1);

	Vertex* verteies = NULL;
	Vertex* _verteies = NULL;

	if( isFirst || isLast )
	{
		_verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 2 );
	}
	else
	{
		_verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
	}


	int loop = 0;
	if( isFirst || isLast )
	{
		loop = 1;
	}
	else
	{
		loop = 2;
	}
	
	for( int j = 0; j < loop; j++ )
	{
		verteies = &_verteies[j*2];

		for( int i = 0; i < 2; i++ )
		{
			verteies[i].Pos.X = instanceParamater.Positions[i];
			verteies[i].Pos.Y = 0.0f;
			verteies[i].Pos.Z = 0.0f;

			verteies[i].Col[0] = (float)instanceParamater.Colors[i].R / 255.0f;
			verteies[i].Col[1] = (float)instanceParamater.Colors[i].G / 255.0f;
			verteies[i].Col[2] = (float)instanceParamater.Colors[i].B / 255.0f;
			verteies[i].Col[3] = (float)instanceParamater.Colors[i].A / 255.0f;	
		}
		
		verteies[0].UV[0] = 0.0f;
		verteies[0].UV[1] = (float)instanceParamater.InstanceIndex / (float)instanceParamater.InstanceCount;

		verteies[1].UV[0] = 1.0f;
		verteies[1].UV[1] = (float)( instanceParamater.InstanceIndex + 1 ) / (float)instanceParamater.InstanceCount;
		
		if( paramater.ViewpointDependent)
		{
			const ::Effekseer::Matrix43& mat = instanceParamater.SRTMatrix43;
			::Effekseer::Vector3D s;
			::Effekseer::Matrix43 r;
			::Effekseer::Vector3D t;
			mat.GetSRT( s, r, t );

			// Šg‘å
			for( int i = 0; i < 2; i++ )
			{
				verteies[i].Pos.X = verteies[i].Pos.X * s.X;
			}

			::Effekseer::Vector3D F;
			::Effekseer::Vector3D R;
			::Effekseer::Vector3D U;

			U = ::Effekseer::Vector3D( r.Value[1][0], r.Value[1][1], r.Value[1][2] );

			const ::Effekseer::Matrix44& camera = m_renderer->GetCameraMatrix();
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

			::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, U, F ) );
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D::Cross( F, R, U ) );
			
			::Effekseer::Matrix43 mat_rot;

			mat_rot.Value[0][0] = - R.X;
			mat_rot.Value[0][1] = - R.Y;
			mat_rot.Value[0][2] = - R.Z;
			mat_rot.Value[1][0] = U.X;
			mat_rot.Value[1][1] = U.Y;
			mat_rot.Value[1][2] = U.Z;
			mat_rot.Value[2][0] = F.X;
			mat_rot.Value[2][1] = F.Y;
			mat_rot.Value[2][2] = F.Z;
			mat_rot.Value[3][0] = t.X;
			mat_rot.Value[3][1] = t.Y;
			mat_rot.Value[3][2] = t.Z;

			for( int i = 0; i < 2; i++ )
			{
				::Effekseer::Vector3D::Transform(
					verteies[i].Pos,
					verteies[i].Pos,
					mat_rot );
			}
		}
		else
		{
			for( int i = 0; i < 2; i++ )
			{
				::Effekseer::Vector3D::Transform(
					verteies[i].Pos,
					verteies[i].Pos,
					instanceParamater.SRTMatrix43 );
			}
		}
	}

	if( !isFirst )
	{
		m_ribbonCount++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RibbonRenderer::EndRendering( const efkRibbonNodeParam& paramater, void* userData )
{
	m_renderer->GetVertexBuffer()->Unlock();

	if( m_ribbonCount <= 1 ) return;

	m_renderer->GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	uint32_t pass;

	ID3DXEffect* shader = NULL;
	if( paramater.ColorTextureIndex >= 0 )
	{
		shader = m_shader->GetInterface();
	}
	else
	{
		shader = m_shader_no_texture->GetInterface();
	}

	shader->SetTechnique( "Texhnique" );
	shader->Begin( &pass, 0 );

	assert( pass == 1 );

	shader->BeginPass(0);

	IDirect3DTexture9* texture = NULL;
	if( paramater.ColorTextureIndex >= 0 )
	{
		texture = (IDirect3DTexture9*)paramater.EffectPointer->GetImage( paramater.ColorTextureIndex );
	}

	if( texture != NULL )
	{

		shader->SetTexture( "Tex0", texture );
	}

	shader->SetMatrix( "mCameraProj", (const D3DXMATRIX *)&m_renderer->GetCameraProjectionMatrix() );

	m_renderer->ApplyAlphaBlend( paramater.AlphaBlend );

	shader->CommitChanges();

	m_renderer->GetDevice()->SetVertexDeclaration( m_vertexDeclaration );
	m_renderer->GetDevice()->SetStreamSource( 0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex) );
	m_renderer->GetDevice()->SetIndices( m_renderer->GetIndexBuffer()->GetInterface() );
	m_renderer->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_ribbonCount * 4, 0, m_ribbonCount * 2 );

	shader->EndPass();
	shader->End();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
