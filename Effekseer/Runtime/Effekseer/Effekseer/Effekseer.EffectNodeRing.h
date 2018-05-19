
#ifndef	__EFFEKSEER_PARAMATERNODE_RING_H__
#define	__EFFEKSEER_PARAMATERNODE_RING_H__

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
struct RingSingleParamater
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
		float fixed;
		random_float random;
		easing_float easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingLocationParamater
{
	enum
	{
		Fixed = 0,
		PVA = 1,
		Easing = 2,

		Paramater_DWORD = 0x7fffffff,
	} type;
	
	union
	{
		struct
		{
			vector2d location;
		} fixed;
	
		struct
		{
			random_vector2d	location;
			random_vector2d	velocity;
			random_vector2d	acceleration;
		} pva;
		
		easing_vector2d easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingColorParamater
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
		color fixed;
		random_color random;
		easing_color easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingSingleValues
{
	float	current;
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
			float  start;
			float  end;
		} easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingLocationValues
{
	vector2d	current;
	union
	{
		struct
		{
	
		} fixed;

		struct
		{
			vector2d  velocity;
			vector2d  acceleration;
		} pva;

		struct
		{
			vector2d  start;
			vector2d  end;
		} easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingColorValues
{
	color	current;
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
			color  start;
			color  end;
		} easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class EffectNodeRing
	: public EffectNode
{
	friend class Manager;
	friend class Effect;
	friend class Instance;

public:

	struct InstanceValues
	{
		RingSingleValues viewingAngle;
		RingLocationValues outerLocation;
		RingLocationValues innerLocation;
		RingSingleValues centerRatio;
		RingColorValues outerColor;
		RingColorValues centerColor;
		RingColorValues innerColor;
	};

public:

	eAlphaBlend		AlphaBlend;
	eBillboardType	Billboard;

	int32_t	VertexCount;

	RingSingleParamater	ViewingAngle;

	RingLocationParamater	OuterLocation;
	RingLocationParamater	InnerLocation;
	
	RingSingleParamater	CenterRatio;
	
	RingColorParamater OuterColor;
	RingColorParamater CenterColor;
	RingColorParamater InnerColor;

	int RingTexture;

	EffectNodeRing( Effect* effect, unsigned char*& pos )
		: EffectNode( effect, pos )
	{
	}

	~EffectNodeRing()
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

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_RING; }

private:
	void LoadSingleParamater( unsigned char*& pos, RingSingleParamater& param );

	void LoadLocationParamater( unsigned char*& pos, RingLocationParamater& param );
	
	void LoadColorParamater( unsigned char*& pos, RingColorParamater& param );
	
	void InitializeSingleValues( const RingSingleParamater& param, RingSingleValues& values );

	void InitializeLocationValues( const RingLocationParamater& param, RingLocationValues& values );
	
	void InitializeColorValues( const RingColorParamater& param, RingColorValues& values );
	
	void UpdateSingleValues( Instance& instance, const RingSingleParamater& param, RingSingleValues& values );
	
	void UpdateLocationValues( Instance& instance, const RingLocationParamater& param, RingLocationValues& values );
	
	void UpdateColorValues( Instance& instance, const RingColorParamater& param, RingColorValues& values );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_PARAMATERNODE_RING_H__
