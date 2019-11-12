#pragma once

#include "RootBase.h"


class ChaseAI final
	: public RootBase
{
public:
	ChaseAI(const bool executable, const uint64_t priority_number)
		: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number), is_first(true)
	{ }
	~ChaseAI() noexcept = default;

	ChaseAI(const ChaseAI&) = delete;
	auto& operator=(const ChaseAI&) = delete;

	ChaseAI(ChaseAI&& _rt) noexcept
		: RootBase(std::move(_rt))
	{ }
	auto& operator=(ChaseAI&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 0u };

public:
	void Update(VF3& enemy_pos, const float elapsed_time, Node* node = nullptr) override;

public:
	static inline float move_speed{ 1.f };

private:
	bool is_first;
};

//-----------------------------------------------------------------------------------------------------------

class GoBackAI final
	: public RootBase
{
public:
	GoBackAI(const bool executable, const uint64_t priority_number)
		: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number)
	{ }
	~GoBackAI() noexcept = default;

	GoBackAI(const GoBackAI&) = delete;
	auto& operator=(const GoBackAI&) = delete;

	GoBackAI(GoBackAI&& _rt) noexcept
		: RootBase(std::move(_rt))
	{ }
	auto& operator=(GoBackAI&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 1u };

public:
	void Update(VF3& enemy_pos, const float elapsed_time, Node* node = nullptr) override;

private:

};

//-----------------------------------------------------------------------------------------------------------

class AttackAI final
	: public RootBase
{
private:
	using AI_Control = Variant<Optional<ChaseAI>, Optional<GoBackAI>>;

public:
	AttackAI(const bool executable, const uint64_t priority_number);
	~AttackAI() noexcept = default;

	AttackAI(const AttackAI&) = delete;
	auto& operator=(const AttackAI&) = delete;

	AttackAI(AttackAI&& _rt) noexcept
		: RootBase(std::move(_rt)), attack_modes(std::move(_rt.attack_modes)),
		attackking_mode(_rt.attackking_mode), change_mode(std::move(_rt.change_mode)),
		mode_count(_rt.mode_count)
	{ }
	auto& operator=(AttackAI&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			attack_modes = move(_rt.attack_modes);
			attackking_mode = _rt.attackking_mode;
			change_mode.swap(_rt.change_mode);
			mode_count = _rt.mode_count;
		}

		return (*this);
	}

public:
	void Update(VF3& enemy_pos, const float elapsed_time, Node* node = nullptr) override;

private:
	Array<AI_Control, 2u> attack_modes;
	AI_Control* attackking_mode;
	std::array<bool, 2u> change_mode;
	size_t mode_count;
};
