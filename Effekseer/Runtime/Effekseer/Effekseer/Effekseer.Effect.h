
#ifndef	__EFFEKSEER_EFFECT_H__
#define	__EFFEKSEER_EFFECT_H__

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
	@brief	�G�t�F�N�g�p�����[�^�[�N���X
	@note
	�G�t�F�N�g�ɐݒ肳�ꂽ�p�����[�^�[�B
*/
class Effect
{
protected:
	Effect() {}
    ~Effect() {}

public:

	/**
		@brief	�G�t�F�N�g�𐶐�����B
		@param	manager	[in]	�Ǘ��N���X
		@param	data	[in]	�f�[�^�z��̐擪�̃|�C���^
		@param	size	[in]	�f�[�^�z��̒���
		@return	�G�t�F�N�g�B���s�����ꍇ��NULL��Ԃ��B
	*/
	static Effect* Create( Manager* manager, void* data, int32_t size );

	/**
		@brief	�G�t�F�N�g�𐶐�����B
		@param	manager	[in]	�Ǘ��N���X
		@param	path	[in]	�Ǎ����̃p�X	
		@return	�G�t�F�N�g�B���s�����ꍇ��NULL��Ԃ��B
	*/
	static Effect* Create( Manager* manager, const EFK_CHAR* path );

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	virtual int AddRef() = 0;

	/**
		@brief	�Q�ƃJ�E���^�����Z����B
		@return	���s��̎Q�ƃJ�E���^�̒l
	*/
	virtual int Release() = 0;

	/**
		@brief	�}�l�[�W���[���擾����B
		@return	�}�l�[�W���[
	*/
	virtual Manager* GetManager() const = 0;
	
	/**
		@brief	�G�t�F�N�g�f�[�^�̃o�[�W�����擾
	*/
	virtual int GetVersion() const = 0;

	/**
		@brief	�i�[����Ă���摜�̃|�C���^���擾����B
		@param	n	[in]	�摜�̃C���f�b�N�X
		@return	�摜�̃|�C���^
	*/
	virtual void* GetImage( int n ) const = 0;

	/**
		@brief	�i�[����Ă��鉹�g�`�̃|�C���^���擾����B
	*/
	virtual void* GetWave( int n ) const = 0;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_H__
