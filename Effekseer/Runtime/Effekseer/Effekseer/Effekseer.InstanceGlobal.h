
#ifndef	__EFFEKSEER_INSTANCEGLOBAL_H__
#define	__EFFEKSEER_INSTANCEGLOBAL_H__

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
	@brief	�C���X�^���X���ʕ���
	@note
	�������ꂽ�C���X�^���X�̑S�Ă���Q�Ƃł��镔��
*/
class InstanceGlobal
{
	friend class ManagerImplemented;

private:
	int			m_instanceCount;
	
	InstanceContainer*	m_rootContainer;

	InstanceGlobal();

	virtual ~InstanceGlobal();

public:
	void IncInstanceCount();

	void DecInstanceCount();

	/**
		@brief	�S�ẴC���X�^���X�����擾
	*/
	int GetInstanceCount();

	InstanceContainer* GetRootContainer() const;
	void SetRootContainer( InstanceContainer* container );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCEGLOBAL_H__
