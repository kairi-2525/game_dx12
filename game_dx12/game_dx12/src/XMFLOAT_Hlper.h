#pragma once

#include <DirectXMath.h>

#include "Vector3.h"
#include "Math.h"
#include "Vector2.h"

//-------------------------------------------------------------------------------------------------------------
//. XMFLOAT4ån
//-------------------------------------------------------------------------------------------------------------

static inline void operator+= (DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
}

static inline void operator-= (DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
}

static inline void operator/= (DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	v1.w /= v2.w;
}

static inline void operator*= (DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;
}

static inline void operator%= (DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x = fmodf(v1.x, v2.x);
	v1.y = fmodf(v1.y, v2.y);
	v1.z = fmodf(v1.z, v2.z);
	v1.w = fmodf(v1.w, v2.w);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	return DirectX::XMFLOAT4{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}

[[nodiscard]] static inline constexpr auto operator- (
	const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	return DirectX::XMFLOAT4{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

[[nodiscard]] static inline constexpr auto operator* (
	const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	return DirectX::XMFLOAT4{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	return DirectX::XMFLOAT4{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w };
}

[[nodiscard]] static inline constexpr auto operator% (
	const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	return DirectX::XMFLOAT4{ fmodf(v1.x, v2.x), fmodf(v1.y, v2.y), fmodf(v1.z, v2.z), fmodf(v1.w, v2.w) };
}

static inline void operator+= (DirectX::XMFLOAT4& v1, const float num)
{
	v1.x += num;
	v1.y += num;
	v1.z += num;
	v1.w += num;
}

static inline void operator-= (DirectX::XMFLOAT4& v1, const float num)
{
	v1.x -= num;
	v1.y -= num;
	v1.z -= num;
	v1.w -= num;
}

static inline void operator/= (DirectX::XMFLOAT4& v1, const float num)
{
	v1.x /= num;
	v1.y /= num;
	v1.z /= num;
	v1.w /= num;
}

static inline void operator*= (DirectX::XMFLOAT4& v1, const float num)
{
	v1.x *= num;
	v1.y *= num;
	v1.z *= num;
	v1.w *= num;
}

static inline void operator%= (DirectX::XMFLOAT4& v1, const float num)
{
	v1.x = fmodf(v1.x, num);
	v1.y = fmodf(v1.y, num);
	v1.z = fmodf(v1.z, num);
	v1.w = fmodf(v1.w, num);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const DirectX::XMFLOAT4& v1, const float num)
{
	return DirectX::XMFLOAT4{ v1.x + num, v1.y + num, v1.z + num, v1.w + num };
}

[[nodiscard]] static inline constexpr auto operator- (
	const DirectX::XMFLOAT4& v1, const float num)
{
	return DirectX::XMFLOAT4{ v1.x - num, v1.y - num, v1.z - num, v1.w - num };
}

[[nodiscard]] static inline constexpr auto operator* (
	const DirectX::XMFLOAT4& v1, const float num)
{
	return DirectX::XMFLOAT4{ v1.x * num, v1.y * num, v1.z * num, v1.w * num };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const DirectX::XMFLOAT4& v1, const float num)
{
	return DirectX::XMFLOAT4{ v1.x / num, v1.y / num, v1.z / num, v1.w / num };
}

[[nodiscard]] static inline constexpr auto operator% (
	const DirectX::XMFLOAT4& v1, const float num)
{
	return DirectX::XMFLOAT4{ fmodf(v1.x, num), fmodf(v1.y, num), fmodf(v1.z, num), fmodf(v1.w, num) };
}

[[nodiscard]] static inline constexpr bool operator== (
	const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	return (Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z) &&
		Math::AdjEqual(v1.w, v2.w));
}

[[nodiscard]] static inline constexpr bool operator!= (
	const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
{
	return !(Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z) &&
		Math::AdjEqual(v1.w, v2.w));
}

//-------------------------------------------------------------------------------------------------------------
//. XMFLOAT3ån
//-------------------------------------------------------------------------------------------------------------

static inline void operator+= (DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

static inline void operator-= (DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
}

static inline void operator/= (DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
}

static inline void operator*= (DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
}

static inline void operator%= (DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	v1.x = fmodf(v1.x, v2.x);
	v1.y = fmodf(v1.y, v2.y);
	v1.z = fmodf(v1.z, v2.z);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	return DirectX::XMFLOAT3{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

[[nodiscard]] static inline constexpr auto operator- (
	const DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	return DirectX::XMFLOAT3{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

[[nodiscard]] static inline constexpr auto operator* (
	const DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	return DirectX::XMFLOAT3{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	return DirectX::XMFLOAT3{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}

[[nodiscard]] static inline constexpr auto operator% (
	const DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	return DirectX::XMFLOAT3{ fmodf(v1.x, v2.x), fmodf(v1.y, v2.y), fmodf(v1.z, v2.z) };
}

static inline void operator+= (Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

static inline void operator-= (Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
}

static inline void operator/= (Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
}

static inline void operator*= (Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
}

static inline void operator%= (Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x = fmodf(v1.x, v2.x);
	v1.y = fmodf(v1.y, v2.y);
	v1.z = fmodf(v1.z, v2.z);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	return Vector3<float>{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

[[nodiscard]] static inline constexpr auto operator- (
	const Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	return Vector3<float>{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

[[nodiscard]] static inline constexpr auto operator* (
	const Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	return Vector3<float>{ v1.x* v2.x, v1.y* v2.y, v1.z* v2.z };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	return Vector3<float>{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}

[[nodiscard]] static inline constexpr auto operator% (
	const Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	return Vector3<float>{ fmodf(v1.x, v2.x), fmodf(v1.y, v2.y), fmodf(v1.z, v2.z) };
}

static inline void operator+= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

static inline void operator-= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
}

static inline void operator*= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
}

static inline void operator/= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
}

static inline void operator%= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	v1.x = fmodf(v1.x, v2.x);
	v1.y = fmodf(v1.y, v2.y);
	v1.z = fmodf(v1.z, v2.z);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return DirectX::XMFLOAT3{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

[[nodiscard]] static inline constexpr auto operator- (
	const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return DirectX::XMFLOAT3{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

[[nodiscard]] static inline constexpr auto operator* (
	const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return DirectX::XMFLOAT3{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return DirectX::XMFLOAT3{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}

[[nodiscard]] static inline constexpr auto operator% (
	const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return DirectX::XMFLOAT3{ fmodf(v1.x, v2.x), fmodf(v1.y, v2.y), fmodf(v1.z, v2.z) };
}

static inline void operator+= (DirectX::XMFLOAT3& v1, const float num)
{
	v1.x += num;
	v1.y += num;
	v1.z += num;
}

static inline void operator-= (DirectX::XMFLOAT3& v1, const float num)
{
	v1.x -= num;
	v1.y -= num;
	v1.z -= num;
}

static inline void operator/= (DirectX::XMFLOAT3& v1, const float num)
{
	v1.x /= num;
	v1.y /= num;
	v1.z /= num;
}

static inline void operator*= (DirectX::XMFLOAT3& v1, const float num)
{
	v1.x *= num;
	v1.y *= num;
	v1.z *= num;
}

static inline void operator%= (DirectX::XMFLOAT3& v1, const float num)
{
	v1.x = fmodf(v1.x, num);
	v1.y = fmodf(v1.y, num);
	v1.z = fmodf(v1.z, num);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const DirectX::XMFLOAT3& v1, const float num)
{
	return DirectX::XMFLOAT3{ v1.x + num, v1.y + num, v1.z + num };
}

[[nodiscard]] static inline constexpr auto operator- (
	const DirectX::XMFLOAT3& v1, const float num)
{
	return DirectX::XMFLOAT3{ v1.x - num, v1.y - num, v1.z - num };
}

[[nodiscard]] static inline constexpr auto operator* (
	const DirectX::XMFLOAT3& v1, const float num)
{
	return DirectX::XMFLOAT3{ v1.x * num, v1.y * num, v1.z * num };
}

[[nodiscard]] static inline constexpr auto operator% (
	const DirectX::XMFLOAT3& v1, const float num)
{
	return DirectX::XMFLOAT3{ fmodf(v1.x, num), fmodf(v1.y, num), fmodf(v1.z, num) };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const DirectX::XMFLOAT3& v1, const float num)
{
	return DirectX::XMFLOAT3{ v1.x / num, v1.y / num, v1.z / num };
}

[[nodiscard]] static inline constexpr bool operator== (
	const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return (Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z));
}

[[nodiscard]] static inline constexpr bool operator!= (
	const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	return !(Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z));
}

[[nodiscard]] static inline constexpr bool operator== (
	const Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	return (Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z));
}

[[nodiscard]] static inline constexpr bool operator!= (
	const Vector3<float>& v1, const DirectX::XMFLOAT3& v2)
{
	return !(Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z));
}

[[nodiscard]] static inline constexpr bool operator== (
	const DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	return (Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z));
}

[[nodiscard]] static inline constexpr bool operator!= (
	const DirectX::XMFLOAT3& v1, const Vector3<float>& v2)
{
	return !(Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y) && Math::AdjEqual(v1.z, v2.z));
}

//-------------------------------------------------------------------------------------------------------------
//. XMFLOAT2ån
//-------------------------------------------------------------------------------------------------------------

static inline void operator+= (DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
}

static inline void operator-= (DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
}

static inline void operator/= (DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
}

static inline void operator*= (DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
}

static inline void operator%= (DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	v1.x = fmodf(v1.x, v2.x);
	v1.y = fmodf(v1.y, v2.y);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	return DirectX::XMFLOAT2{ v1.x + v2.x, v1.y + v2.y, };
}

[[nodiscard]] static inline constexpr auto operator- (
	const DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	return DirectX::XMFLOAT2{ v1.x - v2.x, v1.y - v2.y };
}

[[nodiscard]] static inline constexpr auto operator* (
	const DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	return DirectX::XMFLOAT2{ v1.x * v2.x, v1.y * v2.y };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	return DirectX::XMFLOAT2{ v1.x / v2.x, v1.y / v2.y };
}

[[nodiscard]] static inline constexpr auto operator% (
	const DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	return DirectX::XMFLOAT2{ fmodf(v1.x, v2.x), fmodf(v1.y, v2.y) };
}

static inline void operator+= (Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
}

static inline void operator-= (Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
}

static inline void operator/= (Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
}

static inline void operator*= (Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
}

static inline void operator%= (Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x = fmodf(v1.x, v2.x);
	v1.y = fmodf(v1.y, v2.y);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	return Vec2sub::MakeVector2(v1.x + v2.x, v1.y + v2.y);
}

[[nodiscard]] static inline constexpr auto operator- (
	const Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	return Vec2sub::MakeVector2(v1.x - v2.x, v1.y - v2.y);
}

[[nodiscard]] static inline constexpr auto operator* (
	const Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	return Vec2sub::MakeVector2(v1.x * v2.x, v1.y * v2.y);
}

[[nodiscard]] static inline constexpr auto operator/ (
	const Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	return Vec2sub::MakeVector2(v1.x / v2.x, v1.y / v2.y);
}

[[nodiscard]] static inline constexpr auto operator% (
	const Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	return Vec2sub::MakeVector2(fmodf(v1.x, v2.x), fmodf(v1.y, v2.y));
}

static inline void operator+= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
}

static inline void operator-= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
}

static inline void operator/= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
}

static inline void operator*= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
}

static inline void operator%= (DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x = fmodf(v1.x, v2.x);
	v1.y = fmodf(v1.y, v2.y);
}

[[nodiscard]] static inline constexpr auto operator+ (
	const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	return DirectX::XMFLOAT2{ v1.x + v2.x, v1.y + v2.y };
}

[[nodiscard]] static inline constexpr auto operator- (
	const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	return DirectX::XMFLOAT2{ v1.x - v2.x, v1.y - v2.y };
}

[[nodiscard]] static inline constexpr auto operator* (
	const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	return DirectX::XMFLOAT2{ v1.x * v2.x, v1.y * v2.y };
}

[[nodiscard]] static inline constexpr auto operator/ (
	const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	return DirectX::XMFLOAT2{ v1.x / v2.x, v1.y / v2.y };
}

[[nodiscard]] static inline constexpr auto operator% (
	const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	return DirectX::XMFLOAT2{ fmodf(v1.x, v2.x), fmodf(v1.y, v2.y) };
}

static inline void operator+= (DirectX::XMFLOAT2& v1, const float num)
{
	v1.x += num;
	v1.y += num;
}

static inline void operator-= (DirectX::XMFLOAT2& v1, const float num)
{
	v1.x -= num;
	v1.y -= num;
}

static inline void operator/= (DirectX::XMFLOAT2& v1, const float num)
{
	v1.x /= num;
	v1.y /= num;
}

static inline void operator*= (DirectX::XMFLOAT2& v1, const float num)
{
	v1.x *= num;
	v1.y *= num;
}

static inline void operator%= (DirectX::XMFLOAT2& v1, const float num)
{
	v1.x = fmodf(v1.x, num);
	v1.y = fmodf(v1.y, num);
}

[[nodiscard]] static inline constexpr auto operator+ (const DirectX::XMFLOAT2& v1, const float num)
{
	return DirectX::XMFLOAT2{ v1.x + num, v1.y + num };
}

[[nodiscard]] static inline constexpr auto operator- (const DirectX::XMFLOAT2& v1, const float num)
{
	return DirectX::XMFLOAT2{ v1.x - num, v1.y - num };
}

[[nodiscard]] static inline constexpr auto operator* (const DirectX::XMFLOAT2& v1, const float num)
{
	return DirectX::XMFLOAT2{ v1.x * num, v1.y * num };
}

[[nodiscard]] static inline constexpr auto operator/ (const DirectX::XMFLOAT2& v1, const float num)
{
	return DirectX::XMFLOAT2{ v1.x / num, v1.y / num };
}

[[nodiscard]] static inline constexpr auto operator% (const DirectX::XMFLOAT2& v1, const float num)
{
	return DirectX::XMFLOAT2{ fmodf(v1.x, num), fmodf(v1.y, num) };
}

[[nodiscard]] static inline constexpr bool operator== (
	const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	return (Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y));
}

[[nodiscard]] static inline constexpr bool operator!= (
	const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
{
	return !(Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y));
}

[[nodiscard]] static inline constexpr bool operator== (
	const Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	return (Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y));
}

[[nodiscard]] static inline constexpr bool operator!= (
	const Vector2<float>& v1, const DirectX::XMFLOAT2& v2)
{
	return !(Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y));
}

[[nodiscard]] static inline constexpr bool operator== (
	const DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	return (Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y));
}

[[nodiscard]] static inline constexpr bool operator!= (
	const DirectX::XMFLOAT2& v1, const Vector2<float>& v2)
{
	return !(Math::AdjEqual(v1.x, v2.x) && Math::AdjEqual(v1.y, v2.y));
}

//-------------------------------------------------------------------------------------------------------------
// óòï÷ä÷êîån
//-------------------------------------------------------------------------------------------------------------

//. XMVECTORïœä∑-----------------------------------------------------------------------------------------------

static inline auto ToXMVECTOR(const DirectX::XMFLOAT4& vec)
{
	return DirectX::XMLoadFloat4(&vec);
}

static inline auto ToXMVECTOR(const DirectX::XMFLOAT3& vec)
{
	return DirectX::XMLoadFloat3(&vec);
}

static inline auto ToXMVECTOR(const DirectX::XMFLOAT2& vec)
{
	return DirectX::XMLoadFloat2(&vec);
}

static inline auto ToXMVECTOR(const Vector3<float>& vec)
{
	return DirectX::XMLoadFloat3(&(vec.ToXMFLOAT3()));
}

static inline auto ToXMVECTOR(const Vector2<float>& vec)
{
	return DirectX::XMLoadFloat2(&(vec.ToXMFLOAT2()));
}

static inline auto ToXMVECTOR(const float vec)
{
	return DirectX::XMLoadFloat(&vec);
}

//. XMVECTORïœä∑(ê≥ãKâª)---------------------------------------------------------------------------------------

static inline auto ToNormalizeXMVECTOR(const DirectX::XMFLOAT4& vec)
{
	return DirectX::XMVector4Normalize(ToXMVECTOR(vec));
}

static inline auto ToNormalizeXMVECTOR(const DirectX::XMFLOAT3& vec)
{
	return DirectX::XMVector3Normalize(ToXMVECTOR(vec));
}

static inline auto ToNormalizeXMVECTOR(const DirectX::XMFLOAT2& vec)
{
	return DirectX::XMVector2Normalize(ToXMVECTOR(vec));
}

static inline auto ToNormalizeXMVECTOR(const Vector3<float>& vec)
{
	return DirectX::XMVector3Normalize(ToXMVECTOR(vec));
}

static inline auto ToNormalizeXMVECTOR(const Vector2<float>& vec)
{
	return DirectX::XMVector2Normalize(ToXMVECTOR(vec));
}

//. XMFLOATïœä∑-----------------------------------------------------------------------------------------------

static inline auto ToXMFLOAT4(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT4 rv;

	DirectX::XMStoreFloat4(&rv, vec);

	return rv;
}

static inline auto ToXMFLOAT3(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT3 rv;

	DirectX::XMStoreFloat3(&rv, vec);

	return rv;
}

static inline auto ToXMFLOAT2(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT2 rv;

	DirectX::XMStoreFloat2(&rv, vec);

	return rv;
}

static inline auto ToXMFLOAT1(const DirectX::XMVECTOR& vec)
{
	float rv;

	DirectX::XMStoreFloat(&rv, vec);

	return rv;
}

static inline auto ToVector3(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT3 rv;

	DirectX::XMStoreFloat3(&rv, vec);

	return Vec3sub::MakeVector3<float>(rv.x, rv.y, rv.z);
}

static inline auto ToVector2(const DirectX::XMVECTOR& vec)
{
	DirectX::XMFLOAT2 rv;

	DirectX::XMStoreFloat2(&rv, vec);

	return Vec2sub::MakeVector2<float>(rv.x, rv.y);
}

// éléÃå‹ì¸---------------------------------------------------------------------------------------------------

static inline DirectX::XMFLOAT2 Round(const DirectX::XMFLOAT2& vf2)
{
	DirectX::XMFLOAT2 temp;

	temp.x = roundf(vf2.x);
	temp.y = roundf(vf2.y);

	return temp;
}

static inline DirectX::XMFLOAT3 Round(const DirectX::XMFLOAT3& vf3)
{
	DirectX::XMFLOAT3 temp;

	temp.x = roundf(vf3.x);
	temp.y = roundf(vf3.y);
	temp.z = roundf(vf3.z);

	return temp;
}

static inline DirectX::XMFLOAT4 Round(const DirectX::XMFLOAT4& vf4)
{
	DirectX::XMFLOAT4 temp;

	temp.x = roundf(vf4.x);
	temp.y = roundf(vf4.y);
	temp.z = roundf(vf4.z);
	temp.w = roundf(vf4.w);

	return temp;
}

//. XMFFLOATÇÃëSÇƒÇÃïœêîÇ…ìØÇ∂ílÇë„ì¸-------------------------------------------------------------------------

static inline DirectX::XMFLOAT2 Fill2(const float num)
{
	DirectX::XMFLOAT2 temp;

	temp.x = num;
	temp.y = num;

	return temp;
}

static inline DirectX::XMFLOAT3 Fill3(const float num)
{
	DirectX::XMFLOAT3 temp;

	temp.x = num;
	temp.y = num;
	temp.z = num;

	return temp;
}

static inline DirectX::XMFLOAT4 Fill4(const float num)
{
	DirectX::XMFLOAT4 temp;

	temp.x = num;
	temp.y = num;
	temp.z = num;
	temp.w = num;

	return temp;
}

// ó›èÊ---------------------------------------------------------------------------------------------------

static inline DirectX::XMFLOAT2 Pow(const DirectX::XMFLOAT2& vf2, const float pow_num)
{
	DirectX::XMFLOAT2 temp;

	temp.x = powf(vf2.x, pow_num);
	temp.y = powf(vf2.y, pow_num);

	return temp;
}

static inline DirectX::XMFLOAT3 Pow(const DirectX::XMFLOAT3& vf3, const float pow_num)
{
	DirectX::XMFLOAT3 temp;

	temp.x = powf(vf3.x, pow_num);
	temp.y = powf(vf3.y, pow_num);
	temp.z = powf(vf3.z, pow_num);

	return temp;
}

static inline DirectX::XMFLOAT4 Pow(const DirectX::XMFLOAT4& vf4, const float pow_num)
{
	DirectX::XMFLOAT4 temp;

	temp.x = powf(vf4.x, pow_num);
	temp.y = powf(vf4.y, pow_num);
	temp.z = powf(vf4.z, pow_num);
	temp.w = powf(vf4.w, pow_num);

	return temp;
}

// ïΩï˚ç™---------------------------------------------------------------------------------------------------

static inline DirectX::XMFLOAT2 Sqrt(const DirectX::XMFLOAT2& vf2)
{
	DirectX::XMFLOAT2 temp;

	temp.x = sqrtf(vf2.x);
	temp.y = sqrtf(vf2.y);

	return temp;
}

static inline DirectX::XMFLOAT3 Sqrt(const DirectX::XMFLOAT3& vf3)
{
	DirectX::XMFLOAT3 temp;

	temp.x = sqrtf(vf3.x);
	temp.y = sqrtf(vf3.y);
	temp.z = sqrtf(vf3.z);

	return temp;
}

static inline DirectX::XMFLOAT4 Sqrt(const DirectX::XMFLOAT4& vf4)
{
	DirectX::XMFLOAT4 temp;

	temp.x = sqrtf(vf4.x);
	temp.y = sqrtf(vf4.y);
	temp.z = sqrtf(vf4.z);
	temp.w = sqrtf(vf4.w);

	return temp;
}

// óßï˚ç™---------------------------------------------------------------------------------------------------

static inline DirectX::XMFLOAT2 Cbrt(const DirectX::XMFLOAT2& vf2)
{
	DirectX::XMFLOAT2 temp;

	temp.x = cbrtf(vf2.x);
	temp.y = cbrtf(vf2.y);

	return temp;
}

static inline DirectX::XMFLOAT3 Cbrt(const DirectX::XMFLOAT3& vf3)
{
	DirectX::XMFLOAT3 temp;

	temp.x = cbrtf(vf3.x);
	temp.y = cbrtf(vf3.y);
	temp.z = cbrtf(vf3.z);

	return temp;
}

static inline DirectX::XMFLOAT4 Cbrt(const DirectX::XMFLOAT4& vf4)
{
	DirectX::XMFLOAT4 temp;

	temp.x = cbrtf(vf4.x);
	temp.y = cbrtf(vf4.y);
	temp.z = cbrtf(vf4.z);
	temp.w = cbrtf(vf4.w);

	return temp;
}

// ê‚ëŒíl---------------------------------------------------------------------------------------------------

static inline DirectX::XMFLOAT2 FAbs(const DirectX::XMFLOAT2& vf2)
{
	DirectX::XMFLOAT2 temp;

	temp.x = fabs(vf2.x);
	temp.y = fabs(vf2.y);

	return temp;
}

static inline DirectX::XMFLOAT3 FAbs(const DirectX::XMFLOAT3& vf3)
{
	DirectX::XMFLOAT3 temp;

	temp.x = fabs(vf3.x);
	temp.y = fabs(vf3.y);
	temp.z = fabs(vf3.z);

	return temp;
}

static inline DirectX::XMFLOAT4 FAbs(const DirectX::XMFLOAT4& vf4)
{
	DirectX::XMFLOAT4 temp;

	temp.x = fabs(vf4.x);
	temp.y = fabs(vf4.y);
	temp.z = fabs(vf4.z);
	temp.w = fabs(vf4.w);

	return temp;
}

// ÇOÉNÉäÉAÅ[--------------------------------------------------------------------------------------------------

static inline void Clear(DirectX::XMFLOAT2& vf2)
{
	vf2 = { 0.f, 0.f };
}

static inline void Clear(DirectX::XMFLOAT3& vf3)
{
	vf3 = { 0.f, 0.f, 0.f };
}

static inline void Clear(DirectX::XMFLOAT4& vf4)
{
	vf4 = { 0.f, 0.f, 0.f, 0.f };
}
