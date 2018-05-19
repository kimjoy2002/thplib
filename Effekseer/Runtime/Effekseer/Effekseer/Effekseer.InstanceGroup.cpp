

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.ManagerImplemented.h"

#include "Effekseer.InstanceGroup.h"
#include "Effekseer.Instance.h"
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
InstanceGroup::InstanceGroup( Manager* manager, EffectNode* effectNode, InstanceContainer* container, InstanceGlobal* global )
	: m_manager		( (ManagerImplemented*)manager )
	, m_effectNode	( effectNode )
	, m_container	( container )
	, m_global		( global )
	, IsReferencedFromInstance	( true )
	, NextUsedByInstance	( NULL )
	, NextUsedByContainer	( NULL )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
InstanceGroup::~InstanceGroup()
{
	RemoveForcibly();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::RemoveInvalidInstances()
{
	std::list<Instance*>::iterator it = m_removingInstances.begin();

	while( it != m_removingInstances.end() )
	{
		if( (*it)->m_State == INSTANCE_STATE_ACTIVE )
		{
			assert(0);
		}
		else if( (*it)->m_State == INSTANCE_STATE_REMOVING )
		{
			// 削除中処理
			(*it)->m_State = INSTANCE_STATE_REMOVED;
			it++;
		}
		else if( (*it)->m_State == INSTANCE_STATE_REMOVED )
		{
			// 削除処理
			(*it)->~Instance();
			m_manager->PushInstance( (*it) );
			it = m_removingInstances.erase( it );
			m_global->DecInstanceCount();
		}
	}
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::CreateInstance()
{
	Instance* buf = m_manager->PopInstance();
	if( buf == NULL ) return NULL;

	Instance* instance = new(buf)Instance( m_manager, m_effectNode, m_container );
	m_instances.push_back( instance );
	m_global->IncInstanceCount();
	return instance;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Instance* InstanceGroup::GetFirst()
{
	if( m_instances.size() > 0 )
	{
		return m_instances.front();
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int InstanceGroup::GetInstanceCount() const
{
	return (int32_t)m_instances.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::Update()
{
	RemoveInvalidInstances();

	std::list<Instance*>::iterator it = m_instances.begin();

	while( it != m_instances.end() )
	{
		if( (*it)->m_State == INSTANCE_STATE_ACTIVE )
		{
			// 更新処理
			(*it)->Update();

			// 破棄チェック
			if( (*it)->m_State != INSTANCE_STATE_ACTIVE )
			{
				m_removingInstances.push_back( (*it) );
				it = m_instances.erase( it );
			}
			else
			{
				it++;
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::RemoveForcibly()
{
	KillAllInstances();

	RemoveInvalidInstances();
	RemoveInvalidInstances();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InstanceGroup::KillAllInstances()
{
	std::list<Instance*>::iterator it = m_instances.begin();
	std::list<Instance*>::iterator it_end = m_instances.end();

	while( it != it_end )
	{
		if( (*it)->GetState() == INSTANCE_STATE_ACTIVE )
		{
			(*it)->Kill();
			m_removingInstances.push_back( (*it) );
		}

		it++;
	}

	m_instances.clear();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------