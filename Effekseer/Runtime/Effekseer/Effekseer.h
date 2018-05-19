
#ifndef	__EFFEKSEER_BASE_PRE_H__
#define	__EFFEKSEER_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <stdio.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
#define	EFK_STDCALL	__stdcall
#else
#define	EFK_STDCALL
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
typedef signed char			int8_t;
typedef unsigned char		uint8_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef int					int32_t;
typedef unsigned int		uint32_t;
typedef __int64				int64_t;
typedef unsigned __int64	uint64_t;
#else
#include <stdint.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef uint16_t			EFK_CHAR;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct Vector2D;
struct Vector3D;
struct Matrix43;
struct Matrix44;
struct RectF;

class Manager;
class Effect;

class ParticleRenderer;
class SpriteRenderer;
class RibbonRenderer;
class RingRenderer;
class EffectLoader;
class TextureLoader;

class SoundPlayer;
class SoundLoader;

typedef	int	Handle;

/**
	@brief	�������m�ۊ֐�
*/
typedef void* ( EFK_STDCALL *MallocFunc ) ( unsigned int size );

/**
	@brief	�������j���֐�
*/
typedef	void ( EFK_STDCALL *FreeFunc ) ( void* p, unsigned int size );

/**
	@brief	�����_���֐�
*/
typedef	int ( EFK_STDCALL *RandFunc ) (void);

/**
	@brief	�G�t�F�N�g�̃C���X�^���X�j�����̃R�[���o�b�N�C�x���g
	@param	manager	[in]	�������Ă���}�l�[�W���[
	@param	handle	[in]	�G�t�F�N�g�̃C���X�^���X�̃n���h��
	@param	isRemovingManager	[in]	�}�l�[�W���[��j�������Ƃ��ɃG�t�F�N�g�̃C���X�^���X��j�����Ă��邩
*/
typedef	void ( EFK_STDCALL *EffectInstanceRemovingCallback ) ( Manager* manager, Handle handle, bool isRemovingManager );

#define ES_SAFE_ADDREF(val)						if ( (val) != NULL ) { (val)->AddRef(); }
#define ES_SAFE_RELEASE(val)					if ( (val) != NULL ) { (val)->Release(); (val) = NULL; }
#define ES_SAFE_DELETE(val)						if ( (val) != NULL ) { delete (val); (val) = NULL; }
#define ES_SAFE_DELETE_ARRAY(val)				if ( (val) != NULL ) { delete [] (val); (val) = NULL; }

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�A���t�@�u�����h
*/
enum eAlphaBlend
{
	/// <summary>
	/// �s����
	/// </summary>
	ALPHA_BLEND_OPACITY = 0,
	/// <summary>
	/// ����
	/// </summary>
	ALPHA_BLEND_BLEND = 1,
	/// <summary>
	/// ���Z
	/// </summary>
	ALPHA_BLEND_ADD = 2,
	/// <summary>
	/// ���Z
	/// </summary>
	ALPHA_BLEND_SUB = 3,
	/// <summary>
	/// ��Z
	/// </summary>
	ALPHA_BLEND_MUL = 4,

	ALPHA_BLEND_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum eBillboardType
{
	BillboardType_Billboard = 0,
	BillboardType_YAxisFixed = 1,
	BillboardType_Fixed = 2,
	BillboardType_RotatedBillboard = 3,

	BillboardType_DWORD = 0x7fffffff,
};

enum eCoordinateSystem
{
	COORDINATE_SYSTEM_LH,
	COORDINATE_SYSTEM_RH,
	COORDINATE_SYSTEM_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�ő�l�擾
*/
template <typename T,typename U>
T Max( T t, U u )
{
	if( t > (T)u )
	{
		return t;
	}
	return u;
}

/**
	@brief	�ŏ��l�擾
*/
template <typename T,typename U>
T Min( T t, U u )
{
	if( t < (T)u )
	{
		return t;
	}
	return u;
}

/**
	@brief	�͈͓��l�擾
*/
template <typename T,typename U,typename V>
T Clamp( T t, U max_, V min_ )
{
	if( t > (T)max_ )
	{
		t = (T)max_;
	}

	if( t < (T)min_ )
	{
		t = (T)min_;
	}

	return t;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�����R�[�h��ϊ�����B(UTF16 -> UTF8)
	@param	dst	[out]	�o�͔z��̐擪�|�C���^
	@param	dst_size	[in]	�o�͔z��̒���
	@param	src			[in]	���͔z��̐擪�|�C���^
	@return	������
*/
inline int32_t ConvertUtf16ToUtf8( int8_t* dst, int32_t dst_size, const int16_t* src )
{
	int32_t cnt = 0;
	const int16_t* wp = src;
	int8_t* cp = dst;

	if (dst_size == 0) return 0;
	
	dst_size -= 3;

	for (cnt = 0; cnt < dst_size; )
	{
		int16_t wc = *wp++;
		if (wc == 0)
		{
			break;
		}
		if ((wc & ~0x7f) == 0)
		{
			*cp++ = wc & 0x7f;
			cnt += 1;
		} else if ((wc & ~0x7ff) == 0)
		{
			*cp++ = ((wc >>  6) & 0x1f) | 0xc0;
			*cp++ = ((wc)       & 0x3f) | 0x80;
			cnt += 2;
		} else {
			*cp++ = ((wc >> 12) &  0xf) | 0xe0;
			*cp++ = ((wc >>  6) & 0x3f) | 0x80;
			*cp++ = ((wc)       & 0x3f) | 0x80;
			cnt += 3;
		}
	}
	*cp = '\0';
	return cnt;
}

/**
	@brief	�����R�[�h��ϊ�����B(UTF8 -> UTF16)
	@param	dst	[out]	�o�͔z��̐擪�|�C���^
	@param	dst_size	[in]	�o�͔z��̒���
	@param	src			[in]	���͔z��̐擪�|�C���^
	@return	������
*/
inline int32_t ConvertUtf8ToUtf16( int16_t* dst, int32_t dst_size, const int8_t* src )
{
	int32_t i, code;
	int8_t c0, c1, c2;

	if (dst_size == 0) return 0;
	
	dst_size -= 1;

	for (i = 0; i < dst_size; i++)
	{
		int16_t wc;
		
		c0 = *src++;
		if (c0 == '\0')
		{
			break;
		}
		// UTF8����UTF16�ɕϊ�
		code = (uint8_t)c0 >> 4;
		if (code <= 7)
		{
			// 8bit����
			wc = c0;
		} 
		else if (code >= 12 && code <= 13)
		{
			// 16bit����
			c1 = *src++;
			wc = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
		} 
		else if (code == 14)
		{
			// 24bit����
			c1 = *src++;
			c2 = *src++;
			wc = ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
		} 
		else
		{
			continue;
		}
		dst[i] = wc;
	}
	dst[i] = 0;
	return i;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_BASE_PRE_H__
#ifndef	__EFFEKSEER_VECTOR2D_H__
#define	__EFFEKSEER_VECTOR2D_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	3�����x�N�g��
*/
struct Vector2D
{
public:
	/**
		@brief	X
	*/
	float	X;

	/**
		@brief	Y
	*/
	float	Y;

	/**
		@brief	�R���X�g���N�^
	*/
	Vector2D();

	/**
		@brief	�R���X�g���N�^
	*/
	Vector2D( float x, float y );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__

#ifndef	__EFFEKSEER_VECTOR3D_H__
#define	__EFFEKSEER_VECTOR3D_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	3�����x�N�g��
*/
struct Vector3D
{
public:
	/**
		@brief	X
	*/
	float	X;

	/**
		@brief	Y
	*/
	float	Y;

	/**
		@brief	Z
	*/
	float	Z;

	/**
		@brief	�R���X�g���N�^
	*/
	Vector3D();

	/**
		@brief	�R���X�g���N�^
	*/
	Vector3D( float x, float y, float z );

	/**
		@brief	���Z
	*/
	static void Add( Vector3D* pOut, const Vector3D* pIn1, const Vector3D* pIn2 );

	/**
		@brief	���Z
	*/
	static Vector3D& Sub( Vector3D& o, const Vector3D& in1, const Vector3D& in2 );

	/**
		@brief	����
	*/
	static float Length( const Vector3D& in );

	/**
		@brief	����
	*/
	static float Dot( const Vector3D& in1, const Vector3D& in2 );

	/**
		@brief	�P�ʃx�N�g��
	*/
	static void Normal( Vector3D& o, const Vector3D& in );

	/**
		@brief	�O��
		@note
		�E��n�̏ꍇ�A�E��̐e�w��in1�A�l�����w��in2�Ƃ����Ƃ��A���w�̕�����Ԃ��B<BR>
		����n�̏ꍇ�A����̐e�w��in1�A�l�����w��in2�Ƃ����Ƃ��A���w�̕�����Ԃ��B<BR>
	*/
	static Vector3D& Cross( Vector3D& o, const Vector3D& in1, const Vector3D& in2 );

	static Vector3D& Transform( Vector3D& o, const Vector3D& in, const Matrix43& mat );

	static Vector3D& Transform( Vector3D& o, const Vector3D& in, const Matrix44& mat );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__

#ifndef	__EFFEKSEER_COLOR_H__
#define	__EFFEKSEER_COLOR_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�F
*/
#pragma pack(push,1)
struct Color
{
	/**
		@brief	��
	*/
	uint8_t		R;

	/**
		@brief	��
	*/
	uint8_t		G;

	/**
		@brief	��
	*/
	uint8_t		B;
	
	/**
		@brief	�����x
	*/
	uint8_t		A;

	/**
		@brief	�R���X�g���N�^
	*/
	Color();

	/**
		@brief	�R���X�g���N�^
	*/
	Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );

	/**
		@brief	��Z
	*/
	static void Mul( Color& o, const Color& in1, const Color& in2 );
};
#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_COLOR_H__

#ifndef	__EFFEKSEER_RECTF_H__
#define	__EFFEKSEER_RECTF_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�l�p�`
*/
struct RectF
{
private:

public:
	float	X;

	float	Y;

	float	Width;

	float	Height;

	RectF();

	RectF( float x, float y, float width, float height );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RECTF_H__

#ifndef	__EFFEKSEER_MATRIX43_H__
#define	__EFFEKSEER_MATRIX43_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

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

#ifndef	__EFFEKSEER_MATRIX44_H__
#define	__EFFEKSEER_MATRIX44_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

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
	�E��n<BR>
	����n<BR>
	V[x,y,z,1] * M �̌`<BR>
	[0,0][0,1][0,2][0,3]
	[1,0][1,1][1,2][1,3]
	[2,0][2,1][2,2][2,3]
	[3,0][3,1][3,2][3,3]
*/
#pragma pack(push,1)
struct Matrix44
{
private:

public:

	/**
		@brief	�R���X�g���N�^
	*/
	Matrix44();

	/**
		@brief	�s��̒l
	*/
	float	Values[4][4];

	/**
		@brief	�P�ʍs��
	*/
	Matrix44& Indentity();

	/**
		@brief	�J�����s��(�E��n)
	*/
	Matrix44& LookAtRH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	�J�����s��(����n)
	*/
	Matrix44& LookAtLH( const Vector3D& eye, const Vector3D& at, const Vector3D& up );

	/**
		@brief	�ˉe�s��(�E��n)
	*/
	Matrix44& PerspectiveFovRH( float ovY, float aspect, float zn, float zf );

	/**
		@brief	OpenGL�p�ˉe�s��(�E��n)
	*/
	Matrix44& PerspectiveFovRH_OpenGL( float ovY, float aspect, float zn, float zf );

	/**
		@brief	�ˉe�s��(����n)
	*/
	Matrix44& PerspectiveFovLH( float ovY, float aspect, float zn, float zf );

	/**
		@brief	���ˉe�s��(�E��n)
	*/
	Matrix44& OrthographicRH( float width, float height, float zn, float zf );

	/**
		@brief	���ˉe�s��(����n)
	*/
	Matrix44& OrthographicLH( float width, float height, float zn, float zf );

	/**
		@brief	��Z
	*/
	static Matrix44& Mul( Matrix44& o, const Matrix44& in1, const Matrix44& in2 );
};

#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MATRIX44_H__

#ifndef	__EFFEKSEER_EFFECT_H__
#define	__EFFEKSEER_EFFECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

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

#ifndef	__EFFEKSEER_MANAGER_H__
#define	__EFFEKSEER_MANAGER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief �G�t�F�N�g�Ǘ��N���X
*/
class Manager
{
protected:
	Manager() {}
    ~Manager() {}

public:
	/**
		@brief �}�l�[�W���[�𐶐�����B
		@param	instance_max	[in]	�ő�C���X�^���X��
		@return	�}�l�[�W���[
	*/
	static Manager* Create( int instance_max );

	/**
		@brief �}�l�[�W���[��j������B
		@note
		���̃}�l�[�W���[���琶�����ꂽ�G�t�F�N�g�͑S�ċ����I�ɔj�������B
	*/
	virtual void Destroy() = 0;

	/**
		@brief	�������m�ۊ֐����擾����B
	*/
	virtual MallocFunc GetMallocFunc() const = 0;

	/**
		@brief	�������m�ۊ֐���ݒ肷��B
	*/
	virtual void SetMallocFunc( MallocFunc func ) = 0;

	/**
		@brief	�������j���֐����擾����B
	*/
	virtual FreeFunc GetFreeFunc() const = 0;

	/**
		@brief	�������j���֐���ݒ肷��B
	*/
	virtual void SetFreeFunc( FreeFunc func ) = 0;

	/**
		@brief	�����_���֐����擾����B
	*/
	virtual RandFunc GetRandFunc() const = 0;

	/**
		@brief	�����_���֐���ݒ肷��B
	*/
	virtual void SetRandFunc( RandFunc func ) = 0;

	/**
		@brief	�����_���ő�l���擾����B
	*/
	virtual int GetRandMax() const = 0;

	/**
		@brief	�����_���֐���ݒ肷��B
	*/
	virtual void SetRandMax( int max_ ) = 0;

	/**
		@brief	���W�n���擾����B
		@return	���W�n
	*/
	virtual eCoordinateSystem GetCoordinateSystem() const = 0;

	/**
		@brief	���W�n��ݒ肷��B
		@param	coordinateSystem	[in]	���W�n
		@note
		���W�n��ݒ肷��B
		�G�t�F�N�g�t�@�C����ǂݍ��ޑO�ɐݒ肷��K�v������B
	*/
	virtual void SetCoordinateSystem( eCoordinateSystem coordinateSystem ) = 0;

	/**
		@brief	�X�v���C�g�`��@�\���擾����B
	*/
	virtual SpriteRenderer* GetSpriteRenderer() = 0;

	/**
		@brief	�X�v���C�g�`��@�\��ݒ肷��B
	*/
	virtual void SetSpriteRenderer( SpriteRenderer* renderer ) = 0;

	/**
		@brief	�X�g���C�v�`��@�\���擾����B
	*/
	virtual RibbonRenderer* GetRibbonRenderer() = 0;

	/**
		@brief	�X�g���C�v�`��@�\��ݒ肷��B
	*/
	virtual void SetRibbonRenderer( RibbonRenderer* renderer ) = 0;

	/**
		@brief	�����O�`��@�\���擾����B
	*/
	virtual RingRenderer* GetRingRenderer() = 0;

	/**
		@brief	�����O�`��@�\��ݒ肷��B
	*/
	virtual void SetRingRenderer( RingRenderer* renderer ) = 0;

	/**
		@brief	�G�t�F�N�g�Ǎ��N���X���擾����B
	*/
	virtual EffectLoader* GetEffectLoader() = 0;

	/**
		@brief	�G�t�F�N�g�Ǎ��N���X��ݒ肷��B
	*/
	virtual void SetEffectLoader( EffectLoader* effectLoader ) = 0;

	/**
		@brief	�e�N�X�`���Ǎ��N���X���擾����B
	*/
	virtual TextureLoader* GetTextureLoader() = 0;

	/**
		@brief	�e�N�X�`���Ǎ��N���X��ݒ肷��B
	*/
	virtual void SetTextureLoader( TextureLoader* textureLoader ) = 0;
	
	/**
		@brief	�T�E���h�Đ��@�\���擾����B
	*/
	virtual SoundPlayer* GetSoundPlayer() = 0;

	/**
		@brief	�T�E���h�Đ��@�\��ݒ肷��B
	*/
	virtual void SetSoundPlayer( SoundPlayer* soundPlayer ) = 0;
	
	/**
		@brief	�T�E���h�Ǎ��N���X���擾����
	*/
	virtual SoundLoader* GetSoundLoader() = 0;
	
	/**
		@brief	�T�E���h�Ǎ��N���X��ݒ肷��B
	*/
	virtual void SetSoundLoader( SoundLoader* soundLoader ) = 0;

	/**
		@brief	�G�t�F�N�g���~����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
	*/
	virtual void StopEffect( Handle handle ) = 0;

	/**
		@brief	�S�ẴG�t�F�N�g���~����B
	*/
	virtual void StopAllEffects() = 0;

	/**
		@brief	�G�t�F�N�g�̃��[�g�������~����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
	*/
	virtual void StopRoot( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃��[�g�������~����B
		@param	effect	[in]	�G�t�F�N�g
	*/
	virtual void StopRoot( Effect* effect ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�����݂��Ă��邩�擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	���݂��Ă邩?
	*/
	virtual bool Exists( Handle handle ) const = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɐݒ肳��Ă���s����擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	�s��
	*/
	virtual Matrix43 GetMatrix( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɕϊ��s���ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	mat		[in]	�ϊ��s��
	*/
	virtual void SetMatrix( Handle handle, const Matrix43& mat ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu���擾����B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@return	�ʒu
	*/
	virtual Vector3D GetLocation( Handle handle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu���w�肷��B
		@param	x	[in]	X���W
		@param	y	[in]	Y���W
		@param	z	[in]	Z���W
	*/
	virtual void SetLocation( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu���w�肷��B
		@param	location	[in]	�ʒu
	*/
	virtual void SetLocation( Handle handle, const Vector3D& location ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̈ʒu�ɉ��Z����B
		@param	location	[in]	���Z����l
	*/
	virtual void AddLocation( Handle handle, const Vector3D& location ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̉�]�p�x���w�肷��B(���W�A��)
	*/
	virtual void SetRotation( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̔C�ӎ�����̔����v����̉�]�p�x���w�肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	axis	[in]	��
		@param	angle	[in]	�p�x(���W�A��)
	*/
	virtual void SetRotation( Handle handle, const Vector3D& axis, float angle ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�̊g�嗦���w�肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	x		[in]	X�����g�嗦
		@param	y		[in]	Y�����g�嗦
		@param	z		[in]	Z�����g�嗦
	*/
	virtual void SetScale( Handle handle, float x, float y, float z ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X�ɔp�����̃R�[���o�b�N��ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	callback	[in]	�R�[���o�b�N
	*/
	virtual void SetRemovingCallback( Handle handle, EffectInstanceRemovingCallback callback ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X��Draw���ɕ`�悷�邩�ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	shown	[in]	�`�悷�邩?
	*/
	virtual void SetShown( Handle handle, bool shown ) = 0;

	/**
		@brief	�G�t�F�N�g�̃C���X�^���X��Update���ɍX�V���邩�ݒ肷��B
		@param	handle	[in]	�C���X�^���X�̃n���h��
		@param	paused	[in]	�X�V���邩?
	*/
	virtual void SetPaused( Handle handle, bool paused ) = 0;

	/**
		@brief	�X�V�������s���B
	*/
	virtual void Update() = 0;

	/**
		@brief	�`�揈�����s���B
	*/
	virtual void Draw() = 0;

	/**
		@brief	�Đ�����B
		@param	effect	[in]	�G�t�F�N�g
		@param	x	[in]	X���W
		@param	y	[in]	Y���W
		@param	z	[in]	Z���W
		@return	�G�t�F�N�g�̃C���X�^���X�̃n���h��
	*/
	virtual Handle Play( Effect* effect, float x, float y, float z ) = 0;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_MANAGER_H__

#ifndef	__EFFEKSEER_SPRITE_RENDERER_H__
#define	__EFFEKSEER_SPRITE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SpriteRenderer
{
public:

	struct NodeParamater
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eBillboardType		Billboard;
	};

	struct InstanceParamater
	{
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// �����A�E���A����A�E��
		Color		Colors[4];

		Vector2D	Positions[4];

	};

public:
	SpriteRenderer() {}

	virtual ~SpriteRenderer() {}

	virtual void LoadRenderer(  const NodeParamater& paramater, void*& userData ) {}

	virtual void RemoveRenderer( const NodeParamater& paramater, void*& userData ) {}

	virtual void BeginRendering( const NodeParamater& paramater, void* userData ) {}

	virtual void Rendering( const NodeParamater& paramater, const InstanceParamater& instanceParamater, void* userData ) {}

	virtual void EndRendering( const NodeParamater& paramater, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SPRITE_RENDERER_H__

#ifndef	__EFFEKSEER_RIBBON_RENDERER_H__
#define	__EFFEKSEER_RIBBON_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RibbonRenderer
{
public:

	struct NodeParamater
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		bool				ViewpointDependent;
	};

	struct InstanceParamater
	{
		int32_t			InstanceCount;
		int32_t			InstanceIndex;
		Matrix43		SRTMatrix43;
		Color		AllColor;

		// ���A�E
		Color		Colors[2];

		float	Positions[2];
	};

public:
	RibbonRenderer() {}

	virtual ~RibbonRenderer() {}

	virtual void LoadRenderer(  const NodeParamater& paramater, void*& userData ) {}

	virtual void RemoveRenderer( const NodeParamater& paramater, void*& userData ) {}

	virtual void BeginRendering( const NodeParamater& paramater, void* userData ) {}

	virtual void Rendering( const NodeParamater& paramater, const InstanceParamater& instanceParamater, void* userData ) {}

	virtual void EndRendering( const NodeParamater& paramater, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RIBBON_RENDERER_H__

#ifndef	__EFFEKSEER_RING_RENDERER_H__
#define	__EFFEKSEER_RING_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RingRenderer
{
public:

	struct NodeParamater
	{
		Effect*				EffectPointer;
		int32_t				ColorTextureIndex;
		eAlphaBlend			AlphaBlend;
		eBillboardType		Billboard;
		int32_t				VertexCount;
	};

	struct InstanceParamater
	{
		Matrix43	SRTMatrix43;
		float		ViewingAngle;
		Vector2D	OuterLocation;
		Vector2D	InnerLocation;
		float		CenterRatio;
		Color		OuterColor;
		Color		CenterColor;
		Color		InnerColor;
	};

public:
	RingRenderer() {}

	virtual ~RingRenderer() {}

	virtual void LoadRenderer(  const NodeParamater& paramater, void*& userData ) {}

	virtual void RemoveRenderer( const NodeParamater& paramater, void*& userData ) {}

	virtual void BeginRendering( const NodeParamater& paramater, void* userData ) {}

	virtual void Rendering( const NodeParamater& paramater, const InstanceParamater& instanceParamater, void* userData ) {}

	virtual void EndRendering( const NodeParamater& paramater, void* userData ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_RING_RENDERER_H__

#ifndef	__EFFEKSEER_EFFECTLOADER_H__
#define	__EFFEKSEER_EFFECTLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�G�t�F�N�g�t�@�C���ǂݍ��ݔj���֐��w��N���X
*/
class EffectLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	EffectLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~EffectLoader() {}

	/**
		@brief	�G�t�F�N�g�t�@�C����ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@param	data	[out]	�f�[�^�z��̐擪�̃|�C���^���o�͂����
		@param	size	[out]	�f�[�^�z��̒������o�͂����
		@return	����
		@note
		�G�t�F�N�g�t�@�C����ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual bool Load( const EFK_CHAR* path, void*& data, int32_t& size ) { return false; }

	/**
		@brief	�G�t�F�N�g�t�@�C����j������B
		@param	data	[in]	�f�[�^�z��̐擪�̃|�C���^
		@param	size	[int]	�f�[�^�z��̒���
		@note
		�G�t�F�N�g�t�@�C����j������B
		::Effekseer::Effect::Create���s�I�����Ɏg�p�����B
	*/
	virtual void Unload( void* data, int32_t size ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECTLOADER_H__

#ifndef	__EFFEKSEER_TEXTURELOADER_H__
#define	__EFFEKSEER_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�e�N�X�`���ǂݍ��ݔj���֐��w��N���X
*/
class TextureLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	TextureLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~TextureLoader() {}

	/**
		@brief	�e�N�X�`����ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@return	�e�N�X�`���̃|�C���^
		@note
		�e�N�X�`����ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	�e�N�X�`����j������B
		@param	data	[in]	�e�N�X�`��
		@note
		�e�N�X�`����j������B
		::Effekseer::Effect�̃C���X�^���X���j�����ꂽ���Ɏg�p�����B
	*/
	virtual void Unload( void* data ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_TEXTURELOADER_H__

#ifndef	__EFFEKSEER_SOUND_PLAYER_H__
#define	__EFFEKSEER_SOUND_PLAYER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

typedef void* SoundHandle;
typedef void* SoundTag;

class SoundPlayer
{
public:
	struct InstanceParameter
	{
		void*		Data;
		float		Volume;
		float		Pan;
		float		Pitch;
		bool		Mode3D;
		Vector3D	Position;
		float		Distance;
	};

public:
	SoundPlayer() {}

	virtual ~SoundPlayer() {}

	virtual SoundHandle Play( SoundTag tag, const InstanceParameter& parameter ) = 0;
	
	virtual void Stop( SoundHandle handle, SoundTag tag ) = 0;

	virtual void Pause( SoundHandle handle, SoundTag tag, bool pause ) = 0;

	virtual void StopTag( SoundTag tag ) = 0;

	virtual void PauseTag( SoundTag tag, bool pause ) = 0;

	virtual void StopAll() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUND_PLAYER_H__

#ifndef	__EFFEKSEER_SOUNDLOADER_H__
#define	__EFFEKSEER_SOUNDLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�T�E���h�ǂݍ��ݔj���֐��w��N���X
*/
class SoundLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	SoundLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~SoundLoader() {}

	/**
		@brief	�T�E���h��ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@return	�T�E���h�̃|�C���^
		@note
		�T�E���h��ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	�T�E���h��j������B
		@param	data	[in]	�T�E���h
		@note
		�T�E���h��j������B
		::Effekseer::Effect�̃C���X�^���X���j�����ꂽ���Ɏg�p�����B
	*/
	virtual void Unload( void* source ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUNDLOADER_H__
