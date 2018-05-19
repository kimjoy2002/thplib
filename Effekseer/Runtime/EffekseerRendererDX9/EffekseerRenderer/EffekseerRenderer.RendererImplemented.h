
#ifndef	__EFFEKSEERRENDERER_RENDERER_IMPLEMENTED_H__
#define	__EFFEKSEERRENDERER_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.Base.h"
#include "EffekseerRenderer.Renderer.h"

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib" )
#pragma comment(lib, "d3dx9.lib" )

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

	LPDIRECT3DDEVICE9	m_d3d_device;

	VertexBuffer*		m_vertexBuffer;
	IndexBuffer*		m_indexBuffer;
	int32_t				m_squareMaxCount;

	::Effekseer::Matrix44	m_proj;
	::Effekseer::Matrix44	m_camera;
	::Effekseer::Matrix44	m_cameraProj;

	// ���W�n
	::Effekseer::eCoordinateSystem		m_coordinateSystem;

	std::set<DeviceObject*>	m_deviceObjects;

	// �X�e�[�g
	DWORD	m_state_FVF;

	DWORD	m_state_D3DRS_ALPHABLENDENABLE;
	DWORD	m_state_D3DRS_BLENDOP;
	DWORD	m_state_D3DRS_DESTBLEND;
	DWORD	m_state_D3DRS_SRCBLEND;
	DWORD	m_state_D3DRS_ALPHAREF;

	DWORD	m_state_D3DRS_ZENABLE;
	DWORD	m_state_D3DRS_ZWRITEENABLE;
	DWORD	m_state_D3DRS_ALPHATESTENABLE;
	DWORD	m_state_D3DRS_CULLMODE;

	DWORD	m_state_D3DRS_COLORVERTEX;
	DWORD	m_state_D3DRS_LIGHTING;
	DWORD	m_state_D3DRS_SHADEMODE;

	IDirect3DVertexDeclaration9*	m_state_vertexDeclaration;

	IDirect3DVertexBuffer9* m_state_streamData;
	UINT m_state_OffsetInBytes;
	UINT m_state_pStride;

	IDirect3DIndexBuffer9*	m_state_IndexData;

	IDirect3DBaseTexture9*	m_state_pTexture;

public:
	/**
		@brief	�R���X�g���N�^
	*/
	RendererImplemented( int32_t squareMaxCount );

	/**
		@brief	�f�X�g���N�^
	*/
	~RendererImplemented();

	void OnLostDevice();
	void OnResetDevice();

	/**
		@brief	������
	*/
	bool Initialize( LPDIRECT3DDEVICE9 device );

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
		@brief	�f�o�C�X�擾
	*/
	LPDIRECT3DDEVICE9 GetDevice();

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