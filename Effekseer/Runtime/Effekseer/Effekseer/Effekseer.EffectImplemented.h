
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
	@brief	エフェクトパラメーター
	@note
	エフェクトに設定されたパラメーター。
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

	/* 拡大率 */
	float	m_maginification;

	// 子ノード
	EffectNode* m_pRoot;

public:
	/**
		@brief	生成
	*/
	static Effect* Create( Manager* pManager, void* pData, int size );

	// コンストラクタ
	EffectImplemented( Manager* pManager, void* pData, int size );

	// デストラクタ
	virtual ~EffectImplemented();

	// Rootの取得
	EffectNode* GetRoot() const;

	/* 拡大率の取得 */
	float GetMaginification() const;

	// 初期化されているか?
	bool IsInitialized() const;

	/**
		@brief	初期化を行う。
	*/
	void Initialize();

	/**
		@brief	参照カウンタ加算
	*/
	int AddRef();

	/**
		@brief	参照カウンタ減算
	*/
	int Release();

	/**
		@brief	マネージャー取得
	*/
	Manager* GetManager() const;
	
	/**
		@brief	エフェクトデータのバージョン取得
	*/
	int GetVersion() const;

	/**
		@brief	格納されている画像のポインタを取得する。
	*/
	void* GetImage( int n ) const;
	
	/**
		@brief	格納されている音波形のポインタを取得する。
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
