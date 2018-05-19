
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
	@brief	エフェクトの実体
*/
class Instance
{
	friend class Manager;
	friend class InstanceContainer;
public:

	// マネージャ
	Manager*	m_pManager;

	// パラメーター
	EffectNode* m_pEffectNode;

	// コンテナ
	InstanceContainer*	m_pContainer;

	// グループの連結リストの先頭
	InstanceGroup*	m_headGroups;

	// 親
	Instance*	m_pParent;

	// 位置
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
		

	// 補正位置
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

	// 回転
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

	// 拡大縮小
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

	// 描画
	union
	{
		EffectNodeSprite::InstanceValues	sprite;
		EffectNodeRibbon::InstanceValues	ribbon;
		EffectNodeRing::InstanceValues		ring;
	} rendererValues;
	
	// 音
	union
	{
		int		delay;
	} soundValues;

	// 状態
	eInstanceState	m_State;

	// 生存時間
	float		m_LivedTime;

	// 生成されてからの時間
	float		m_LivingTime;

	// 生成位置
	Matrix43		m_generation_location;

	// 変換用行列
	Matrix43		m_GlobalMatrix43;

	// 親の変換用行列
	Matrix43		m_ParentMatrix43;

	// コンストラクタ
	Instance( Manager* pManager, EffectNode* pEffectNode, InstanceContainer* pContainer );

	// デストラクタ
	virtual ~Instance();
public:

	/**
		@brief	状態の取得
	*/
	eInstanceState GetState() const;

	/**
		@brief	行列の取得
	*/
	const Matrix43& GetGlobalMatrix43() const;

	/**
		@brief	初期化
	*/
	void Initialize( Instance* parent );

	/**
		@brief	更新
	*/
	void Update();

	/**
		@brief	描画
	*/
	void Draw();

	/**
		@brief	破棄
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
