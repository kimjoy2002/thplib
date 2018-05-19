
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.VertexBuffer.h"
#include "EffekseerRenderer.IndexBuffer.h"
#include "EffekseerRenderer.SpriteRenderer.h"
#include "EffekseerRenderer.Shader.h"

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
#include "EffekseerRenderer.SpriteRenderer.Shader.h"
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderNoTexture
{
static
#include "EffekseerRenderer.SpriteRenderer.ShaderNoTexture.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer::SpriteRenderer( RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture )
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
SpriteRenderer::~SpriteRenderer()
{
	ES_SAFE_DELETE( m_shader );
	ES_SAFE_DELETE( m_shader_no_texture );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer* SpriteRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	ID3DXBuffer* buf = NULL;

	Shader* shader = Shader::Create( renderer, Shader_::g_effect, sizeof(Shader_::g_effect), "SpriteRenderer" );
	if( shader == NULL ) return NULL;

	Shader* shader_no_texture = Shader::Create( renderer, ShaderNoTexture::g_effect, sizeof(ShaderNoTexture::g_effect), "SpriteRenderer No Texture" );
	if( shader_no_texture == NULL )
	{
		ES_SAFE_DELETE( shader_no_texture );
	}

	return new SpriteRenderer( renderer, shader, shader_no_texture );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::BeginRendering( const efkSpriteNodeParam& paramater, void* userData )
{
	m_spriteCount = 0;
	m_renderer->GetVertexBuffer()->Lock();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::Rendering( const efkSpriteNodeParam& paramater, const efkSpriteInstanceParam& instanceParamater, void* userData )
{
	if( m_spriteCount == m_renderer->GetSquareMaxCount() ) return;

	Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );

	for( int i = 0; i < 4; i++ )
	{
		verteies[i].Pos.X = instanceParamater.Positions[i].X;
		verteies[i].Pos.Y = instanceParamater.Positions[i].Y;
		verteies[i].Pos.Z = 0.0f;

		verteies[i].Col[0] = (float)instanceParamater.Colors[i].R / 255.0f;
		verteies[i].Col[1] = (float)instanceParamater.Colors[i].G / 255.0f;
		verteies[i].Col[2] = (float)instanceParamater.Colors[i].B / 255.0f;
		verteies[i].Col[3] = (float)instanceParamater.Colors[i].A / 255.0f;	
	}
	
	verteies[0].UV[0] = 0.0f;
	verteies[0].UV[1] = 1.0f;

	verteies[1].UV[0] = 1.0f;
	verteies[1].UV[1] = 1.0f;
	
	verteies[2].UV[0] = 0.0f;
	verteies[2].UV[1] = 0.0f;

	verteies[3].UV[0] = 1.0f;
	verteies[3].UV[1] = 0.0f;

	if( paramater.Billboard == ::Effekseer::BillboardType_Billboard ||
		paramater.Billboard == ::Effekseer::BillboardType_RotatedBillboard ||
		paramater.Billboard == ::Effekseer::BillboardType_YAxisFixed)
	{
		const ::Effekseer::Matrix43& mat = instanceParamater.SRTMatrix43;
		::Effekseer::Vector3D s;
		::Effekseer::Matrix43 r;
		::Effekseer::Vector3D t;
		mat.GetSRT( s, r, t );

		// Šg‘å
		for( int i = 0; i < 4; i++ )
		{
			verteies[i].Pos.X = verteies[i].Pos.X * s.X;
			verteies[i].Pos.Y = verteies[i].Pos.Y * s.Y;
		}

		::Effekseer::Vector3D F;
		::Effekseer::Vector3D R;
		::Effekseer::Vector3D U;

		
		if( paramater.Billboard == ::Effekseer::BillboardType_Billboard )
		{
			::Effekseer::Vector3D Up( 0.0f, 1.0f, 0.0f );

			const ::Effekseer::Matrix44& camera = m_renderer->GetCameraMatrix();
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

			::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, Up, F ) );
			::Effekseer::Vector3D::Normal( U, ::Effekseer::Vector3D::Cross( U, F, R ) );
		}
		else if( paramater.Billboard == ::Effekseer::BillboardType_RotatedBillboard )
		{
			::Effekseer::Vector3D Up( 0.0f, 1.0f, 0.0f );

			const ::Effekseer::Matrix44& camera = m_renderer->GetCameraMatrix();
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

			::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, Up, F ) );
			::Effekseer::Vector3D::Normal( U, ::Effekseer::Vector3D::Cross( U, F, R ) );

			float c_zx = sqrt( 1.0f - r.Value[2][1] * r.Value[2][1] );
			float s_z = 0.0f;
			float c_z = 0.0f;

			if( abs( c_zx ) > 0.05f )
			{
				s_z = - r.Value[0][1] / c_zx;
				c_z = sqrt( 1.0f - s_z * s_z );
				if( r.Value[1][1] < 0.0f ) c_z = - c_z;
			}
			else
			{
				s_z = 0.0f;
				c_z = 1.0f;
			}
			
			::Effekseer::Vector3D r_temp = R;
			::Effekseer::Vector3D u_temp = U;

			R.X = r_temp.X * c_z + u_temp.X * s_z;
			R.Y = r_temp.Y * c_z + u_temp.Y * s_z;
			R.Z = r_temp.Z * c_z + u_temp.Z * s_z;

			U.X = u_temp.X * c_z - r_temp.X * s_z;
			U.Y = u_temp.Y * c_z - r_temp.Y * s_z;
			U.Z = u_temp.Z * c_z - r_temp.Z * s_z;
		}
		else if( paramater.Billboard == ::Effekseer::BillboardType_YAxisFixed )
		{
			U = ::Effekseer::Vector3D( r.Value[1][0], r.Value[1][1], r.Value[1][2] );

			const ::Effekseer::Matrix44& camera = m_renderer->GetCameraMatrix();
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

			::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, U, F ) );
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D::Cross( F, R, U ) );
		}
		
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

		for( int i = 0; i < 4; i++ )
		{
			::Effekseer::Vector3D::Transform(
				verteies[i].Pos,
				verteies[i].Pos,
				mat_rot );
		}
	}
	else if( paramater.Billboard == ::Effekseer::BillboardType_Fixed )
	{
		for( int i = 0; i < 4; i++ )
		{
			::Effekseer::Vector3D::Transform(
				verteies[i].Pos,
				verteies[i].Pos,
				instanceParamater.SRTMatrix43 );
		}
	}

	m_spriteCount++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::EndRendering( const efkSpriteNodeParam& paramater, void* userData )
{
	m_renderer->GetVertexBuffer()->Unlock();

	if( m_spriteCount == 0 ) return;

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
	m_renderer->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_spriteCount * 4, 0, m_spriteCount * 2 );

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
