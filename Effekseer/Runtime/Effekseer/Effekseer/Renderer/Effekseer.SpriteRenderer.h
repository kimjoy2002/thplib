
#ifndef	__EFFEKSEER_SPRITE_RENDERER_H__
#define	__EFFEKSEER_SPRITE_RENDERER_H__

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

class SpriteRenderer
{
public:

	struct NodeParamater
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eBillboardType		Billboard;
	};

	struct InstanceParamater
	{
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// 左下、右下、左上、右上
		Color		Colors[4];

		Vector2D	Positions[4];

	};

public:
	SpriteRenderer() {}

	virtual ~SpriteRenderer() {}

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
#endif	// __EFFEKSEER_SPRITE_RENDERER_H__
