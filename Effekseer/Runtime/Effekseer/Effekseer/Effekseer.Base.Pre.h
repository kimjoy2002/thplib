
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