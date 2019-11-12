//==================================================
//
//!+++ Vector2.h
//
//==================================================
#pragma once

#include <stdexcept>
#include <cassert>
#include <cmath>
#include <type_traits>
#include <cstring>
#include <limits>
#include <initializer_list>
#include <DirectXMath.h>

//!++ Vector2 (普通版)
#if 1
template<class T>
class Vector2 final
{
private:
	template <class Type = T>
	static constexpr Type _0{ static_cast<Type>(0.0) }; // 関数内で使用

public:  //+ コンストラクタ系

	// デフォルトコンストラクタ
	template<class _Ty = T>
	constexpr Vector2() noexcept
		: x(std::forward<_Ty>(static_cast<_Ty>(0))), y(std::forward<_Ty>(static_cast<_Ty>(0))) {};

	// コンストラクタ（別々指定）
	template<class _Ty = T>
	constexpr Vector2(_Ty && x, _Ty && y) noexcept
		: x(std::forward<_Ty>(x)), y(std::forward<_Ty>(y)) { }

	// コンストラクタ（別々指定）
	template<class _Ty = T>
	constexpr Vector2(const _Ty & x, const _Ty & y) noexcept
		: x(x), y(y) { }

	// コンストラクタ（まとめて指定）
	template<class _Ty = T>
	constexpr Vector2(const _Ty num) noexcept
		: x(num), y(num) {  }

	//. XMFLOAT2用コンストラクタ
	constexpr Vector2(const DirectX::XMFLOAT2& xmfloat2) noexcept
		: x(xmfloat2.x), y(xmfloat2.y)
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");
	}

	// コピーコンストラクタ
	template<class _Ty = T>
	constexpr Vector2(const Vector2<_Ty> & v) noexcept
		: x(v.x), y(v.y) { }

	// ムーブコンストラクタ
	template<class _Ty = T>
	constexpr Vector2(Vector2<_Ty> && _Right) noexcept
		: x(std::forward<_Ty>(_Right.x)), y(std::forward<_Ty>(_Right.y)) { }

	//. XMFLOAT2用コピー代入演算子
	decltype(auto) operator= (const DirectX::XMFLOAT2& xmfloat2)
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");

		x = xmfloat2.x;
		y = xmfloat2.y;
	}

	// ムーブ代入演算子
	template<class _Ty = T>
	decltype(auto) operator= (Vector2<_Ty> && _Right) noexcept
	{
		using std::forward;

		if (this != &_Right)
		{
			Clear();

			x = forward<_Ty>(_Right.x);
			y = forward<_Ty>(_Right.y);
		}

		return (*this);
	}

	//. XMFLOAT2用ムーブ代入演算子
	decltype(auto) operator= (DirectX::XMFLOAT2&& xmfloat2) noexcept
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");

		x = std::forward<float>(xmfloat2.x);
		y = std::forward<float>(xmfloat2.y);
	}

	// イニシャライザーリスト代入演算子
	template<class _Ty = T>
	decltype(auto) operator= (const std::initializer_list<T> & list)
	{
		if (list.size() != 2)
			throw std::invalid_argument("不正なイニシャライザーリスト");

		auto itr{ list.begin() };

		x = *itr;
		itr++;
		y = *itr;
	}

	// コピー代入演算子
	template<class _Ty = T>
	decltype(auto) operator=(const Vector2<_Ty> & v) noexcept
	{
		x = v.x;
		y = v.y;

		return (*this);
	}

	// 代入演算子（まとめて指定）
	template<class _Ty = T>
	decltype(auto) operator=(const _Ty & _Right) noexcept
	{
		x = (_Right);
		y = (_Right);

		return (*this);
	}

	// デストラクタ
	~Vector2() noexcept = default;

public:
	static constexpr auto NumMaxLimit{ (std::numeric_limits<std::remove_reference<T>>::max)() };
	static constexpr auto NumMinLimit{ (std::numeric_limits<std::remove_reference<T>>::lowest)() };

private:  //+ エイリアステンプレート

	template<class _Ty = T>
	using V2 = Vector2<_Ty>;

public:  //+ 演算子オーバーロード

	// Vector2同士

	constexpr decltype(auto) operator+ (const V2<>& v) const noexcept;
	constexpr decltype(auto) operator- (const V2<>& v) const noexcept;
	constexpr decltype(auto) operator* (const V2<>& v) const noexcept;
	constexpr decltype(auto) operator/ (const V2<>& v) const;
	constexpr decltype(auto) operator% (const V2<>& v) const;

	decltype(auto) operator+= (const V2<>& v) noexcept;
	decltype(auto) operator-= (const V2<>& v) noexcept;
	decltype(auto) operator*= (const V2<>& v) noexcept;
	decltype(auto) operator/= (const V2<>& v);
	decltype(auto) operator%= (const V2<>& v);

	constexpr bool operator== (const V2<>& v) const noexcept;
	constexpr bool operator!= (const V2<>& v) const noexcept;

	constexpr bool operator< (const V2<>& v) const noexcept;
	constexpr bool operator> (const V2<>& v) const noexcept;
	constexpr bool operator<= (const V2<>& v) const noexcept;
	constexpr bool operator>= (const V2<>& v) const noexcept;

	auto operator++(int);
	auto operator--(int);
	decltype(auto) operator++();
	decltype(auto) operator--();

	decltype(auto) operator-() const;
	decltype(auto) operator+() const;

	// Vector2と数値

	constexpr decltype(auto) operator+ (const T n) const noexcept;
	constexpr decltype(auto) operator- (const T n) const noexcept;
	constexpr decltype(auto) operator* (const T n) const noexcept;
	constexpr decltype(auto) operator/ (const T n) const;
	constexpr decltype(auto) operator% (const T n) const;

	decltype(auto) operator+= (const T n) noexcept;
	decltype(auto) operator-= (const T n) noexcept;
	decltype(auto) operator*= (const T n) noexcept;
	decltype(auto) operator/= (const T n);
	decltype(auto) operator%= (const T n);

	constexpr bool operator== (const T n) const noexcept;
	constexpr bool operator!= (const T n) const noexcept;

	constexpr bool operator< (const T n) const noexcept;
	constexpr bool operator> (const T n) const noexcept;
	constexpr bool operator<= (const T n) const noexcept;
	constexpr bool operator>= (const T n) const noexcept;

public: //+ 関数

	template<class _Ty>
	[[nodiscard]] constexpr decltype(auto) ToChangeType() const;  // 型変換関数
	void Swap(V2<>& _Right);  // 中身交換関数
	void Clear();  // 中身の初期化
	[[nodiscard]] decltype(auto) Fabsf();  // 絶対値にする
	constexpr bool isEmpty() const;  // 中身が０かどうか判断する
	[[nodiscard]] decltype(auto) SwapXY();  // X・Yの中身を交換する
	[[nodiscard]] constexpr auto ToXMFLOAT2() const
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");

		return (DirectX::XMFLOAT2{ x, y });
	}

private:
	template<class _Ty>  // std::swapを呼び出す
	void _Swap_adl(_Ty& _Left, _Ty& _Right);
	constexpr bool isInclude0(const V2<>& _Right) const;  // 要素が一つでも０かどうか判断する
	template<typename _Ty>
	[[nodiscard]] constexpr bool AdjEqual(const _Ty epsilon_num, const _Ty num) const
	{
		static_assert(std::is_floating_point<_Ty>(), "This type isn't floating point");

		constexpr auto Fabs{ [] (const _Ty num) constexpr {
			if (num > static_cast<T>(0.0)) return num; else return -num;
		} };

		static constexpr auto Epsilon{ std::numeric_limits<_Ty>::epsilon() };
		auto dis{ Fabs(epsilon_num - num) };

		return (dis <= Epsilon);
	}

public:  //++ メンバ変数

	T x, y;
};

//+ 関数
#if 1
// 型変換関数
template<class T> template<class _Ty>
inline constexpr decltype(auto) Vector2<T>::ToChangeType() const
{
	using std::forward;

	// 違う型でなければならない
	static_assert(!std::is_same<T, _Ty>::value, "This is same type. This have to be different type.");

	return (V2<_Ty> { forward<_Ty>(static_cast<_Ty>(x)), forward<_Ty>(static_cast<_Ty>(y)) });
}

// std::swapを呼び出す
template<class T> template<class _Ty>
inline void Vector2<T>::_Swap_adl(_Ty& _Left, _Ty& _Right)
{
	std::swap<_Ty, _Ty>(_Left, _Right);
}

// 別のVector2と中身を交換（同じ型でする事）
template<class T>
inline void Vector2<T>::Swap(V2<>& _Right)
{
	if (this != &_Right)
	{
		_Swap_adl(x, _Right.x);
		_Swap_adl(y, _Right.y);
	}
}

// 中身の初期化
template<class T>
inline void Vector2<T>::Clear()
{
	using std::forward;

	x = _0<>;
	y = _0<>;
}

// 絶対値にする
template<class T>
inline decltype(auto) Vector2<T>::Fabsf()
{
	using std::forward;

	// 符号ありでなければならない
	static_assert(std::is_signed<T>::value, "This type is unsigned. This have to be signed.");

	x = forward<T>(static_cast<T>(fabs(x)));
	y = forward<T>(static_cast<T>(fabs(y)));

	return (*this);
}

// 中身が０かどうか判断する
template<class T>
inline constexpr bool Vector2<T>::isEmpty() const
{
	if constexpr (std::is_floating_point<T>())
	{
		return AdjEqual(x, _0<>) && AdjEqual(y, _0<>);
	}
	else
	{
		return (x == _0<> && y == _0<>);
	}
}

// XとYの値を交換する
template<class T>
inline decltype(auto) Vector2<T>::SwapXY()
{
	std::swap<T, T>(x, y);

	return (*this);
}

// 要素が一つでも０かどうか判断する
template<class T>
inline constexpr bool Vector2<T>::isInclude0(const V2<>& Right) const
{
	if constexpr (std::is_floating_point<T>())
	{
		return AdjEqual(Right.x * Right.y, _0<>);
	}
	else
	{
		return (Right.x * Right.y == _0<>);
	}
}

#endif

//+ 演算子オーバーロード（Vector2同士）
#if 1
template<class T>
inline constexpr decltype(auto) Vector2<T>::operator+(const V2<> & v) const noexcept
{
	using std::forward;

	return  V2<>{ forward<T>(x + v.x), forward<T>(y + v.y) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator-(const V2<>& v) const noexcept
{
	using std::forward;

	return  V2<>{ forward<T>(x - v.x), forward<T>(y - v.y) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator*(const V2<>& v) const noexcept
{
	using std::forward;

	return  V2<>{ forward<T>(x* v.x), forward<T>(y* v.y) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator/(const V2<>& v) const
{
	assert(!isInclude0(v) && "0で割ろうとしている");

	using std::forward;

	return  V2<>{ forward<T>(x / v.x), forward<T>(y / v.y) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator%(const V2<>& v) const
{
	assert(!isInclude0(v) && "0で割ろうとしている");

	using std::forward;

	if constexpr (std::is_floating_point<T>())
	{
		return  V2<>{ forward<T>(std::fmod<T>(x, v.x)), forward<T>(std::fmod<T>(y, v.y)) };
	}
	else
	{
		return  V2<>{ forward<T>(x% v.x), forward<T>(y% v.y) };
	}
}

template<class T>
inline decltype(auto) Vector2<T>::operator+=(const V2<>& v) noexcept
{
	x += (v.x);
	y += (v.y);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator-=(const V2<>& v) noexcept
{
	x -= (v.x);
	y -= (v.y);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator*=(const V2<>& v) noexcept
{
	x *= (v.x);
	y *= (v.y);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator/=(const V2<>& v)
{
	assert(!isInclude0(v) && "0で割ろうとしている");

	x /= (v.x);
	y /= (v.y);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator%=(const V2<>& v)
{
	assert(!isInclude0(v) && "0で割ろうとしている");

	if constexpr (std::is_floating_point<T>())
	{
		x = std::fmod<T>(x, v.x);
		y = std::fmod<T>(y, v.y);
	}
	else
	{
		x %= (v.x);
		y %= (v.y);
	}

	return (*this);
}

template<class T>
inline constexpr bool Vector2<T>::operator==(const V2<>& v) const noexcept
{
	if constexpr (std::is_floating_point<T>())
	{
		return (AdjEqual(x, v.x) && AdjEqual(y, v.y));
	}
	else
	{
		return (x == v.x) && (y == v.y);
	}
}

template<class T>
inline constexpr bool Vector2<T>::operator!=(const V2<>& v) const noexcept
{
	if constexpr (std::is_floating_point<T>())
	{
		return !(AdjEqual(x, v.x) && AdjEqual(y, v.y));
	}
	else
	{
		return (x != v.x || y != v.y);
	}
}

template<class T>
inline constexpr bool Vector2<T>::operator<(const V2<>& v) const noexcept
{
	return (x < v.x) && (y < v.y);
}

template<class T>
inline constexpr bool Vector2<T>::operator>(const V2<>& v) const noexcept
{
	return (x > v.x) && (y > v.y);
}

template<class T>
inline constexpr bool Vector2<T>::operator<=(const V2<>& v) const noexcept
{
	return (x <= v.x) && (y <= v.y);
}

template<class T>
inline constexpr bool Vector2<T>::operator>=(const V2<>& v) const noexcept
{
	return (x >= v.x) && (y >= v.y);
}

template<class T>
inline auto Vector2<T>::operator++(int)
{
	// 整数型でなければならない
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "この型の最大値・最大値を超えている");

	Vector2<T> temp{ *this };

	x++;
	y++;

	return (temp);
}

template<class T>
inline auto Vector2<T>::operator--(int)
{
	// 整数型でなければならない
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "この型の最大値・最大値を超えている");

	Vector2<T> temp{ *this };

	x--;
	y--;

	return (temp);
}

template<class T>
inline decltype(auto) Vector2<T>::operator++()
{
	// 整数型でなければならない
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "この型の最大値・最大値を超えている");

	x++;
	y++;

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator--()
{
	// 整数型でなければならない
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "この型の最大値・最大値を超えている");

	x--;
	y--;

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator-() const
{
	// 符号ありでなければならない
	static_assert(std::is_signed<T>::value,
		"This type is unsigned. This have to be signed.");

	return  V2<T>(-x, -y);
}

template<class T>
inline decltype(auto) Vector2<T>::operator+() const
{
	return  V2<T>(+x, +y);
}

#endif

//+ 演算子オーバーロード（Vector2と数値）
#if 1
template<class T>
inline constexpr decltype(auto) Vector2<T>::operator+(const T n) const noexcept
{
	using std::forward;

	return V2<> { forward<T>(x + n), forward<T>(y + n) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator-(const T n) const noexcept
{
	using std::forward;

	return V2<> { forward<T>(x - n), forward<T>(y - n) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator*(const T n) const noexcept
{
	using std::forward;

	return V2<> { forward<T>(x* n), forward<T>(y* n) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator/(const T n) const
{
	assert(n != 0 && "0で割ろうとしている");

	using std::forward;

	return V2<> { forward<T>(x / n), forward<T>(y / n) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator%(const T n) const
{
	assert(n != 0 && "0で割ろうとしている");

	using std::forward;

	if constexpr (std::is_floating_point<T>())
	{
		return  V2<>{ forward<T>(std::fmod<T>(x, n)), forward<T>(std::fmod<T>(y, n)) };
	}
	else
	{
		return V2<> { forward<T>(x % n), forward<T>(y % n) };
	}
}

template<class T>
inline decltype(auto) Vector2<T>::operator+=(const T n) noexcept
{
	x += (n);
	y += (n);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator-=(const T n) noexcept
{
	x -= (n);
	y -= (n);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator*=(const T n) noexcept
{
	x *= (n);
	y *= (n);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator/=(const T n)
{
	assert(n != static_cast<T>(0) && "0で割ろうとしている");

	x /= (n);
	y /= (n);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator%=(const T n)
{
	assert(n != 0 && "0で割ろうとしている");

	if constexpr (std::is_floating_point<T>())
	{
		x = std::fmod<T>(x, n);
		y = std::fmod<T>(y, n);
	}
	else
	{
		x %= (n);
		y %= (n);
	}

	return (*this);
}

template<class T>
inline constexpr bool Vector2<T>::operator==(const T n) const noexcept
{
	if constexpr (std::is_floating_point<T>())
	{
		return (AdjEqual(x, n) && AdjEqual(y, n));
	}
	else
	{
		return (x == n) && (y == n);
	}
}

template<class T>
inline constexpr bool Vector2<T>::operator!=(const T n) const noexcept
{
	if constexpr (std::is_floating_point<T>())
	{
		return !(AdjEqual(x, n) && AdjEqual(y, n));
	}
	else
	{
		return (x != n) || (y != n);
	}
}

template<class T>
inline constexpr bool Vector2<T>::operator<(const T n) const noexcept
{
	return (x < n) && (y < n);
}

template<class T>
inline constexpr bool Vector2<T>::operator>(const T n) const noexcept
{
	return (x > n) && (y > n);
}

template<class T>
inline constexpr bool Vector2<T>::operator<=(const T n) const noexcept
{
	return (x <= n) && (y <= n);
}

template<class T>
inline constexpr bool Vector2<T>::operator>=(const T n) const noexcept
{
	return (x >= n) && (y >= n);
}
#endif
#endif

//!++ Vector2 (bool版)
#if true
template<>
class Vector2<bool> final
{
public:  //+ コンストラクタ系

	// デフォルトコンストラクタ
	constexpr explicit Vector2() noexcept
		: x(std::forward<bool>(false)), y(std::forward<bool>(false))
	{ };

	// コンストラクタ（別々指定）
	constexpr explicit Vector2(bool&& x, bool&& y) noexcept
		: x(std::forward<bool>(x)), y(std::forward<bool>(y))
	{ };

	// コンストラクタ（まとめて指定）
	constexpr explicit Vector2(bool num) noexcept
		: x((num)), y((num))
	{ };

	// コピーコンストラクタ
	constexpr Vector2(const Vector2<bool>& v) noexcept
		: x(v.x), y(v.y)
	{ }

	// ムーブコンストラクタ
	constexpr Vector2(Vector2<bool>&& _Right) noexcept
		: x(std::forward<bool>(_Right.x)), y(std::forward<bool>(_Right.y))
	{ }

	// ムーブ代入演算子
	decltype(auto) operator= (Vector2<bool>&& _Right) noexcept
	{
		using std::forward;

		if (this != &_Right)
		{
			Clear();

			x = forward<bool>(_Right.x);
			y = forward<bool>(_Right.y);
		}

		return (*this);
	}

	// コピー代入演算子
	decltype(auto) operator=(const Vector2<bool>& v) noexcept
	{
		x = v.x;
		y = v.y;

		return (*this);
	}

	// 代入演算子
	decltype(auto) operator=(const bool _Right) noexcept
	{
		x = _Right;
		y = _Right;

		return (*this);
	}

	// デストラクタ
	~Vector2() = default;

private:  //+ エイリアステンプレート

	using V2 = Vector2<bool>;

public:  //+ 演算子オーバーロード

	// Vector2同士

	constexpr bool operator== (const V2& v) const noexcept
	{
		return (x == v.x) && (y == v.y);
	}
	constexpr bool operator!= (const V2& v) const noexcept
	{
		return (x != v.x) && (y != v.y);
	}

	// Vector2と数値

	constexpr bool operator== (const bool&& n) const noexcept
	{
		return (x == n) && (y == n);
	}
	constexpr bool operator!= (const bool&& n) const noexcept
	{
		return (x != n) && (y != n);
	}

public: //+ 関数

	void Clear()  // 中身の初期化
	{
		using std::forward;

		x = forward<bool>(false);
		y = forward<bool>(false);
	}

	constexpr bool isEmpty() const // 中身が０かどうか判断する
	{
		return (x == false && y == false);
	}
public:  //++ メンバ変数

	bool x, y;
};
#endif

//!+++ Vector2クラスの補助関数
namespace Vec2sub
{
	//!++ Vector2の簡易作成版
	// 宣言時にautoを組み合わせると楽
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeVector2(_Ty&& content1, _Ty&& content2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty>(forward<_Ty>(content1), forward<_Ty>(content2)));
	}

	//!++ Vector2の簡易作成版
	// 宣言時にautoを組み合わせると楽
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeVector2(const _Ty& content1, const _Ty& content2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty>((content1), (content2)));
	}

	//!++ Vector2の右辺値参照の簡易作成版
	template<class _Ty> inline
		constexpr decltype(auto)
		ForwardAsVector2(_Ty&& content1, _Ty&& content2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty&&>(forward<_Ty>(content1), forward<_Ty>(content2)));
	}

	//!++ Vector2の絶対値を取りたい場合
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeFabsVector2(Vector2<_Ty>& v2) noexcept
	{
		using std::forward;

		// 符号ありでなければならない
		static_assert(std::is_signed<_Ty>::value, "This type is unsigned. This have to be signed.");

		return (Vector2<_Ty>(forward<_Ty>(static_cast<_Ty>(fabs(v2.x))), forward<_Ty>(static_cast<_Ty>(fabs(v2.y)))));
	}

	//!++ Vector2のXY逆転させた数値を取りたい場合
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeSwapXYVector2(Vector2<_Ty>& v2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty>(forward<_Ty>(static_cast<_Ty>(fabs(v2.y))), forward<_Ty>(static_cast<_Ty>(fabs(v2.x)))));
	}

	//!++ 各数値を絶対値にした上でVector2を取りたい場合
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeFabsVector2(_Ty&& content1, _Ty&& content2) noexcept
	{
		using std::forward;

		// 符号ありでなければならない
		static_assert(std::is_signed<_Ty>::value, "This type is unsigned. This have to be signed.");

		return (Vector2<_Ty>(forward<_Ty>(static_cast<_Ty>(fabs(content1))), forward<_Ty>(static_cast<_Ty>(fabs(content2)))));
	}
}
