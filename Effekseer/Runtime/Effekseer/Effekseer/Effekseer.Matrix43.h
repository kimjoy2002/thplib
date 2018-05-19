
#ifndef	__EFFEKSEER_MATRIX43_H__
#define	__EFFEKSEER_MATRIX43_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	行列
	@note
	右手系(回転:反時計回り)<BR>
	左手系(回転:時計回り)<BR>
	V[x,y,z,1] * M の形<BR>
	[0,0][0,1][0,2]
	[1,0][1,1][1,2]
	[2,0][2,1][2,2]
	[3,0][3,1][3,2]
*/
#pragma pack(push,1)
struct Matrix43
{
private:

public:

	/**
		@brief	コンストラクタ
	*/
	Matrix43();

	/**
		@brief	行列の値
	*/
	float	Value[4][3];

	/**
		@brief	単位行列化
	*/
	void Indentity();

	/**
		@brief	拡大行列化
	*/
	void Scaling( float x, float y, float z );

	/**
		@brief	X軸回転行列
	*/
	void RotationX( float angle );

	/**
		@brief	Y軸回転行列
	*/
	void RotationY( float angle );

	/**
		@brief	Z軸回転行列
	*/
	void RotationZ( float angle );

	/**
		@brief	任意軸反時計回転行列
	*/
	void RotationAxis( const Vector3D& axis, float angle );

	/**
		@brief	任意軸反時計回転行列
	*/
	void RotationAxis( const Vector3D& axis, float s, float c );

	/**
		@brief	移動行列化
	*/
	void Translation( float x, float y, float z );

	/**
		@brief	行列の拡大、回転、移動の分解
	*/
	void GetSRT( Vector3D& s, Matrix43& r, Vector3D& t ) const; 

	/**
		@brief	行列の拡大、回転、移動を設定する。
	*/
	void SetSRT( const Vector3D& s, const Matrix43& r, const Vector3D& t );

	/**
		@brief	乗算
	*/
	static void Multiple( Matrix43& out, const Matrix43& in1, const Matrix43& in2 );
};

#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MATRIX43_H__
