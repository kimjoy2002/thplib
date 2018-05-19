
#ifndef	__EFFEKSEER_PARTICLE_RENDERER_H__
#define	__EFFEKSEER_PARTICLE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Effekseer.Base.h"
#include "../Effekseer.Vector3D.h"
#include "../Effekseer.Matrix43.h"
#include "../Effekseer.Color.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ParticleRenderer
{
public:

	struct NodeParamater
	{
		Effect*				EffectPointer;
		int32_t				TextureIndex;
		eAlphaBlend			AlphaBlend;
	};

	struct InstanceParamater
	{
		Vector3D	Position;
		float		Size;
		Color		ParticleColor;
	};

public:
	ParticleRenderer() {}

	virtual ~ParticleRenderer() {}

	virtual void LoadRenderer(  const NodeParamater& paramater, void*& userData ) {}

	virtual void RemoveRenderer( const NodeParamater& paramater, void*& userData ) {}

	virtual void BeginRendering( const NodeParamater& paramater, void* userData ) {}

	virtual void Rendering( const NodeParamater& paramater, const InstanceParamater& instanceParamater, void* userData ) {}

	virtual void EndRendering( const NodeParamater& paramater, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_PARTICLE_RENDERER_H__
