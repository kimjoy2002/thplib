
#ifndef	__EFFEKSEER_INSTANCEGROUP_H__
#define	__EFFEKSEER_INSTANCEGROUP_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�C���X�^���X�O���[�v
	@note
	�C���X�^���X�R���e�i���ł���ɃC���X�^���X���O���[�v������N���X
*/
class InstanceGroup
{
friend class InstanceContainer;

private:
	ManagerImplemented*		m_manager;
	EffectNode*			m_effectNode;
	InstanceContainer*	m_container;
	InstanceGlobal*		m_global;

	// �C���X�^���X�̎���
	std::list<Instance*> m_instances;
	std::list<Instance*> m_removingInstances;

	InstanceGroup( Manager* manager, EffectNode* effectNode, InstanceContainer* container, InstanceGlobal* global );

	~InstanceGroup();

	void RemoveInvalidInstances();

public:
	/**
		@brief	�C���X�^���X�̐���
	*/
	Instance* CreateInstance();

	Instance* GetFirst();

	int GetInstanceCount() const;

	void Update();

	void RemoveForcibly();

	void KillAllInstances();

	/**
		@brief	�O���[�v�𐶐������C���X�^���X����̎Q�Ƃ��c���Ă��邩?
	*/
	bool IsReferencedFromInstance;

	/**
		@brief	�C���X�^���X���痘�p����A�����X�g�̎��̃I�u�W�F�N�g�ւ̃|�C���^
	*/
	InstanceGroup*	NextUsedByInstance;

	/**
		@brief	�R���e�i���痘�p����A�����X�g�̎��̃I�u�W�F�N�g�ւ̃|�C���^
	*/
	InstanceGroup*	NextUsedByContainer;

};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCEGROUP_H__
