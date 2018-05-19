
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
	@brief	描画クラス
	@note
	ツール向けの描画機能。
*/
class RendererImplemented
	: public Renderer
{
friend class DeviceObject;

private:
	/* 参照カウンタ */
	int	m_reference;

	VertexBuffer*		m_vertexBuffer;
	IndexBuffer*		m_indexBuffer;
	int32_t				m_squareMaxCount;

	::Effekseer::Matrix44	m_proj;
	::Effekseer::Matrix44	m_camera;
	::Effekseer::Matrix44	m_cameraProj;

	std::set<DeviceObject*>	m_deviceObjects;

	// ステート保存用
	GLboolean	m_stBlend;
	GLboolean	m_stCullFace;
	GLboolean	m_stDepthTest;
	GLboolean	m_stTexture;
	GLboolean	m_stDepthWrite;
	GLint		m_stBlendSrc;
	GLint		m_stBlendDst;

public:
	/**
		@brief	コンストラクタ
	*/
	RendererImplemented( int32_t squareMaxCount );

	/**
		@brief	デストラクタ
	*/
	~RendererImplemented();

	/**
		@brief	初期化
	*/
	bool Initialize();

	/**
		@brief	参照カウンタを加算する。
		@return	実行後の参照カウンタの値
	*/
	int AddRef();

	/**
		@brief	参照カウンタを減算する。
		@return	実行後の参照カウンタの値
	*/
	int Release();

	void Destory();

	/**
		@brief	描画開始
	*/
	bool BeginRendering();

	/**
		@brief	描画終了
	*/
	bool EndRendering();

	/**
		@brief	頂点バッファ取得
	*/
	VertexBuffer* GetVertexBuffer();

	/**
		@brief	インデックスバッファ取得
	*/
	IndexBuffer* GetIndexBuffer();

	/**
		@brief	最大描画スプライト数
	*/
	int32_t GetSquareMaxCount() const;

	/**
		@brief	投影行列を取得する。
	*/
	const ::Effekseer::Matrix44& GetProjectionMatrix() const;

	/**
		@brief	投影行列を設定する。
	*/
	void SetProjectionMatrix( const ::Effekseer::Matrix44& mat );

	/**
		@brief	カメラ行列を取得する。
	*/
	const ::Effekseer::Matrix44& GetCameraMatrix() const;

	/**
		@brief	カメラ行列を設定する。
	*/
	void SetCameraMatrix( const ::Effekseer::Matrix44& mat );

	/**
		@brief	カメラプロジェクション行列を取得する。
	*/
	::Effekseer::Matrix44& GetCameraProjectionMatrix();

	/**
		@brief	アルファブレンドの適用
	*/
	void ApplyAlphaBlend( ::Effekseer::eAlphaBlend alphaBlend );

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	::Effekseer::SpriteRenderer* CreateSpriteRenderer();

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRenderer* CreateRibbonRenderer();
	
	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRenderer* CreateRingRenderer();

	/**
		@brief	テクスチャ読込クラスを生成する。
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