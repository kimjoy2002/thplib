
#ifndef	__EFFEKSEER_RING_RENDERER_H__
#define	__EFFEKSEER_RING_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Effekseer.Base.h"
#include "../Effekseer.Vector2D.h"
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

class RingRenderer
{
public:

	struct NodeParamater
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eBillboardType		Billboard;
		int32_t				VertexCount;
	};

	struct InstanceParamater
	{
		Matrix43	SRTMatrix43;
		float		ViewingAngle;
		Vector2D	OuterLocation;
		Vector2D	InnerLocation;
		float		CenterRatio;
		Color		OuterColor;
		Color		CenterColor;
		Color		InnerColor;
	};

public:
	RingRenderer() {}

	virtual ~RingRenderer() {}

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
#endif	// __EFFEKSEER_RING_RENDERER_H__
