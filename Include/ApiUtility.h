// 通用数学库。恒心东方 赵德贤. 2017-11-30
#pragma once
#include <vector>
#include <map>
#include <string>

#define	safe_delete(obj)	{if(obj)delete obj; obj = 0;}
#ifndef PI
#define	PI	3.1415926535897932f
#endif
inline	float Abs(const float A) { return (A >= (float)0) ? A : -A; }

#pragma pack(push,1)
struct color4 {
	union {
		struct {
			unsigned char R, G, B, A;
		};
		struct {
			unsigned char c[4];
		};
	};
	
	color4() { R = G = B = A = 0; }
	color4(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		R = r;
		G = g;
		B = b;
		A = a;
	}
	const unsigned char operator[](int index) const { return c[index]; }
	unsigned char&	operator[](int index) { return c[index]; }
};
struct color4f {
	union {
		struct {
			double R, G, B, A;
		};
		struct {
			double c[4];
		};
	};
	color4f() { R = G = B = A = 0; }
	color4f(double r, double g, double b, double a) {
		R = r;
		G = g;
		B = b;
		A = a;
	}
	color4f operator*(const color4f& c) const
	{
		return color4f(R*c.R, G*c.G, B*c.B, A*c.A);
	}
	const double operator[](int index) const { return c[index]; }
	double&	operator[](int index){ return c[index]; }
	color4f operator+(const color4f& V) const
	{
		return color4f(R + V.R, G + V.G, B + V.B, A + V.A);
	}
	color4f&	operator+=(const color4f& V)
	{
		R += V.R;
		G += V.G;
		B += V.B;
		A += V.A;
		return *this;
	}
	color4f&	operator-=(const color4f& V)
	{
		R -= V.R;
		G -= V.G;
		B -= V.B;
		A -= V.A;
		return *this;
	}
	color4f&	operator*=(const color4f& V)
	{
		R *= V.R;
		G *= V.G;
		B *= V.B;
		A *= V.A;
		return *this;
	}
	color4f&	operator*=(const float V)
	{
		R *= V;
		G *= V;
		B *= V;
		A *= V;
		return *this;
	}
	color4f operator-(const color4f& V) const
	{
		return color4f(R - V.R, G - V.G, B - V.B, A - V.A);
	}
	color4f operator*(float V) const
	{
		return color4f(R * V, G * V, B * V, A * V);
	}
};

struct float2
{
	float X, Y;
	float2(float _x, float _y) { X = _x; Y = _y; }
	float2() { X = Y = 0; }
	float2 operator+(const float2& V) const
	{
		return float2(X + V.X, Y + V.Y);
	}
	float2&	operator+=(const float2& V)
	{
		X += V.X;
		Y += V.Y;
		return *this;
	}
	float2&	operator-=(const float2& V)
	{
		X -= V.X;
		Y -= V.Y;
		return *this;
	}
	float2&	operator*=(const float2& V)
	{
		X *= V.X;
		Y *= V.Y;
		return *this;
	}
	float2&	operator*=(const float V)
	{
		X *= V;
		Y *= V;
		return *this;
	}
	float2 operator-(const float2& V) const
	{
		return float2(X - V.X, Y - V.Y);
	}
	float2 operator*(float Scale) const
	{
		return float2(X * Scale, Y * Scale);
	}
	bool Equals(const float2& V) const
	{
		float Tolerance = 1.e-8f;
		return Abs(X - V.X) <= Tolerance && Abs(Y - V.Y) <= Tolerance;
	}

	void Normalize()
	{
		float Tolerance = 1.e-8f;
		const float SquareSum = X*X + Y*Y;
		if (SquareSum > Tolerance)
		{
			const float Scale = 1.0f / sqrtf(SquareSum);
			X *= Scale;
			Y *= Scale;
			return;
		}
		X = 0.0f;
		Y = 0.0f;
	}
	static float DistSquared(const float2 &V1, const float2 &V2)
	{
		return (V2.X - V1.X)*(V2.X - V1.X) + (V2.Y - V1.Y)*(V2.Y - V1.Y);
	}
	static float Distance(const float2& V1, const float2& V2)
	{
		return sqrtf(DistSquared(V1, V2));
	}
	float operator|(const float2& V) const
	{
		return X*V.X + Y*V.Y;
	}
	float Dot(const float2& V) const
	{
		return X*V.X + Y*V.Y;
	}
	float operator^(const float2& V) const
	{
		return X*V.Y - Y*V.X;
	}
	static float DotProduct(const float2& A, const float2& B)
	{
		return A | B;
	}
	float Size() const
	{
		return sqrtf(X*X + Y*Y);
	}
	float SizeSquared() const
	{
		return X*X + Y*Y;
	}
};

struct float3
{
public:
	float X;
	float Y;
	float Z;
	float3() { X = Y = Z = 0; }
	float3(float _x, float _y, float _z) {
		X = _x;
		Y = _y;
		Z = _z;
	}
	float3 operator^(const float3& V) const
	{
		return float3
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}

	static float3 CrossProduct(const float3& A, const float3& B)
	{
		return A ^ B;
	}

	float operator|(const float3& V) const
	{
		return X*V.X + Y*V.Y + Z*V.Z;
	}
	static float DotProduct(const float3& A, const float3& B)
	{
		return A | B;
	}

	float3 operator+(const float3& V) const
	{
		return float3(X + V.X, Y + V.Y, Z + V.Z);
	}
	void operator += (const float3& V)
	{
		X += V.X;
		Y += V.Y;
		Z += V.Z;
	}
	void operator /= (float f)
	{
		X /= f;
		Y /= f;
		Z /= f;
	}
	float3 operator-(const float3& V) const
	{
		return float3(X - V.X, Y - V.Y, Z - V.Z);
	}

	float3 operator-(float Bias) const
	{
		return float3(X - Bias, Y - Bias, Z - Bias);
	}

	float3 operator+(float Bias) const
	{
		return float3(X + Bias, Y + Bias, Z + Bias);
	}

	float3 operator*(float Scale) const
	{
		return float3(X * Scale, Y * Scale, Z * Scale);
	}

	float3 operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return float3(X * RScale, Y * RScale, Z * RScale);
	}

	float3 operator*(const float3& V) const
	{
		return float3(X * V.X, Y * V.Y, Z * V.Z);
	}

	float3 operator/(const float3& V) const
	{
		return float3(X / V.X, Y / V.Y, Z / V.Z);
	}

	bool operator==(const float3& V) const
	{
		return X == V.X && Y == V.Y && Z == V.Z;
	}

	bool operator!=(const float3& V) const
	{
		return X != V.X || Y != V.Y || Z != V.Z;
	}
	float Size() const
	{
		return sqrtf(X*X + Y*Y + Z*Z);
	}

	float SizeSquared() const
	{
		return X*X + Y*Y + Z*Z;
	}
	bool Equals(const float3& V) const
	{
		float Tolerance = 1.e-8f;
		return Abs(X - V.X) <= Tolerance && Abs(Y - V.Y) <= Tolerance && Abs(Z - V.Z) <= Tolerance;
	}
	bool Normalize(float Tolerance = 1.e-8f)
	{
		const float SquareSum = X*X + Y*Y + Z*Z;
		if (SquareSum > Tolerance)
		{
			const float Scale = 1.0f / sqrtf(SquareSum);
			X *= Scale; Y *= Scale; Z *= Scale;
			return true;
		}
		return false;
	}
	float3(const float2 V, float InZ)
		: X(V.X), Y(V.Y), Z(InZ)
	{
	}
};
//矩形
template <typename T>
struct rect4
{
	T	left, top, right, bottom;
	rect4() {
		left = top = right = bottom = 0;
	}
	rect4(T _left, T _top, T _right, T _bottom) {
		left = _left;
		top = _top;
		right = _right;
		bottom = _bottom;
	}
	inline	T	width() { return right - left; }
	inline	T	height() { return bottom - top; }
	//检测边界
	void		border(float2 p) {
		if ((T)p.X < left)
			left = (T)p.X;
		if ((T)p.Y < top)
			top = (T)p.Y;
		if ((T)p.X > right)
			right = (T)p.X;
		if ((T)p.Y > bottom)
			bottom = (T)p.Y;
	}
	template <typename Tother>
	operator rect4<Tother>() {
		return rect4<Tother>((Tother)left, (Tother)top, (Tother)right, (Tother)bottom);
	}
};
//三角形索引值数据
struct triangle3
{
	union {
		struct {
			unsigned short	v1, v2, v3;
		};
		struct {
			unsigned short v[3];
		};
	};
	triangle3() { v1 = v2 = v3 = 0; }
	triangle3(unsigned short _v1, unsigned short _v2, unsigned short _v3) {
		v1 = _v1;
		v2 = _v2;
		v3 = _v3;
	}
	unsigned short	operator[](int index) const { return v[index]; }
	unsigned short&	operator[](int index){ return v[index]; }
};
//平面数据
struct plane4 : public float3
{
	float W;
	static float3	LinePlaneIntersection(const float3 &Point1, const float3 &Point2, const plane4  &Plane)
	{
		return Point1 + (Point2 - Point1) *	((Plane.W - (Point1 | Plane)) / ((Point2 - Point1) | Plane));
	}
	static float3	LinePlaneIntersection(const float3 &Point1, const float3 &Point2, const float3 &PlaneOrigin, const float3 &PlaneNormal)
	{
		return Point1 + (Point2 - Point1) *	(((PlaneOrigin - Point1) | PlaneNormal) / ((Point2 - Point1) | PlaneNormal));
	}
};

class HXAllocation
{
public:
	void*		alloc(size_t t) { return (void*)::malloc(t); }
	void		free(void* v) { ::free(v); }
	void*		realloc(void* v, size_t t) { return (void*)::realloc(v, t); }
	static HXAllocation&	Get() {
		static HXAllocation s;
		return s;
	}
};
//跨dll传输数组的时候采用
template <typename T>
class HXArray
{
public:
	HXArray() {
		m_size = 0;
		m_pointer = 0;
	}
	HXArray(std::vector<T>& v) {
		if (v.size() > 0) {
			m_size = v.size();
			m_pointer = &v.at(0);
		}
		else {
			m_size = 0;
			m_pointer = 0;
		}
	}
	HXArray(T* v, size_t size) {
		m_size = size;
		m_pointer = v;
	}
	void		operator = (const HXArray<T>& v) {
		clean();
		m_pointer = v.m_pointer;
		m_size = v.m_size;
	}
	~HXArray(){
		clean();
	}
	void		clean() {
		m_size = 0;
		m_pointer = 0;
	}
	bool		empty() const { return (m_pointer == 0); }
	size_t		size() const { return m_size; }
	T*			pointer() const { return m_pointer; }
	T&			operator[](int index) const { return m_pointer[index]; }
	void		copyto(std::vector<T>& result) const {
		if (m_size <= 0)return;
		result.resize(m_size);
		memcpy(&result.at(0), m_pointer, sizeof(T)*m_size);
	}
private:
	T*				m_pointer;
	size_t			m_size;
};
//跨dll传输数组的时候采用
template <typename T>
class HXArrayVector
{
public:
	HXArrayVector() {
		m_size = 0;
		m_pointer = 0;
		m_alloc = &HXAllocation::Get();
		m_allocSize = 0;
	}
	HXArrayVector(std::vector<T>& v) {
		m_alloc = &HXAllocation::Get();
		m_allocSize = 0;
		m_size = 0;
		m_pointer = 0;
		from(v);
	}
	~HXArrayVector() {
		clear();
	}
	void			clear() {
		if (m_alloc && m_pointer)
			m_alloc->free(m_pointer);
		m_size = 0;
		m_pointer = 0;
		m_allocSize = 0;
	}
	bool			empty() const { return (m_pointer == 0); }
	size_t			size() const { return m_size; }
	T*				pointer() const { return m_pointer; }
	T&				operator[](int index) const { return m_pointer[index]; }
	void			operator = (const HXArray<T>& v) {
		clean();
		from(v);
	}
	void			copyto(std::vector<T>& result) const {
		if (m_size <= 0)return;
		result.resize(m_size);
		memcpy(&result.at(0), m_pointer, sizeof(T)*m_size);
	}
	void			from(const std::vector<T>& result){
		clean();
		setCount(result.size());
		for (int i = 0; i < m_size; i++) {
			m_pointer[i] = reuslt[i];
		}
	}
	void			operator +=	(const T& t) {
		int s = m_size;
		setCount(m_size + 1);
		m_pointer[s] = t;
	}
	void			setCount(int newCount) {
		if (m_allocSize >= newCount) {
			m_size = newCount;
			return;
		}
		m_allocSize = newCount + 10;
		if (m_pointer)
			m_pointer = (T*)m_alloc->realloc(m_pointer, sizeof(T)*m_allocSize);
		else
			m_pointer = (T*)m_alloc->alloc(sizeof(T)*m_allocSize);
		m_size = newCount;
	}
private:
	HXAllocation*	m_alloc;
	T*				m_pointer;
	size_t			m_size;
	size_t			m_allocSize;
};

template <typename T>
inline	T	clamp(T x, T fmin, T fmax) {
	if (x < fmin)return fmin;
	if (x > fmax)return fmax;
	return x;
}
#pragma pack(pop)
