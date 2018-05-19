
#ifndef	__EFFEKSEER_PARAMATERNODE_SPRITE_H__
#define	__EFFEKSEER_PARAMATERNODE_SPRITE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.EffectNode.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct SpriteAllColorParamater
{
	enum
	{
		Fixed = 0,
		Random = 1,
		Easing = 2,

		Paramater_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
			color all;
		} fixed;

		struct
		{
			random_color all;
		} random;

		struct
		{
			easing_color all;
		} easing;
	};
};

struct SpriteColorParamater
{
	enum
	{
		Default = 0,
		Fixed = 1,

		Paramater_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
		
		} def;

		struct
		{
			color ll;
			color lr;
			color ul;
			color ur;
		} fixed;
	};
};

struct SpritePositionParamater
{
	enum
	{
		Default = 0,
		Fixed = 1,

		Paramater_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
		
		} def;

		struct
		{
			vector2d ll;
			vector2d lr;
			vector2d ul;
			vector2d ur;
		} fixed;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class EffectNodeSprite
	: public EffectNode
{
	friend class Manager;
	friend class Effect;
	friend class Instance;

public:

	struct InstanceValues
	{
		// êF
		color _color;

		union 
		{
			struct
			{
			
			} fixed;

			struct
			{

			} random;

			struct
			{
				color start;
				color  end;

			} easing;

		} allColorValues;

		union
		{
	
		} colorValues;

		union
		{
	
		} positionValues;
	};

public:

	eAlphaBlend		AlphaBlend;
	eBillboardType	Billboard;

	SpriteAllColorParamater	SpriteAllColor;

	SpriteColorParamater SpriteColor;
	SpritePositionParamater SpritePosition;

	int SpriteTexture;


	EffectNodeSprite( Effect* effect, unsigned char*& pos )
		: EffectNode( effect, pos )
	{
	}

	~EffectNodeSprite()
	{
	}

	void LoadRendererParamater( unsigned char*& pos );

	void InitializeRenderer();

	void FinalizeRenderer();

	void BeginRendering();

	void Rendering( const Instance& instance );

	void EndRendering();

	void InitializeRenderedInstance( Instance& instance );

	void UpdateRenderedInstance( Instance& instance );

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_SPRITE; }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_PARAMATERNODE_SPRITE_H__
