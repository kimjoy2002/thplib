

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
#include "Effekseer.EffectNodeRing.h"

#include "Renderer/Effekseer.RingRenderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadRendererParamater( unsigned char*& pos )
{
	int32_t type = 0;
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Ring\n");

	int32_t size = 0;

	memcpy( &RenderingOrder, pos, sizeof(int) );
	pos += sizeof(int);
	memcpy( &AlphaBlend, pos, sizeof(int) );
	pos += sizeof(int);
	memcpy( &Billboard, pos, sizeof(int) );
	pos += sizeof(int);
	
	memcpy( &VertexCount, pos, sizeof(int) );
	pos += sizeof(int);
	
	LoadSingleParamater( pos, ViewingAngle );

	LoadLocationParamater( pos, OuterLocation );
	
	LoadLocationParamater( pos, InnerLocation );
	
	LoadSingleParamater( pos, CenterRatio );

	LoadColorParamater( pos, OuterColor);

	LoadColorParamater( pos, CenterColor);

	LoadColorParamater( pos, InnerColor);

	memcpy( &RingTexture, pos, sizeof(int) );
	pos += sizeof(int);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeRenderer()
{
	RingRenderer* renderer = GetEffect()->GetManager()->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = RingTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->LoadRenderer( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::FinalizeRenderer()
{
	RingRenderer* renderer = GetEffect()->GetManager()->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = RingTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->RemoveRenderer( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::BeginRendering()
{
	RingRenderer* renderer =  GetEffect()->GetManager()->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = RingTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->BeginRendering( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::Rendering( const Instance& instance )
{
	const InstanceValues& instValues = instance.rendererValues.ring;
	RingRenderer* renderer = GetEffect()->GetManager()->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParamater nodeParamater;
		nodeParamater.EffectPointer = GetEffect();
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.VertexCount = VertexCount;
		nodeParamater.ColorTextureIndex = RingTexture;

		RingRenderer::InstanceParamater instanceParamater;
		instanceParamater.SRTMatrix43 = instance.GetGlobalMatrix43();

		instanceParamater.ViewingAngle = instValues.viewingAngle.current;
		
		instValues.outerLocation.current.setValueToArg( instanceParamater.OuterLocation );
		instValues.innerLocation.current.setValueToArg( instanceParamater.InnerLocation );
		
		instanceParamater.CenterRatio = instValues.centerRatio.current;

		instValues.outerColor.current.setValueToArg( instanceParamater.OuterColor );
		instValues.centerColor.current.setValueToArg( instanceParamater.CenterColor );
		instValues.innerColor.current.setValueToArg( instanceParamater.InnerColor );

		renderer->Rendering( nodeParamater, instanceParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::EndRendering()
{
	RingRenderer* renderer =  GetEffect()->GetManager()->GetRingRenderer();
	if( renderer != NULL )
	{
		RingRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = RingTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->EndRendering( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.ring;

	InitializeSingleValues( ViewingAngle, instValues.viewingAngle );

	InitializeLocationValues( OuterLocation, instValues.outerLocation );
	InitializeLocationValues( InnerLocation, instValues.innerLocation );
	
	InitializeSingleValues( CenterRatio, instValues.centerRatio );

	InitializeColorValues( OuterColor, instValues.outerColor );
	InitializeColorValues( CenterColor, instValues.centerColor );
	InitializeColorValues( InnerColor, instValues.innerColor );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.ring;
	
	UpdateSingleValues( instance, ViewingAngle, instValues.viewingAngle );

	UpdateLocationValues( instance, OuterLocation, instValues.outerLocation );
	UpdateLocationValues( instance, InnerLocation, instValues.innerLocation );
	
	UpdateSingleValues( instance, CenterRatio, instValues.centerRatio );

	UpdateColorValues( instance, OuterColor, instValues.outerColor );
	UpdateColorValues( instance, CenterColor, instValues.centerColor );
	UpdateColorValues( instance, InnerColor, instValues.innerColor );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadSingleParamater( unsigned char*& pos, RingSingleParamater& param )
{
	memcpy( &param.type, pos, sizeof(int) );
	pos += sizeof(int);

	if( param.type == RingSingleParamater::Fixed )
	{
		memcpy( &param.fixed, pos, sizeof(float) );
		pos += sizeof(float);
	}
	else if( param.type == RingSingleParamater::Random )
	{
		memcpy( &param.random, pos, sizeof(param.random) );
		pos += sizeof(param.random);
	}
	else if( param.type == RingSingleParamater::Easing )
	{
		memcpy( &param.easing, pos, sizeof(param.easing) );
		pos += sizeof(param.easing);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadLocationParamater( unsigned char*& pos, RingLocationParamater& param )
{
	memcpy( &param.type, pos, sizeof(int) );
	pos += sizeof(int);
	
	if( param.type == RingLocationParamater::Fixed )
	{
		memcpy( &param.fixed, pos, sizeof(param.fixed) );
		pos += sizeof(param.fixed);
	}
	else if( param.type == RingLocationParamater::PVA )
	{
		memcpy( &param.pva, pos, sizeof(param.pva) );
		pos += sizeof(param.pva);
	}
	else if( param.type == RingLocationParamater::Easing )
	{
		memcpy( &param.easing, pos, sizeof(param.easing) );
		pos += sizeof(param.easing);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::LoadColorParamater( unsigned char*& pos, RingColorParamater& param )
{
	memcpy( &param.type, pos, sizeof(int) );
	pos += sizeof(int);
	
	if( param.type == RingColorParamater::Fixed )
	{
		memcpy( &param.fixed, pos, sizeof(param.fixed) );
		pos += sizeof(param.fixed);
	}
	else if( param.type == RingColorParamater::Random )
	{
		memcpy( &param.random, pos, sizeof(param.random) );
		pos += sizeof(param.random);
	}
	else if( param.type == RingColorParamater::Easing )
	{
		memcpy( &param.easing, pos, sizeof(param.easing) );
		pos += sizeof(param.easing);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeSingleValues( const RingSingleParamater& param, RingSingleValues& values )
{
	Manager* manager = m_effect->GetManager();
	switch( param.type )
	{
		case RingSingleParamater::Fixed:
			values.current = param.fixed;
			break;
		case RingSingleParamater::Random:
			values.current = param.random.getValue( *manager );
			break;
		case RingSingleParamater::Easing:
			values.easing.start = param.easing.start.getValue( *manager );
			values.easing.end = param.easing.end.getValue( *manager );
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeLocationValues( const RingLocationParamater& param, RingLocationValues& values )
{
	Manager* manager = m_effect->GetManager();
	switch( param.type )
	{
		case RingLocationParamater::Fixed:
			values.current = param.fixed.location;
			break;
		case RingLocationParamater::PVA:
			values.current = param.pva.location.getValue( *manager );
			values.pva.velocity = param.pva.velocity.getValue( *manager );
			values.pva.acceleration = param.pva.acceleration.getValue( *manager );
			break;
		case RingLocationParamater::Easing:
			values.easing.start = param.easing.start.getValue( *manager );
			values.easing.end = param.easing.end.getValue( *manager );
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::InitializeColorValues( const RingColorParamater& param, RingColorValues& values )
{
	Manager* manager = m_effect->GetManager();
	switch( param.type )
	{
		case RingColorParamater::Fixed:
			values.current = param.fixed;
			break;
		case RingColorParamater::Random:
			values.current = param.random.getValue( *manager );
			break;
		case RingColorParamater::Easing:
			values.easing.start = param.easing.start.getValue( *manager );
			values.easing.end = param.easing.end.getValue( *manager );
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateSingleValues( Instance& instance, const RingSingleParamater& param, RingSingleValues& values )
{
	if( param.type == RingLocationParamater::Easing )
	{
		param.easing.setValueToArg(
			values.current,
			values.easing.start,
			values.easing.end,
			instance.m_LivingTime / instance.m_LivedTime );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateLocationValues( Instance& instance, const RingLocationParamater& param, RingLocationValues& values )
{
	if( param.type == RingLocationParamater::PVA )
	{
		values.pva.velocity += values.pva.acceleration;
		values.current += values.pva.velocity;
	}
	else if( param.type == RingLocationParamater::Easing )
	{
		param.easing.setValueToArg(
			values.current,
			values.easing.start,
			values.easing.end,
			instance.m_LivingTime / instance.m_LivedTime );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeRing::UpdateColorValues( Instance& instance, const RingColorParamater& param, RingColorValues& values )
{
	if( param.type == RingColorParamater::Easing )
	{
		param.easing.setValueToArg(
			values.current, 
			values.easing.start,
			values.easing.end,
			instance.m_LivingTime / instance.m_LivedTime );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
