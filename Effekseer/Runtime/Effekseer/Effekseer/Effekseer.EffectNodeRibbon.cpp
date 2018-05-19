

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
#include "Effekseer.EffectNodeRibbon.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::LoadRendererParamater( unsigned char*& pos )
{
	int32_t type = 0;
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Ribbon\n");

	int32_t size = 0;

	memcpy( &AlphaBlend, pos, sizeof(int) );
	pos += sizeof(int);

	memcpy( &ViewpointDependent, pos, sizeof(int) );
	pos += sizeof(int);
	
	memcpy( &RibbonAllColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorAllType : %d\n", RibbonAllColor.type );

	if( RibbonAllColor.type == RibbonAllColorParamater::Fixed )
	{
		memcpy( &RibbonAllColor.fixed, pos, sizeof(RibbonAllColor.fixed) );
		pos += sizeof(RibbonAllColor.fixed);
	}
	else if( RibbonAllColor.type == RibbonAllColorParamater::Random )
	{
		memcpy( &RibbonAllColor.random, pos, sizeof(RibbonAllColor.random) );
		pos += sizeof(RibbonAllColor.random);
	}
	else if( RibbonAllColor.type == RibbonAllColorParamater::Easing )
	{
		memcpy( &RibbonAllColor.easing, pos, sizeof(RibbonAllColor.easing) );
		pos += sizeof(RibbonAllColor.easing);
	}

	memcpy( &RibbonColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonColorType : %d\n", RibbonColor.type );

	if( RibbonColor.type == RibbonColor.Default )
	{
	}
	else if( RibbonColor.type == RibbonColor.Fixed )
	{
		memcpy( &RibbonColor.fixed, pos, sizeof(RibbonColor.fixed) );
		pos += sizeof(RibbonColor.fixed);
	}

	memcpy( &RibbonPosition.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("RibbonPosition : %d\n", RibbonPosition.type );

	if( RibbonPosition.type == RibbonPosition.Default )
	{
	}
	else if( RibbonPosition.type == RibbonPosition.Fixed )
	{
		memcpy( &RibbonPosition.fixed, pos, sizeof(RibbonPosition.fixed) );
		pos += sizeof(RibbonPosition.fixed);
	}

	memcpy( &RibbonTexture, pos, sizeof(int) );
	pos += sizeof(int);

	// ‰EŽèŒn¶ŽèŒn•ÏŠ·
	if( m_effect->GetManager()->GetCoordinateSystem() == COORDINATE_SYSTEM_LH )
	{
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::InitializeRenderer()
{
	RibbonRenderer* renderer = GetEffect()->GetManager()->GetRibbonRenderer();
	if( renderer != NULL )
	{
		m_nodeParamater.AlphaBlend = AlphaBlend;
		m_nodeParamater.ViewpointDependent = ViewpointDependent != 0;
		m_nodeParamater.ColorTextureIndex = RibbonTexture;
		m_nodeParamater.EffectPointer = GetEffect();
		renderer->LoadRenderer( m_nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::FinalizeRenderer()
{
	RibbonRenderer* renderer = GetEffect()->GetManager()->GetRibbonRenderer();
	if( renderer != NULL )
	{
		renderer->RemoveRenderer( m_nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::BeginRendering()
{
	RibbonRenderer* renderer = GetEffect()->GetManager()->GetRibbonRenderer();
	if( renderer != NULL )
	{
		renderer->BeginRendering( m_nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::BeginRenderingGroup( int32_t instanceCount )
{
	RibbonRenderer* renderer = GetEffect()->GetManager()->GetRibbonRenderer();
	if( renderer != NULL )
	{
		m_instanceParamater.InstanceCount = instanceCount;
		m_instanceParamater.InstanceIndex = 0;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::Rendering( const Instance& instance )
{
	const InstanceValues& instValues = instance.rendererValues.ribbon;
	RibbonRenderer* renderer = GetEffect()->GetManager()->GetRibbonRenderer();
	if( renderer != NULL )
	{
		instValues._color.setValueToArg( m_instanceParamater.AllColor );
		m_instanceParamater.SRTMatrix43 = instance.GetGlobalMatrix43();

		if( RibbonColor.type == RibbonColorParamater::Default )
		{
			instValues._color.setValueToArg( m_instanceParamater.Colors[0] );
			instValues._color.setValueToArg( m_instanceParamater.Colors[1] );
		}
		else if( RibbonColor.type == RibbonColorParamater::Fixed )
		{
			color::mul( instValues._color, RibbonColor.fixed.l ).setValueToArg( m_instanceParamater.Colors[0] );
			color::mul( instValues._color, RibbonColor.fixed.r ).setValueToArg( m_instanceParamater.Colors[1] );
		}
		
		if( RibbonPosition.type == RibbonPositionParamater::Default )
		{
			m_instanceParamater.Positions[0] = -0.5f;
			m_instanceParamater.Positions[1] = 0.5f;
		}
		else if( RibbonPosition.type == RibbonPositionParamater::Fixed )
		{
			m_instanceParamater.Positions[0] = RibbonPosition.fixed.l;
			m_instanceParamater.Positions[1] = RibbonPosition.fixed.r;
		}

		renderer->Rendering( m_nodeParamater, m_instanceParamater, m_userData );

		m_instanceParamater.InstanceIndex++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::EndRendering()
{
	RibbonRenderer* renderer =  GetEffect()->GetManager()->GetRibbonRenderer();
	if( renderer != NULL )
	{
		renderer->EndRendering( m_nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::InitializeRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.ribbon;

	if( RibbonAllColor.type == RibbonAllColorParamater::Fixed )
	{
		instValues._color = RibbonAllColor.fixed.all;
	}
	else if( RibbonAllColor.type == RibbonAllColorParamater::Random )
	{
		instValues._color = RibbonAllColor.random.all.getValue( *(m_effect->GetManager()) );
	}
	else if( RibbonAllColor.type == RibbonAllColorParamater::Easing )
	{
		instValues.allColorValues.easing.start = RibbonAllColor.easing.all.start.getValue( *(m_effect->GetManager()) );
		instValues.allColorValues.easing.end = RibbonAllColor.easing.all.end.getValue( *(m_effect->GetManager()) );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRibbon::UpdateRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.ribbon;

	if( RibbonAllColor.type == RibbonAllColorParamater::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		RibbonAllColor.easing.all.setValueToArg(
			instValues._color, 
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
