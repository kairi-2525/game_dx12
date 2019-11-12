#pragma once

//--------------------------------
//!++	�C���N���[�h
//--------------------------------
#include <random>
#include <type_traits>
#include <limits>

//--------------------------------
//!+++	�N���X
//--------------------------------

//++ ���������N���X�i�����Łj
class RndIntMaker final
{
public:  // �R���X�g���N�^�E�f�X�g���N�^
	RndIntMaker(const int max = (std::numeric_limits<int>::max)(),
		const int min = (std::numeric_limits<int>::min)()) :
		mt { device() }, random{ min, max }, max { max }, min { min }
	{ }
	~RndIntMaker() = default;
	RndIntMaker(const RndIntMaker&) = delete;
	RndIntMaker(RndIntMaker&&) noexcept = delete;
	auto operator=(const RndIntMaker&) = delete;
	auto operator=(RndIntMaker&&) noexcept = delete;

private:  // �ϐ�
	std::random_device device;  // �����_��������
	std::mt19937 mt;  // �����Z���k�E�c�C�X�^������
	std::uniform_int_distribution<> random;  // �͈͎w�藐�������iint�Łj
public:
	const int max;  // �����ő�l
	const int min;  // �����ŏ��l

public:  // �֐�

	// �����_���l�擾�i�����Łj
	template<typename _Ty = int>
	[[nodiscard]] _Ty GetRnd()
	{
		static_assert(std::is_integral<_Ty>::value, "This type isn't integral. It has to be integral.");
		return static_cast<_Ty>(random(mt));
	}
};

//++ ���������N���X�i���������Łj
class RndDoubleMaker final
{
public:  // �R���X�g���N�^�E�f�X�g���N�^
	RndDoubleMaker(const double max = (std::numeric_limits<double>::max)(),
		const double min = (std::numeric_limits<double>::min)()) :
		mt { device() }, random{ min, max }, max { max }, min { min }
	{ }
	~RndDoubleMaker() = default;
	RndDoubleMaker(const RndDoubleMaker&) = delete;
	RndDoubleMaker(RndDoubleMaker&&) noexcept = delete;
	auto operator=(const RndDoubleMaker&) = delete;
	auto operator=(RndDoubleMaker&&) noexcept = delete;

private:  // �ϐ�
	std::random_device device;  // �����_��������
	std::mt19937 mt;  // �����Z���k�E�c�C�X�^������
	std::uniform_real_distribution<> random; // �͈͎w�藐�������idouble�Łj
public:
	const double max;  // �����ő�l
	const double min;  // �����ŏ��l

public:  // �֐�

	// �����_���l�擾�i���������_�Łj
	template<typename _Ty = double>
	[[nodiscard]] _Ty GetRnd()
	{
		static_assert(std::is_floating_point<_Ty>::value, "This type isn't floating_point. It has to be floating_point.");
		return static_cast<_Ty>(random(mt));
	}
};