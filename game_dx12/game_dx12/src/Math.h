//==================================================
//
//!+++ Math.h
//
//==================================================
#pragma once

#if !_HAS_CXX17
#error "�u-std=c++17�v���R�}���h�I�v�V�����Ɏw�肵�Ă��������B"
#endif

//---------------------------------------------------------
//!+++  �C���N���[�h
//---------------------------------------------------------
#include	<cmath>
#include	<cassert>
#include	<type_traits>
#include	<algorithm>
#include	<limits>
#include	<DirectXMath.h>

#ifndef HAS_VECTOR2
#define HAS_VECTOR2 __has_include("Vector2.h")
#endif
#ifndef HAS_VECTOR3
#define HAS_VECTOR3 __has_include("Vector3.h")
#endif

#if HAS_VECTOR3
#include "Vector3.h"
#endif

#if HAS_VECTOR2
#include "Vector2.h"
#endif

//---------------------------------------------------------
//!+++  �֐�
//---------------------------------------------------------

//!+++ ���w�֌W
namespace Math
{
	template <class _Ty = double>
	static constexpr _Ty _180{ static_cast<_Ty>(180.0) };  // �֐����Ŏg�p
	template <class _Ty = double>
	static constexpr _Ty _2{ static_cast<_Ty>(2.0) };  // �֐����Ŏg�p
	template <class _Ty = double>
	static constexpr _Ty _05{ static_cast<_Ty>(0.5) };  // �֐����Ŏg�p
	template <class _Ty = double>
	static constexpr _Ty PAI{ static_cast<_Ty>(3.141592653589793) }; // �~����
	template <class _Ty = double>
	static constexpr _Ty _0{ static_cast<_Ty>(0.0) }; // �֐����Ŏg�p

	//!++ �����E����f
	// ������
	// int factor : ���f�������l
	// �߂�l��
	// �����̌^�F�P�͊�A�O�͋���
	template<class _Ty>
	_NODISCARD static inline constexpr int OddEvenJudge(const _Ty number)
	{
		static_assert(std::is_integral<_Ty>(), "This type isn't integral");

		return (number & 1);
	}

	//!++ ���`���
	template<typename _Ty>
	_NODISCARD static inline constexpr _Ty Leap(const _Ty num1, const _Ty num2, const _Ty time)
	{
		return num1 + time * (num2 - num1);
	}

	//!++ �l���덷���܂񂾏�œ������ǂ���
	// ������
	// ���� epsilon_num : �덷���܂ސ��l
	// ���� num : epsilon_num�Ɣ�ׂ������l
	// �߂�l��
	// �����̌^�F�͈͓��̐��l
	template<typename _Ty>
	_NODISCARD static inline constexpr bool AdjEqual(const _Ty epsilon_num, const _Ty num)
	{
		static_assert(std::is_floating_point<_Ty>(), "This type isn't floating point");

		constexpr auto Fabs{ [](const _Ty num) constexpr {
			if (num > _0<_Ty>) return num; else return -num;
		} };

		static constexpr auto Epsilon{ std::numeric_limits<_Ty>::epsilon() };
		auto dis{ Fabs(epsilon_num - num) };

		return (dis <= Epsilon);
	}

	//!++ �l���덷���܂񂾏�œ������ǂ���
	// ������
	// ���� epsilon_num : �덷���܂ސ��l
	// ���� num : epsilon_num�Ɣ�ׂ������l
	// ���� pow_num : ���ۂ̌덷�̉��{�܂ŋ��e���邩(1.19209e-07)
	// �߂�l��
	// �����̌^�F�͈͓��̐��l
	template<typename _Ty>
	_NODISCARD static inline constexpr bool NearyEqual(const _Ty epsilon_num, const _Ty num, const _Ty pow_num)
	{
		static_assert(std::is_floating_point<_Ty>(), "This type isn't floating point");

		constexpr auto Fabs{ [](const _Ty num) constexpr {
			if (num > _0<_Ty>) return num; else return -num;
		} };

		const auto Epsilon{ std::numeric_limits<_Ty>::epsilon() * pow_num };
		auto dis{ Fabs(epsilon_num - num) };

		return (dis <= Epsilon);
	}

	//!++ A���l��B���l�̊ԂɌ��؂��������l�����邩�ǂ���
	// ������
	// ���� first_element : A���l
	// ���� second_element : B���l
	// ���� verification_elemet : ���؂��������l
	// �߂�l��
	// �����̌^�FA��B�̊ԂɌ��ؐ��l�����݂��邩�ǂ���
	template<bool IsIncludeEqual = true, typename _Ty = float>
	_NODISCARD static inline constexpr bool IsBetweenNumber(const _Ty first_element, const _Ty second_element,
		const _Ty verification_elemet)
	{
		if constexpr (IsIncludeEqual)
		{
			if constexpr (std::is_floating_point<_Ty>())
			{
				// ���ؒl�Ɠ������ǂ������ׂ�
				if (AdjEqual(verification_elemet, first_element) || AdjEqual(verification_elemet, second_element))
					return true;
			}
			else
			{
				if (verification_elemet == first_element || verification_elemet == second_element)
					return true;
			}
		}

		const _Ty
			min_num{ (std::min)(first_element, second_element) },
			max_num{ (std::max)(first_element, second_element) };

		return (verification_elemet > min_num&& verification_elemet < max_num);
	}

	//!++ �p�x�̐��K���i���W�A���j
	// ������
	// ���� radian : ���K�������p�x�i���W�A���j
	// �߂�l��
	// �����̌^ �F -PI �` +PI
	template<typename _Ty>
	_NODISCARD static inline _Ty RadiansNormalize(const _Ty radian)
	{
		static_assert(std::is_floating_point<_Ty>::value, "This type isn't floating point.");

		return (std::atan2(std::sin(radian), std::cos(radian)));
	}

	//!++ �p�x�̐��K���i�f�O���[�j
	// ������
	// ���� degrees : ���K�������p�x�i�f�O���[�j
	// �߂�l��
	// �����̌^�F
	template<typename _Ty>
	_NODISCARD static inline _Ty DegreesNormalize(const _Ty degrees)
	{
		static_assert(std::is_floating_point<_Ty>::value, "This type isn't floating point.");

		return (ToDegree(RadiansNormalize(ToRadian(degrees))));
	}

	//!++ ��悷��
	// ������
	// factor : ��������l
	// �߂�l��
	// �����̌^�F���l
	template<class _Ty>
	_NODISCARD static inline constexpr _Ty Pow2(const _Ty factor)
	{
		return (factor * factor);
	}

	//!++ �p�x���烉�W�A����
	// ������
	// float, double angle  : �p�x
	// �߂�l��
	// float, double�F���W�A��
	template<class _Ty>
	_NODISCARD static inline constexpr _Ty ToRadian(const _Ty angle)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(angle * PAI<_Ty> / _180<_Ty>);
	}

	//!++ �p�x���烉�W�A����
	// ������
	// float, double angle  : �p�x
	// �߂�l��
	// float, double�F���W�A��
	_NODISCARD static inline DirectX::XMFLOAT3 ToRadian(const DirectX::XMFLOAT3& degree)
	{
		DirectX::XMFLOAT3 rad;

		rad.x = degree.x * PAI<float> / _180<float>;
		rad.y = degree.y * PAI<float> / _180<float>;
		rad.z = degree.z * PAI<float> / _180<float>;

		return rad;
	}

	//!++ ���W�A������p�x��
	// ������
	// float radian : ���W�A��
	// �߂�l��
	// float�F�p�x
	template<class _Ty>
	_NODISCARD static inline constexpr _Ty ToDegree(const _Ty radian)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(radian * _180<_Ty> / PAI<_Ty>);
	}

	//!++ ���W�A������p�x��
	// ������
	// float radian : ���W�A��
	// �߂�l��
	// float�F�p�x
	_NODISCARD static inline DirectX::XMFLOAT3 ToDegree(const DirectX::XMFLOAT3& radian)
	{
		DirectX::XMFLOAT3 deg;

		deg.x = radian.x * PAI<float> / _180<float>;
		deg.y = radian.y * PAI<float> / _180<float>;
		deg.z = radian.z * PAI<float> / _180<float>;

		return deg;
	}

	//!++ �����ƒ�ӂ���p�x�����߂�
	// ������
	// width : ���
	// height : �Ε�
	// �߂�l��
	// float�F���W�A��
	template<class _Ty>
	_NODISCARD static inline _Ty Atan2(const _Ty width, const _Ty hight)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(std::atan2<_Ty, _Ty>(hight, width));
	}

#if HAS_VECTOR2
	//!++ ����W���瑼�̍��W�ւ̊p�x�����߂�
	// ������
	// Vector2 base_pos : ���߂����p�x���ł�����W
	// Vector2 another_pos : ���߂����p�x��ł�����W
	// �߂�l��
	// float�F���W�A��
	template<class _Ty>
	_NODISCARD static inline _Ty PosToRadian(const Vector2<_Ty>& base_pos,
		const Vector2<_Ty>& another_pos)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return Atan2<_Ty>(another_pos.x - base_pos.x, another_pos.y - base_pos.y);
	}
#endif

	//!++ ����W���瑼�̍��W�ւ̊p�x�����߂�
	// ������
	// DirectX::XMFLOAT2 base_pos : ���߂����p�x���ł�����W
	// DirectX::XMFLOAT2 another_pos : ���߂����p�x��ł�����W
	// �߂�l��
	// float�F���W�A��
	_NODISCARD static inline float PosToRadian(const DirectX::XMFLOAT2& base_pos,
		const DirectX::XMFLOAT2& another_pos)
	{
		return Atan2<float>(another_pos.x - base_pos.x, another_pos.y - base_pos.y);
	}

	//!++ �T�C���J�[�u�����߂�
	// ������
	// int amolitude : �U���i�`�̃J�E���g��(1�b60�J�E���g)�j
	// float period : �����i�}�`�͈̔͂�J�ڂ���j
	// int count : �J�E���g
	// int base : ��_�i0����`������_��ς���j(������0)
	// �߂�l��
	// float�F���l
	template<class _Ty>
	_NODISCARD static inline _Ty SinCurve(const int amolitude, const int period, const int count, const int base = 0)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(base + period + std::sin(PAI<_Ty> * _2<_Ty> / period * count) * amolitude);
	}

#if HAS_VECTOR2
	//!++ ��_�Ԃ̋��������߂�i�񎟌��j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F����
	template<class _Ty>
	_NODISCARD static inline _Ty Distance(const Vector2<_Ty>& pos1, const Vector2<_Ty>& pos2)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(std::pow<_Ty, _Ty>(
			Pow2<_Ty>(pos2.x - pos1.x) + Pow2<_Ty>(pos2.y - pos1.y), _05<_Ty>
			));
	}
#endif

	//!++ ��_�Ԃ̋��������߂�i�񎟌��j
	// ������
	// DirectX::XMFLOAT2 : ��P���W
	// DirectX::XMFLOAT2 : ��Q���W
	// �߂�l��
	// �����̌^�F����
	_NODISCARD static inline float Distance(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2)
	{
		return std::pow(Pow2<float>(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y), _05<float>);
	}

#if HAS_VECTOR3
	//!++ ��_�Ԃ̋��������߂�i�O�����j
	// ������
	// Vector3 : ��P���W
	// Vector3 : ��Q���W
	// �߂�l��
	// �����̌^�F����
	template<class _Ty>
	_NODISCARD static inline _Ty Distance(const Vector3<_Ty>& pos1, const Vector3<_Ty>& pos2)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(std::pow<_Ty, _Ty>(
			Pow2<_Ty>(pos2.x - pos1.x) + Pow2<_Ty>(pos2.y - pos1.y) + Pow2<_Ty>(pos2.z - pos1.z), _05<_Ty>
			));
}
#endif

	//!++ ��_�Ԃ̋��������߂�i�O�����j
	// ������
	// DirectX::XMFLOAT3 : ��P���W
	// DirectX::XMFLOAT3 : ��Q���W
	// �߂�l��
	// �����̌^�F����
	_NODISCARD static inline float Distance(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2)
	{
		return (std::pow(Pow2<float>(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y) + Pow2<float>(pos2.z - pos1.z), _05<float>));
	}

	//!++ ��_�Ԃ̋��������߂�i��悳�ꂽ�܂܁j�i�O�����j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F�����̓��
	_NODISCARD static inline float DistanceSq(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2)
	{
		return (Pow2<float>(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y) + Pow2<float>(pos2.z - pos1.z));
	}

	//!++ ��_�Ԃ̋��������߂�i��悳�ꂽ�܂܁j�i2�����j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F�����̓��
	_NODISCARD static inline float DistanceSq(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2)
	{
		return (Pow2<float>(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y));
	}

#if HAS_VECTOR2
	//!++ ��_�Ԃ̋��������߂�i�ȈՔŁj
	// ���m�ȋ����͋��܂�Ȃ��B�������r���������ȂǑ�̂̋�����OK�Ȏ��Ȃǁi�ȈՓI�ȕ��ADistance�֐��Ɣ�ׂ�ƂƂĂ����邢�j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F�����i�s���m�F�������A��ׂ���x�Ȃ�g����j
	template<class _Ty>
	_NODISCARD static inline _Ty EasyDistance(const Vector2<_Ty>& pos1, const Vector2<_Ty>& pos2)
	{
		const auto dis_vec2{ (pos1 - pos2).Fabsf() };

		return (dis_vec2.x + dis_vec2.y);
	}
#endif

	//!++ ��_�Ԃ̋��������߂�i�ȈՔŁj
	// ���m�ȋ����͋��܂�Ȃ��B�������r���������ȂǑ�̂̋�����OK�Ȏ��Ȃǁi�ȈՓI�ȕ��ADistance�֐��Ɣ�ׂ�ƂƂĂ����邢�j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F�����i�s���m�F�������A��ׂ���x�Ȃ�g����j
	_NODISCARD static inline float EasyDistance(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2)
	{
		auto FAbs{ [](const auto& vf3, const auto& vf3b) {
			DirectX::XMFLOAT3 temp;

			temp.x = fabs(vf3.x - vf3b.x);
			temp.y = fabs(vf3.y - vf3b.y);
			temp.z = fabs(vf3.z - vf3b.z);

			return temp;
		} };

		const auto dis_vec3{ FAbs(pos2, pos1) };

		return (dis_vec3.x + dis_vec3.y + dis_vec3.z);
	}

#if HAS_VECTOR2
	//!++ ��_�Ԃ̒��_�����߂�
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F��_�Ԃ̒��_
	template<class _Ty>
	_NODISCARD static inline constexpr decltype(auto) MiddlePoint(const Vector2<_Ty>& pos1, const Vector2<_Ty>& pos2)
	{
		return ((pos1 + pos2) / _2<_Ty>);
	}
#endif

	//!++ ��_�Ԃ̒��_�����߂�
	// ������
	// DirectX::XMFLOAT2 : ��P���W
	// DirectX::XMFLOAT2 : ��Q���W
	// �߂�l��
	// �����̌^�F��_�Ԃ̒��_
	_NODISCARD static inline DirectX::XMFLOAT2 MiddlePoint(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2)
	{
		return DirectX::XMFLOAT2{ ((pos1.x + pos2.x) / 2.f), ((pos1.y + pos2.y) / 2.f) };
	}

	//!++ �O�_�Ԃ̒��_�����߂�
	// ������
	// DirectX::XMFLOAT2 : ��P���W
	// DirectX::XMFLOAT2 : ��Q���W
	// �߂�l��
	// �����̌^�F��_�Ԃ̒��_
	_NODISCARD static inline DirectX::XMFLOAT3 MiddlePoint(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2)
	{
		return 	DirectX::XMFLOAT3{ ((pos1.x + pos2.x) / 2.f), ((pos1.y + pos2.y) / 2.f), ((pos1.x + pos2.x) / 2.f) };
	}

	//!++ X�̑��x
	// ������
	// float speed : ���x
	// float ���W�A�� : �p�x
	// �߂�l��
	// �����̌^�F���xX
	template<class _Ty>
	_NODISCARD static _Ty Xspeed(const _Ty speed, const _Ty rad)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(speed * std::cos(rad));
	}

	//!++ Y�̑��x
	// ������
	// float speed : ���x
	// float ���W�A�� : �p�x
	// �߂�l��
	// �����̌^�F���xY
	template<class _Ty>
	_NODISCARD static _Ty Yspeed(const _Ty speed, const _Ty rad)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(speed * std::sin(rad));
	}
		};