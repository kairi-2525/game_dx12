#pragma once
//---------------------------------------------------------
//
//!+++　　　 Time.h
//
//---------------------------------------------------------

//-------------------------------------------
//!+++ インクルード
//-------------------------------------------
#include <chrono>
#include <utility>
#include <cassert>

//-------------------------------------------
//!+++ クラス
//-------------------------------------------

class Timer
{
private:  //+ using
	using sys_clock = std::chrono::system_clock;

public:  //+ コンストラクタ・デストラクタ
	Timer() : check(false){ };
	~Timer() = default;

	Timer(const Timer&) = delete;
	auto& operator=(const Timer&) = delete;

	Timer(Timer&& _Right) noexcept
		: tim(std::move(_Right.tim)), check(_Right.check)
	{ }
	auto& operator=(Timer&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			tim = move(_rt.tim);
			check = move(_rt.check);
		}
		return (*this);
	}

private:  //+ 変数
	std::pair<sys_clock::time_point, sys_clock::time_point> tim; // 開始時間・終了時間
	bool check;  // 計測の有無

public:  //+ 関数
	void Start()  // 計測開始
	{
		assert(!check && "時間計測終了出来ていない");

		tim.first = sys_clock::now();
		check = true;
	}
	void ReStart()  // 再計測
	{
		assert(check && "時間計測を始めていない");

		tim.first = sys_clock::now();
		check = true;
	}
	[[nodiscard]] auto NowNanoTime()  // 現在のナノ秒
	{
		assert(check && "時間計測を始めていない");

		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] auto NowMicroTime()  // 現在のマイクロ秒
	{
		assert(check && "時間計測を始めていない");

		return std::chrono::duration_cast<std::chrono::microseconds>(
			sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] auto NowMilliTime()  // 現在のミリ秒
	{
		assert(check && "時間計測を始めていない");

		return std::chrono::duration_cast<std::chrono::milliseconds>(sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] auto NowSecTime()  // 現在の秒
	{
		assert(check && "時間計測を始めていない");

		return std::chrono::duration_cast<std::chrono::seconds>(
			sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] void End()  // 計測終了
	{
		assert(check && "時間計測を始めていない");

		tim.second = sys_clock::now();
		check = false;
	}

public:  //+ ゲッター
	[[nodiscard]] auto GetNanoTimer() const  // ナノ秒取得
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			tim.second - tim.first).count();
	}

	[[nodiscard]] auto GetMicroTimer() const  // マイクロ秒取得
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(
			tim.second - tim.first).count();
	}

	[[nodiscard]] auto GetMilliTimer() const  // ミリ秒取得
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			tim.second - tim.first).count();
	}

	[[nodiscard]] auto GetSecTimer() const  // 秒取得
	{
		return std::chrono::duration_cast<std::chrono::seconds>(
			tim.second - tim.first).count();
	}
};

namespace Time
{
	// 秒をフレームに計算（double版）
	[[nodiscard]] inline constexpr time_t ToFrameSec(const double min)
	{
		return static_cast<time_t>(60.0 * min);
	}

	// 秒をフレームに計算（unsigned int版）
	[[nodiscard]] inline constexpr time_t ToFrameSec(const uint32_t min)
	{
		return static_cast<time_t>(60ull * min);
	}

	// フレームを秒に計算（unsigned int版）
	[[nodiscard]] inline constexpr double ToSecFrame(const uint32_t min)
	{
		return (60.0 / static_cast<double>(min));
	}
}