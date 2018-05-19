
#ifndef	__EFFEKSEERRENDERER_SPRITE_RENDERER_H__
#define	__EFFEKSEERRENDERER_SPRITE_RENDERER_H__

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
typedef ::Effekseer::SpriteRenderer::NodeParamater efkSpriteNodeParam;
typedef ::Effekseer::SpriteRenderer::InstanceParamater efkSpriteInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class SpriteRenderer
	: public ::Effekseer::SpriteRenderer
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Color		Col;
		uint16_t				UV[2];
	};

	RendererImplemented*	m_renderer;
	Shader*		m_shader;
	int32_t		m_spriteCount;
	GLint		m_aid[3];

	SpriteRenderer( RendererImplemented* renderer, Shader* shader );
public:

	virtual ~SpriteRenderer();

	static SpriteRenderer* Create( RendererImplemented* renderer );

public:
	void BeginRendering( const efkSpriteNodeParam& paramater, void* userData );

	void Rendering( const efkSpriteNodeParam& paramater, const efkSpriteInstanceParam& instanceParamater, void* userData );

	void EndRendering( const efkSpriteNodeParam& paramater, void* userData );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_SPRITE_RENDERER_H__