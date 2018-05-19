
#ifndef	__EFFEKSEER_EFFECT_IMPLEMENTED_H__
#define	__EFFEKSEER_EFFECT_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Effect.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	�G�t�F�N�g�p�����[�^�[
	@note
	�G�t�F�N�g�ɐݒ肳�ꂽ�p�����[�^�[�B
*/

class EffectImplemented
	: public Effect
{
	friend class ManagerImplemented;
private:
	ManagerImplemented* m_pManager;

	bool m_isInitialized;

	int	m_reference;

	int	m_version;

	int	m_ImageCount;
	EFK_CHAR**		m_ImagePaths;
	void**			m_pImages;
	
	int	m_WaveCount;
	EFK_CHAR**		m_WavePaths;
	void**			m_pWaves;

	/* �g�嗦 */
	float	m_maginification;

	// �q�m�[�h
	EffectNode* m_pRoot;

public:
	/**
		@brief	����
	*/
	static Effect* Create( Manager* pManager, void* pData, int size );

	// �R���X�g���N�^
	EffectImplemented( Manager* pManager, void* pData, int size );

	// �f�X�g���N�^
	virtual ~EffectImplemented();

	// Root�̎擾
	EffectNode* GetRoot() const;

	/* �g�嗦�̎擾 */
	float GetMaginification() const;

	// ����������Ă��邩?
	bool IsInitialized() const;

	/**
		@brief	���������s���B
	*/
	void Initialize();

	/**
		@brief	�Q�ƃJ�E���^���Z
	*/
	int AddRef();

	/**
		@brief	�Q�ƃJ�E���^���Z
	*/
	int Release();

	/**
		@brief	�}�l�[�W���[�擾
	*/
	Manager* GetManager() const;
	
	/**
		@brief	�G�t�F�N�g�f�[�^�̃o�[�W�����擾
	*/
	int GetVersion() const;

	/**
		@brief	�i�[����Ă���摜�̃|�C���^���擾����B
	*/
	void* GetImage( int n ) const;
	
	/**
		@brief	�i�[����Ă��鉹�g�`�̃|�C���^���擾����B
	*/
	void* GetWave( int n ) const;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_IMPLEMENTED_H__
