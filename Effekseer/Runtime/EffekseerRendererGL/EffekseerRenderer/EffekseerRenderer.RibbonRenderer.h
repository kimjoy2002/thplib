
#ifndef	__EFFEKSEERRENDERER_STRIPE_RENDERER_H__
#define	__EFFEKSEERRENDERER_STRIPE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.Renderer.h"

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
		::Effekseer::Color		Col;
		float					UV[2];
	};

	RendererImplemented*	m_renderer;
	Shader*		m_shader;
	int32_t		m_ribbonCount;
	GLint		m_aid[3];

	RibbonRenderer( RendererImplemented* renderer, Shader* shader );
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
#endif	// __EFFEKSEERRENDERER_STRIPE_RENDERER_H__