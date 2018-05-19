
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
	@brief	�s��
	@note
	�E��n(��]:�����v���)<BR>
	����n(��]:���v���)<BR>
	V[x,y,z,1] * M �̌`<BR>
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
		@brief	�R���X�g���N�^
	*/
	Matrix43();

	/**
		@brief	�s��̒l
	*/
	float	Value[4][3];

	/**
		@brief	�P�ʍs��
	*/
	void Indentity();

	/**
		@brief	�g��s��
	*/
	void Scaling( float x, float y, float z );

	/**
		@brief	X����]�s��
	*/
	void RotationX( float angle );

	/**
		@brief	Y����]�s��
	*/
	void RotationY( float angle );

	/**
		@brief	Z����]�s��
	*/
	void RotationZ( float angle );

	/**
		@brief	�C�ӎ������v��]�s��
	*/
	void RotationAxis( const Vector3D& axis, float angle );

	/**
		@brief	�C�ӎ������v��]�s��
	*/
	void RotationAxis( const Vector3D& axis, float s, float c );

	/**
		@brief	�ړ��s��
	*/
	void Translation( float x, float y, float z );

	/**
		@brief	�s��̊g��A��]�A�ړ��̕���
	*/
	void GetSRT( Vector3D& s, Matrix43& r, Vector3D& t ) const; 

	/**
		@brief	�s��̊g��A��]�A�ړ���ݒ肷��B
	*/
	void SetSRT( const Vector3D& s, const Matrix43& r, const Vector3D& t );

	/**
		@brief	��Z
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
