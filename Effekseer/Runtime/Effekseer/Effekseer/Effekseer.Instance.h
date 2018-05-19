
#ifndef	__EFFEKSEER_INSTANCE_H__
#define	__EFFEKSEER_INSTANCE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

#include "Effekseer.Vector3D.h"
#include "Effekseer.Matrix43.h"
#include "Effekseer.RectF.h"
#include "Effekseer.Color.h"

#include "Effekseer.EffectNodeSprite.h"
#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.EffectNodeRing.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�G�t�F�N�g�̎���
*/
class Instance
{
	friend class Manager;
	friend class InstanceContainer;
public:

	// �}�l�[�W��
	Manager*	m_pManager;

	// �p�����[�^�[
	EffectNode* m_pEffectNode;

	// �R���e�i
	InstanceContainer*	m_pContainer;

	// �O���[�v�̘A�����X�g�̐擪
	InstanceGroup*	m_headGroups;

	// �e
	Instance*	m_pParent;

	// �ʒu
	Vector3D	m_LocalPosition;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d velocity;
			vector3d acceleration;
		} random;

		struct
		{
			vector3d	start;
			vector3d	end;
		} easing;

	} translation_values;

	float memory_local[128];
	float memory_static[128];
		

	// �␳�ʒu
	vector3d	m_globalRevisionLocation;

	union 
	{
		struct
		{
		
		} none;

		struct
		{
			vector3d	velocity;
		} gravity;

	} translation_abs_values;

	// ��]
	Vector3D	m_LocalAngle;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d velocity;
			vector3d acceleration;
		} random;

		struct
		{
			vector3d start;
			vector3d  end;
		} easing;

	} rotation_values;

	// �g��k��
	Vector3D	m_LocalScaling;

	union 
	{
		struct
		{
		
		} fixed;

		struct
		{
			vector3d velocity;
			vector3d  acceleration;
		} random;

		struct
		{
			vector3d  start;
			vector3d  end;
		} easing;

	} scaling_values;

	// �`��
	union
	{
		EffectNodeSprite::InstanceValues	sprite;
		EffectNodeRibbon::InstanceValues	ribbon;
		EffectNodeRing::InstanceValues		ring;
	} rendererValues;
	
	// ��
	union
	{
		int		delay;
	} soundValues;

	// ���
	eInstanceState	m_State;

	// ��������
	float		m_LivedTime;

	// ��������Ă���̎���
	float		m_LivingTime;

	// �����ʒu
	Matrix43		m_generation_location;

	// �ϊ��p�s��
	Matrix43		m_GlobalMatrix43;

	// �e�̕ϊ��p�s��
	Matrix43		m_ParentMatrix43;

	// �R���X�g���N�^
	Instance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer );

	// �f�X�g���N�^
	virtual ~Instance();
public:

	/**
		@brief	��Ԃ̎擾
	*/
	eInstanceState GetState() const;

	/**
		@brief	�s��̎擾
	*/
	const Matrix43& GetGlobalMatrix43() const;

	/**
		@brief	������
	*/
	void Initialize( Instance* parent );

	/**
		@brief	�X�V
	*/
	void Update();

	/**
		@brief	�`��
	*/
	void Draw();

	/**
		@brief	�j��
	*/
	void Kill();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INSTANCE_H__
