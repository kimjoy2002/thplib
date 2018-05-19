
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
static const char g_sprite_vs_src[] = (
#if defined(__EFFEKSEER_RENDERER_GLES2__)
#else
	"#version 110\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
#endif
	"attribute vec4 atPosition;\n"
	"attribute vec4 atColor;\n"
	"attribute vec4 atTexCoord;\n"
	"varying vec4 vaColor;\n"
	"varying vec4 vaTexCoord;\n"
	"uniform mat4 uMatProjection;\n"
	"void main() {\n"
	"	gl_Position = uMatProjection * atPosition;\n"
	"	vaColor = atColor;\n"
	"	vaTexCoord = atTexCoord;\n"
	"}\n"
);

static const char g_sprite_fs_src[] = (
#if defined(__EFFEKSEER_RENDERER_GLES2__)
	"precision mediump float;\n"
#else
	"#version 110\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
#endif
	"varying lowp vec4 vaColor;\n"
	"varying mediump vec4 vaTexCoord;\n"
	"uniform sampler2D uTexture0;\n"
	"uniform bool uTexEnable;\n"
	"void main() {\n"
	"	if (uTexEnable) {\n"
	"		gl_FragColor = vaColor * texture2D(uTexture0, vaTexCoord.xy);\n"
	"	} else {\n"
	"		gl_FragColor = vaColor;\n"
	"	}\n"
	"}\n"
);

static ShaderAttribInfo g_sprite_attribs[3] = {
	{"atPosition", GL_FLOAT, 3, 0, false},
	{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
	{"atTexCoord", GL_UNSIGNED_SHORT, 2, 16, true}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer::SpriteRenderer( RendererImplemented* renderer, Shader* shader )
	: m_renderer	( renderer )
	, m_shader		( shader )
{
	// 頂点属性IDを取得
	shader->GetAttribIdList(3, g_sprite_attribs, m_aid);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer::~SpriteRenderer()
{
	ES_SAFE_DELETE( m_shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer* SpriteRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	
	Shader* shader = Shader::Create( renderer, 
		g_sprite_vs_src, g_sprite_fs_src, "SpriteRenderer" );
	if( shader == NULL ) return NULL;

	return new SpriteRenderer( renderer, shader );
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
		
		verteies[i].Col = instanceParamater.Colors[i];
	}
	
	verteies[0].UV[0] = verteies[2].UV[0] = 0;
	verteies[0].UV[1] = verteies[1].UV[1] = 65535;
	verteies[3].UV[0] = verteies[1].UV[0] = 65535;
	verteies[3].UV[1] = verteies[2].UV[1] = 0;

	if( paramater.Billboard == ::Effekseer::BillboardType_Billboard ||
		paramater.Billboard == ::Effekseer::BillboardType_RotatedBillboard ||
		paramater.Billboard == ::Effekseer::BillboardType_YAxisFixed)
	{
		const ::Effekseer::Matrix43& mat = instanceParamater.SRTMatrix43;
		::Effekseer::Vector3D s;
		::Effekseer::Matrix43 r;
		::Effekseer::Vector3D t;
		mat.GetSRT( s, r, t );

		// 拡大
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
	
	m_shader->BeginScene();

	Texture* texture = NULL;
	if( paramater.ColorTextureIndex >= 0 )
	{
		// テクスチャ有り
		GLuint texture = (GLuint)paramater.EffectPointer->GetImage( paramater.ColorTextureIndex );
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glUniform1i(m_shader->GetUniformId("uTexEnable"), GL_TRUE);
	}
	else
	{
		// テクスチャ無し
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(m_shader->GetUniformId("uTexEnable"), GL_FALSE);
	}
	
	glUniformMatrix4fv(m_shader->GetUniformId("uMatProjection"),
		1, GL_FALSE, &m_renderer->GetCameraProjectionMatrix().Values[0][0]);

	m_renderer->ApplyAlphaBlend( paramater.AlphaBlend );
	
	glBindBuffer(GL_ARRAY_BUFFER, m_renderer->GetVertexBuffer()->GetInterface());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer->GetIndexBuffer()->GetInterface());
	m_shader->EnableAttribs(3, m_aid);
	m_shader->SetVertex(3, g_sprite_attribs, m_aid, NULL, sizeof(Vertex));
	
	glDrawElements(GL_TRIANGLES, m_spriteCount * 6, GL_UNSIGNED_SHORT, NULL);

	m_shader->DisableAttribs(3, m_aid);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_shader->EndScene();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
