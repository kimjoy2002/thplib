
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRenderer.Renderer.h>
#include <EffekseerRenderer/EffekseerRenderer.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRenderer.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRenderer.Shader.h>
#include "EffekseerTool.Guide.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace __Shader
{
static 
#include "EffekseerTool.Guide.Shader.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Guide::Guide( RendererImplemented* renderer, Shader* shader )
	: m_renderer	( renderer )
	, m_shader		( shader )
	, m_vertexDeclaration	( NULL )
{
	// À•W(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		D3DDECL_END()
	};

	renderer->GetDevice()->CreateVertexDeclaration( decl, &m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Guide::~Guide()
{
	ES_SAFE_DELETE( m_shader );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Guide* Guide::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	Shader* shader = Shader::Create( renderer, __Shader::g_effect, sizeof(__Shader::g_effect), "Guide" );
	
	if( shader == NULL ) return NULL;

	return new Guide( renderer, shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Guide::Rendering( int32_t width, int32_t height, int32_t guide_width, int32_t guide_height )
{
	m_renderer->GetVertexBuffer()->Lock();

	float ul_x = 0;
	float ul_y = 0;
	float ul_gx = (width - guide_width) / 2.0f;
	float ul_gy = (height - guide_height) / 2.0f;

	float ur_x = ul_x + width;
	float ur_y = ul_y;
	float ur_gx = ul_gx + guide_width;
	float ur_gy = ul_gy;

	float dl_x = ul_x;
	float dl_y = ul_y + height;
	float dl_gx = ul_gx;
	float dl_gy = ul_gy + guide_height;

	float dr_x = ur_x;
	float dr_y = dl_y;
	float dr_gx = ur_gx;
	float dr_gy = dl_gy;

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = ul_gx;
		verteies[0].y = ul_gy;
		verteies[1].x = ul_x;
		verteies[1].y = ul_y;
		verteies[3].x = ur_x;
		verteies[3].y = ur_y;
		verteies[2].x = ur_gx;
		verteies[2].y = ur_gy;
	}

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = ul_x;
		verteies[0].y = ul_y;
		verteies[1].x = ul_gx;
		verteies[1].y = ul_gy;
		verteies[3].x = dl_gx;
		verteies[3].y = dl_gy;
		verteies[2].x = dl_x;
		verteies[2].y = dl_y;
	}

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = dl_x;
		verteies[0].y = dl_y;
		verteies[1].x = dl_gx;
		verteies[1].y = dl_gy;
		verteies[3].x = dr_gx;
		verteies[3].y = dr_gy;
		verteies[2].x = dr_x;
		verteies[2].y = dr_y;
	}

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = ur_x;
		verteies[0].y = ur_y;
		verteies[1].x = dr_x;
		verteies[1].y = dr_y;
		verteies[3].x = dr_gx;
		verteies[3].y = dr_gy;
		verteies[2].x = ur_gx;
		verteies[2].y = ur_gy;
	}

	m_renderer->GetVertexBuffer()->Unlock();

	m_renderer->GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_renderer->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	uint32_t pass;
	ID3DXEffect* shader = m_shader->GetInterface();

	shader->SetTechnique( "Texhnique" );
	shader->Begin( &pass, 0 );

	assert( pass == 1 );

	shader->BeginPass(0);

	shader->SetFloat( "Width", (float)width );
	shader->SetFloat( "Height", (float)height );

	m_renderer->ApplyAlphaBlend( ::Effekseer::ALPHA_BLEND_BLEND );

	shader->CommitChanges();

	m_renderer->GetDevice()->SetVertexDeclaration( m_vertexDeclaration );
	m_renderer->GetDevice()->SetStreamSource( 0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex) );
	m_renderer->GetDevice()->SetIndices( m_renderer->GetIndexBuffer()->GetInterface() );
	m_renderer->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4 * 4, 0, 4 * 2 );

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
