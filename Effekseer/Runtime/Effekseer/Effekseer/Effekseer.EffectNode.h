
#ifndef	__EFFEKSEER_EFFECTNODE_H__
#define	__EFFEKSEER_EFFECTNODE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Color.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.InternalStruct.h"
#include "Sound/Effekseer.SoundPlayer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum BindType
{
	BindType_NotBind = 0,
	BindType_NotBind_Root = 3,
	BindType_WhenCreating = 1,
	BindType_Always = 2,

	BindType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterCommonValues
{
	int MaxGeneration;
	BindType TranslationBindType;
	BindType RotationBindType;
	BindType ScalingBindType;
	int		RemoveWhenLifeIsExtinct;
	int		RemoveWhenParentIsRemoved;
	int		RemoveWhenChildrenIsExtinct;
	random_int	life;
	float GenerationTime;
	float GenerationTimeOffset;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParamaterTranslationType
{
	ParamaterTranslationType_Fixed = 0,
	ParamaterTranslationType_PVA = 1,
	ParamaterTranslationType_Easing = 2,

	ParamaterTranslationType_None = 0x7fffffff - 1,

	ParamaterTranslationType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterTranslationFixed
{
	Vector3D Position;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterTranslationPVA
{
	random_vector3d	location;
	random_vector3d	velocity;
	random_vector3d	acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct LocationAbsParamater
{
	enum
	{
		None = 0,
		Gravity = 1,
		//AttractiveForce = 2,
		//UniformlyAttractiveForce = 3,
		Paramater_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
		
		} none;

		vector3d	gravity;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParamaterRotationType
{
	ParamaterRotationType_Fixed = 0,
	ParamaterRotationType_PVA = 1,
	ParamaterRotationType_Easing = 2,

	ParamaterRotationType_None = 0x7fffffff - 1,

	ParamaterRotationType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterRotationFixed
{
	Vector3D Position;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterRotationPVA
{
	random_vector3d	rotation;
	random_vector3d	velocity;
	random_vector3d	acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParamaterScalingType
{
	ParamaterScalingType_Fixed = 0,
	ParamaterScalingType_PVA = 1,
	ParamaterScalingType_Easing = 2,

	ParamaterScalingType_None = 0x7fffffff - 1,

	ParamaterScalingType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterScalingFixed
{
	Vector3D Position;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterScalingPVA
{
	random_vector3d Position;
	random_vector3d Velocity;
	random_vector3d Acceleration;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterGenerationLocation
{
	int	EffectsRotation;

	enum
	{
		TYPE_POINT = 0,
		TYPE_SPHERE = 1,

		TYPE_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
			random_vector3d location;
		} point;

		struct
		{
			random_float radius;
			random_float rotation_x;
			random_float rotation_y;
		} sphere;
	};

	void load( uint8_t*& pos )
	{
		memcpy( &EffectsRotation, pos, sizeof(int) );
		pos += sizeof(int);

		memcpy( &type, pos, sizeof(int) );
		pos += sizeof(int);

		if( type == TYPE_POINT )
		{
			memcpy( &point, pos, sizeof(point) );
			pos += sizeof(point);
		}
		else if( type == TYPE_SPHERE )
		{
			memcpy( &sphere, pos, sizeof(sphere) );
			pos += sizeof(sphere);
		}
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParamaterSciptType
{
	ParamaterSciptType_None = 0,
	ParamaterSciptType_Script = 1,

	ParamaterSciptType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParamaterSoundType
{
	ParamaterSoundType_None = 0,
	ParamaterSoundType_Use = 1,

	ParamaterSoundType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParamaterSoundPanType
{
	ParamaterSoundPanType_2D = 0,
	ParamaterSoundPanType_3D = 1,

	ParamaterSoundPanType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParamaterSound
{
	int32_t			WaveId;
	random_float	Volume;
	random_float	Pitch;
	ParamaterSoundPanType PanType;
	random_float	Pan;
	float			Distance;
	random_int		Delay;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum eRenderingOrder
{
	RenderingOrder_FirstCreatedInstanceIsFirst = 0,
	RenderingOrder_FirstCreatedInstanceIsLast = 1,

	RenderingOrder_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�m�[�h�C���X�^���X�����N���X
	@note
	�G�t�F�N�g�̃m�[�h�̎��̂𐶐�����B
*/
class EffectNode
{
	friend class Manager;
	friend class EffectImplemented;
	friend class Instance;

protected:
	// �������Ă���p�����[�^�[
	Effect*	m_effect;

	// �q�m�[�h
	std::vector<EffectNode*>	m_Nodes;

	// ���[�U�[�f�[�^
	void* m_userData;

	// �R���X�g���N�^
	EffectNode( Effect* effect, unsigned char*& pos );

	// �f�X�g���N�^
	virtual ~EffectNode();

	// �Ǎ�
	void LoadParamater( unsigned char*& pos );

	// ������
	void Initialize();

public:

	/**
		@brief	�`�悷�邩?
	*/
	bool IsRendered;

	ParamaterCommonValues		CommonValues;

	ParamaterTranslationType	TranslationType;
	ParamaterTranslationFixed	TranslationFixed;
	ParamaterTranslationPVA		TranslationPVA;
	easing_vector3d				TranslationEasing;
	
	LocationAbsParamater		LocationAbs;

	ParamaterRotationType		RotationType;
	ParamaterRotationFixed		RotationFixed;
	ParamaterRotationPVA		RotationPVA;
	easing_vector3d				RotationEasing;
	
	ParamaterScalingType		ScalingType;
	ParamaterScalingFixed		ScalingFixed;
	ParamaterScalingPVA			ScalingPVA;
	easing_vector3d				ScalingEasing;
	
	ParamaterGenerationLocation	GenerationLocation;

	ParamaterSciptType			ScriptType;

	ParamaterSoundType			SoundType;
	ParamaterSound				Sound;

	eRenderingOrder	RenderingOrder;

	/**
		@biref	�I�v�V�����ǂݍ���
	*/
	void LoadOption( uint8_t*& pos );

	/**
		@brief	�������Ă���G�t�F�N�g�̎擾
	*/
	Effect* GetEffect() const;

	/**
		@brief	�q�̐��擾
	*/
	int GetChildrenCount() const;

	/**
		@brief	�q�̎擾
	*/
	EffectNode* GetChild( int num ) const;

	/**
		@brief	�`�敔���̓Ǎ�
	*/
	virtual void LoadRendererParamater( unsigned char*& pos );

	/**
		@brief	�`�敔���̏�����
	*/
	virtual void InitializeRenderer();

	/**
		@brief	�`�敔���̏I��
	*/
	virtual void FinalizeRenderer();

	/**
		@brief	�`��J�n
	*/
	virtual void BeginRendering();

	/**
		@brief	�O���[�v�`��J�n
	*/
	virtual void BeginRenderingGroup( int32_t instanceCount );

	/**
		@brief	�`��
	*/
	virtual void Rendering( const Instance& instance );

	/**
		@brief	�`��I��
	*/
	virtual void EndRendering();

	/**
		@brief	�`�敔��������
	*/
	virtual void InitializeRenderedInstance( Instance& instance );

	/**
		@brief	�`�敔���X�V
	*/
	virtual void UpdateRenderedInstance( Instance& instance );

	/**
		@brief	�T�E���h�Đ�
	*/
	virtual void PlaySound( Instance& instance, SoundTag tag );

	/**
		@brief	�G�t�F�N�g�m�[�h����
	*/
	static EffectNode* Create( Effect* effect, unsigned char*& pos );

	/**
		@brief	�m�[�h�̎�ގ擾
	*/
	virtual eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_NONE; }
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECTNODE_H__
