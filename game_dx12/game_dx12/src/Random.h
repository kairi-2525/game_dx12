#pragma once

//--------------------------------
//!++	�C���N���[�h
//--------------------------------
#include <random>
#include <type_traits>
#include <limits>
#include <memory>

//--------------------------------
//!+++	�N���X
//--------------------------------

//++ ���������N���X�i�����Łj
class RndIntMaker final
{
public:
	RndIntMaker(const int max = (std::numeric_limits<int>::max)(),
		const int min = (std::numeric_limits<int>::min)())
		: device(std::make_unique<std::random_device>()), mt{ (*device)() }, random{ min, max }, max{ max },
		min{ min }
	{ }
	~RndIntMaker() = default;
	RndIntMaker(const RndIntMaker&) = delete;
	auto& operator=(const RndIntMaker&) = delete;
	RndIntMaker(RndIntMaker&& _rt) noexcept
	{
		using std::move;

		device = move(_rt.device);
		mt = move(_rt.mt);
		random = move(_rt.random);
	}
	auto& operator=(RndIntMaker&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			device = move(_rt.device);
			mt = move(_rt.mt);
			random = move(_rt.random);
		}
		return (*this);
	}

private:
	std::unique_ptr<std::random_device> device;  // �����_��������
	std::mt19937 mt;  // �����Z���k�E�c�C�X�^������
	std::uniform_int_distribution<> random;  // �͈͎w�藐�������iint�Łj
	int max;  // �����ő�l
	int min;  // �����ŏ��l

public:

	auto GetMinMax() const noexcept { return std::make_pair(max, min); }
	auto& GetEngine() const noexcept { return mt; }

	// �����_���l�擾�i�����Łj
	template<typename _Ty = int>
	_NODISCARD _Ty GetRnd()
	{
		static_assert(std::is_integral<_Ty>::value, "This type isn't integral. It has to be integral.");
		return static_cast<_Ty>(random(mt));
	}
};

//++ ���������N���X�i���������Łj
class RndDoubleMaker final
{
public:
	RndDoubleMaker(const double max = (std::numeric_limits<double>::max)(),
		const double min = (std::numeric_limits<double>::min)())
		: device(std::make_unique<std::random_device>()), mt { (*device)() }, random{ min, max }, max { max },
		min { min }
	{ }
	~RndDoubleMaker() = default;
	RndDoubleMaker(const RndDoubleMaker&) = delete;
	auto& operator=(const RndDoubleMaker&) = delete;
	RndDoubleMaker(RndDoubleMaker&& _rt) noexcept
	{
		using std::move;

		device = move(_rt.device);
		mt = move(_rt.mt);
		random = move(_rt.random);
	}
	auto& operator=(RndDoubleMaker&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			device = move(_rt.device);
			mt = move(_rt.mt);
			random = move(_rt.random);
		}
		return (*this);
	}

private:
	std::unique_ptr<std::random_device> device;  // �����_��������
	std::mt19937 mt;  // �����Z���k�E�c�C�X�^������
	std::uniform_real_distribution<> random; // �͈͎w�藐�������idouble�Łj
	double max;  // �����ő�l
	double min;  // �����ŏ��l

public:

	auto GetMinMax() const noexcept { return std::make_pair(max, min); }
	auto& GetEngine() const noexcept { return mt; }

	// �����_���l�擾�i���������_�Łj
	template<typename _Ty = double>
	_NODISCARD _Ty GetRnd()
	{
		static_assert(std::is_floating_point<_Ty>::value, "This type isn't floating_point. It has to be floating_point.");
		return static_cast<_Ty>(random(mt));
	}
};