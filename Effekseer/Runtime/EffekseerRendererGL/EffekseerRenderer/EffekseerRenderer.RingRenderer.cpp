
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
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
static const char g_ring_vs_src[] = (
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

static const char g_ring_fs_src[] = (
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
	{"atTexCoord", GL_FLOAT, 2, 16, false}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::RingRenderer( RendererImplemented* renderer, Shader* shader )
	: m_renderer	( renderer )
	, m_shader		( shader )
{
	// 頂点属性IDを取得
	shader->GetAttribIdList(3, g_sprite_attribs, m_aid);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::~RingRenderer()
{
	ES_SAFE_DELETE( m_shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* RingRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	
	Shader* shader = Shader::Create( renderer, 
		g_ring_vs_src, g_ring_fs_src, "RingRenderer" );
	if( shader == NULL ) return NULL;

	return new RingRenderer( renderer, shader );
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
	
	uint32_t outerColor = 
		(instanceParamater.OuterColor.R) +
		(instanceParamater.OuterColor.G <<  8) +
		(instanceParamater.OuterColor.B << 16) +
		(instanceParamater.OuterColor.A << 24);
	uint32_t innerColor = 
		(instanceParamater.InnerColor.R) +
		(instanceParamater.InnerColor.G <<  8) +
		(instanceParamater.InnerColor.B << 16) +
		(instanceParamater.InnerColor.A << 24);
	uint32_t centerColor = 
		(instanceParamater.CenterColor.R) +
		(instanceParamater.CenterColor.G <<  8) +
		(instanceParamater.CenterColor.B << 16) +
		(instanceParamater.CenterColor.A << 24);

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
		
		
		// 拡大
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
