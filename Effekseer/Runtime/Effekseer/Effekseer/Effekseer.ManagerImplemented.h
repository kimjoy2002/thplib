
#ifndef	__EFFEKSEER_MANAGER_IMPLEMENTED_H__
#define	__EFFEKSEER_MANAGER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Manager.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief �G�t�F�N�g�Ǘ��N���X(����)
*/
class ManagerImplemented
	: public Manager
{
	friend class Effect;
	friend class EffectNode;
	friend class InstanceContainer;
	friend class InstanceGroup;

private:

	/**
		@brief	�Đ��I�u�W�F�N�g�̑g�ݍ��킹
	*/
	class DrawSet
	{
	public:
		Effect*			ParamaterPointer;
		InstanceContainer*	InstanceContainerPointer;
		InstanceGlobal*		GlobalPointer;
		bool				IsPaused;
		bool				IsShown;
		bool				IsRemoving;
		int32_t				Time;
		EffectInstanceRemovingCallback	RemovingCallback;

		DrawSet( Effect* effect, InstanceContainer* pContainer, InstanceGlobal* pGlobal )
			: ParamaterPointer			( effect )
			, InstanceContainerPointer	( pContainer )
			, GlobalPointer				( pGlobal )
			, IsPaused					( false )
			, IsShown					( true )
			, IsRemoving				( false )
			, Time						( 0 )
			, RemovingCallback			( NULL )
		{
		
		}

		DrawSet()
			: ParamaterPointer			( NULL )
			, InstanceContainerPointer	( NULL )
			, GlobalPointer				( NULL )
			, IsPaused					( false )
			, IsShown					( true )
			, IsRemoving				( false )
			, Time						( 0 )
			, RemovingCallback			( NULL )
		{
		
		}
	};

private:
	/* �Q�ƃJ�E���^ */
	int	m_reference;

	// ����Handle
	Handle		m_NextHandle;

	// �������m�ۊ֐�
	MallocFunc	m_MallocFunc;

	// �������j���֐�
	FreeFunc	m_FreeFunc;

	// �����_���֐�
	RandFunc	m_randFunc;

	// �����_���֐��ő�l
	int			m_randMax;

	// ���W�n
	eCoordinateSystem		m_coordinateSystem;

	// �m�ۍς݃C���X�^���X��
	int m_instance_max;

	// �m�ۍς݃C���X�^���X
	std::queue<Instance*>	m_reserved_instances;

	// �m�ۍς݃C���X�^���X�o�b�t�@
	uint8_t*				m_reserved_instances_buffer;

	// �Đ����I�u�W�F�N�g�̑g�ݍ��킹�W����
	std::map<Handle,DrawSet>	m_DrawSets;

	// �j���҂��I�u�W�F�N�g
	std::map<Handle,DrawSet>	m_RemovingDrawSets[2];

	/* �X�v���C�g�`��@�\�p�C���X�^���X */
	SpriteRenderer*				m_spriteRenderer;

	/* ���{���`��@�\�p�C���X�^���X */
	RibbonRenderer*				m_ribbonRenderer;

	/* �����O�`��@�\�p�C���X�^���X */
	RingRenderer*				m_ringRenderer;

	/* �e�N�X�`���Ǎ��C���X�^���X */
	TextureLoader*				m_textureLoader;

	/* �G�t�F�N�g�Ǎ��C���X�^���X */
	EffectLoader*				m_effectLoader;

	/* �T�E���h�Đ��p�C���X�^���X */
	SoundPlayer*				m_soundPlayer;

	/* �T�E���h�Ǎ��C���X�^���X */
	SoundLoader*				m_soundLoader;
	
	// �`��I�u�W�F�N�g�ǉ�
	Handle AddDrawSet( Effect* effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer );

	// �`��I�u�W�F�N�g�j������
	void GCDrawSet( bool isRemovingManager );

	// �C���X�^���X�R���e�i����
	InstanceContainer* CreateInstanceContainer( EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot = false, Instance* pParent = NULL );

	// �������m�ۊ֐�
	static void* EFK_STDCALL Malloc( unsigned int size );

	// �������j���֐�
	static void EFK_STDCALL Free( void* p, unsigned int size );

	// �����_���֐�
	static int EFK_STDCALL Rand();
public:

	// �R���X�g���N�^
	ManagerImplemented( int instance_max );

	// �f�X�g���N�^
	virtual ~ManagerImplemented();

	// �j���ς݃C���X�^���X�o�b�t�@���
	void PushInstance( Instance* instance );

	// �C���X�^���X�o�b�t�@�擾
	Instance* PopInstance();

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	int AddRef();

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	int Release();

	/**
		@brief �}�l�[�W���[�j��
		@note
		���̃}�l�[�W���[���琶�����ꂽ�G�t�F�N�g�͑S�ċ����I�ɔj������܂��B
	*/
	void Destroy();

	/**
		@brief	�������m�ۊ֐��擾
	*/
	MallocFunc GetMallocFunc() const;

	/**
		@brief	�������m�ۊ֐��ݒ�
	*/
	void SetMallocFunc( MallocFunc func );

	/**
		@brief	�������j���֐��擾
	*/
	FreeFunc GetFreeFunc() const;

	/**
		@brief	�������j���֐��ݒ�
	*/
	void SetFreeFunc( FreeFunc func );

	/**
		@brief	�����_���֐��擾
	*/
	RandFunc GetRandFunc() const;

	/**
		@brief	�����_���֐��ݒ�
	*/
	void SetRandFunc( RandFunc func );

	/**
		@brief	�����_���ő�l�擾
	*/
	int GetRandMax() const;

	/**
		@brief	�����_���֐��ݒ�
	*/
	void SetRandMax( int max_ );

	/**
		@brief	���W�n���擾����B
	*/
	eCoordinateSystem GetCoordinateSystem() const;

	/**
		@brief	���W�n��ݒ肷��B
	*/
	void SetCoordinateSystem( eCoordinateSystem coordinateSystem );

	/**
		@brief	�X�v���C�g�`��@�\�擾
	*/
	SpriteRenderer* GetSpriteRenderer();

	/**
		@brief	�X�v���C�g�`��@�\�ݒ�
	*/
	void SetSpriteRenderer( SpriteRenderer* renderer );

	/**
		@brief	���{���`��@�\�擾
	*/
	RibbonRenderer* GetRibbonRenderer();

	/**
		@brief	���{���`��@�\�ݒ�
	*/
	void SetRibbonRenderer( RibbonRenderer* renderer );

	/**
		@brief	�����O�`��@�\�擾
	*/
	RingRenderer* GetRingRenderer();

	/**
		@brief	�����O�`��@�\�ݒ�
	*/
	void SetRingRenderer( RingRenderer* renderer );

	/**
		@brief	�G�t�F�N�g�Ǎ��N���X���擾����B
	*/
	EffectLoader* GetEffectLoader();

	/**
		@brief	�G�t�F�N�g�Ǎ��N���X��ݒ肷��B
	*/
	void SetEffectLoader( EffectLoader* effectLoader );

	/**
		@brief	�e�N�X�`���Ǎ��N���X���擾����B
	*/
	TextureLoader* GetTextureLoader();

	/**
		@brief	�e�N�X�`���Ǎ��N���X��ݒ肷��B
	*/
	void SetTextureLoader( TextureLoader* textureLoader );
	
	/**
		@brief	�T�E���h�Đ��擾
	*/
	SoundPlayer* GetSoundPlayer();

	/**
		@brief	�T�E���h�Đ��@�\�ݒ�
	*/
	void SetSoundPlayer( SoundPlayer* soundPlayer );
	
	/**
		@brief	�T�E���h�Đ��擾
	*/
	SoundLoader* GetSoundLoader();

	/**
		@brief	�T�E���h�Đ��@�\�ݒ�
	*/
	void SetSoundLoader( SoundLoader* soundLoader );


	/**
		@brief	�G�t�F�N�g��~
	*/
	void StopEffect( Handle handle );

	/**
		@brief	�S�ẴG�t�F�N�g��~
	*/
	void StopAllEffects();

	/**
		@brief	�G�t�F�N�g��~
	*/
	void StopRoot( Handle handle );

	/**
		@brief	�G�t�F�N�g��~
	*/
	void StopRoot( Effect* effect );

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�����݂��Ă��邩�擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	���݂��Ă邩?
	*/
	bool Exists( Handle handle ) const;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɐݒ肳��Ă���s����擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	�s��
	*/
	Matrix43 GetMatrix( Handle handle );

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɕϊ��s���ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	mat		[in]	�ϊ��s��
	*/
	void SetMatrix( Handle handle, const Matrix43& mat );

	Vector3D GetLocation( Handle handle );
	void SetLocation( Handle handle, float x, float y, float z );
	void SetLocation( Handle handle, const Vector3D& location );
	void AddLocation( Handle handle, const Vector3D& location );

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̉�]�p�x���w�肷��B(���W�A��)
	*/
	void SetRotation( Handle handle, float x, float y, float z );

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̔C�ӎ�����̔����v����̉�]�p�x���w�肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	axis	[in]	��
		@param	angle	[in]	�p�x(���W�A��)
	*/
	void SetRotation( Handle handle, const Vector3D& axis, float angle );

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̊g�嗦���w�肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	x		[in]	X�����g�嗦
		@param	y		[in]	Y�����g�嗦
		@param	z		[in]	Z�����g�嗦
	*/
	void SetScale( Handle handle, float x, float y, float z );

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɔp�����̃R�[���o�b�N��ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	callback	[in]	�R�[���o�b�N
	*/
	void SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback );

	void SetShown( Handle handle, bool shown );
	void SetPaused( Handle handle, bool paused );

	/**
		@brief	�X�V����
	*/
	void Update();

	/**
		@brief	�`�揈��
	*/
	void Draw();

	/**
		@brief	�Đ�
	*/
	Handle Play( Effect* effect, float x, float y, float z );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MANAGER_IMPLEMENTED_H__
