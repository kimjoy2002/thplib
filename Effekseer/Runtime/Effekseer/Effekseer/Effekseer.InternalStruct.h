
#ifndef	__EFFEKSEER_INTERNAL_STRUCT_H__
#define	__EFFEKSEER_INTERNAL_STRUCT_H__

/**
	@file
	@brief	内部計算用構造体
	@note
	union等に使用するための構造体。<BR>

*/

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Vector2D.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Color.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_float
{
	float	max;
	float	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_float) );
	};

	float getValue( const Manager& manager ) const
	{
		return (max - min) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + min;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_int
{
	int	max;
	int	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_int) );
	};

	int getValue( const Manager& manager ) const
	{
		return (int) ( (max - min) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) ) + min;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct vector2d
{
	float	x;
	float	y;

	void reset()
	{
		assert( sizeof(vector2d) == sizeof(float) * 2 );
		memset( this, 0, sizeof(vector2d) );
	};

	void setValueToArg( Vector2D& v ) const
	{
		v.X = x;
		v.Y = y;
	}

	vector2d operator + ( const vector2d& o ) const
	{
		vector2d ret;
		ret.x = x + o.x;
		ret.y = x + o.y;
		return ret;
	}

	vector2d& operator += ( const vector2d& o )
	{
		x += o.x;
		y += o.y;
		return *this;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_vector2d
{
	vector2d	max;
	vector2d	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_vector2d) );
	};

	vector2d getValue( const Manager& manager ) const
	{
		vector2d r;
		r.x = (max.x - min.x) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + min.x;
		r.y = (max.y - min.y) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + min.y;
		return r;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_float
{
	random_float	start;
	random_float	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( float& o, const float start_, const float end_, float t ) const
	{
		float df = end_ - start_;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o = start_ + d * df;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_vector2d
{
	random_vector2d	start;
	random_vector2d	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( vector2d& o, const vector2d& start_, const vector2d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.x = start_.x + d * d_x;
		o.y = start_.y + d * d_y;
	}

	void setValueToArg( Vector2D& o, const vector2d& start_, const vector2d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.X = start_.x + d * d_x;
		o.Y = start_.y + d * d_y;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct vector3d
{
	float	x;
	float	y;
	float	z;

	void reset()
	{
		assert( sizeof(vector3d) == sizeof(float) * 3 );
		memset( this, 0, sizeof(vector3d) );
	};

	void setValueToArg( Vector3D& v ) const
	{
		v.X = x;
		v.Y = y;
		v.Z = z;
	}

	vector3d operator + ( const vector3d& o ) const
	{
		vector3d ret;
		ret.x = x + o.x;
		ret.y = x + o.y;
		ret.z = x + o.z;
		return ret;
	}

	vector3d& operator += ( const vector3d& o )
	{
		x += o.x;
		y += o.y;
		z += o.z;
		return *this;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_vector3d
{
	vector3d	max;
	vector3d	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_vector3d) );
	};

	vector3d getValue( const Manager& manager ) const
	{
		vector3d r;
		r.x = (max.x - min.x) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + min.x;
		r.y = (max.y - min.y) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + min.y;
		r.z = (max.z - min.z) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + min.z;
		return r;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_vector3d
{
	random_vector3d	start;
	random_vector3d	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( vector3d& o, const vector3d& start_, const vector3d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d_z = end_.z - start_.z;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.x = start_.x + d * d_x;
		o.y = start_.y + d * d_y;
		o.z = start_.z + d * d_z;
	}

	void setValueToArg( Vector3D& o, const vector3d& start_, const vector3d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d_z = end_.z - start_.z;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.X = start_.x + d * d_x;
		o.Y = start_.y + d * d_y;
		o.Z = start_.z + d * d_z;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct color
{
	uint8_t		r;
	uint8_t		g;
	uint8_t		b;
	uint8_t		a;

	void reset()
	{
		assert( sizeof(color) == 4 );
		memset( this, 255, sizeof(color) );
	};

	void setValueToArg( Color& c ) const
	{
		c.R = r;
		c.G = g;
		c.B = b;
		c.A = a;
	}

	static color mul( const color& in1, const color& in2 )
	{
		color o;
		o.r = (uint8_t)((float)in1.r * (float)in2.r / 255.0f);
		o.g = (uint8_t)((float)in1.g * (float)in2.g / 255.0f);
		o.b = (uint8_t)((float)in1.b * (float)in2.b / 255.0f);
		o.a = (uint8_t)((float)in1.a * (float)in2.a / 255.0f);
		return o;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_color
{
	color	max;
	color	min;

	void reset()
	{
		assert( sizeof(random_color) == 8 );
		memset( this, 255, sizeof(random_color) );
	};

	color getValue( const Manager& manager ) const
	{
		color r;
		r.r = (uint8_t)(((float)max.r - (float)min.r) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + (float)min.r);
		r.g = (uint8_t)(((float)max.g - (float)min.g) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + (float)min.g);
		r.b = (uint8_t)(((float)max.b - (float)min.b) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + (float)min.b);
		r.a = (uint8_t)(((float)max.a - (float)min.a) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + (float)min.a);
		return r;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_color
{
	random_color	start;
	random_color	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( color& o, const color& start_, const color& end_, float t ) const
	{
		int d_r = end_.r - start_.r;
		int d_g = end_.g - start_.g;
		int d_b = end_.b - start_.b;
		int d_a = end_.a - start_.a;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.r = (uint8_t)Clamp( start_.r + d * d_r, 255.0f, 0.0f );
		o.g = (uint8_t)Clamp( start_.g + d * d_g, 255.0f, 0.0f );
		o.b = (uint8_t)Clamp( start_.b + d * d_b, 255.0f, 0.0f );
		o.a = (uint8_t)Clamp( start_.a + d * d_a, 255.0f, 0.0f );
	}

	void setValueToArg( Color& o, const color& start_, const color& end_, float t ) const
	{
		int d_r = end_.r - start_.r;
		int d_g = end_.g - start_.g;
		int d_b = end_.b - start_.b;
		int d_a = end_.a - start_.a;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.R = (uint8_t)Clamp( start_.r + d * d_r, 255.0f, 0.0f );
		o.G = (uint8_t)Clamp( start_.g + d * d_g, 255.0f, 0.0f );
		o.B = (uint8_t)Clamp( start_.b + d * d_b, 255.0f, 0.0f );
		o.A = (uint8_t)Clamp( start_.a + d * d_a, 255.0f, 0.0f );
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INTERNAL_STRUCT_H__