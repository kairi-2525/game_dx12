#pragma once

#include <type_traits>
#include <math.h>
#include <DirectXMath.h>

namespace KDL
{
	namespace MATH
	{
		template <class T>
		class Number2
		{
			static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "Number2 Class Error");
		public:
			union
			{
				struct
				{
					T x, y;
				};
				struct
				{
					T u, v;
				};
				struct
				{
					T w, h;
				};
				struct
				{
					T min, max;
				};
			};

			Number2(T x = 0, T y = 0) : x(x), y(y) {};

			//オペレータ
			Number2<T>& operator=(const Number2<T>& num)
			{
				x = num.x; y = num.y;
				return *this;
			}
			Number2<T>& operator+=(const Number2<T>& num)
			{
				x += num.x; y += num.y;
				return *this;
			}
			Number2<T>& operator-=(const Number2<T>& num)
			{
				x -= num.x; y -= num.y;
				return *this;
			}
			Number2<T>& operator*=(const Number2<T>& num)
			{
				x *= num.x; y *= num.y;
				return *this;
			}
			Number2<T>& operator/=(const Number2<T>& num)
			{
				if (num.x != 0) x /= num.x;
				if (num.y != 0) y /= num.y;
				return *this;
			}
			Number2<T> operator+(const Number2<T>& num) const
			{
				return { x + num.x, y + num.y };
			}
			Number2<T> operator-(const Number2<T>& num) const
			{
				return { x - num.x, y - num.y };
			}
			Number2<T> operator*(const Number2<T>& num) const
			{
				return { x * num.x, y * num.y };
			}
			Number2<T> operator/(const Number2<T>& num) const
			{
				return {
					num.x == 0 ? x : x / num.x,
					num.y == 0 ? y : y / num.y
				};
			}
			Number2<T>& operator=(const T& num)
			{
				x = num; y = num;
				return *this;
			}
			Number2<T>& operator+=(const T& num)
			{
				x += num; y += num;
				return *this;
			}
			Number2<T>& operator-=(const T& num)
			{
				x -= num; y -= num;
				return *this;
			}
			Number2<T>& operator*=(const T& num)
			{
				x *= num; y *= num;
				return *this;
			}
			Number2<T>& operator/=(const T& num)
			{
				if (num != 0)
				{
					x /= num; y /= num;
				}
				return *this;
			}
			Number2<T> operator+(const T& num) const
			{
				return { x + num, y + num };
			}
			Number2<T> operator-(const T& num) const
			{
				return { x - num, y - num };
			}
			Number2<T> operator*(const T& num) const
			{
				return { x * num, y * num };
			}
			Number2<T> operator/(const T& num) const
			{
				if (num == 0)
					return { x, y };
				else
					return { x / num, y / num };
			}

			//変換用
			//DirectXMath変換用
			Number2<T>& operator=(const DirectX::XMFLOAT2& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				return *this;
			}
			Number2<T>& operator=(const DirectX::XMINT2& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				return *this;
			}
			Number2<T>& operator=(const DirectX::XMUINT2& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				return *this;
			}
			Number2(const DirectX::XMFLOAT2& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)) {}
			Number2(const DirectX::XMINT2& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)) {}
			Number2(const DirectX::XMUINT2& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)) {}

			//算術関数
			//内積
			T Dot(const Number2<T>& num) const
			{
				return x * num.x + y * num.y;
			}
			//内積(戻り値型指定)
			template <class Gt>
			Gt Dot(const Number2<T>& num) const
			{
				return static_cast<Gt>(x)* num.x + static_cast<Gt>(y)* num.y;
			}

			//外積
			T Cross(const Number2<T>& num) const
			{
				return x * num.y - y * num.x;
			}
			//外積(戻り値型指定)
			template <class Gt>
			Gt Cross(const Number2<T>& num) const
			{
				return static_cast<Gt>(x)* num.y - static_cast<Gt>(y)* num.x;
			}

			//長さ
			T Length()
			{
				return static_cast<T>(sqrt(Dot<double>(*this)));
			}
			//長さ(戻り値型指定)
			template <class Gt>
			Gt Length()
			{
				return static_cast<Gt>(sqrt(Dot<double>(*this)));
			}

			//型変換
			template <class Gt>
			Number2<Gt> Cast() const
			{
				return { static_cast<Gt>(x), static_cast<Gt>(y) };
			}
			template <class Dt, class Gt>
			Dt DCast() const
			{
				return { static_cast<Gt>(x), static_cast<Gt>(y) };
			}
		};

		template <class T>
		class Number3
		{
			static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "Number3 Class Error");
		public:
			union
			{
				struct
				{
					T x, y, z;
				};
				struct
				{
					T r, g, b;
				};
				struct
				{
					T u, v, w;
				};
			};

			Number3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {};

			//オペレータ
			Number3<T>& operator=(const Number3<T>& num)
			{
				x = num.x; y = num.y; z = num.z;
				return *this;
			}
			Number3<T>& operator+=(const Number3<T>& num)
			{
				x += num.x; y += num.y; z += num.z;
				return *this;
			}
			Number3<T>& operator-=(const Number3<T>& num)
			{
				x -= num.x; y -= num.y; z -= num.z;
				return *this;
			}
			Number3<T>& operator*=(const Number3<T>& num)
			{
				x *= num.x; y *= num.y; z *= num.z;
				return *this;
			}
			Number3<T>& operator/=(const Number3<T>& num)
			{
				if (num.x != 0) x /= num.x;
				if (num.y != 0) y /= num.y;
				if (num.z != 0) z /= num.z;
				return *this;
			}
			Number3<T> operator+(const Number3<T>& num) const
			{
				return { x + num.x, y + num.y, z + num.z };
			}
			Number3<T> operator-(const Number3<T>& num) const
			{
				return { x - num.x, y - num.y, z - num.z };
			}
			Number3<T> operator*(const Number3<T>& num) const
			{
				return { x * num.x, y * num.y, z * num.z };
			}
			Number3<T> operator/(const Number3<T>& num) const
			{
				return {
					num.x == 0 ? x : x / num.x,
					num.y == 0 ? y : y / num.y,
					num.z == 0 ? z : z / num.z
				};
			}
			Number3<T>& operator=(const T& num)
			{
				x = num; y = num; z = num;
				return *this;
			}
			Number3<T>& operator+=(const T& num)
			{
				x += num; y += num; z += num;
				return *this;
			}
			Number3<T>& operator-=(const T& num)
			{
				x -= num; y -= num; z -= num;
				return *this;
			}
			Number3<T>& operator*=(const T& num)
			{
				x *= num; y *= num; z *= num;
				return *this;
			}
			Number3<T>& operator/=(const T& num)
			{
				if (num != 0)
				{
					x /= num; y /= num; z /= num;
				}
				return *this;
			}
			Number3<T> operator+(const T& num) const
			{
				return { x + num, y + num, z + num };
			}
			Number3<T> operator-(const T& num) const
			{
				return { x - num, y - num, z - num };
			}
			Number3<T> operator*(const T& num) const
			{
				return { x * num, y * num, z * num };
			}
			Number3<T> operator/(const T& num) const
			{
				if (num == 0)
					return { x, y, z };
				else
					return { x / num, y / num, z / num };
			}

			//DirectXMath変換用
			Number3<T>& operator=(const DirectX::XMFLOAT3& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				z = static_cast<T>(d.z);
				return *this;
			}
			Number3<T>& operator=(const DirectX::XMINT3& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				z = static_cast<T>(d.z);
				return *this;
			}
			Number3<T>& operator=(const DirectX::XMUINT3& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				z = static_cast<T>(d.z);
				return *this;
			}
			Number3(const DirectX::XMFLOAT3& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)),
				z(static_cast<T>(d.z)) {}
			Number3(const DirectX::XMINT3& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)),
				z(static_cast<T>(d.z)) {}
			Number3(const DirectX::XMUINT3& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)),
				z(static_cast<T>(d.z)) {}

			//算術関数
			//内積
			T Dot(const Number3<T>& num) const
			{
				return x * num.x + y * num.y + z * num.z;
			}
			//内積(戻り値型指定)
			template <class Gt>
			Gt Dot(const Number3<T>& num) const
			{
				return static_cast<Gt>(x)* num.x + static_cast<Gt>(y)* num.y + static_cast<Gt>(z)* num.z;
			}

			//外積
			Number3<T> Cross(const Number3<T>& num) const
			{
				return {
					y * num.z - z * num.y,
					z * num.x - x * num.z,
					x * num.y - y * num.x
				};
			}
			//外積(戻り値型指定)
			template <class Gt>
			Number3<Gt> Cross(const Number3<T>& num) const
			{
				return {
					static_cast<Gt>(y)* num.z - static_cast<Gt>(z)* num.y,
					static_cast<Gt>(z)* num.x - static_cast<Gt>(x)* num.z,
					static_cast<Gt>(x)* num.y - static_cast<Gt>(y)* num.x
				};
			}

			//長さ
			T Length()
			{
				return static_cast<T>(sqrt(Dot<double>(*this)));
			}
			//長さ(戻り値型指定)
			template <class Gt>
			Gt Length()
			{
				return static_cast<Gt>(sqrt(Dot<double>(*this)));
			}

			//型変換
			template <class Gt>
			Number3<Gt> Cast() const
			{
				return { static_cast<Gt>(x), static_cast<Gt>(y), static_cast<Gt>(z) };
			}
			template <class Dt, class Gt>
			Dt DCast() const
			{
				return { static_cast<Gt>(x), static_cast<Gt>(y), static_cast<Gt>(z) };
			}
		};

		template <class T>
		class Number4
		{
			static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "Number4 Class Error");
		public:
			union
			{
				struct
				{
					T x, y, z, w;
				};
				struct
				{
					T r, g, b, a;
				};
				struct
				{
					T left, top, right, bottom;
				};
			};

			Number4(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {};

			//オペレータ
			Number4<T>& operator=(const Number4<T>& num)
			{
				x = num.x; y = num.y; z = num.z; w = num.w;
				return *this;
			}
			Number4<T>& operator+=(const Number4<T>& num)
			{
				x += num.x; y += num.y; z += num.z; w += num.w;
				return *this;
			}
			Number4<T>& operator-=(const Number4<T>& num)
			{
				x -= num.x; y -= num.y; z -= num.z; w -= num.w;
				return *this;
			}
			Number4<T>& operator*=(const Number4<T>& num)
			{
				x *= num.x; y *= num.y; z *= num.z; w *= num.w;
				return *this;
			}
			Number4<T>& operator/=(const Number4<T>& num)
			{
				if (num.x != 0) x /= num.x;
				if (num.y != 0) y /= num.y;
				if (num.z != 0) z /= num.z;
				if (num.w != 0) w /= num.w;
				return *this;
			}
			Number4<T> operator+(const Number4<T>& num) const
			{
				return { x + num.x, y + num.y, z + num.z, w + num.w };
			}
			Number4<T> operator-(const Number4<T>& num) const
			{
				return { x - num.x, y - num.y, z - num.z, w - num.w };
			}
			Number4<T> operator*(const Number4<T>& num) const
			{
				return { x * num.x, y * num.y, z * num.z, w * num.w };
			}
			Number4<T> operator/(const Number4<T>& num) const
			{
				return {
					num.x == 0 ? x : x / num.x,
					num.y == 0 ? y : y / num.y,
					num.z == 0 ? z : z / num.z,
					num.w == 0 ? w : w / num.w
				};
			}
			Number4<T>& operator=(const T& num)
			{
				x = num; y = num; z = num; w = num;
				return *this;
			}
			Number4<T>& operator+=(const T& num)
			{
				x += num; y += num; z += num; w += num;
				return *this;
			}
			Number4<T>& operator-=(const T& num)
			{
				x -= num; y -= num; z -= num; w -= num;
				return *this;
			}
			Number4<T>& operator*=(const T& num)
			{
				x *= num; y *= num; z *= num; w *= num;
				return *this;
			}
			Number4<T>& operator/=(const T& num)
			{
				if (num != 0)
				{
					x /= num; y /= num; z /= num; w /= num;
				}
				return *this;
			}
			Number4<T> operator+(const T& num) const
			{
				return { x + num, y + num, z + num, w + num };
			}
			Number4<T> operator-(const T& num) const
			{
				return { x - num, y - num, z - num, w - num };
			}
			Number4<T> operator*(const T& num) const
			{
				return { x * num, y * num, z * num, w * num };
			}
			Number4<T> operator/(const T& num) const
			{
				if (num == 0)
					return { x, y, z, w };
				else
					return { x / num, y / num, z / num, w / num };
			}

			//DirectXMath変換用
			Number4<T>& operator=(const DirectX::XMFLOAT4& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				z = static_cast<T>(d.z);
				w = static_cast<T>(d.w);
				return *this;
			}
			Number4<T>& operator=(const DirectX::XMINT4& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				z = static_cast<T>(d.z);
				w = static_cast<T>(d.w);
				return *this;
			}
			Number4<T>& operator=(const DirectX::XMUINT4& d)
			{
				x = static_cast<T>(d.x);
				y = static_cast<T>(d.y);
				z = static_cast<T>(d.z);
				w = static_cast<T>(d.w);
				return *this;
			}
			Number4(const DirectX::XMFLOAT4& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)),
				z(static_cast<T>(d.z)),
				w(static_cast<T>(d.w)) {}
			Number4(const DirectX::XMINT4& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)),
				z(static_cast<T>(d.z)),
				w(static_cast<T>(d.w)) {}
			Number4(const DirectX::XMUINT4& d) :
				x(static_cast<T>(d.x)),
				y(static_cast<T>(d.y)),
				z(static_cast<T>(d.z)),
				w(static_cast<T>(d.w)) {}

			//算術関数
			//内積
			T Dot(const Number4<T>& num) const
			{
				return x * num.x + y * num.y + z * num.z + w * num.w;
			}
			//内積(戻り値型指定)
			template <class Gt>
			Gt Dot(const Number4<T>& num) const
			{
				return static_cast<Gt>(x)* num.x + static_cast<Gt>(y)* num.y + static_cast<Gt>(z)* num.z + static_cast<Gt>(w)* num.w;
			}

			//長さ
			T Length()
			{
				return static_cast<T>(sqrt(Dot<double>(*this)));
			}
			//長さ(戻り値型指定)
			template <class Gt>
			Gt Length()
			{
				return static_cast<Gt>(sqrt(Dot<double>(*this)));
			}

			//型変換
			template <class Gt>
			Number4<Gt> Cast() const
			{
				return { static_cast<Gt>(x), static_cast<Gt>(y), static_cast<Gt>(z), static_cast<Gt>(w) };
			}
			template <class Dt, class Gt>
			Dt DCast()
			{
				return { static_cast<Gt>(x), static_cast<Gt>(y), static_cast<Gt>(z), static_cast<Gt>(w) };
			}
		};

		using INT2 = Number2<int>;
		using INT3 = Number3<int>;
		using INT4 = Number4<int>;

		using UINT2 = Number2<unsigned int>;
		using UINT3 = Number3<unsigned int>;
		using UINT4 = Number4<unsigned int>;

		using FLOAT2 = Number2<float>;
		using FLOAT3 = Number3<float>;
		using FLOAT4 = Number4<float>;

		using DOUBLE2 = Number2<double>;
		using DOUBLE3 = Number3<double>;
		using DOUBLE4 = Number4<double>;

		using POINT2 = INT2;
		using POINT3 = INT3;
		using POINT4 = INT4;

		using POINT2F = FLOAT2;
		using POINT3F = FLOAT3;
		using POINT4F = FLOAT4;

		using POINT2D = DOUBLE2;
		using POINT3D = DOUBLE3;
		using POINT4D = DOUBLE4;

		using VECTOR2 = INT2;
		using VECTOR3 = INT3;
		using VECTOR4 = INT4;

		using VECTOR2F = FLOAT2;
		using VECTOR3F = FLOAT3;
		using VECTOR4F = FLOAT4;

		using VECTOR2D = DOUBLE2;
		using VECTOR3D = DOUBLE3;
		using VECTOR4D = DOUBLE4;

		using COLOR2 = UINT2;
		using COLOR3 = UINT3;
		using COLOR4 = UINT4;

		using COLOR2F = FLOAT2;
		using COLOR3F = FLOAT3;
		using COLOR4F = FLOAT4;

		using COLOR2D = DOUBLE2;
		using COLOR3D = DOUBLE3;
		using COLOR4D = DOUBLE4;

		extern int GetRand(int min, int max);
		extern float GetRandF(float min, float max, size_t cut_num = 1.f);
		extern double GetRandD(double min, double max, size_t cut_num = 1.f);

		template <class T>
		extern inline bool IsHitCircleAndCircle(const Number3<T>& c1_pos, T c1_radius, const Number3<T>& c2_pos, T c2_radius)
		{
			return (c1_pos.x - c2_pos.x) * (c1_pos.x - c2_pos.x) + (c1_pos.y - c2_pos.y) * (c1_pos.y - c2_pos.y) + (c1_pos.z - c2_pos.z) * (c1_pos.z - c2_pos.z) <= (c1_radius + c2_radius) * (c1_radius + c2_radius);
		}
	}
}