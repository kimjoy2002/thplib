

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"

#include "Effekseer.EffectNode.h"
#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.DefaultEffectLoader.h"
#include "Effekseer.TextureLoader.h"

#include "Renderer/Effekseer.SpriteRenderer.h"
#include "Renderer/Effekseer.RibbonRenderer.h"
#include "Renderer/Effekseer.RingRenderer.h"

#include "Effekseer.SoundLoader.h"
#include "Sound/Effekseer.SoundPlayer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Manager* Manager::Create( int instance_max )
{
	return new ManagerImplemented( instance_max );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Handle ManagerImplemented::AddDrawSet( Effect* effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer )
{
	Handle Temp = m_NextHandle;
	m_NextHandle++;
	
	DrawSet drawset( effect, pInstanceContainer, pGlobalPointer );

	ES_SAFE_ADDREF( effect );

	m_DrawSets[Temp] = drawset;

	return Temp;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::GCDrawSet( bool isRemovingManager )
{
	// インスタンスグループ自体の削除処理
	{
		std::map<Handle,DrawSet>::iterator it = m_RemovingDrawSets[1].begin();
		while( it != m_RemovingDrawSets[1].end() )
		{
			DrawSet& drawset = (*it).second;

			// 全破棄処理
			drawset.InstanceContainerPointer->RemoveForcibly( true );
			drawset.InstanceContainerPointer->~InstanceContainer();
			InstanceContainer::operator delete( drawset.InstanceContainerPointer, this );
			ES_SAFE_RELEASE( drawset.ParamaterPointer );
			ES_SAFE_DELETE( drawset.GlobalPointer );
			m_RemovingDrawSets[1].erase( it++ );
		}
		m_RemovingDrawSets[1].clear();
	}

	{
		std::map<Handle,DrawSet>::iterator it = m_RemovingDrawSets[0].begin();
		while( it != m_RemovingDrawSets[0].end() )
		{
			DrawSet& drawset = (*it).second;
			m_RemovingDrawSets[1][ (*it).first ] = (*it).second;
			m_RemovingDrawSets[0].erase( it++ );
		}
		m_RemovingDrawSets[0].clear();
	}

	{
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		while( it != m_DrawSets.end() )
		{
			DrawSet& draw_set = (*it).second;

			// 削除フラグが立っている時
			bool isRemoving = draw_set.IsRemoving;

			// 何も存在しない時
			if( !isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 0 )
			{
				isRemoving = true;
			}

			// ルートのみ存在し、既に新しく生成する見込みがないとき
			if( !isRemoving && draw_set.GlobalPointer->GetInstanceCount() == 1 )
			{
				InstanceContainer* pRootContainer = draw_set.InstanceContainerPointer;
				InstanceGroup* group = pRootContainer->GetFirstGroup();
				Instance* pRootInstance = group->GetFirst();

				if( pRootInstance->GetState() == INSTANCE_STATE_ACTIVE )
				{
					int maxcreate_count = 0;
					for( int i = 0; i < pRootInstance->m_pEffectNode->GetChildrenCount(); i++ )
					{
						float last_generation_time = 
							pRootInstance->m_pEffectNode->GetChild(i)->CommonValues.GenerationTime *
							(pRootInstance->m_pEffectNode->GetChild(i)->CommonValues.MaxGeneration - 1) +
							pRootInstance->m_pEffectNode->GetChild(i)->CommonValues.GenerationTimeOffset +
							1.0f;

						if( pRootInstance->m_LivingTime >= last_generation_time )
						{
							maxcreate_count++;
						}
						else
						{
							break;
						}
					}
					
					if( maxcreate_count == pRootInstance->m_pEffectNode->GetChildrenCount() )
					{
						isRemoving = true;
					}
				}
			}

			if( isRemoving )
			{
				// 消去処理
				StopEffect( (*it).first );

				if( (*it).second.RemovingCallback != NULL )
				{
					(*it).second.RemovingCallback( this, (*it).first, isRemovingManager );
				}

				m_RemovingDrawSets[0][ (*it).first ] = (*it).second;
				m_DrawSets.erase( it++ );
			}
			else
			{
				++it;
			}			
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceContainer* ManagerImplemented::CreateInstanceContainer( EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot, Instance* pParent )
{
	InstanceContainer* pContainer = new(this) InstanceContainer(
		this,
		pEffectNode,
		pGlobal,
		pEffectNode->GetChildrenCount() );
	
	for( int i = 0; i < pEffectNode->GetChildrenCount(); i++ )
	{
		pContainer->SetChild( i, CreateInstanceContainer( pEffectNode->GetChild(i), pGlobal ) );
	}

	if( isRoot )
	{
		InstanceGroup* group = pContainer->CreateGroup();
		Instance* instance = group->CreateInstance();
		instance->Initialize( NULL );

		/* インスタンスが生成したわけではないためfalseに変更 */
		group->IsReferencedFromInstance = false;
	}

	return pContainer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* EFK_STDCALL ManagerImplemented::Malloc( unsigned int size )
{
	return (void*)new char*[size];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EFK_STDCALL ManagerImplemented::Free( void* p, unsigned int size )
{
	char* pData = (char*)p;
	delete [] pData;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EFK_STDCALL ManagerImplemented::Rand()
{
	return rand();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ManagerImplemented::ManagerImplemented( int instance_max )
	: m_reference	( 1 )
	, m_NextHandle	( 0 )
	, m_MallocFunc		( NULL )
	, m_FreeFunc		( NULL )
	, m_randFunc		( NULL )
	, m_randMax			( 0 )
	, m_coordinateSystem	( COORDINATE_SYSTEM_RH )
	, m_instance_max	( instance_max )
	, m_spriteRenderer	( NULL )
	, m_ribbonRenderer	( NULL )
	, m_ringRenderer	( NULL )
	, m_effectLoader	( new DefaultEffectLoader() )
	, m_textureLoader	( NULL )
	, m_soundPlayer		( NULL )
	, m_soundLoader		( NULL )
{
	SetMallocFunc( Malloc );
	SetFreeFunc( Free );
	SetRandFunc( Rand );
	SetRandMax( RAND_MAX );

	m_reserved_instances_buffer = new uint8_t[ sizeof(Instance) * m_instance_max ];

	for( int i = 0; i < m_instance_max; i++ )
	{
		Instance* instances = (Instance*)m_reserved_instances_buffer;
		m_reserved_instances.push( &instances[i] );
	}

	EffekseerPrintDebug("*** Create : Manager\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ManagerImplemented::~ManagerImplemented()
{
	StopAllEffects();

	for( int i = 0; i < 5; i++ )
	{
		GCDrawSet( true );
	}

	assert( m_reserved_instances.size() == m_instance_max ); 
	ES_SAFE_DELETE_ARRAY( m_reserved_instances_buffer );
	
	ES_SAFE_DELETE( m_effectLoader );
	ES_SAFE_DELETE( m_textureLoader );
	ES_SAFE_DELETE( m_spriteRenderer );
	ES_SAFE_DELETE( m_ribbonRenderer );
	ES_SAFE_DELETE( m_ringRenderer );
	ES_SAFE_DELETE( m_soundLoader );
	ES_SAFE_DELETE( m_soundPlayer );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::PushInstance( Instance* instance )
{
	m_reserved_instances.push( instance );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* ManagerImplemented::PopInstance()
{
	if( m_reserved_instances.empty() )
	{
		return NULL;
	}

	Instance* ret = m_reserved_instances.front();
	m_reserved_instances.pop();
	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int ManagerImplemented::AddRef()
{
	m_reference++;
	return m_reference;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int ManagerImplemented::Release()
{
	m_reference--;
	int count = m_reference;
	if ( count == 0 )
	{
		delete this;
	}
	return count;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Destroy()
{
	StopAllEffects();

	for( int i = 0; i < 5; i++ )
	{
		GCDrawSet( true );
	}

	Release();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
MallocFunc ManagerImplemented::GetMallocFunc() const
{
	return m_MallocFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetMallocFunc( MallocFunc func )
{
	m_MallocFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
FreeFunc ManagerImplemented::GetFreeFunc() const
{
	return m_FreeFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetFreeFunc( FreeFunc func )
{
	m_FreeFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RandFunc ManagerImplemented::GetRandFunc() const
{
	return m_randFunc;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRandFunc( RandFunc func )
{
	m_randFunc = func;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int ManagerImplemented::GetRandMax() const
{
	return m_randMax;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRandMax( int max_ )
{
	m_randMax = max_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
eCoordinateSystem ManagerImplemented::GetCoordinateSystem() const
{
	return m_coordinateSystem;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetCoordinateSystem( eCoordinateSystem coordinateSystem )
{
	m_coordinateSystem = coordinateSystem;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer* ManagerImplemented::GetSpriteRenderer()
{
	return m_spriteRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetSpriteRenderer( SpriteRenderer* renderer )
{
	ES_SAFE_DELETE( m_spriteRenderer );
	m_spriteRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer* ManagerImplemented::GetRibbonRenderer()
{
	return m_ribbonRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRibbonRenderer( RibbonRenderer* renderer )
{
	ES_SAFE_DELETE( m_ribbonRenderer );
	m_ribbonRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* ManagerImplemented::GetRingRenderer()
{
	return m_ringRenderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRingRenderer( RingRenderer* renderer )
{
	ES_SAFE_DELETE( m_ringRenderer );
	m_ringRenderer = renderer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectLoader* ManagerImplemented::GetEffectLoader()
{
	return m_effectLoader;
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetEffectLoader( EffectLoader* effectLoader )
{
	ES_SAFE_DELETE( m_effectLoader );
	m_effectLoader = effectLoader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader* ManagerImplemented::GetTextureLoader()
{
	return m_textureLoader;
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetTextureLoader( TextureLoader* textureLoader )
{
	ES_SAFE_DELETE( m_textureLoader );
	m_textureLoader = textureLoader;
}

SoundPlayer* ManagerImplemented::GetSoundPlayer()
{
	return m_soundPlayer;
}

void ManagerImplemented::SetSoundPlayer( SoundPlayer* soundPlayer )
{
	ES_SAFE_DELETE( m_soundPlayer );
	m_soundPlayer = soundPlayer;
}

SoundLoader* ManagerImplemented::GetSoundLoader()
{
	return m_soundLoader;
}

void ManagerImplemented::SetSoundLoader( SoundLoader* soundLoader )
{
	ES_SAFE_DELETE( m_soundLoader );
	m_soundLoader = soundLoader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopEffect( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		DrawSet& drawSet = m_DrawSets[handle];
		InstanceContainer* pContainer = drawSet.InstanceContainerPointer;
		pContainer->KillAllInstances( true );
		drawSet.IsRemoving = true;
		if( m_soundPlayer )
		{
			m_soundPlayer->StopTag( drawSet.GlobalPointer );
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopAllEffects()
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	while( it != m_DrawSets.end() )
	{
		InstanceContainer* pContainer = (*it).second.InstanceContainerPointer;
		pContainer->KillAllInstances( true );
		(*it).second.IsRemoving = true;
		++it;		
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopRoot( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		for( int i = 0; i < pContainer->m_ChildrenCount; i++ )
		{
			pContainer->GetChild( i )->KillAllInstances( false );
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::StopRoot( Effect* effect )
{
	std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
	std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();

	while( it != it_end )
	{
		if( (*it).second.ParamaterPointer == effect )
		{
			InstanceContainer* pContainer = (*it).second.InstanceContainerPointer;
			for( int i = 0; i < pContainer->m_ChildrenCount; i++ )
			{
				pContainer->GetChild( i )->KillAllInstances( false );
			}
		}
		++it;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool ManagerImplemented::Exists( Handle handle ) const
{
	return m_DrawSets.count( handle ) > 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Matrix43 ManagerImplemented::GetMatrix( Handle handle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		return pInstance->m_GlobalMatrix43;
	}

	return Matrix43();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetMatrix( Handle handle, const Matrix43& mat )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		pInstance->m_GlobalMatrix43 = mat;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Vector3D ManagerImplemented::GetLocation( Handle handle )
{
	Vector3D location;

	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		location.X = pInstance->m_GlobalMatrix43.Value[3][0];
		location.Y = pInstance->m_GlobalMatrix43.Value[3][1];
		location.Z = pInstance->m_GlobalMatrix43.Value[3][2];
	}

	return location;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetLocation( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		pInstance->m_GlobalMatrix43.Value[3][0] = x;
		pInstance->m_GlobalMatrix43.Value[3][1] = y;
		pInstance->m_GlobalMatrix43.Value[3][2] = z;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetLocation( Handle handle, const Vector3D& location )
{
	SetLocation( handle, location.X, location.Y, location.Z );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::AddLocation( Handle handle, const Vector3D& location )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		pInstance->m_GlobalMatrix43.Value[3][0] += location.X;
		pInstance->m_GlobalMatrix43.Value[3][1] += location.Y;
		pInstance->m_GlobalMatrix43.Value[3][2] += location.Z;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRotation( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		Matrix43 MatRotX,MatRotY,MatRotZ;

		MatRotX.RotationX( x );
		MatRotY.RotationY( y );
		MatRotZ.RotationZ( z );
		
		Matrix43 r;
		Vector3D s, t;

		pInstance->m_GlobalMatrix43.GetSRT( s, r, t );

		Matrix43::Multiple( pInstance->m_GlobalMatrix43, pInstance->m_GlobalMatrix43, MatRotZ );
		Matrix43::Multiple( pInstance->m_GlobalMatrix43, pInstance->m_GlobalMatrix43, MatRotX );
		Matrix43::Multiple( pInstance->m_GlobalMatrix43, pInstance->m_GlobalMatrix43, MatRotY );

		pInstance->m_GlobalMatrix43.SetSRT( s, r, t );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRotation( Handle handle, const Vector3D& axis, float angle )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		Matrix43 r;
		Vector3D s, t;

		pInstance->m_GlobalMatrix43.GetSRT( s, r, t );

		r.RotationAxis( axis, angle );

		pInstance->m_GlobalMatrix43.SetSRT( s, r, t );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetScale( Handle handle, float x, float y, float z )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		InstanceContainer* pContainer = m_DrawSets[handle].InstanceContainerPointer;
		
		Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

		Matrix43 r;
		Vector3D s, t;

		pInstance->m_GlobalMatrix43.GetSRT( s, r, t );

		s.X = x;
		s.Y = y;
		s.Z = z;

		pInstance->m_GlobalMatrix43.SetSRT( s, r, t );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].RemovingCallback = callback;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetShown( Handle handle, bool shown )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsShown = shown;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::SetPaused( Handle handle, bool paused )
{
	if( m_DrawSets.count( handle ) > 0 )
	{
		m_DrawSets[handle].IsPaused = paused;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Update()
{
	// DrawSet削除処理
	GCDrawSet( false );

	{
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();
		while( it != it_end )
		{
			Instance* pInstance = NULL;

			if( !(*it).second.IsPaused )
			{
				if( ((EffectImplemented*)(*it).second.ParamaterPointer)->GetMaginification() != 1.0f )
				{
					/* エフェクト拡大適用時 */
					Instance* pInstance = (*it).second.InstanceContainerPointer->GetFirstGroup()->GetFirst();
					Matrix43 temp_mat = pInstance->m_GlobalMatrix43;

					Matrix43 r;
					Vector3D s, t;

					pInstance->m_GlobalMatrix43.GetSRT( s, r, t );

					float mag = ((EffectImplemented*)(*it).second.ParamaterPointer)->GetMaginification();
					s.X *= mag;
					s.Y *= mag;
					s.Z *= mag;

					pInstance->m_GlobalMatrix43.SetSRT( s, r, t );

					(*it).second.InstanceContainerPointer->Update( true );
					(*it).second.Time++;

					pInstance->m_GlobalMatrix43 = temp_mat;
				}
				else
				{
					/* エフェクト標準サイズ時 */
					(*it).second.InstanceContainerPointer->Update( true );
					(*it).second.Time++;
				}
			}

			++it;	
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ManagerImplemented::Draw()
{
	{
		std::map<Handle,DrawSet>::iterator it = m_DrawSets.begin();
		std::map<Handle,DrawSet>::iterator it_end = m_DrawSets.end();
		while( it != it_end )
		{
			if( (*it).second.IsShown )
			{
				(*it).second.InstanceContainerPointer->Draw( true );
			}

			++it;	
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Handle ManagerImplemented::Play( Effect* effect, float x, float y, float z )
{
	assert( effect != NULL );
	
	// ルート生成
	InstanceGlobal* pGlobal = new InstanceGlobal();
	InstanceContainer* pContainer = CreateInstanceContainer( ((EffectImplemented*)effect)->GetRoot(), pGlobal, true, NULL );
	
	pGlobal->SetRootContainer(  pContainer );

	Instance* pInstance = pContainer->GetFirstGroup()->GetFirst();

	pInstance->m_GlobalMatrix43.Value[3][0] = x;
	pInstance->m_GlobalMatrix43.Value[3][1] = y;
	pInstance->m_GlobalMatrix43.Value[3][2] = z;

	Handle handle = AddDrawSet( effect, pContainer, pGlobal );

	return handle;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
