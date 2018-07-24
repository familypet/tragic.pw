#pragma once
#include <Windows.h>
#include <math.h>

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)
#define M_PI	3.14159265358979323846f

inline float isqrt(float _Xx);

class VectorNew;

VectorNew CrossProduct(VectorNew& a, VectorNew& b);

class VectorNew
{
public:
	float x = 0, y = 0, z = 0;

	VectorNew(void);

	VectorNew(float X, float Y, float Z);

	VectorNew(const float* clr)
	{
		x = clr[0];
		y = clr[1];
		z = clr[2];
	}

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);

	float operator[](int i) const;

	float& operator[](int i);

	inline void Zero();

	bool operator==(const VectorNew& v) const;

	bool operator!=(const VectorNew& v) const;

	__forceinline VectorNew& operator+=(const VectorNew& v);

	__forceinline VectorNew& operator-=(const VectorNew& v);

	__forceinline VectorNew& operator*=(const VectorNew& v);

	__forceinline VectorNew& operator*=(float s);

	__forceinline VectorNew& operator/=(const VectorNew& v);

	__forceinline VectorNew& operator/=(float s);

	__forceinline VectorNew& operator+=(float fl);

	__forceinline VectorNew& operator-=(float fl);

	inline float Length() const;

	inline void SinCos2(const float &r, float &s, float &c);

	inline float deg2rad2(float deg);

	inline void Rotate2D(const float &f);

	__forceinline float VectorNew::LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (x * x + y * y + z * z);
	}

	

	bool VectorNew::IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}

	

	float NormalizeInPlace();

	VectorNew VectorNew::Normalized() const
	{
		VectorNew res = *this;
		float l = res.Length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res.x = res.y = 0.0f;
		}
		return res;
	}

	VectorNew VectorNew::Normalize()
	{
		while (this->y > 180)
		{
			this->y -= 360;
		}
		while (this->y < -180)
		{
			this->y += 360;
		}

		while (this->x > 89)
		{
			this->x -= 180;
		}

		while (this->x < -89)
		{
			this->x += 180;
		}
		return *this;
	}

	__forceinline float DistToSqr(const VectorNew& vOther) const;

	__forceinline float DistTo(const VectorNew& vOther) const;

	float Dot(const VectorNew& vOther) const;

	float Length2D(void) const;

	float Length2DSqr(void) const;

	VectorNew& operator=(const VectorNew& vOther);

	VectorNew operator-(void) const;

	VectorNew operator+(const VectorNew& v) const;

	VectorNew operator-(const VectorNew& v) const;

	VectorNew operator*(const VectorNew& v) const;

	VectorNew operator/(const VectorNew& v) const;

	VectorNew operator*(float fl) const;

	VectorNew operator/(float fl) const;

	inline VectorNew VectorNew::Cross(VectorNew& vOther)
	{
		VectorNew res;
		res = CrossProduct(*this, vOther);
		return res;
	}


};

inline VectorNew operator*(float lhs, const VectorNew& rhs)
{
	return rhs * lhs;
}
inline VectorNew operator/(float lhs, const VectorNew& rhs)
{
	return rhs / lhs;
}

class VectorNew3
{
public:
	VectorNew3(void)
	{
		x = y = z = 0.0f;
	}

	VectorNew3(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}

	VectorNew3(float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	VectorNew3(const float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	VectorNew3(const VectorNew3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	VectorNew3& operator+=(const VectorNew3& v)
	{
		x += v.x; y += v.y; z += v.z; return *this;
	}

	VectorNew3& operator-=(const VectorNew3& v)
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	VectorNew3& operator*=(const VectorNew3& v)
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	VectorNew3& operator/=(const VectorNew3& v)
	{
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	bool operator==(const VectorNew3& v)
	{
		if (x == v.x &&
			y == v.y &&
			z == v.z)
			return true;
		else
			return false;
	}

	bool operator!=(const VectorNew3& v)
	{
		if (x != v.x ||
			y != v.y ||
			z != v.z)
			return true;
		else
			return false;
	}

	VectorNew3& operator+=(float v)
	{
		x += v; y += v; z += v; return *this;
	}

	VectorNew3& operator-=(float v)
	{
		x -= v; y -= v; z -= v; return *this;
	}

	VectorNew3& operator*=(float v)
	{
		x *= v; y *= v; z *= v; return *this;
	}

	VectorNew3& operator/=(float v)
	{
		x /= v; y /= v; z /= v; return *this;
	}

	VectorNew3 operator+(const VectorNew3& v) const
	{
		return VectorNew3(x + v.x, y + v.y, z + v.z);
	}

	VectorNew3 operator-(const VectorNew3& v) const
	{
		return VectorNew3(x - v.x, y - v.y, z - v.z);
	}

	VectorNew3 operator*(const VectorNew3& v) const
	{
		return VectorNew3(x * v.x, y * v.y, z * v.z);
	}

	VectorNew3 operator/(const VectorNew3& v) const
	{
		return VectorNew3(x / v.x, y / v.y, z / v.z);
	}

	VectorNew3 operator+(float v) const
	{
		return VectorNew3(x + v, y + v, z + v);
	}

	VectorNew3 operator-(float v) const
	{
		return VectorNew3(x - v, y - v, z - v);
	}

	VectorNew3 operator*(float v) const
	{
		return VectorNew3(x * v, y * v, z * v);
	}

	VectorNew3 operator/(float v) const
	{
		return VectorNew3(x / v, y / v, z / v);
	}

	void Set(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X; y = Y; z = Z;
	}

	float Length(void) const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}

	float Length2d(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float Length2dSqr(void) const
	{
		return (x * x + y * y);
	}

	float DistTo(const VectorNew3& v) const
	{
		return (*this - v).Length();
	}

	float DistToSqr(const VectorNew3& v) const
	{
		return (*this - v).LengthSqr();
	}

	float Dot(const VectorNew3& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	VectorNew3 Cross(const VectorNew3& v) const
	{
		return VectorNew3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	bool IsZero(void) const
	{
		return (x > -0.01f && x < 0.01f &&
			y > -0.01f && y < 0.01f &&
			z > -0.01f && z < 0.01f);
	}

	void Init(float _x = 0, float _y = 0, float _z = 0)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	VectorNew3 Normalize()
	{
		if (this->x != this->x)
			this->x = 0;
		if (this->y != this->y)
			this->y = 0;
		if (this->z != this->z)
			this->z = 0;

		if (this->x > 89.f)
			this->x = 89.f;
		if (this->x < -89.f)
			this->x = -89.f;

		while (this->y > 180)
			this->y -= 360;
		while (this->y <= -180)
			this->y += 360;

		if (this->y > 180.f)
			this->y = 180.f;
		if (this->y < -180.f)
			this->y = -180.f;

		this->z = 0;

		return *this;
	}


public:

	float x, y, z;
};

class VectorNewAligned : public VectorNew
{
public:
	VectorNewAligned();

	VectorNewAligned(const VectorNew& in);

	float w;
};

inline void VectorNewClear(VectorNew& a);

__forceinline void VectorNewCopy(const VectorNew& src, VectorNew& dst);

inline float VectorNewLength(const VectorNew& v)
{
	return (float)isqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

//===============================================
inline void VectorNew::Init(float ix, float iy, float iz)
{
	x = ix;
	y = iy;
	z = iz;
	CHECK_VALID(*this);
}

//===============================================
inline VectorNew::VectorNew(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
	CHECK_VALID(*this);
}

//===============================================
inline VectorNew::VectorNew(void)
{
}

//===============================================
inline void VectorNew::Zero()
{
	x = y = z = 0.0f;
}

//===============================================
inline void VectorNewClear(VectorNew& a)
{
	a.x = a.y = a.z = 0.0f;
}

//===============================================
inline VectorNew& VectorNew::operator=(const VectorNew& vOther)
{
	CHECK_VALID(vOther);
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	return *this;
}

//===============================================
inline float& VectorNew::operator[](int i)
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}

//===============================================
inline float VectorNew::operator[](int i) const
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}

//===============================================
inline bool VectorNew::operator==(const VectorNew& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

//===============================================
inline bool VectorNew::operator!=(const VectorNew& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}

//===============================================
__forceinline void VectorNewCopy(const VectorNew& src, VectorNew& dst)
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

//===============================================
__forceinline VectorNew& VectorNew::operator+=(const VectorNew& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

//===============================================
__forceinline VectorNew& VectorNew::operator-=(const VectorNew& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

//===============================================
__forceinline VectorNew& VectorNew::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline VectorNew& VectorNew::operator*=(const VectorNew& v)
{
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline VectorNew& VectorNew::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline VectorNew& VectorNew::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline VectorNew& VectorNew::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline VectorNew& VectorNew::operator/=(const VectorNew& v)
{
	CHECK_VALID(v);
	Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
inline float VectorNew::Length(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqsr = x * x + y * y + z * z;

	__asm
	{
		sqrtss xmm0, sqsr
		movss root, xmm0
	}

	return root;
}

void VectorNew::SinCos2(const float &r, float &s, float &c)
{
	s = sin(r);
	c = cos(r);
}
float VectorNew::deg2rad2(float deg)
{
	return (deg * M_PI / 180.0f);
}

inline void VectorNew::Rotate2D(const float &f)
{
	float _x, _y;

	float s, c;

	SinCos2(deg2rad2(f), s, c);

	_x = x;
	_y = y;

	x = (_x * c) - (_y * s);
	y = (_x * s) + (_y * c);
}

//===============================================
inline float VectorNew::Length2D(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqst = x * x + y * y;

	__asm
	{
		sqrtss xmm0, sqst
		movss root, xmm0
	}

	return root;
}

//===============================================
inline float VectorNew::Length2DSqr(void) const
{
	return (x * x + y * y);
}

//===============================================
inline VectorNew CrossProduct(VectorNew& a, VectorNew& b)
{
	return VectorNew(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

//===============================================
float VectorNew::DistToSqr(const VectorNew& vOther) const
{
	VectorNew delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}

float VectorNew::DistTo(const VectorNew& vOther) const
{
	VectorNew delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.Length();
}

//===============================================
inline float VectorNew::NormalizeInPlace()
{
	VectorNew& v = *this;

	float iradius = 1.f / (this->Length() + 1.192092896e-07F); //FLT_EPSILON

	v.x *= iradius;
	v.y *= iradius;
	v.z *= iradius;
	return 0.f;
}

//===============================================
inline VectorNew VectorNew::operator+(const VectorNew& v) const
{
	VectorNew res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

//===============================================
inline VectorNew VectorNew::operator-(const VectorNew& v) const
{
	VectorNew res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

inline VectorNew VectorNew::operator-(void) const
{
	VectorNew res;
	res.x = -x;
	res.y = -y;
	res.z = -z;
	return res;
}

//===============================================
inline VectorNew VectorNew::operator*(float fl) const
{
	VectorNew res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

//===============================================
inline VectorNew VectorNew::operator*(const VectorNew& v) const
{
	VectorNew res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;
}

//===============================================
inline VectorNew VectorNew::operator/(float fl) const
{
	VectorNew res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}

//===============================================
inline VectorNew VectorNew::operator/(const VectorNew& v) const
{
	VectorNew res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}

inline float VectorNew::Dot(const VectorNew& vOther) const
{
	const VectorNew& a = *this;

	return (a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
}

class VectorNew2D2
{
public:
	float x, y;

	VectorNew2D2() { x = 0; y = 0; };
	VectorNew2D2(float X, float Y) { x = X; y = Y; };

	inline VectorNew2D2 operator*(const int n) { return VectorNew2D2(x*n, y*n); }
	inline VectorNew2D2 operator*(const float n) { return VectorNew2D2(x*n, y*n); }
	inline VectorNew2D2 operator+(const VectorNew2D2& v) { return VectorNew2D2(x + v.x, y + v.y); }
};

class VectorNew2D
{
public:
	// Members
	float x, y;

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f);

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	float operator[](int i) const;

	float& operator[](int i);

	// Base address...
	float* Base();

	float const* Base() const;

	// Initialization methods
	void Random(float minVal, float maxVal);

	// equality
	bool operator==(const VectorNew2D& v) const;

	bool operator!=(const VectorNew2D& v) const;

	// arithmetic operations
	VectorNew2D& operator+=(const VectorNew2D& v);

	VectorNew2D& operator-=(const VectorNew2D& v);

	VectorNew2D& operator*=(const VectorNew2D& v);

	VectorNew2D& operator*=(float s);

	VectorNew2D& operator/=(const VectorNew2D& v);

	VectorNew2D& operator/=(float s);

	// negate the VectorNew2D components
	void Negate();

	// Get the VectorNew2D's magnitude.
	float Length() const;

	// Get the VectorNew2D's magnitude squared.
	float LengthSqr(void) const;

	// return true if this VectorNew is (0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}

	// Normalize in place and return the old length.
	float NormalizeInPlace();

	// Compare length.
	bool IsLengthGreaterThan(float val) const;

	bool IsLengthLessThan(float val) const;

	// Get the distance from this VectorNew2D to the other one.
	float DistTo(const VectorNew2D& vOther) const;

	// Get the distance from this VectorNew2D to the other one squared.
	float DistToSqr(const VectorNew2D& vOther) const;

	// Copy
	void CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual VectorNew2D equation (because it's done per-component
	// rather than per-VectorNew2D).
	void MulAdd(const VectorNew2D& a, const VectorNew2D& b, float scalar);

	// Dot product.
	float Dot(const VectorNew2D& vOther) const;

	// assignment
	VectorNew2D& operator=(const VectorNew2D& vOther);

#ifndef VectorNew_NO_SLOW_OPERATIONS
	// copy constructors
	VectorNew2D(const VectorNew2D& vOther);

	// arithmetic operations
	VectorNew2D operator-(void) const;

	VectorNew2D operator+(const VectorNew2D& v) const;

	VectorNew2D operator-(const VectorNew2D& v) const;

	VectorNew2D operator*(const VectorNew2D& v) const;

	VectorNew2D operator/(const VectorNew2D& v) const;

	VectorNew2D operator*(float fl) const;

	VectorNew2D operator/(float fl) const;

	// Cross product between two VectorNews.
	VectorNew2D Cross(const VectorNew2D& vOther) const;

	// Returns a VectorNew2D with the min or max in X, Y, and Z.
	VectorNew2D Min(const VectorNew2D& vOther) const;

	VectorNew2D Max(const VectorNew2D& vOther) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	VectorNew2D(const VectorNew2D& vOther);
#endif
};

//=========================================================
// 4D VectorNew4D
//=========================================================

class VectorNew4D
{
public:
	// Members
	float x, y, z, w;

	// Construction/destruction
	VectorNew4D(void);

	VectorNew4D(float X, float Y, float Z, float W);

	VectorNew4D(const float* pFloat);

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f);

	void Init(const VectorNew& src, float iw = 0.0f);

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	float operator[](int i) const;

	float& operator[](int i);

	// Base address...
	inline float* Base();

	inline float const* Base() const;

	// Cast to VectorNew and VectorNew2D...
	VectorNew& AsVectorNew3D();

	VectorNew const& AsVectorNew3D() const;

	VectorNew2D& AsVectorNew2D();

	VectorNew2D const& AsVectorNew2D() const;

	// Initialization methods
	void Random(float minVal, float maxVal);

	// equality
	bool operator==(const VectorNew4D& v) const;

	bool operator!=(const VectorNew4D& v) const;

	// arithmetic operations
	VectorNew4D& operator+=(const VectorNew4D& v);

	VectorNew4D& operator-=(const VectorNew4D& v);

	VectorNew4D& operator*=(const VectorNew4D& v);

	VectorNew4D& operator*=(float s);

	VectorNew4D& operator/=(const VectorNew4D& v);

	VectorNew4D& operator/=(float s);

	VectorNew4D operator-(void) const;

	VectorNew4D operator*(float fl) const;

	VectorNew4D operator/(float fl) const;

	VectorNew4D operator*(const VectorNew4D& v) const;

	VectorNew4D operator+(const VectorNew4D& v) const;

	VectorNew4D operator-(const VectorNew4D& v) const;

	// negate the VectorNew4D components
	void Negate();

	// Get the VectorNew4D's magnitude.
	float Length() const;

	// Get the VectorNew4D's magnitude squared.
	float LengthSqr(void) const;

	// return true if this VectorNew is (0,0,0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance &&
			w > -tolerance && w < tolerance);
	}

	// Get the distance from this VectorNew4D to the other one.
	float DistTo(const VectorNew4D& vOther) const;

	// Get the distance from this VectorNew4D to the other one squared.
	float DistToSqr(const VectorNew4D& vOther) const;

	// Copy
	void CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual VectorNew4D equation (because it's done per-component
	// rather than per-VectorNew4D).
	void MulAdd(VectorNew4D const& a, VectorNew4D const& b, float scalar);

	// Dot product.
	float Dot(VectorNew4D const& vOther) const;

	// No copy constructors allowed if we're in optimal mode
#ifdef VectorNew_NO_SLOW_OPERATIONS
private:
#else
public:
#endif
	VectorNew4D(VectorNew4D const& vOther);

	// No assignment operators either...
	VectorNew4D& operator=(VectorNew4D const& src);
};

const VectorNew4D vec4_origin(0.0f, 0.0f, 0.0f, 0.0f);
//const VectorNew4D vec4_invalid(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

//-----------------------------------------------------------------------------
// VectorNew4D related operations
//-----------------------------------------------------------------------------

// VectorNew4D clear
void VectorNew4DClear(VectorNew4D& a);

// Copy
void VectorNew4DCopy(VectorNew4D const& src, VectorNew4D& dst);

// VectorNew4D arithmetic
void VectorNew4DAdd(VectorNew4D const& a, VectorNew4D const& b, VectorNew4D& result);

void VectorNew4DSubtract(VectorNew4D const& a, VectorNew4D const& b, VectorNew4D& result);

void VectorNew4DMultiply(VectorNew4D const& a, float b, VectorNew4D& result);

void VectorNew4DMultiply(VectorNew4D const& a, VectorNew4D const& b, VectorNew4D& result);

void VectorNew4DDivide(VectorNew4D const& a, float b, VectorNew4D& result);

void VectorNew4DDivide(VectorNew4D const& a, VectorNew4D const& b, VectorNew4D& result);

void VectorNew4DMA(VectorNew4D const& start, float s, VectorNew4D const& dir, VectorNew4D& result);

// VectorNew4DAligned arithmetic

#define VectorNew4DExpand( v ) (v).x, (v).y, (v).z, (v).w

// Linearly interpolate between two VectorNews
void VectorNew4DLerp(VectorNew4D const& src1, VectorNew4D const& src2, float t, VectorNew4D& dest);

//-----------------------------------------------------------------------------
//
// Inlined VectorNew4D methods
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------

inline VectorNew4D::VectorNew4D(void)
{
}

inline VectorNew4D::VectorNew4D(float X, float Y, float Z, float W)
{
	x = X;
	y = Y;
	z = Z;
	w = W;
	Assert(IsValid());
}

inline VectorNew4D::VectorNew4D(const float* pFloat)
{
	Assert(pFloat);
	x = pFloat[0];
	y = pFloat[1];
	z = pFloat[2];
	w = pFloat[3];
	Assert(IsValid());
}

//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

inline VectorNew4D::VectorNew4D(const VectorNew4D& vOther)
{
	Assert(vOther.IsValid());
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	w = vOther.w;
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void VectorNew4D::Init(float ix, float iy, float iz, float iw)
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
	Assert(IsValid());
}

inline void VectorNew4D::Init(const VectorNew& src, float iw)
{
	x = src.x;
	y = src.y;
	z = src.z;
	w = iw;
	Assert(IsValid());
}

inline void VectorNew4D::Random(float minVal, float maxVal)
{
	x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
	z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
	w = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}

inline void VectorNew4DClear(VectorNew4D& a)
{
	a.x = a.y = a.z = a.w = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline VectorNew4D& VectorNew4D::operator=(const VectorNew4D& vOther)
{
	Assert(vOther.IsValid());
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	w = vOther.w;
	return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

inline float& VectorNew4D::operator[](int i)
{
	Assert((i >= 0) && (i < 4));
	return ((float*)this)[i];
}

inline float VectorNew4D::operator[](int i) const
{
	Assert((i >= 0) && (i < 4));
	return ((float*)this)[i];
}

//-----------------------------------------------------------------------------
// Cast to VectorNew and VectorNew2D...
//-----------------------------------------------------------------------------

inline VectorNew& VectorNew4D::AsVectorNew3D()
{
	return *(VectorNew*)this;
}

inline VectorNew const& VectorNew4D::AsVectorNew3D() const
{
	return *(VectorNew const*)this;
}

inline VectorNew2D& VectorNew4D::AsVectorNew2D()
{
	return *(VectorNew2D*)this;
}

inline VectorNew2D const& VectorNew4D::AsVectorNew2D() const
{
	return *(VectorNew2D const*)this;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

inline float* VectorNew4D::Base()
{
	return (float*)this;
}

inline float const* VectorNew4D::Base() const
{
	return (float const*)this;
}

//----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool VectorNew4D::operator==(VectorNew4D const& src) const
{
	Assert(src.IsValid() && IsValid());
	return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

inline bool VectorNew4D::operator!=(VectorNew4D const& src) const
{
	Assert(src.IsValid() && IsValid());
	return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}

//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

inline void VectorNew4DCopy(VectorNew4D const& src, VectorNew4D& dst)
{
	Assert(src.IsValid());
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
	dst.w = src.w;
}

inline void VectorNew4D::CopyToArray(float* rgfl) const
{
	Assert(IsValid());
	Assert(rgfl);
	rgfl[0] = x;
	rgfl[1] = y;
	rgfl[2] = z;
	rgfl[3] = w;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline void VectorNew4D::Negate()
{
	Assert(IsValid());
	x = -x;
	y = -y;
	z = -z;
	w = -w;
}

inline VectorNew4D& VectorNew4D::operator+=(const VectorNew4D& v)
{
	Assert(IsValid() && v.IsValid());
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

inline VectorNew4D& VectorNew4D::operator-=(const VectorNew4D& v)
{
	Assert(IsValid() && v.IsValid());
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

inline VectorNew4D& VectorNew4D::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	w *= fl;
	Assert(IsValid());
	return *this;
}

inline VectorNew4D& VectorNew4D::operator*=(VectorNew4D const& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
	Assert(IsValid());
	return *this;
}

inline VectorNew4D VectorNew4D::operator-(void) const
{
	return VectorNew4D(-x, -y, -z, -w);
}

inline VectorNew4D VectorNew4D::operator+(const VectorNew4D& v) const
{
	VectorNew4D res;
	VectorNew4DAdd(*this, v, res);
	return res;
}

inline VectorNew4D VectorNew4D::operator-(const VectorNew4D& v) const
{
	VectorNew4D res;
	VectorNew4DSubtract(*this, v, res);
	return res;
}

inline VectorNew4D VectorNew4D::operator*(float fl) const
{
	VectorNew4D res;
	VectorNew4DMultiply(*this, fl, res);
	return res;
}

inline VectorNew4D VectorNew4D::operator*(const VectorNew4D& v) const
{
	VectorNew4D res;
	VectorNew4DMultiply(*this, v, res);
	return res;
}

inline VectorNew4D VectorNew4D::operator/(float fl) const
{
	VectorNew4D res;
	VectorNew4DDivide(*this, fl, res);
	return res;
}

inline VectorNew4D operator*(float fl, const VectorNew4D& v)
{
	return v * fl;
}

inline VectorNew4D& VectorNew4D::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	w *= oofl;
	Assert(IsValid());
	return *this;
}

inline VectorNew4D& VectorNew4D::operator/=(VectorNew4D const& v)
{
	Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f && v.w != 0.0f);
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
	Assert(IsValid());
	return *this;
}

