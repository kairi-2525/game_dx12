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

//!++ Vector2 (���ʔ�)
#if 1
template<class T>
class Vector2 final
{
private:
	template <class Type = T>
	static constexpr Type _0{ static_cast<Type>(0.0) }; // �֐����Ŏg�p

public:  //+ �R���X�g���N�^�n

	// �f�t�H���g�R���X�g���N�^
	template<class _Ty = T>
	constexpr Vector2() noexcept
		: x(std::forward<_Ty>(static_cast<_Ty>(0))), y(std::forward<_Ty>(static_cast<_Ty>(0))) {};

	// �R���X�g���N�^�i�ʁX�w��j
	template<class _Ty = T>
	constexpr Vector2(_Ty && x, _Ty && y) noexcept
		: x(std::forward<_Ty>(x)), y(std::forward<_Ty>(y)) { }

	// �R���X�g���N�^�i�ʁX�w��j
	template<class _Ty = T>
	constexpr Vector2(const _Ty & x, const _Ty & y) noexcept
		: x(x), y(y) { }

	// �R���X�g���N�^�i�܂Ƃ߂Ďw��j
	template<class _Ty = T>
	constexpr Vector2(const _Ty num) noexcept
		: x(num), y(num) {  }

	//. XMFLOAT2�p�R���X�g���N�^
	constexpr Vector2(const DirectX::XMFLOAT2& xmfloat2) noexcept
		: x(xmfloat2.x), y(xmfloat2.y)
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");
	}

	// �R�s�[�R���X�g���N�^
	template<class _Ty = T>
	constexpr Vector2(const Vector2<_Ty> & v) noexcept
		: x(v.x), y(v.y) { }

	// ���[�u�R���X�g���N�^
	template<class _Ty = T>
	constexpr Vector2(Vector2<_Ty> && _Right) noexcept
		: x(std::forward<_Ty>(_Right.x)), y(std::forward<_Ty>(_Right.y)) { }

	//. XMFLOAT2�p�R�s�[������Z�q
	decltype(auto) operator= (const DirectX::XMFLOAT2& xmfloat2)
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");

		x = xmfloat2.x;
		y = xmfloat2.y;
	}

	// ���[�u������Z�q
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

	//. XMFLOAT2�p���[�u������Z�q
	decltype(auto) operator= (DirectX::XMFLOAT2&& xmfloat2) noexcept
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");

		x = std::forward<float>(xmfloat2.x);
		y = std::forward<float>(xmfloat2.y);
	}

	// �C�j�V�����C�U�[���X�g������Z�q
	template<class _Ty = T>
	decltype(auto) operator= (const std::initializer_list<T> & list)
	{
		if (list.size() != 2)
			throw std::invalid_argument("�s���ȃC�j�V�����C�U�[���X�g");

		auto itr{ list.begin() };

		x = *itr;
		itr++;
		y = *itr;
	}

	// �R�s�[������Z�q
	template<class _Ty = T>
	decltype(auto) operator=(const Vector2<_Ty> & v) noexcept
	{
		x = v.x;
		y = v.y;

		return (*this);
	}

	// ������Z�q�i�܂Ƃ߂Ďw��j
	template<class _Ty = T>
	decltype(auto) operator=(const _Ty & _Right) noexcept
	{
		x = (_Right);
		y = (_Right);

		return (*this);
	}

	// �f�X�g���N�^
	~Vector2() noexcept = default;

public:
	static constexpr auto NumMaxLimit{ (std::numeric_limits<std::remove_reference<T>>::max)() };
	static constexpr auto NumMinLimit{ (std::numeric_limits<std::remove_reference<T>>::lowest)() };

private:  //+ �G�C���A�X�e���v���[�g

	template<class _Ty = T>
	using V2 = Vector2<_Ty>;

public:  //+ ���Z�q�I�[�o�[���[�h

	// Vector2���m

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

	// Vector2�Ɛ��l

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

public: //+ �֐�

	template<class _Ty>
	[[nodiscard]] constexpr decltype(auto) ToChangeType() const;  // �^�ϊ��֐�
	void Swap(V2<>& _Right);  // ���g�����֐�
	void Clear();  // ���g�̏�����
	[[nodiscard]] decltype(auto) Fabsf();  // ��Βl�ɂ���
	constexpr bool isEmpty() const;  // ���g���O���ǂ������f����
	[[nodiscard]] decltype(auto) SwapXY();  // X�EY�̒��g����������
	[[nodiscard]] constexpr auto ToXMFLOAT2() const
	{
		static_assert(std::is_floating_point<T>::value, "This type isn't Float.");

		return (DirectX::XMFLOAT2{ x, y });
	}

private:
	template<class _Ty>  // std::swap���Ăяo��
	void _Swap_adl(_Ty& _Left, _Ty& _Right);
	constexpr bool isInclude0(const V2<>& _Right) const;  // �v�f����ł��O���ǂ������f����
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

public:  //++ �����o�ϐ�

	T x, y;
};

//+ �֐�
#if 1
// �^�ϊ��֐�
template<class T> template<class _Ty>
inline constexpr decltype(auto) Vector2<T>::ToChangeType() const
{
	using std::forward;

	// �Ⴄ�^�łȂ���΂Ȃ�Ȃ�
	static_assert(!std::is_same<T, _Ty>::value, "This is same type. This have to be different type.");

	return (V2<_Ty> { forward<_Ty>(static_cast<_Ty>(x)), forward<_Ty>(static_cast<_Ty>(y)) });
}

// std::swap���Ăяo��
template<class T> template<class _Ty>
inline void Vector2<T>::_Swap_adl(_Ty& _Left, _Ty& _Right)
{
	std::swap<_Ty, _Ty>(_Left, _Right);
}

// �ʂ�Vector2�ƒ��g�������i�����^�ł��鎖�j
template<class T>
inline void Vector2<T>::Swap(V2<>& _Right)
{
	if (this != &_Right)
	{
		_Swap_adl(x, _Right.x);
		_Swap_adl(y, _Right.y);
	}
}

// ���g�̏�����
template<class T>
inline void Vector2<T>::Clear()
{
	using std::forward;

	x = _0<>;
	y = _0<>;
}

// ��Βl�ɂ���
template<class T>
inline decltype(auto) Vector2<T>::Fabsf()
{
	using std::forward;

	// ��������łȂ���΂Ȃ�Ȃ�
	static_assert(std::is_signed<T>::value, "This type is unsigned. This have to be signed.");

	x = forward<T>(static_cast<T>(fabs(x)));
	y = forward<T>(static_cast<T>(fabs(y)));

	return (*this);
}

// ���g���O���ǂ������f����
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

// X��Y�̒l����������
template<class T>
inline decltype(auto) Vector2<T>::SwapXY()
{
	std::swap<T, T>(x, y);

	return (*this);
}

// �v�f����ł��O���ǂ������f����
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

//+ ���Z�q�I�[�o�[���[�h�iVector2���m�j
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
	assert(!isInclude0(v) && "0�Ŋ��낤�Ƃ��Ă���");

	using std::forward;

	return  V2<>{ forward<T>(x / v.x), forward<T>(y / v.y) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator%(const V2<>& v) const
{
	assert(!isInclude0(v) && "0�Ŋ��낤�Ƃ��Ă���");

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
	assert(!isInclude0(v) && "0�Ŋ��낤�Ƃ��Ă���");

	x /= (v.x);
	y /= (v.y);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator%=(const V2<>& v)
{
	assert(!isInclude0(v) && "0�Ŋ��낤�Ƃ��Ă���");

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
	// �����^�łȂ���΂Ȃ�Ȃ�
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "���̌^�̍ő�l�E�ő�l�𒴂��Ă���");

	Vector2<T> temp{ *this };

	x++;
	y++;

	return (temp);
}

template<class T>
inline auto Vector2<T>::operator--(int)
{
	// �����^�łȂ���΂Ȃ�Ȃ�
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "���̌^�̍ő�l�E�ő�l�𒴂��Ă���");

	Vector2<T> temp{ *this };

	x--;
	y--;

	return (temp);
}

template<class T>
inline decltype(auto) Vector2<T>::operator++()
{
	// �����^�łȂ���΂Ȃ�Ȃ�
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "���̌^�̍ő�l�E�ő�l�𒴂��Ă���");

	x++;
	y++;

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator--()
{
	// �����^�łȂ���΂Ȃ�Ȃ�
	static_assert(std::is_integral<T>::value, "This type isn't integral. This have to be integral");

	assert(x != NumMaxLimit || y != NumMaxLimit && "���̌^�̍ő�l�E�ő�l�𒴂��Ă���");

	x--;
	y--;

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator-() const
{
	// ��������łȂ���΂Ȃ�Ȃ�
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

//+ ���Z�q�I�[�o�[���[�h�iVector2�Ɛ��l�j
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
	assert(n != 0 && "0�Ŋ��낤�Ƃ��Ă���");

	using std::forward;

	return V2<> { forward<T>(x / n), forward<T>(y / n) };
}

template<class T>
inline constexpr decltype(auto) Vector2<T>::operator%(const T n) const
{
	assert(n != 0 && "0�Ŋ��낤�Ƃ��Ă���");

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
	assert(n != static_cast<T>(0) && "0�Ŋ��낤�Ƃ��Ă���");

	x /= (n);
	y /= (n);

	return (*this);
}

template<class T>
inline decltype(auto) Vector2<T>::operator%=(const T n)
{
	assert(n != 0 && "0�Ŋ��낤�Ƃ��Ă���");

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

//!++ Vector2 (bool��)
#if true
template<>
class Vector2<bool> final
{
public:  //+ �R���X�g���N�^�n

	// �f�t�H���g�R���X�g���N�^
	constexpr explicit Vector2() noexcept
		: x(std::forward<bool>(false)), y(std::forward<bool>(false))
	{ };

	// �R���X�g���N�^�i�ʁX�w��j
	constexpr explicit Vector2(bool&& x, bool&& y) noexcept
		: x(std::forward<bool>(x)), y(std::forward<bool>(y))
	{ };

	// �R���X�g���N�^�i�܂Ƃ߂Ďw��j
	constexpr explicit Vector2(bool num) noexcept
		: x((num)), y((num))
	{ };

	// �R�s�[�R���X�g���N�^
	constexpr Vector2(const Vector2<bool>& v) noexcept
		: x(v.x), y(v.y)
	{ }

	// ���[�u�R���X�g���N�^
	constexpr Vector2(Vector2<bool>&& _Right) noexcept
		: x(std::forward<bool>(_Right.x)), y(std::forward<bool>(_Right.y))
	{ }

	// ���[�u������Z�q
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

	// �R�s�[������Z�q
	decltype(auto) operator=(const Vector2<bool>& v) noexcept
	{
		x = v.x;
		y = v.y;

		return (*this);
	}

	// ������Z�q
	decltype(auto) operator=(const bool _Right) noexcept
	{
		x = _Right;
		y = _Right;

		return (*this);
	}

	// �f�X�g���N�^
	~Vector2() = default;

private:  //+ �G�C���A�X�e���v���[�g

	using V2 = Vector2<bool>;

public:  //+ ���Z�q�I�[�o�[���[�h

	// Vector2���m

	constexpr bool operator== (const V2& v) const noexcept
	{
		return (x == v.x) && (y == v.y);
	}
	constexpr bool operator!= (const V2& v) const noexcept
	{
		return (x != v.x) && (y != v.y);
	}

	// Vector2�Ɛ��l

	constexpr bool operator== (const bool&& n) const noexcept
	{
		return (x == n) && (y == n);
	}
	constexpr bool operator!= (const bool&& n) const noexcept
	{
		return (x != n) && (y != n);
	}

public: //+ �֐�

	void Clear()  // ���g�̏�����
	{
		using std::forward;

		x = forward<bool>(false);
		y = forward<bool>(false);
	}

	constexpr bool isEmpty() const // ���g���O���ǂ������f����
	{
		return (x == false && y == false);
	}
public:  //++ �����o�ϐ�

	bool x, y;
};
#endif

//!+++ Vector2�N���X�̕⏕�֐�
namespace Vec2sub
{
	//!++ Vector2�̊ȈՍ쐬��
	// �錾����auto��g�ݍ��킹��Ɗy
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeVector2(_Ty&& content1, _Ty&& content2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty>(forward<_Ty>(content1), forward<_Ty>(content2)));
	}

	//!++ Vector2�̊ȈՍ쐬��
	// �錾����auto��g�ݍ��킹��Ɗy
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeVector2(const _Ty& content1, const _Ty& content2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty>((content1), (content2)));
	}

	//!++ Vector2�̉E�Ӓl�Q�Ƃ̊ȈՍ쐬��
	template<class _Ty> inline
		constexpr decltype(auto)
		ForwardAsVector2(_Ty&& content1, _Ty&& content2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty&&>(forward<_Ty>(content1), forward<_Ty>(content2)));
	}

	//!++ Vector2�̐�Βl����肽���ꍇ
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeFabsVector2(Vector2<_Ty>& v2) noexcept
	{
		using std::forward;

		// ��������łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_signed<_Ty>::value, "This type is unsigned. This have to be signed.");

		return (Vector2<_Ty>(forward<_Ty>(static_cast<_Ty>(fabs(v2.x))), forward<_Ty>(static_cast<_Ty>(fabs(v2.y)))));
	}

	//!++ Vector2��XY�t�]���������l����肽���ꍇ
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeSwapXYVector2(Vector2<_Ty>& v2) noexcept
	{
		using std::forward;

		return (Vector2<_Ty>(forward<_Ty>(static_cast<_Ty>(fabs(v2.y))), forward<_Ty>(static_cast<_Ty>(fabs(v2.x)))));
	}

	//!++ �e���l���Βl�ɂ������Vector2����肽���ꍇ
	template<class _Ty> inline
		constexpr decltype(auto)
		MakeFabsVector2(_Ty&& content1, _Ty&& content2) noexcept
	{
		using std::forward;

		// ��������łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_signed<_Ty>::value, "This type is unsigned. This have to be signed.");

		return (Vector2<_Ty>(forward<_Ty>(static_cast<_Ty>(fabs(content1))), forward<_Ty>(static_cast<_Ty>(fabs(content2)))));
	}
}
