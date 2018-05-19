
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance::Instance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer )
	: m_pManager			( pManager )
	, m_pEffectNode		( pEffectNode )
	, m_pContainer			( pContainer )
	, m_headGroups		( NULL )
	, m_pParent			( NULL )
	, m_LocalScaling	( 1.0f, 1.0f, 1.0f )
	, m_State			( INSTANCE_STATE_ACTIVE )
	, m_LivedTime		( 0 )
	, m_LivingTime		( 0 )

{
	InstanceGroup* group = NULL;

	for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++ )
	{
		InstanceContainer* pContainer = m_pContainer->GetChild( i );

		if( group != NULL )
		{
			group->NextUsedByInstance = pContainer->CreateGroup();
			group = group->NextUsedByInstance;
		}
		else
		{
			group = pContainer->CreateGroup();
			m_headGroups = group;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance::~Instance()
{
	assert( m_State != INSTANCE_STATE_ACTIVE );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
eInstanceState Instance::GetState() const
{
	return m_State;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const Matrix43& Instance::GetGlobalMatrix43() const
{
	return m_GlobalMatrix43;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Initialize( Instance* parent )
{
	EffectNode* paramater = m_pEffectNode;

	// 親の設定
	m_pParent = parent;

	if( m_pParent == NULL )
	{
		// 状態の初期化
		m_State = INSTANCE_STATE_ACTIVE;

		// 時間周りの初期化
		m_LivingTime = 0.0f;
		m_LivedTime = FLT_MAX;

		// SRTの初期化
		m_LocalPosition = Vector3D( 0.0f, 0.0f, 0.0f );
		m_LocalAngle = Vector3D( 0.0f, 0.0f, 0.0f );
		m_LocalScaling = Vector3D( 1.0f, 1.0f, 1.0f );
		m_generation_location.Indentity();
		m_GlobalMatrix43.Indentity();
		m_ParentMatrix43.Indentity();

		// 親の初期化
		m_ParentMatrix43 = GetGlobalMatrix43();

		return;
	}


	// 状態の初期化
	m_State = INSTANCE_STATE_ACTIVE;

	// 時間周りの初期化
	m_LivingTime = 0.0f;
	m_LivedTime = (float)paramater->CommonValues.life.getValue( *m_pManager );
	
	// SRTの初期化
	m_LocalPosition = Vector3D( 0.0f, 0.0f, 0.0f );
	m_globalRevisionLocation.reset();
	m_LocalAngle = Vector3D( 0.0f, 0.0f, 0.0f );
	m_LocalScaling = Vector3D( 1.0f, 1.0f, 1.0f );
	m_generation_location.Indentity();
	m_GlobalMatrix43.Indentity();
	m_ParentMatrix43.Indentity();

	// 親の初期化
	if( paramater->CommonValues.TranslationBindType == BindType_WhenCreating )
	{
		m_ParentMatrix43.Value[3][0] = m_pParent->m_GlobalMatrix43.Value[3][0];
		m_ParentMatrix43.Value[3][1] = m_pParent->m_GlobalMatrix43.Value[3][1];
		m_ParentMatrix43.Value[3][2] = m_pParent->m_GlobalMatrix43.Value[3][2];
	}

	if( paramater->CommonValues.RotationBindType == BindType_WhenCreating &&
		paramater->CommonValues.ScalingBindType == BindType_WhenCreating )
	{
		for( int m = 0; m < 3; m++ )
		{
			for( int n = 0; n < 3; n++ )
			{
				m_ParentMatrix43.Value[m][n] = m_pParent->m_GlobalMatrix43.Value[m][n];
			}
		}
	}
	else if ( paramater->CommonValues.RotationBindType == BindType_WhenCreating )
	{
		for( int m = 0; m < 3; m++ )
		{
			for( int n = 0; n < 3; n++ )
			{
				m_ParentMatrix43.Value[m][n] = m_pParent->m_GlobalMatrix43.Value[m][n];
			}
		}

		float s[3];
		for( int m = 0; m < 3; m++ )
		{
			s[m] = 0;
			for( int n = 0; n < 3; n++ )
			{
				s[m] += m_ParentMatrix43.Value[m][n] * m_ParentMatrix43.Value[m][n];
			}
			s[m] = sqrt( s[m] );
		}

		for( int m = 0; m < 3; m++ )
		{
			for( int n = 0; n < 3; n++ )
			{
				m_ParentMatrix43.Value[m][n] = m_ParentMatrix43.Value[m][n] / s[m];
			}
		}
	}
	else if ( paramater->CommonValues.ScalingBindType == BindType_WhenCreating )
	{
		float s[3];
		for( int m = 0; m < 3; m++ )
		{
			s[m] = 0;
			for( int n = 0; n < 3; n++ )
			{
				s[m] += m_pParent->m_GlobalMatrix43.Value[m][n] * m_pParent->m_GlobalMatrix43.Value[m][n];
			}
			s[m] = sqrt( s[m] );
		}
		m_ParentMatrix43.Value[0][0] = s[0];
		m_ParentMatrix43.Value[1][1] = s[1];
		m_ParentMatrix43.Value[2][2] = s[2];
	}



	// 設定適用
	if( m_pEffectNode->TranslationType == ParamaterTranslationType_Fixed )
	{
		m_LocalPosition.X = m_pEffectNode->TranslationFixed.Position.X;
		m_LocalPosition.Y = m_pEffectNode->TranslationFixed.Position.Y;
		m_LocalPosition.Z = m_pEffectNode->TranslationFixed.Position.Z;
	}
	else if( m_pEffectNode->TranslationType == ParamaterTranslationType_PVA )
	{
		m_pEffectNode->TranslationPVA.location.getValue( *m_pManager ).setValueToArg( m_LocalPosition );
		translation_values.random.velocity = m_pEffectNode->TranslationPVA.velocity.getValue( *m_pManager );
		translation_values.random.acceleration = m_pEffectNode->TranslationPVA.acceleration.getValue( *m_pManager );
	}
	else if( m_pEffectNode->TranslationType == ParamaterTranslationType_Easing )
	{
		translation_values.easing.start = m_pEffectNode->TranslationEasing.start.getValue( *m_pManager );
		translation_values.easing.end = m_pEffectNode->TranslationEasing.end.getValue( *m_pManager );
		
		m_LocalPosition.X = translation_values.easing.start.x;
		m_LocalPosition.Y = translation_values.easing.start.y;
		m_LocalPosition.Z = translation_values.easing.start.z;
	}

	if( m_pEffectNode->LocationAbs.type == LocationAbsParamater::None )
	{
	}
	else if( m_pEffectNode->LocationAbs.type == LocationAbsParamater::Gravity )
	{
		translation_abs_values.gravity.velocity.reset();
	}

	if( m_pEffectNode->RotationType == ParamaterRotationType_Fixed )
	{
		m_LocalAngle.X = m_pEffectNode->RotationFixed.Position.X;
		m_LocalAngle.Y = m_pEffectNode->RotationFixed.Position.Y;
		m_LocalAngle.Z = m_pEffectNode->RotationFixed.Position.Z;
	}
	else if( m_pEffectNode->RotationType == ParamaterRotationType_PVA )
	{
		m_pEffectNode->RotationPVA.rotation.getValue( *m_pManager ).setValueToArg( m_LocalAngle );
		rotation_values.random.velocity = m_pEffectNode->RotationPVA.velocity.getValue( *m_pManager );
		rotation_values.random.acceleration = m_pEffectNode->RotationPVA.acceleration.getValue( *m_pManager );
	}
	else if( m_pEffectNode->RotationType == ParamaterRotationType_Easing )
	{
		rotation_values.easing.start = m_pEffectNode->RotationEasing.start.getValue( *m_pManager );
		rotation_values.easing.end = m_pEffectNode->RotationEasing.end.getValue( *m_pManager );
		rotation_values.easing.start.setValueToArg( m_LocalAngle );
	}

	if( m_pEffectNode->ScalingType == ParamaterScalingType_Fixed )
	{
		m_LocalScaling.X = m_pEffectNode->ScalingFixed.Position.X;
		m_LocalScaling.Y = m_pEffectNode->ScalingFixed.Position.Y;
		m_LocalScaling.Z = m_pEffectNode->ScalingFixed.Position.Z;
	}
	else if( m_pEffectNode->ScalingType == ParamaterScalingType_PVA )
	{
		m_pEffectNode->ScalingPVA.Position.getValue( *m_pManager ).setValueToArg( m_LocalScaling );
		scaling_values.random.velocity = m_pEffectNode->ScalingPVA.Velocity.getValue( *m_pManager );
		scaling_values.random.acceleration = m_pEffectNode->ScalingPVA.Acceleration.getValue( *m_pManager );
	}
	else if( m_pEffectNode->ScalingType == ParamaterScalingType_Easing )
	{
		scaling_values.easing.start = m_pEffectNode->ScalingEasing.start.getValue( *m_pManager );
		scaling_values.easing.end = m_pEffectNode->ScalingEasing.end.getValue( *m_pManager );
		scaling_values.easing.start.setValueToArg( m_LocalScaling );
	}

	if( m_pEffectNode->GenerationLocation.type == ParamaterGenerationLocation::TYPE_POINT )
	{
		vector3d p = m_pEffectNode->GenerationLocation.point.location.getValue( *m_pManager );
		m_generation_location.Translation( p.x, p.y, p.z );
	}
	else if( m_pEffectNode->GenerationLocation.type == ParamaterGenerationLocation::TYPE_SPHERE )
	{
		Matrix43 mat_x, mat_y;
		mat_x.RotationX( m_pEffectNode->GenerationLocation.sphere.rotation_x.getValue( *m_pManager ) );
		mat_y.RotationY( m_pEffectNode->GenerationLocation.sphere.rotation_y.getValue( *m_pManager ) );
		float r = m_pEffectNode->GenerationLocation.sphere.radius.getValue( *m_pManager );
		m_generation_location.Translation( 0, r, 0 );
		Matrix43::Multiple( m_generation_location, m_generation_location, mat_x );
		Matrix43::Multiple( m_generation_location, m_generation_location, mat_y );
	}

	if( m_pEffectNode->SoundType == ParamaterSoundType_Use )
	{
		soundValues.delay = m_pEffectNode->Sound.Delay.getValue( *m_pManager );
	}

	m_pEffectNode->InitializeRenderedInstance( *this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Update()
{
	assert( m_pEffectNode != NULL );
	assert( m_pContainer != NULL );

	// 親の処理
	if( m_pParent != NULL )
	{
		if( m_pParent->GetState() == INSTANCE_STATE_ACTIVE )
		{
			if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
			{
				BindType lType = m_pEffectNode->CommonValues.TranslationBindType;
				BindType rType = m_pEffectNode->CommonValues.RotationBindType;
				BindType sType = m_pEffectNode->CommonValues.ScalingBindType;

				const Matrix43* targetL;
				const Matrix43* targetR;
				const Matrix43* targetS;

				const Instance* rootInstance;
				if( m_pContainer->GetRootInstance()->GetRootContainer()->GetFirstGroup()->GetInstanceCount() > 0 )
				{
					rootInstance = m_pContainer->GetRootInstance()->GetRootContainer()->GetFirstGroup()->GetFirst();
				}
				else
				{
					rootInstance = NULL;
				}

				if( lType == BindType_Always )
				{
					targetL = &m_pParent->GetGlobalMatrix43();
				}
				else if( lType == BindType_NotBind_Root && rootInstance != NULL )
				{
					targetL = &rootInstance->GetGlobalMatrix43();
				}
				else
				{
					targetL = NULL;
				}

				if( rType == BindType_Always )
				{
					targetR = &m_pParent->GetGlobalMatrix43();
				}
				else if( rType == BindType_NotBind_Root && rootInstance != NULL )
				{
					targetR = &rootInstance->GetGlobalMatrix43();
				}
				else
				{
					targetR = NULL;
				}

				if( sType == BindType_Always )
				{
					targetS = &m_pParent->GetGlobalMatrix43();
				}
				else if( sType == BindType_NotBind_Root && rootInstance != NULL )
				{
					targetS = &rootInstance->GetGlobalMatrix43();
				}
				else
				{
					targetS = NULL;
				}

				if( (lType == BindType_Always && rType == BindType_Always && sType == BindType_Always) || 
					(lType == BindType_NotBind_Root && rType == BindType_NotBind_Root && sType == BindType_NotBind_Root) )
				{
					m_ParentMatrix43 = *targetL;
				}
				else
				{
					if( lType == BindType_Always || lType == BindType_NotBind_Root )
					{
						// 移動を毎回更新する場合
						m_ParentMatrix43.Value[3][0] = targetL->Value[3][0];
						m_ParentMatrix43.Value[3][1] = targetL->Value[3][1];
						m_ParentMatrix43.Value[3][2] = targetL->Value[3][2];
					}

					if( (rType == BindType_Always && sType == BindType_Always) || 
						(rType == BindType_NotBind_Root && sType == BindType_NotBind_Root ) )
					{
						// 回転と拡大を毎回更新する場合
						for( int i = 0; i < 3; i++ )
						{
							for( int j = 0; j < 3; j++ )
							{
								m_ParentMatrix43.Value[i][j] = targetR->Value[i][j];
							}
						}
					}
					else
					{
						if( rType == BindType_Always || rType == BindType_NotBind_Root )
						{
							// 回転を毎回更新する場合
							float s[3];
							for( int i = 0; i < 3; i++ )
							{
								s[i] = 0;
								for( int j = 0; j < 3; j++ )
								{
									s[i] += targetR->Value[i][j] * targetR->Value[i][j];
								}
								s[i] = sqrt( s[i] );
							}

							float s_parent[3];
							for( int i = 0; i < 3; i++ )
							{
								s_parent[i] = 0;
								for( int j = 0; j < 3; j++ )
								{
									s_parent[i] += m_ParentMatrix43.Value[i][j] * m_ParentMatrix43.Value[i][j];
								}
								s_parent[i] = sqrt(  s_parent[i] );
							}

							for( int i = 0; i < 3; i++ )
							{
								for( int j = 0; j < 3; j++ )
								{
									m_ParentMatrix43.Value[i][j] = m_ParentMatrix43.Value[i][j] / s_parent[i] * s[i];
								}
							}
						}

						if( sType == BindType_Always || sType == BindType_NotBind_Root )
						{
							// 拡大を毎フレーム更新する場合
							float s[3];
							for( int i = 0; i < 3; i++ )
							{
								s[i] = 0;
								for( int j = 0; j < 3; j++ )
								{
									s[i] += targetS->Value[i][j] * targetS->Value[i][j];
								}
								s[i] = sqrt( s[i] );
							}

							float s_parent[3];
							for( int i = 0; i < 3; i++ )
							{
								s_parent[i] = 0;
								for( int j = 0; j < 3; j++ )
								{
									 s_parent[i] += m_pParent->GetGlobalMatrix43().Value[i][j] * m_pParent->GetGlobalMatrix43().Value[i][j];
								}
								s_parent[i] = sqrt(  s_parent[i] );
							}

							for( int i = 0; i < 3; i++ )
							{
								for( int j = 0; j < 3; j++ )
								{
									m_ParentMatrix43.Value[i][j] = m_ParentMatrix43.Value[i][j] / s[i] * s_parent[i];
								}
							}
						}
						if( sType == BindType_NotBind )
						{
							// 拡大を消す場合
							float s_parent[3];
							for( int i = 0; i < 3; i++ )
							{
								s_parent[i] = 0;
								for( int j = 0; j < 3; j++ )
								{
									 s_parent[i] += m_pParent->GetGlobalMatrix43().Value[i][j] * m_pParent->GetGlobalMatrix43().Value[i][j];
								}
								s_parent[i] = sqrt(  s_parent[i] );
							}

							for( int i = 0; i < 3; i++ )
							{
								for( int j = 0; j < 3; j++ )
								{
									m_ParentMatrix43.Value[i][j] = m_ParentMatrix43.Value[i][j] / s_parent[i];
								}
							}
						}
					}
				}
			}
			else
			{
				// Rootの場合
				m_ParentMatrix43 = m_pParent->GetGlobalMatrix43();
			}
		}
		else
		{
			// 親が消えた場合、子も削除
			if( m_pEffectNode->GetType() == EFFECT_NODE_TYPE_ROOT )
			{
				
			}
			else
			{
				m_pParent = NULL;
				if( m_pEffectNode->CommonValues.RemoveWhenParentIsRemoved )
				{
					Kill();
					return;
				}
			}
			
			m_pParent = NULL;
		}
	}

	// 更新処理
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		if( m_pEffectNode->TranslationType == ParamaterTranslationType_Fixed )
		{
		
		}
		else if( m_pEffectNode->TranslationType == ParamaterTranslationType_PVA )
		{
			translation_values.random.velocity += translation_values.random.acceleration;
			m_LocalPosition.X += translation_values.random.velocity.x;
			m_LocalPosition.Y += translation_values.random.velocity.y;
			m_LocalPosition.Z += translation_values.random.velocity.z;
		}
		else if( m_pEffectNode->TranslationType == ParamaterTranslationType_Easing )
		{
			m_pEffectNode->TranslationEasing.setValueToArg(
				m_LocalPosition,
				translation_values.easing.start,
				translation_values.easing.end,
				m_LivingTime / m_LivedTime );
		}

		if( !m_pEffectNode->GenerationLocation.EffectsRotation )
		{
			m_LocalPosition.X += m_generation_location.Value[3][0];
			m_LocalPosition.Y += m_generation_location.Value[3][1];
			m_LocalPosition.Z += m_generation_location.Value[3][2];
		}

		if( m_pEffectNode->LocationAbs.type == LocationAbsParamater::None )
		{	
		}
		else if( m_pEffectNode->LocationAbs.type == LocationAbsParamater::Gravity )
		{
			translation_abs_values.gravity.velocity += m_pEffectNode->LocationAbs.gravity;
			m_globalRevisionLocation += translation_abs_values.gravity.velocity;
		}

		if( m_pEffectNode->RotationType == ParamaterRotationType_Fixed )
		{
		
		}
		else if( m_pEffectNode->RotationType == ParamaterRotationType_PVA )
		{
			rotation_values.random.velocity += rotation_values.random.acceleration;
			m_LocalAngle.X += rotation_values.random.velocity.x;
			m_LocalAngle.Y += rotation_values.random.velocity.y;
			m_LocalAngle.Z += rotation_values.random.velocity.z;
		}
		else if( m_pEffectNode->RotationType == ParamaterRotationType_Easing )
		{
			m_pEffectNode->RotationEasing.setValueToArg(
				m_LocalAngle,
				rotation_values.easing.start,
				rotation_values.easing.end,
				m_LivingTime / m_LivedTime );
		}

		if( m_pEffectNode->ScalingType == ParamaterScalingType_Fixed )
		{
		
		}
		else if( m_pEffectNode->ScalingType == ParamaterScalingType_PVA )
		{
			scaling_values.random.velocity.x += scaling_values.random.acceleration.x;
			scaling_values.random.velocity.y += scaling_values.random.acceleration.y;
			scaling_values.random.velocity.z += scaling_values.random.acceleration.z;
			m_LocalScaling.X += scaling_values.random.velocity.x;
			m_LocalScaling.Y += scaling_values.random.velocity.y;
			m_LocalScaling.Z += scaling_values.random.velocity.z;
		}
		else if( m_pEffectNode->ScalingType == ParamaterScalingType_Easing )
		{
			m_pEffectNode->ScalingEasing.setValueToArg(
				m_LocalScaling,
				scaling_values.easing.start,
				scaling_values.easing.end,
				m_LivingTime / m_LivedTime );
		}

		m_pEffectNode->UpdateRenderedInstance( *this );

		if( m_pEffectNode->SoundType == ParamaterSoundType_Use )
		{
			float living_time = m_LivingTime;
			float living_time_p = living_time + 1.0f;
			
			if( living_time <= (float)soundValues.delay && (float)soundValues.delay < living_time_p )
			{
				m_pEffectNode->PlaySound( *this, m_pContainer->GetRootInstance() );
			}
		}
	}
	
	// 行列の更新
	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{
		Matrix43 MatRotX,MatRotY,MatRotZ,MatTra,MatScale,MatTraGlobal;

		
		MatTra.Translation( m_LocalPosition.X, m_LocalPosition.Y, m_LocalPosition.Z );
		
		MatRotX.RotationX( m_LocalAngle.X );
		MatRotY.RotationY( m_LocalAngle.Y );
		MatRotZ.RotationZ( m_LocalAngle.Z );

		MatScale.Scaling( m_LocalScaling.X, m_LocalScaling.Y,  m_LocalScaling.Z );
			
		MatTraGlobal.Translation( m_globalRevisionLocation.x, m_globalRevisionLocation.y, m_globalRevisionLocation.z );
		
		
		m_GlobalMatrix43 = MatScale;
		
		if( m_pEffectNode->RotationType != ParamaterRotationType_None )
		{
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatRotZ );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatRotX );
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatRotY );
		}

		if( m_pEffectNode->TranslationType != ParamaterTranslationType_None )
		{
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatTra );
		}

		if( m_pEffectNode->GenerationLocation.EffectsRotation )
		{
			Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, m_generation_location );
		}

		Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, m_ParentMatrix43 );
		Matrix43::Multiple( m_GlobalMatrix43, m_GlobalMatrix43, MatTraGlobal );
		
	}

	// 子の処理
	{
		InstanceGroup* group = m_headGroups;

		for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++, group = group->NextUsedByInstance )
		{
			EffectNode* pNode = m_pEffectNode->GetChild( i );
			InstanceContainer* pContainer = m_pContainer->GetChild( i );
			assert( group != NULL );

			// インスタンス生成
			float living_time = m_LivingTime - pNode->CommonValues.GenerationTimeOffset;
			float living_time_p = living_time + 1.0f;

			if( living_time < 0 ) continue;

			int generation_count = (int)(living_time / pNode->CommonValues.GenerationTime);
			int generation_count_p = (int)(living_time_p / pNode->CommonValues.GenerationTime);
			
			// 最初の1フレーム
			if( living_time < 1.0f )
			{
				generation_count_p += 1;
			}
			else
			{
				generation_count += 1;
				generation_count_p += 1;
			}

			generation_count = Min( generation_count, pNode->CommonValues.MaxGeneration );
			generation_count_p = Min( generation_count_p, pNode->CommonValues.MaxGeneration );

			for( int j = generation_count; j < generation_count_p; j++ )
			{
				// 生成処理
				Instance* pNewInstance = group->CreateInstance();
				if( pNewInstance != NULL )
				{
					pNewInstance->Initialize(this);
				}
				else
				{
					break;
				}
			}
		}
	}

	// 時間の進行
	m_LivingTime += 1.0f;
		

	if( m_pEffectNode->GetType() != EFFECT_NODE_TYPE_ROOT )
	{	
		// 死亡判定
		if( m_pEffectNode->CommonValues.RemoveWhenLifeIsExtinct )
		{
			if( m_LivingTime > m_LivedTime )
			{
				Kill();
				return;
			}
		}

		if( m_pEffectNode->CommonValues.RemoveWhenChildrenIsExtinct )
		{
			int maxcreate_count = 0;
			InstanceGroup* group = m_headGroups;

			for( int i = 0; i < m_pEffectNode->GetChildrenCount(); i++, group = group->NextUsedByInstance )
			{
				float last_generation_time = 
					m_pEffectNode->GetChild(i)->CommonValues.GenerationTime *
					(m_pEffectNode->GetChild(i)->CommonValues.MaxGeneration - 1) +
					m_pEffectNode->GetChild(i)->CommonValues.GenerationTimeOffset + 
					1.0f;

				if( m_LivingTime >= last_generation_time && group->GetInstanceCount() == 0 )
				{
					maxcreate_count++;
				}
				else
				{
					break;
				}
			}
			
			if( maxcreate_count == m_pEffectNode->GetChildrenCount() )
			{
				Kill();
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Draw()
{
	assert( m_pEffectNode != NULL );

	if( !m_pEffectNode->IsRendered ) return;

	m_pEffectNode->Rendering( *this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Instance::Kill()
{
	if( m_State == INSTANCE_STATE_ACTIVE )
	{
		for( InstanceGroup* group = m_headGroups; group != NULL; group = group->NextUsedByInstance )
		{
			group->IsReferencedFromInstance = false;
		}

		m_State = INSTANCE_STATE_REMOVING;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------