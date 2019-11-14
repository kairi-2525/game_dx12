#pragma once

#include "RootBase.h"

class WayPoint;

class ChaseAI final
	: public RootBase
{
public:
	ChaseAI(const bool executable, const uint64_t priority_number, const uint16_t kind_node)
		: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
		is_first(true), timer(0.f), second_timer(0.f)
	{ }
	~ChaseAI() noexcept = default;

	ChaseAI(const ChaseAI&) = delete;
	auto& operator=(const ChaseAI&) = delete;

	ChaseAI(ChaseAI&& _rt) noexcept
		: RootBase(std::move(_rt)), is_first(_rt.is_first), timer(_rt.timer), second_timer(_rt.second_timer)
	{ }
	auto& operator=(ChaseAI&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			is_first = _rt.is_first;
			timer = _rt.timer;
			second_timer = _rt.second_timer;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 0u };

public:
	void Update(VF3& enemy_pos, float elapsed_time, Node* node = nullptr) override;

public:
	static inline float move_speed{ 1.f };      // à⁄ìÆë¨ìx
	static inline float max_chase_time{ 5.f };  // ç≈ëÂí«ê’éûä‘

private:
	bool is_first;
	float timer;
	float second_timer;
};

//-----------------------------------------------------------------------------------------------------------

class GoBackAI final
	: public RootBase
{
public:
	GoBackAI(const bool executable, const uint64_t priority_number, const uint16_t kind_node,
		std::deque<WayPoint>* way_points)
		: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
		is_first(true), way_points(way_points)
	{ }
	~GoBackAI() noexcept = default;

	GoBackAI(const GoBackAI&) = delete;
	auto& operator=(const GoBackAI&) = delete;

	GoBackAI(GoBackAI&& _rt) noexcept
		: RootBase(std::move(_rt)), is_first(_rt.is_first), way_points(_rt.way_points)
	{ }
	auto& operator=(GoBackAI&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			is_first = _rt.is_first;
			way_points = _rt.way_points;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 1u };

public:
	void Update(VF3& enemy_pos, float elapsed_time, Node* node = nullptr) override;

public:
	static inline float move_speed{ 1.f }; // à⁄ìÆë¨ìx

private:
	bool is_first;
	std::deque<WayPoint>* way_points;
};

//-----------------------------------------------------------------------------------------------------------

class AttackAI final
	: public RootBase
{
private:
	using AI_Control = Variant<Optional<ChaseAI>, Optional<GoBackAI>>;

public:
	AttackAI(const bool executable, const uint64_t priority_number, const uint16_t kind_node,
		std::deque<WayPoint>* way_points);
	~AttackAI() noexcept = default;

	AttackAI(const AttackAI&) = delete;
	auto& operator=(const AttackAI&) = delete;

	AttackAI(AttackAI&& _rt) noexcept
		: RootBase(std::move(_rt)), attack_modes(std::move(_rt.attack_modes)),
		attackking_mode(_rt.attackking_mode), mode_state(std::move(_rt.mode_state)),
		mode_count(_rt.mode_count), change_mode(_rt.change_mode)
	{ }
	auto& operator=(AttackAI&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			attack_modes = move(_rt.attack_modes);
			attackking_mode = _rt.attackking_mode;
			mode_state.swap(_rt.mode_state);
			mode_count = _rt.mode_count;
			change_mode = _rt.change_mode;
		}

		return (*this);
	}

public:
	void Update(VF3& enemy_pos, float elapsed_time, Node* node = nullptr) override;

private:
	Array<AI_Control, 2u> attack_modes;
	AI_Control* attackking_mode;

	std::array<bool, 2u> mode_state;
	bool change_mode;
	int16_t mode_count;
};
