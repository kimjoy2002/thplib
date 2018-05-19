
#ifndef	__EFFEKSEERRENDERER_RING_RENDERER_H__
#define	__EFFEKSEERRENDERER_RING_RENDERER_H__

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
typedef ::Effekseer::RingRenderer::NodeParamater efkRingNodeParam;
typedef ::Effekseer::RingRenderer::InstanceParamater efkRingInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class RingRenderer
	: public ::Effekseer::RingRenderer
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		uint32_t		Col;
		float			UV[2];
	};

	RendererImplemented*	m_renderer;
	Shader*		m_shader;
	int32_t		m_spriteCount;
	GLint		m_aid[3];

	RingRenderer( RendererImplemented* renderer, Shader* shader );
public:

	virtual ~RingRenderer();

	static RingRenderer* Create( RendererImplemented* renderer );

public:
	void BeginRendering( const efkRingNodeParam& paramater, void* userData );

	void Rendering( const efkRingNodeParam& paramater, const efkRingInstanceParam& instanceParamater, void* userData );

	void EndRendering( const efkRingNodeParam& paramater, void* userData );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RING_RENDERER_H__