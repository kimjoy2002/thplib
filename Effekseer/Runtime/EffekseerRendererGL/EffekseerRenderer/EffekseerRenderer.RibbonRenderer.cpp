
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
static const char g_ribbon_vs_src[] = (
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

static const char g_ribbon_fs_src[] = (
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


static ShaderAttribInfo g_ribbon_attribs[3] = {
	{"atPosition", GL_FLOAT, 3, 0, false},
	{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
	{"atTexCoord", GL_FLOAT, 2, 16, false}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::RibbonRenderer( RendererImplemented* renderer, Shader* shader )
	: m_renderer	( renderer )
	, m_shader		( shader )
{
	// 頂点属性IDを取得
	shader->GetAttribIdList(3, g_ribbon_attribs, m_aid);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::~RibbonRenderer()
{
	ES_SAFE_DELETE( m_shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer* RibbonRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );

	Shader* shader = Shader::Create( renderer, g_ribbon_vs_src, g_ribbon_fs_src, "RibbonRenderer" );
	if( shader == NULL ) return NULL;

	return new RibbonRenderer( renderer, shader );
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

			verteies[i].Col = instanceParamater.Colors[i];	
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

			// 拡大
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
	
	m_shader->BeginScene();
	
	if (paramater.ColorTextureIndex >= 0)
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
	m_shader->SetVertex(3, g_ribbon_attribs, m_aid, NULL, sizeof(Vertex));
	
	glDrawElements(GL_TRIANGLES, m_ribbonCount * 6, GL_UNSIGNED_SHORT, NULL);

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
