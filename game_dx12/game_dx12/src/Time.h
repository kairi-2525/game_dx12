#pragma once
//---------------------------------------------------------
//
//!+++�@�@�@ Time.h
//
//---------------------------------------------------------

//-------------------------------------------
//!+++ �C���N���[�h
//-------------------------------------------
#include <chrono>
#include <utility>
#include <cassert>

//-------------------------------------------
//!+++ �N���X
//-------------------------------------------

class Timer
{
private:  //+ using
	using sys_clock = std::chrono::system_clock;

public:  //+ �R���X�g���N�^�E�f�X�g���N�^
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

private:  //+ �ϐ�
	std::pair<sys_clock::time_point, sys_clock::time_point> tim; // �J�n���ԁE�I������
	bool check;  // �v���̗L��

public:  //+ �֐�
	void Start()  // �v���J�n
	{
		assert(!check && "���Ԍv���I���o���Ă��Ȃ�");

		tim.first = sys_clock::now();
		check = true;
	}
	void ReStart()  // �Čv��
	{
		assert(check && "���Ԍv�����n�߂Ă��Ȃ�");

		tim.first = sys_clock::now();
		check = true;
	}
	[[nodiscard]] auto NowNanoTime()  // ���݂̃i�m�b
	{
		assert(check && "���Ԍv�����n�߂Ă��Ȃ�");

		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] auto NowMicroTime()  // ���݂̃}�C�N���b
	{
		assert(check && "���Ԍv�����n�߂Ă��Ȃ�");

		return std::chrono::duration_cast<std::chrono::microseconds>(
			sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] auto NowMilliTime()  // ���݂̃~���b
	{
		assert(check && "���Ԍv�����n�߂Ă��Ȃ�");

		return std::chrono::duration_cast<std::chrono::milliseconds>(sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] auto NowSecTime()  // ���݂̕b
	{
		assert(check && "���Ԍv�����n�߂Ă��Ȃ�");

		return std::chrono::duration_cast<std::chrono::seconds>(
			sys_clock::now() - tim.first).count();
	}

	[[nodiscard]] void End()  // �v���I��
	{
		assert(check && "���Ԍv�����n�߂Ă��Ȃ�");

		tim.second = sys_clock::now();
		check = false;
	}

public:  //+ �Q�b�^�[
	[[nodiscard]] auto GetNanoTimer() const  // �i�m�b�擾
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			tim.second - tim.first).count();
	}

	[[nodiscard]] auto GetMicroTimer() const  // �}�C�N���b�擾
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(
			tim.second - tim.first).count();
	}

	[[nodiscard]] auto GetMilliTimer() const  // �~���b�擾
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			tim.second - tim.first).count();
	}

	[[nodiscard]] auto GetSecTimer() const  // �b�擾
	{
		return std::chrono::duration_cast<std::chrono::seconds>(
			tim.second - tim.first).count();
	}
};

namespace Time
{
	// �b���t���[���Ɍv�Z�idouble�Łj
	[[nodiscard]] inline constexpr time_t ToFrameSec(const double min)
	{
		return static_cast<time_t>(60.0 * min);
	}

	// �b���t���[���Ɍv�Z�iunsigned int�Łj
	[[nodiscard]] inline constexpr time_t ToFrameSec(const uint32_t min)
	{
		return static_cast<time_t>(60ull * min);
	}

	// �t���[����b�Ɍv�Z�iunsigned int�Łj
	[[nodiscard]] inline constexpr double ToSecFrame(const uint32_t min)
	{
		return (60.0 / static_cast<double>(min));
	}
}