
#ifndef	__EFFEKSEERRENDERER_RIBBON_RENDERER_H__
#define	__EFFEKSEERRENDERER_RIBBON_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::RibbonRenderer::NodeParamater efkRibbonNodeParam;
typedef ::Effekseer::RibbonRenderer::InstanceParamater efkRibbonInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class RibbonRenderer
	: public ::Effekseer::RibbonRenderer
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		float	Col[4];
		float	UV[2];
	};

	RendererImplemented*			m_renderer;
	Shader*							m_shader;
	Shader*							m_shader_no_texture;
	IDirect3DVertexDeclaration9*	m_vertexDeclaration;
	int32_t							m_ribbonCount;

	RibbonRenderer( RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture );
public:

	virtual ~RibbonRenderer();

	static RibbonRenderer* Create( RendererImplemented* renderer );

public:
	void BeginRendering( const efkRibbonNodeParam& paramater, void* userData );

	void Rendering( const efkRibbonNodeParam& paramater, const efkRibbonInstanceParam& instanceParamater, void* userData );

	void EndRendering( const efkRibbonNodeParam& paramater, void* userData );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RIBBON_RENDERER_H__