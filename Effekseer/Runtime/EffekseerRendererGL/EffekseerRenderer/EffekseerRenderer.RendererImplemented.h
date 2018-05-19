
#ifndef	__EFFEKSEERRENDERER_RENDERER_IMPLEMENTED_H__
#define	__EFFEKSEERRENDERER_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.Base.h"
#include "EffekseerRenderer.Renderer.h"

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�`��N���X
	@note
	�c�[�������̕`��@�\�B
*/
class RendererImplemented
	: public Renderer
{
friend class DeviceObject;

private:
	/* �Q�ƃJ�E���^ */
	int	m_reference;

	VertexBuffer*		m_vertexBuffer;
	IndexBuffer*		m_indexBuffer;
	int32_t				m_squareMaxCount;

	::Effekseer::Matrix44	m_proj;
	::Effekseer::Matrix44	m_camera;
	::Effekseer::Matrix44	m_cameraProj;

	std::set<DeviceObject*>	m_deviceObjects;

	// �X�e�[�g�ۑ��p
	GLboolean	m_stBlend;
	GLboolean	m_stCullFace;
	GLboolean	m_stDepthTest;
	GLboolean	m_stTexture;
	GLboolean	m_stDepthWrite;
	GLint		m_stBlendSrc;
	GLint		m_stBlendDst;

public:
	/**
		@brief	�R���X�g���N�^
	*/
	RendererImplemented( int32_t squareMaxCount );

	/**
		@brief	�f�X�g���N�^
	*/
	~RendererImplemented();

	/**
		@brief	������
	*/
	bool Initialize();

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

	void Destory();

	/**
		@brief	�`��J�n
	*/
	bool BeginRendering();

	/**
		@brief	�`��I��
	*/
	bool EndRendering();

	/**
		@brief	���_�o�b�t�@�擾
	*/
	VertexBuffer* GetVertexBuffer();

	/**
		@brief	�C���f�b�N�X�o�b�t�@�擾
	*/
	IndexBuffer* GetIndexBuffer();

	/**
		@brief	�ő�`��X�v���C�g��
	*/
	int32_t GetSquareMaxCount() const;

	/**
		@brief	���e�s����擾����B
	*/
	const ::Effekseer::Matrix44& GetProjectionMatrix() const;

	/**
		@brief	���e�s���ݒ肷��B
	*/
	void SetProjectionMatrix( const ::Effekseer::Matrix44& mat );

	/**
		@brief	�J�����s����擾����B
	*/
	const ::Effekseer::Matrix44& GetCameraMatrix() const;

	/**
		@brief	�J�����s���ݒ肷��B
	*/
	void SetCameraMatrix( const ::Effekseer::Matrix44& mat );

	/**
		@brief	�J�����v���W�F�N�V�����s����擾����B
	*/
	::Effekseer::Matrix44& GetCameraProjectionMatrix();

	/**
		@brief	�A���t�@�u�����h�̓K�p
	*/
	void ApplyAlphaBlend( ::Effekseer::eAlphaBlend alphaBlend );

	/**
		@brief	�X�v���C�g�����_���[�𐶐�����B
	*/
	::Effekseer::SpriteRenderer* CreateSpriteRenderer();

	/**
		@brief	���{�������_���[�𐶐�����B
	*/
	::Effekseer::RibbonRenderer* CreateRibbonRenderer();
	
	/**
		@brief	�����O�����_���[�𐶐�����B
	*/
	::Effekseer::RingRenderer* CreateRingRenderer();

	/**
		@brief	�e�N�X�`���Ǎ��N���X�𐶐�����B
	*/
	::Effekseer::TextureLoader* CreateTextureLoader();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RENDERER_IMPLEMENTED_H__