

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
#include "Effekseer.EffectNodeSprite.h"

#include "Renderer/Effekseer.SpriteRenderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::LoadRendererParamater( unsigned char*& pos )
{
	int32_t type = 0;
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Sprite\n");

	int32_t size = 0;

	memcpy( &RenderingOrder, pos, sizeof(int) );
	pos += sizeof(int);
	memcpy( &AlphaBlend, pos, sizeof(int) );
	pos += sizeof(int);
	memcpy( &Billboard, pos, sizeof(int) );
	pos += sizeof(int);
	
	memcpy( &SpriteAllColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("SpriteColorAllType : %d\n", SpriteAllColor.type );

	if( SpriteAllColor.type == SpriteAllColorParamater::Fixed )
	{
		memcpy( &SpriteAllColor.fixed, pos, sizeof(SpriteAllColor.fixed) );
		pos += sizeof(SpriteAllColor.fixed);
	}
	else if( SpriteAllColor.type == SpriteAllColorParamater::Random )
	{
		memcpy( &SpriteAllColor.random, pos, sizeof(SpriteAllColor.random) );
		pos += sizeof(SpriteAllColor.random);
	}
	else if( SpriteAllColor.type == SpriteAllColorParamater::Easing )
	{
		memcpy( &SpriteAllColor.easing, pos, sizeof(SpriteAllColor.easing) );
		pos += sizeof(SpriteAllColor.easing);
	}

	memcpy( &SpriteColor.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("SpriteColorType : %d\n", SpriteColor.type );

	if( SpriteColor.type == SpriteColor.Default )
	{
	}
	else if( SpriteColor.type == SpriteColor.Fixed )
	{
		memcpy( &SpriteColor.fixed, pos, sizeof(SpriteColor.fixed) );
		pos += sizeof(SpriteColor.fixed);
	}

	memcpy( &SpritePosition.type, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("SpritePosition : %d\n", SpritePosition.type );

	if( SpritePosition.type == SpritePosition.Default )
	{
	}
	else if( SpritePosition.type == SpritePosition.Fixed )
	{
		memcpy( &SpritePosition.fixed, pos, sizeof(SpritePosition.fixed) );
		pos += sizeof(SpritePosition.fixed);
	}

	memcpy( &SpriteTexture, pos, sizeof(int) );
	pos += sizeof(int);

	// ‰EŽèŒn¶ŽèŒn•ÏŠ·
	if( m_effect->GetManager()->GetCoordinateSystem() == COORDINATE_SYSTEM_LH )
	{
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::InitializeRenderer()
{
	SpriteRenderer* renderer = GetEffect()->GetManager()->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = SpriteTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->LoadRenderer( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::FinalizeRenderer()
{
	SpriteRenderer* renderer = GetEffect()->GetManager()->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = SpriteTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->RemoveRenderer( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::BeginRendering()
{
	SpriteRenderer* renderer =  GetEffect()->GetManager()->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = SpriteTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->BeginRendering( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::Rendering( const Instance& instance )
{
	const InstanceValues& instValues = instance.rendererValues.sprite;
	SpriteRenderer* renderer = GetEffect()->GetManager()->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = SpriteTexture;
		nodeParamater.EffectPointer = GetEffect();

		SpriteRenderer::InstanceParamater instanceParamater;
		instValues._color.setValueToArg( instanceParamater.AllColor );
		instanceParamater.SRTMatrix43 = instance.GetGlobalMatrix43();

		if( SpriteColor.type == SpriteColorParamater::Default )
		{
			instValues._color.setValueToArg( instanceParamater.Colors[0] );
			instValues._color.setValueToArg( instanceParamater.Colors[1] );
			instValues._color.setValueToArg( instanceParamater.Colors[2] );
			instValues._color.setValueToArg( instanceParamater.Colors[3] );
		}
		else if( SpriteColor.type == SpriteColorParamater::Fixed )
		{
			color::mul( instValues._color, SpriteColor.fixed.ll ).setValueToArg( instanceParamater.Colors[0] );
			color::mul( instValues._color, SpriteColor.fixed.lr ).setValueToArg( instanceParamater.Colors[1] );
			color::mul( instValues._color, SpriteColor.fixed.ul ).setValueToArg( instanceParamater.Colors[2] );
			color::mul( instValues._color, SpriteColor.fixed.ur ).setValueToArg( instanceParamater.Colors[3] );
		}
		
		if( SpritePosition.type == SpritePosition.Default )
		{
			instanceParamater.Positions[0].X = -0.5f;
			instanceParamater.Positions[0].Y = -0.5f;
			instanceParamater.Positions[1].X = 0.5f;
			instanceParamater.Positions[1].Y = -0.5f;
			instanceParamater.Positions[2].X = -0.5f;
			instanceParamater.Positions[2].Y = 0.5f;
			instanceParamater.Positions[3].X = 0.5f;
			instanceParamater.Positions[3].Y = 0.5f;
		}
		else if( SpritePosition.type == SpritePosition.Fixed )
		{
			SpritePosition.fixed.ll.setValueToArg( instanceParamater.Positions[0] );
			SpritePosition.fixed.lr.setValueToArg( instanceParamater.Positions[1] );
			SpritePosition.fixed.ul.setValueToArg( instanceParamater.Positions[2] );
			SpritePosition.fixed.ur.setValueToArg( instanceParamater.Positions[3] );
		}

		renderer->Rendering( nodeParamater, instanceParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::EndRendering()
{
	SpriteRenderer* renderer =  GetEffect()->GetManager()->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParamater nodeParamater;
		nodeParamater.AlphaBlend = AlphaBlend;
		nodeParamater.Billboard = Billboard;
		nodeParamater.ColorTextureIndex = SpriteTexture;
		nodeParamater.EffectPointer = GetEffect();
		renderer->EndRendering( nodeParamater, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::InitializeRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.sprite;

	if( SpriteAllColor.type == SpriteAllColorParamater::Fixed )
	{
		instValues._color = SpriteAllColor.fixed.all;
	}
	else if( SpriteAllColor.type == SpriteAllColorParamater::Random )
	{
		instValues._color = SpriteAllColor.random.all.getValue( *(m_effect->GetManager()) );
	}
	else if( SpriteAllColor.type == SpriteAllColorParamater::Easing )
	{
		instValues.allColorValues.easing.start = SpriteAllColor.easing.all.start.getValue( *(m_effect->GetManager()) );
		instValues.allColorValues.easing.end = SpriteAllColor.easing.all.end.getValue( *(m_effect->GetManager()) );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::UpdateRenderedInstance( Instance& instance )
{
	InstanceValues& instValues = instance.rendererValues.sprite;

	if( SpriteAllColor.type == SpriteAllColorParamater::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		SpriteAllColor.easing.all.setValueToArg(
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
