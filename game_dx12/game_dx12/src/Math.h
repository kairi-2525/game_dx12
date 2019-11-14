//==================================================
//
//!+++ Math.h
//
//==================================================
#pragma once

//---------------------------------------------------------
//!+++  �C���N���[�h
//---------------------------------------------------------
#include	<cmath>
#include	<cassert>
#include	<type_traits>
#include	<algorithm>
#include	<limits>
#include	"Vector2.h"
#include	"Vector3.h"

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
	static inline constexpr int OddEvenJudge(const int number)
	{
		return (number & 1);
	}

	//!++ �l���덷���܂񂾏�œ������ǂ���
	// ������
	// ���� epsilon_num : �덷���܂ސ��l
	// ���� num : epsilon_num�Ɣ�ׂ������l
	// �߂�l��
	// �����̌^�F�͈͓��̐��l
	template<typename _Ty>
	[[nodiscard]] static inline constexpr bool AdjEqual(const _Ty epsilon_num, const _Ty num)
	{
		static_assert(std::is_floating_point<_Ty>(), "This type isn't floating point");

		constexpr auto Fabs{ [](const _Ty num) constexpr {
			if (num > _0<_Ty>) return num; else return -num;
		} };

		static constexpr auto Epsilon{ std::numeric_limits<_Ty>::epsilon() };
		auto dis{ Fabs(epsilon_num - num) };

		return (dis <= Epsilon);
	}

	//!++ �p�x�̐��K���i���W�A���j
	// ������
	// ���� radian : ���K�������p�x�i���W�A���j
	// �߂�l��
	// �����̌^ �F -PI �` +PI
	template<typename _Ty>
	[[nodiscard]] static inline _Ty RadiansNormalize(const _Ty radian)
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
	[[nodiscard]] static inline _Ty DegreesNormalize(const _Ty degrees)
	{
		static_assert(std::is_floating_point<_Ty>::value, "This type isn't floating point.");

		return (Math::ToDegree(RadiansNormalize(Math::ToRadian(degrees))));
	}


	//!++ ��悷��
	// ������
	// factor : ��������l
	// �߂�l��
	// �����̌^�F���l
	template<class _Ty>
	static inline constexpr _Ty Pow2(const _Ty factor)
	{
		return (factor * factor);
	}

	//!++ �p�x���烉�W�A����
	// ������
	// float, double angle  : �p�x
	// �߂�l��
	// float, double�F���W�A��
	template<class _Ty>
	static inline constexpr _Ty ToRadian(const _Ty angle)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(angle * PAI<_Ty> / _180<_Ty>);
	}

	//!++ ���W�A������p�x��
	// ������
	// float radian : ���W�A��
	// �߂�l��
	// float�F�p�x
	template<class _Ty>
	static inline constexpr _Ty ToDegree(const _Ty radian)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(radian * _180<_Ty> / PAI<_Ty>);
	}

	//!++ �����ƒ�ӂ���p�x�����߂�
	// ������
	// width : ���
	// height : �Ε�
	// �߂�l��
	// float�F���W�A��
	template<class _Ty>
	static inline _Ty Atan2(const _Ty width, const _Ty hight)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(std::atan2<_Ty, _Ty>(hight, width));
	}

	//!++ ����W���瑼�̍��W�ւ̊p�x�����߂�
	// ������
	// Vector2 base_pos : ���߂����p�x���ł�����W
	// Vector2 another_pos : ���߂����p�x��ł�����W
	// �߂�l��
	// float�F���W�A��
	template<class _Ty>
	static inline _Ty PosToRadian(const Vector2<_Ty>& base_pos,
		const Vector2<_Ty>& another_pos)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return Atan2<_Ty>(another_pos.x - base_pos.x, another_pos.y - base_pos.y);
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
	static inline _Ty SinCurve(const int amolitude, const int period, const int count, const int base = 0)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(base + period + std::sin(PAI<_Ty> * _2<_Ty> / period * count) * amolitude);
	}

	//!++ ��_�Ԃ̋��������߂�i�񎟌��j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F����
	template<class _Ty>
	static inline constexpr _Ty Distance(const Vector2<_Ty>& pos1, const Vector2<_Ty>& pos2)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(std::pow<_Ty, _Ty>(
			Pow2<_Ty>(pos2.x - pos1.x) + Pow2<_Ty>(pos2.y - pos1.y), _05<_Ty>
			));
	}

	//!++ ��_�Ԃ̋��������߂�i�O�����j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F����
	template<class _Ty>
	static inline constexpr _Ty Distance(const Vector3<_Ty>& pos1, const Vector3<_Ty>& pos2)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(std::pow<_Ty, _Ty>(
			Pow2<_Ty>(pos2.x - pos1.x) + Pow2<_Ty>(pos2.y - pos1.y) + Pow2<_Ty>(pos2.z - pos1.z), _05<_Ty>
			));
	}

	//!++ ��_�Ԃ̋��������߂�i�O�����j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F����
	static inline float Distance(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2)
	{
		return (std::pow<float, float>(
			Pow2<float>(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y) + Pow2<float>(pos2.z - pos1.z), _05<float>
			));
	}

	//!++ ��_�Ԃ̋��������߂�i�ȈՔŁj
	// ���m�ȋ����͋��܂�Ȃ��B�������r���������ȂǑ�̂̋�����OK�Ȏ��Ȃǁi�ȈՓI�ȕ��ADistance�֐��Ɣ�ׂ�ƂƂĂ����邢�j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F�����i�s���m�F�������A��ׂ���x�Ȃ�g����j
	template<class _Ty>
	static inline _Ty EasyDistance(const Vector2<_Ty>& pos1, const Vector2<_Ty>& pos2)
	{
		const auto dis_vec2{ (pos1 - pos2).Fabsf() };

		return (dis_vec2.x + dis_vec2.y);
	}

	//!++ ��_�Ԃ̋��������߂�i�ȈՔŁj
	// ���m�ȋ����͋��܂�Ȃ��B�������r���������ȂǑ�̂̋�����OK�Ȏ��Ȃǁi�ȈՓI�ȕ��ADistance�֐��Ɣ�ׂ�ƂƂĂ����邢�j
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F�����i�s���m�F�������A��ׂ���x�Ȃ�g����j
	static inline float EasyDistance(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2)
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

	//!++ ��_�Ԃ̒��_�����߂�
	// ������
	// Vector2 : ��P���W
	// Vector2 : ��Q���W
	// �߂�l��
	// �����̌^�F��_�Ԃ̒��_
	template<class _Ty>
	static inline constexpr decltype(auto) MiddlePoint(const Vector2<_Ty>& pos1, const Vector2<_Ty>& pos2)
	{
		return ((pos1 + pos2) / _2<_Ty>);
	}

	//!++ X�̑��x
	// ������
	// float speed : ���x
	// float ���W�A�� : �p�x
	// �߂�l��
	// �����̌^�F���xX
	template<class _Ty>
	static _Ty Xspeed(const _Ty speed, const _Ty rad)
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
	static _Ty Yspeed(const _Ty speed, const _Ty rad)
	{
		// �����łȂ���΂Ȃ�Ȃ�
		static_assert(std::is_floating_point<_Ty>::value == true,
			"Have to be deciaml.");

		return static_cast<_Ty>(speed * std::sin(rad));
	}
};