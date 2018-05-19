
#ifndef	__EFFEKSEER_PARAMATERNODE_RIBBON_H__
#define	__EFFEKSEER_PARAMATERNODE_RIBBON_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.RibbonRenderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RibbonAllColorParamater
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

struct RibbonColorParamater
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
			color l;
			color r;
		} fixed;
	};
};

struct RibbonPositionParamater
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
			float l;
			float r;
		} fixed;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class EffectNodeRibbon
	: public EffectNode
{
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

	RibbonRenderer::NodeParamater	m_nodeParamater;
	RibbonRenderer::InstanceParamater m_instanceParamater;
public:

	eAlphaBlend		AlphaBlend;

	int				ViewpointDependent;

	RibbonAllColorParamater	RibbonAllColor;

	RibbonColorParamater RibbonColor;
	RibbonPositionParamater RibbonPosition;

	int RibbonTexture;

	EffectNodeRibbon( Effect* effect, unsigned char*& pos )
		: EffectNode( effect, pos )
	{
	}

	~EffectNodeRibbon()
	{
	}

	void LoadRendererParamater( unsigned char*& pos );

	void InitializeRenderer();

	void FinalizeRenderer();

	void BeginRendering();

	void BeginRenderingGroup( int32_t instanceCount );

	void Rendering( const Instance& instance );

	void EndRendering();

	void InitializeRenderedInstance( Instance& instance );

	void UpdateRenderedInstance( Instance& instance );

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_RIBBON; }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_PARAMATERNODE_RIBBON_H__
