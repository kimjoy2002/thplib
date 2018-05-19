
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
	@brief エフェクト管理クラス(実装)
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
		@brief	再生オブジェクトの組み合わせ
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
	/* 参照カウンタ */
	int	m_reference;

	// 次のHandle
	Handle		m_NextHandle;

	// メモリ確保関数
	MallocFunc	m_MallocFunc;

	// メモリ破棄関数
	FreeFunc	m_FreeFunc;

	// ランダム関数
	RandFunc	m_randFunc;

	// ランダム関数最大値
	int			m_randMax;

	// 座標系
	eCoordinateSystem		m_coordinateSystem;

	// 確保済みインスタンス数
	int m_instance_max;

	// 確保済みインスタンス
	std::queue<Instance*>	m_reserved_instances;

	// 確保済みインスタンスバッファ
	uint8_t*				m_reserved_instances_buffer;

	// 再生中オブジェクトの組み合わせ集合体
	std::map<Handle,DrawSet>	m_DrawSets;

	// 破棄待ちオブジェクト
	std::map<Handle,DrawSet>	m_RemovingDrawSets[2];

	/* スプライト描画機能用インスタンス */
	SpriteRenderer*				m_spriteRenderer;

	/* リボン描画機能用インスタンス */
	RibbonRenderer*				m_ribbonRenderer;

	/* リング描画機能用インスタンス */
	RingRenderer*				m_ringRenderer;

	/* テクスチャ読込インスタンス */
	TextureLoader*				m_textureLoader;

	/* エフェクト読込インスタンス */
	EffectLoader*				m_effectLoader;

	/* サウンド再生用インスタンス */
	SoundPlayer*				m_soundPlayer;

	/* サウンド読込インスタンス */
	SoundLoader*				m_soundLoader;
	
	// 描画オブジェクト追加
	Handle AddDrawSet( Effect* effect, InstanceContainer* pInstanceContainer, InstanceGlobal* pGlobalPointer );

	// 描画オブジェクト破棄処理
	void GCDrawSet( bool isRemovingManager );

	// インスタンスコンテナ生成
	InstanceContainer* CreateInstanceContainer( EffectNode* pEffectNode, InstanceGlobal* pGlobal, bool isRoot = false, Instance* pParent = NULL );

	// メモリ確保関数
	static void* EFK_STDCALL Malloc( unsigned int size );

	// メモリ破棄関数
	static void EFK_STDCALL Free( void* p, unsigned int size );

	// ランダム関数
	static int EFK_STDCALL Rand();
public:

	// コンストラクタ
	ManagerImplemented( int instance_max );

	// デストラクタ
	virtual ~ManagerImplemented();

	// 破棄済みインスタンスバッファ回収
	void PushInstance( Instance* instance );

	// インスタンスバッファ取得
	Instance* PopInstance();

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

	/**
		@brief マネージャー破棄
		@note
		このマネージャーから生成されたエフェクトは全て強制的に破棄されます。
	*/
	void Destroy();

	/**
		@brief	メモリ確保関数取得
	*/
	MallocFunc GetMallocFunc() const;

	/**
		@brief	メモリ確保関数設定
	*/
	void SetMallocFunc( MallocFunc func );

	/**
		@brief	メモリ破棄関数取得
	*/
	FreeFunc GetFreeFunc() const;

	/**
		@brief	メモリ破棄関数設定
	*/
	void SetFreeFunc( FreeFunc func );

	/**
		@brief	ランダム関数取得
	*/
	RandFunc GetRandFunc() const;

	/**
		@brief	ランダム関数設定
	*/
	void SetRandFunc( RandFunc func );

	/**
		@brief	ランダム最大値取得
	*/
	int GetRandMax() const;

	/**
		@brief	ランダム関数設定
	*/
	void SetRandMax( int max_ );

	/**
		@brief	座標系を取得する。
	*/
	eCoordinateSystem GetCoordinateSystem() const;

	/**
		@brief	座標系を設定する。
	*/
	void SetCoordinateSystem( eCoordinateSystem coordinateSystem );

	/**
		@brief	スプライト描画機能取得
	*/
	SpriteRenderer* GetSpriteRenderer();

	/**
		@brief	スプライト描画機能設定
	*/
	void SetSpriteRenderer( SpriteRenderer* renderer );

	/**
		@brief	リボン描画機能取得
	*/
	RibbonRenderer* GetRibbonRenderer();

	/**
		@brief	リボン描画機能設定
	*/
	void SetRibbonRenderer( RibbonRenderer* renderer );

	/**
		@brief	リング描画機能取得
	*/
	RingRenderer* GetRingRenderer();

	/**
		@brief	リング描画機能設定
	*/
	void SetRingRenderer( RingRenderer* renderer );

	/**
		@brief	エフェクト読込クラスを取得する。
	*/
	EffectLoader* GetEffectLoader();

	/**
		@brief	エフェクト読込クラスを設定する。
	*/
	void SetEffectLoader( EffectLoader* effectLoader );

	/**
		@brief	テクスチャ読込クラスを取得する。
	*/
	TextureLoader* GetTextureLoader();

	/**
		@brief	テクスチャ読込クラスを設定する。
	*/
	void SetTextureLoader( TextureLoader* textureLoader );
	
	/**
		@brief	サウンド再生取得
	*/
	SoundPlayer* GetSoundPlayer();

	/**
		@brief	サウンド再生機能設定
	*/
	void SetSoundPlayer( SoundPlayer* soundPlayer );
	
	/**
		@brief	サウンド再生取得
	*/
	SoundLoader* GetSoundLoader();

	/**
		@brief	サウンド再生機能設定
	*/
	void SetSoundLoader( SoundLoader* soundLoader );


	/**
		@brief	エフェクト停止
	*/
	void StopEffect( Handle handle );

	/**
		@brief	全てのエフェクト停止
	*/
	void StopAllEffects();

	/**
		@brief	エフェクト停止
	*/
	void StopRoot( Handle handle );

	/**
		@brief	エフェクト停止
	*/
	void StopRoot( Effect* effect );

	/**
		@brief	エフェクトのインスタンスが存在しているか取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	存在してるか?
	*/
	bool Exists( Handle handle ) const;

	/**
		@brief	エフェクトのインスタンスに設定されている行列を取得する。
		@param	handle	[in]	インスタンスのハンドル
		@return	行列
	*/
	Matrix43 GetMatrix( Handle handle );

	/**
		@brief	エフェクトのインスタンスに変換行列を設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	mat		[in]	変換行列
	*/
	void SetMatrix( Handle handle, const Matrix43& mat );

	Vector3D GetLocation( Handle handle );
	void SetLocation( Handle handle, float x, float y, float z );
	void SetLocation( Handle handle, const Vector3D& location );
	void AddLocation( Handle handle, const Vector3D& location );

	/**
		@brief	エフェクトのインスタンスの回転角度を指定する。(ラジアン)
	*/
	void SetRotation( Handle handle, float x, float y, float z );

	/**
		@brief	エフェクトのインスタンスの任意軸周りの反時計周りの回転角度を指定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	axis	[in]	軸
		@param	angle	[in]	角度(ラジアン)
	*/
	void SetRotation( Handle handle, const Vector3D& axis, float angle );

	/**
		@brief	エフェクトのインスタンスの拡大率を指定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	x		[in]	X方向拡大率
		@param	y		[in]	Y方向拡大率
		@param	z		[in]	Z方向拡大率
	*/
	void SetScale( Handle handle, float x, float y, float z );

	/**
		@brief	エフェクトのインスタンスに廃棄時のコールバックを設定する。
		@param	handle	[in]	インスタンスのハンドル
		@param	callback	[in]	コールバック
	*/
	void SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback );

	void SetShown( Handle handle, bool shown );
	void SetPaused( Handle handle, bool paused );

	/**
		@brief	更新処理
	*/
	void Update();

	/**
		@brief	描画処理
	*/
	void Draw();

	/**
		@brief	再生
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
