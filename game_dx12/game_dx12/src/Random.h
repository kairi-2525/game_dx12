#pragma once

//--------------------------------
//!++	インクルード
//--------------------------------
#include <random>
#include <type_traits>
#include <limits>
#include <memory>

//--------------------------------
//!+++	クラス
//--------------------------------

//++ 乱数生成クラス（整数版）
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
	std::unique_ptr<std::random_device> device;  // ランダム生成器
	std::mt19937 mt;  // メルセンヌ・ツイスタ生成器
	std::uniform_int_distribution<> random;  // 範囲指定乱数生成（int版）
	int max;  // 乱数最大値
	int min;  // 乱数最小値

public:

	auto GetMinMax() const noexcept { return std::make_pair(max, min); }
	auto& GetEngine() const noexcept { return mt; }

	// ランダム値取得（整数版）
	template<typename _Ty = int>
	_NODISCARD _Ty GetRnd()
	{
		static_assert(std::is_integral<_Ty>::value, "This type isn't integral. It has to be integral.");
		return static_cast<_Ty>(random(mt));
	}
};

//++ 乱数生成クラス（浮動小数版）
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
	std::unique_ptr<std::random_device> device;  // ランダム生成器
	std::mt19937 mt;  // メルセンヌ・ツイスタ生成器
	std::uniform_real_distribution<> random; // 範囲指定乱数生成（double版）
	double max;  // 乱数最大値
	double min;  // 乱数最小値

public:

	auto GetMinMax() const noexcept { return std::make_pair(max, min); }
	auto& GetEngine() const noexcept { return mt; }

	// ランダム値取得（浮動小数点版）
	template<typename _Ty = double>
	_NODISCARD _Ty GetRnd()
	{
		static_assert(std::is_floating_point<_Ty>::value, "This type isn't floating_point. It has to be floating_point.");
		return static_cast<_Ty>(random(mt));
	}
};