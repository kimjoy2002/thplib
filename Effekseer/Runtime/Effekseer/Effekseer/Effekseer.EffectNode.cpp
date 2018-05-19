

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"

#include "Effekseer.Instance.h"

#include "Effekseer.EffectNodeRoot.h"
#include "Effekseer.EffectNodeSprite.h"
#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.EffectNodeRing.h"
#include "Sound/Effekseer.SoundPlayer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode::EffectNode( Effect* effect, unsigned char*& pos )
	: m_effect		( effect )
	, IsRendered		( true )
	, SoundType			( ParamaterSoundType_None )
	, m_userData		( NULL )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::LoadParamater( unsigned char*& pos )
{
	Manager* pManager = m_effect->GetManager();

	int size = 0;
	int node_type = 0;
	memcpy( &node_type, pos, sizeof(int) );
	pos += sizeof(int);

	if( node_type == -1 )
	{
		TranslationType = ParamaterTranslationType_None;
		LocationAbs.type = LocationAbsParamater::None;
		RotationType = ParamaterRotationType_None;
		ScalingType = ParamaterScalingType_None;
		CommonValues.MaxGeneration = 1;

		GenerationLocation.EffectsRotation = 0;
		GenerationLocation.type = ParamaterGenerationLocation::TYPE_POINT;
		GenerationLocation.point.location.reset();
	}
	else
	{
		memcpy( &size, pos, sizeof(int) );
		pos += sizeof(int);
		assert( size == sizeof(ParamaterCommonValues) );
		memcpy( &CommonValues, pos, size );
		pos += size;
		

		memcpy( &TranslationType, pos, sizeof(int) );
		pos += sizeof(int);

		if( TranslationType == ParamaterTranslationType_Fixed )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParamaterTranslationFixed) );
			memcpy( &TranslationFixed, pos, size );
			pos += size;

			// ñ≥å¯âª
			if( TranslationFixed.Position.X == 0.0f &&
				TranslationFixed.Position.Y == 0.0f &&
				TranslationFixed.Position.Z == 0.0f )
			{
				TranslationType = ParamaterTranslationType_None;
				EffekseerPrintDebug("LocationType Change None\n");
			}
		}
		else if( TranslationType == ParamaterTranslationType_PVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParamaterTranslationPVA) );
			memcpy( &TranslationPVA, pos, size );
			pos += size;
		}
		else if( TranslationType == ParamaterTranslationType_Easing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(easing_vector3d) );
			memcpy( &TranslationEasing, pos, size );
			pos += size;
		}

		memcpy( &LocationAbs.type, pos, sizeof(int) );
		pos += sizeof(int);

		if( LocationAbs.type == LocationAbsParamater::None )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == 0 );
			memcpy( &LocationAbs.none, pos, size );
			pos += size;
		}
		else if( LocationAbs.type == LocationAbsParamater::Gravity )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(vector3d) );
			memcpy( &LocationAbs.gravity, pos, size );
			pos += size;
		}

		memcpy( &RotationType, pos, sizeof(int) );
		pos += sizeof(int);
		EffekseerPrintDebug("RotationType %d\n", RotationType);
		if( RotationType == ParamaterRotationType_Fixed )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParamaterRotationFixed) );
			memcpy( &RotationFixed, pos, size );
			pos += size;

			// ñ≥å¯âª
			if( RotationFixed.Position.X == 0.0f &&
				RotationFixed.Position.Y == 0.0f &&
				RotationFixed.Position.Z == 0.0f )
			{
				RotationType = ParamaterRotationType_None;
				EffekseerPrintDebug("RotationType Change None\n");
			}
		}
		else if( RotationType == ParamaterRotationType_PVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParamaterRotationPVA) );
			memcpy( &RotationPVA, pos, size );
			pos += size;
		}
		else if( RotationType == ParamaterRotationType_Easing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(easing_vector3d) );
			memcpy( &RotationEasing, pos, size );
			pos += size;
		}


		memcpy( &ScalingType, pos, sizeof(int) );
		pos += sizeof(int);
		EffekseerPrintDebug("ScalingType %d\n", ScalingType);
		if( ScalingType == ParamaterScalingType_Fixed )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParamaterScalingFixed) );
			memcpy( &ScalingFixed, pos, size );
			pos += size;

			// ñ≥å¯âª
			if( ScalingFixed.Position.X == 1.0f &&
				ScalingFixed.Position.Y == 1.0f &&
				ScalingFixed.Position.Z == 1.0f )
			{
				ScalingType = ParamaterScalingType_None;
				EffekseerPrintDebug("ScalingType Change None\n");
			}
		}
		else if( ScalingType == ParamaterScalingType_PVA )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(ParamaterScalingPVA) );
			memcpy( &ScalingPVA, pos, size );
			pos += size;
		}
		else if( ScalingType == ParamaterScalingType_Easing )
		{
			memcpy( &size, pos, sizeof(int) );
			pos += sizeof(int);
			assert( size == sizeof(easing_vector3d) );
			memcpy( &ScalingEasing, pos, size );
			pos += size;
		}

		GenerationLocation.load( pos );

		// âEéËånç∂éËånïœä∑
		if( pManager->GetCoordinateSystem() == COORDINATE_SYSTEM_LH )
		{
			// Translation
			if( TranslationType == ParamaterTranslationType_Fixed )
			{
				TranslationFixed.Position.Z *= -1.0f;
			}
			else if( TranslationType == ParamaterTranslationType_PVA )
			{
				TranslationPVA.location.max.z *= -1.0f;
				TranslationPVA.location.min.z *= -1.0f;
				TranslationPVA.velocity.max.z *= -1.0f;
				TranslationPVA.velocity.min.z *= -1.0f;
				TranslationPVA.acceleration.max.z *= -1.0f;
				TranslationPVA.acceleration.min.z *= -1.0f;
			}
			else if( TranslationType == ParamaterTranslationType_Easing )
			{
				TranslationEasing.start.max.z *= -1.0f;
				TranslationEasing.start.min.z *= -1.0f;
				TranslationEasing.end.max.z *= -1.0f;
				TranslationEasing.end.min.z *= -1.0f;
			}

			// Rotation
			if( RotationType == ParamaterRotationType_Fixed )
			{
				RotationFixed.Position.X *= -1.0f;
				RotationFixed.Position.Y *= -1.0f;
			}
			else if( RotationType == ParamaterRotationType_PVA )
			{
				RotationPVA.rotation.max.x *= -1.0f;
				RotationPVA.rotation.min.x *= -1.0f;
				RotationPVA.rotation.max.y *= -1.0f;
				RotationPVA.rotation.min.y *= -1.0f;
				RotationPVA.velocity.max.x *= -1.0f;
				RotationPVA.velocity.min.x *= -1.0f;
				RotationPVA.velocity.max.y *= -1.0f;
				RotationPVA.velocity.min.y *= -1.0f;
				RotationPVA.acceleration.max.x *= -1.0f;
				RotationPVA.acceleration.min.x *= -1.0f;
				RotationPVA.acceleration.max.y *= -1.0f;
				RotationPVA.acceleration.min.y *= -1.0f;
			}
			else if( RotationType == ParamaterRotationType_Easing )
			{
				RotationEasing.start.max.x *= -1.0f;
				RotationEasing.start.min.x *= -1.0f;
				RotationEasing.start.max.y *= -1.0f;
				RotationEasing.start.min.y *= -1.0f;
				RotationEasing.end.max.x *= -1.0f;
				RotationEasing.end.min.x *= -1.0f;
				RotationEasing.end.max.y *= -1.0f;
				RotationEasing.end.min.y *= -1.0f;
			}

			// GenerationLocation
			if( GenerationLocation.type == ParamaterGenerationLocation::TYPE_POINT )
			{
			
			}
			else if( GenerationLocation.type == ParamaterGenerationLocation::TYPE_SPHERE )
			{
				GenerationLocation.sphere.rotation_x.max *= -1.0f;
				GenerationLocation.sphere.rotation_x.min *= -1.0f;
				GenerationLocation.sphere.rotation_y.max *= -1.0f;
				GenerationLocation.sphere.rotation_y.min *= -1.0f;
			}
		}

		LoadRendererParamater( pos );
		
		if( m_effect->GetVersion() >= 1)
		{
			// ÉTÉEÉìÉh
			memcpy( &SoundType, pos, sizeof(int) );
			pos += sizeof(int);
			if( SoundType == ParamaterSoundType_Use )
			{
				memcpy( &Sound.WaveId, pos, sizeof(int32_t) );
				pos += sizeof(int32_t);
				memcpy( &Sound.Volume, pos, sizeof(random_float) );
				pos += sizeof(random_float);
				memcpy( &Sound.Pitch, pos, sizeof(random_float) );
				pos += sizeof(random_float);
				memcpy( &Sound.PanType, pos, sizeof(ParamaterSoundPanType) );
				pos += sizeof(ParamaterSoundPanType);
				memcpy( &Sound.Pan, pos, sizeof(random_float) );
				pos += sizeof(random_float);
				memcpy( &Sound.Distance, pos, sizeof(float) );
				pos += sizeof(float);
				memcpy( &Sound.Delay, pos, sizeof(random_int) );
				pos += sizeof(random_int);
			}
		}
	}

	// ÉmÅ[Éh
	int nodeCount = 0;
	memcpy( &nodeCount, pos, sizeof(int) );
	pos += sizeof( int );
	EffekseerPrintDebug("ChildrenCount : %d\n", nodeCount );
	m_Nodes.resize( nodeCount );
	for( size_t i = 0; i < m_Nodes.size(); i++ )
	{
		m_Nodes[i] = EffectNode::Create( m_effect, pos );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode::~EffectNode()
{
	FinalizeRenderer();

	for( size_t i = 0; i < m_Nodes.size(); i++ )
	{
		ES_SAFE_DELETE( m_Nodes[i] );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::Initialize()
{
	InitializeRenderer();

	// èâä˙âª
	for( size_t i = 0; i < m_Nodes.size(); i++ )
	{
		m_Nodes[i]->Initialize();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::LoadOption( uint8_t*& pos )
{
	int is_rendered = 0;
	memcpy( &is_rendered, pos, sizeof(int) );
	pos += sizeof(int);

	IsRendered = is_rendered != 0;

	int count = 0;
	memcpy( &count, pos, sizeof(int) );
	pos += sizeof(int);

	for( int i = 0; i < count; i++ )
	{
		m_Nodes[i]->LoadOption( pos );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* EffectNode::GetEffect() const
{
	return m_effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EffectNode::GetChildrenCount() const
{
	return (int)m_Nodes.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectNode::GetChild( int num ) const
{
	if( num >= GetChildrenCount() ) return NULL;
	return m_Nodes[ num ];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::LoadRendererParamater( unsigned char*& pos )
{
	int32_t type = 0;
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : None\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::InitializeRenderer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::FinalizeRenderer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::BeginRendering()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::BeginRenderingGroup( int32_t instanceCount )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::Rendering( const Instance& instance )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::EndRendering()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::InitializeRenderedInstance( Instance& instance )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::UpdateRenderedInstance( Instance& instance )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNode::PlaySound( Instance& instance, SoundTag tag )
{
	SoundPlayer* player = m_effect->GetManager()->GetSoundPlayer();
	if( player == NULL )
	{
		return;
	}

	if( Sound.WaveId >= 0 )
	{
		Manager* manager = m_effect->GetManager();

		SoundPlayer::InstanceParameter parameter;
		parameter.Data = m_effect->GetWave( Sound.WaveId );
		parameter.Volume = Sound.Volume.getValue( *manager );
		parameter.Pitch = Sound.Pitch.getValue( *manager );
		parameter.Pan = Sound.Pan.getValue( *manager );
		
		parameter.Mode3D = Sound.PanType == ParamaterSoundPanType_3D;
		if( parameter.Mode3D )
		{
			::Effekseer::Vector3D::Transform( parameter.Position, 
				Vector3D(0.0f, 0.0f, 0.0f), instance.GetGlobalMatrix43() );
			parameter.Distance = Sound.Distance;
		}

		player->Play( tag, parameter );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectNode::Create( Effect* effect, unsigned char*& pos )
{
	EffectNode* effectnode = NULL;

	int node_type = 0;
	memcpy( &node_type, pos, sizeof(int) );

	if( node_type == EFFECT_NODE_TYPE_ROOT )
	{
		EffekseerPrintDebug("* Create : EffectNodeRoot\n");
		effectnode = new EffectNodeRoot( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_NONE )
	{
		EffekseerPrintDebug("* Create : EffectNodeNone\n");
		effectnode = new EffectNode( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_SPRITE )
	{
		EffekseerPrintDebug("* Create : EffectNodeSprite\n");
		effectnode = new EffectNodeSprite( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_RIBBON )
	{
		EffekseerPrintDebug("* Create : EffectNodeRibbon\n");
		effectnode = new EffectNodeRibbon( effect, pos );
	}
	else if( node_type == EFFECT_NODE_TYPE_RING )
	{
		EffekseerPrintDebug("* Create : EffectNodeRing\n");
		effectnode = new EffectNodeRing( effect, pos );
	}
	else
	{
		assert(0);
	}

	effectnode->LoadParamater( pos );

	return effectnode;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
