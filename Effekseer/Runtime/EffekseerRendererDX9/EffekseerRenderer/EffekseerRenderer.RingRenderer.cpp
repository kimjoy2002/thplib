
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.VertexBuffer.h"
#include "EffekseerRenderer.IndexBuffer.h"
#include "EffekseerRenderer.RingRenderer.h"
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
#include "EffekseerRenderer.RingRenderer.Shader.h"
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderNoTexture
{
static
#include "EffekseerRenderer.RingRenderer.ShaderNoTexture.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::RingRenderer( RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture )
: m_renderer	( renderer )
	, m_shader		( shader )
	, m_shader_no_texture		( shader_no_texture )
	, m_vertexDeclaration	( NULL )
{
	// À•W(3) F(4) UV(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0,	12,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		{0,	16,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};

	renderer->GetDevice()->CreateVertexDeclaration( decl, &m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::~RingRenderer()
{
	ES_SAFE_DELETE( m_shader );
	ES_SAFE_DELETE( m_shader_no_texture );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* RingRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	ID3DXBuffer* buf = NULL;

	Shader* shader = Shader::Create( renderer, Shader_::g_effect, sizeof(Shader_::g_effect), "RingRenderer" );
	if( shader == NULL ) return NULL;

	Shader* shader_no_texture = Shader::Create( renderer, ShaderNoTexture::g_effect, sizeof(ShaderNoTexture::g_effect), "RingRenderer No Texture" );
	if( shader_no_texture == NULL )
	{
		ES_SAFE_DELETE( shader_no_texture );
	}

	return new RingRenderer( renderer, shader, shader_no_texture );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::BeginRendering( const efkRingNodeParam& paramater, void* userData )
{
	m_spriteCount = 0;
	m_renderer->GetVertexBuffer()->Lock();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::Rendering( const efkRingNodeParam& paramater, const efkRingInstanceParam& instanceParamater, void* userData )
{
	if( m_spriteCount == m_renderer->GetSquareMaxCount() ) return;

	int32_t vertexCount = paramater.VertexCount * 8;
	Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect(
		sizeof(Vertex) * vertexCount );

	float radian = instanceParamater.ViewingAngle / 180.0f * 3.141592f;
	float stepAngle = radian / (paramater.VertexCount);
	float currentAngle = -radian / 2.0f;
	
	float outerRadius = instanceParamater.OuterLocation.X;
	float innerRadius = instanceParamater.InnerLocation.X;
	float centerRadius = innerRadius + (outerRadius - innerRadius) * instanceParamater.CenterRatio;

	float outerHeight = instanceParamater.OuterLocation.Y;
	float innerHeight = instanceParamater.InnerLocation.Y;
	float centerHeight = innerHeight + (outerHeight - innerHeight) * instanceParamater.CenterRatio;
	
	D3DCOLOR outerColor = D3DCOLOR_ARGB(
		instanceParamater.OuterColor.A,
		instanceParamater.OuterColor.R,
		instanceParamater.OuterColor.G,
		instanceParamater.OuterColor.B);
	D3DCOLOR innerColor = D3DCOLOR_ARGB(
		instanceParamater.InnerColor.A,
		instanceParamater.InnerColor.R,
		instanceParamater.InnerColor.G,
		instanceParamater.InnerColor.B);
	D3DCOLOR centerColor = D3DCOLOR_ARGB(
		instanceParamater.CenterColor.A,
		instanceParamater.CenterColor.R,
		instanceParamater.CenterColor.G,
		instanceParamater.CenterColor.B);

	float c = cosf(currentAngle);
	float s = sinf(currentAngle);
	::Effekseer::Vector3D outerCurrent( c * outerRadius, s * outerRadius, outerHeight );
	::Effekseer::Vector3D innerCurrent( c * innerRadius, s * innerRadius, innerHeight );
	::Effekseer::Vector3D centerCurrent( c * centerRadius, s * centerRadius, centerHeight );
	float texCurrent = 0.0f;
	float texStep = 1.0f / paramater.VertexCount;

	for( int i = 0; i < vertexCount; i += 8 )
	{
		currentAngle += stepAngle;
		c = cosf(currentAngle);
		s = sinf(currentAngle);

		::Effekseer::Vector3D outerNext( c * outerRadius, s * outerRadius, outerHeight );
		::Effekseer::Vector3D innerNext( c * innerRadius, s * innerRadius, innerHeight );
		::Effekseer::Vector3D centerNext( c * centerRadius, s * centerRadius, centerHeight );
		float texNext = texCurrent + texStep;
		
		Vertex* v = &verteies[i];
		v[0].Pos = outerCurrent;
		v[0].Col = outerColor;
		v[0].UV[0] = texCurrent;
		v[0].UV[1] = 0.0f;

		v[1].Pos = centerCurrent;
		v[1].Col = centerColor;
		v[1].UV[0] = texCurrent;
		v[1].UV[1] = 0.5f;

		v[2].Pos = outerNext;
		v[2].Col = outerColor;
		v[2].UV[0] = texNext;
		v[2].UV[1] = 0.0f;
		
		v[3].Pos = centerNext;
		v[3].Col = centerColor;
		v[3].UV[0] = texNext;
		v[3].UV[1] = 0.5f;

		v[4].Pos = centerCurrent;
		v[4].Col = centerColor;
		v[4].UV[0] = texCurrent;
		v[4].UV[1] = 0.5f;

		v[5].Pos = innerCurrent;
		v[5].Col = innerColor;
		v[5].UV[0] = texCurrent;
		v[5].UV[1] = 1.0f;

		v[6].Pos = centerNext;
		v[6].Col = centerColor;
		v[6].UV[0] = texNext;
		v[6].UV[1] = 0.5f;

		v[7].Pos = innerNext;
		v[7].Col = innerColor;
		v[7].UV[0] = texNext;
		v[7].UV[1] = 1.0f;

		outerCurrent = outerNext;
		innerCurrent = innerNext;
		centerCurrent = centerNext;
		texCurrent = texNext;
	}

	if( paramater.Billboard == ::Effekseer::BillboardType_Billboard ||
		paramater.Billboard == ::Effekseer::BillboardType_RotatedBillboard ||
		paramater.Billboard == ::Effekseer::BillboardType_YAxisFixed)
	{
		const ::Effekseer::Matrix43& mat = instanceParamater.SRTMatrix43;
		::Effekseer::Vector3D s;
		::Effekseer::Matrix43 r;
		::Effekseer::Vector3D t;

		mat.GetSRT( s, r, t );

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
		
		
		// Šg‘å
		::Effekseer::Matrix43 mat_final, mat_scale;
		mat_scale.Scaling( s.X, s.Y, s.Z );
		::Effekseer::Matrix43::Multiple( mat_final, mat_rot, mat_scale );

		for( int i = 0; i < vertexCount; i++ )
		{
			::Effekseer::Vector3D::Transform(
				verteies[i].Pos,
				verteies[i].Pos,
				mat_final );
		}
	}
	else if( paramater.Billboard == ::Effekseer::BillboardType_Fixed )
	{
		for( int i = 0; i < vertexCount; i++ )
		{
			::Effekseer::Vector3D::Transform(
				verteies[i].Pos,
				verteies[i].Pos,
				instanceParamater.SRTMatrix43 );
		}
	}

	m_spriteCount += 2 * paramater.VertexCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::EndRendering( const efkRingNodeParam& paramater, void* userData )
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
