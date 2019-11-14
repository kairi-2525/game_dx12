#pragma once

#include <deque>
#include "RootBase.h"

class WayPoint;

class MoveAI final
	: public RootBase
{
public:
	MoveAI(const bool executable, const uint64_t priority_number, size_t& way_point_count,
		std::deque<WayPoint>* way_points, const uint16_t kind_node)
		: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
		is_first(true), way_point_count(way_point_count), way_points(way_points)
	{}
	~MoveAI() noexcept = default;

	MoveAI(const MoveAI&) = delete;
	auto& operator=(const MoveAI&) = delete;

	MoveAI(MoveAI && _rt) noexcept
		: RootBase(std::move(_rt)), is_first(_rt.is_first),
		way_point_count(_rt.way_point_count), way_points(_rt.way_points)
	{ }
	auto& operator=(MoveAI && _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			is_first = _rt.is_first;
			way_point_count = _rt.way_point_count;
			way_points = _rt.way_points;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 0u };

public:
	void Update(VF3& enemy_pos, float elapsed_time, Node * node = nullptr) override;

public:
	static inline float move_speed{ 1.f };

private:
	bool is_first;
	size_t& way_point_count;
	std::deque<WayPoint>* way_points;
};

//-----------------------------------------------------------------------------------------------------------

class StopAI final
	: public RootBase
{
public:
	StopAI(const bool executable, const uint64_t priority_number, const uint16_t kind_node)
		: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
		timer(0.f)
	{ }
	~StopAI() noexcept = default;

	StopAI(const StopAI&) = delete;
	auto& operator=(const StopAI&) = delete;

	StopAI(StopAI && _rt) noexcept
		: RootBase(std::move(_rt)), timer(_rt.timer)
	{ }
	auto& operator=(StopAI && _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			timer = _rt.timer;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 1u };

public:
	void Update(VF3& enemy_pos, float elapsed_time, Node * node = nullptr) override;

public:
	static inline float stop_time{ 2.f };

private:
	float timer;
};

//-----------------------------------------------------------------------------------------------------------

class FindAI final
	: public RootBase
{
public:
	FindAI(const bool executable, const uint64_t priority_number, const uint16_t kind_node)
		: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
		timer(0.f)
	{ }
	~FindAI() noexcept = default;

	FindAI(const FindAI&) = delete;
	auto& operator=(const FindAI&) = delete;

	FindAI(FindAI && _rt) noexcept
		: RootBase(std::move(_rt)), timer(_rt.timer)
	{ }
	auto& operator=(FindAI && _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			timer = _rt.timer;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 2u };

public:
	void Update(VF3& enemy_pos, float elapsed_time, Node * node = nullptr) override;

public:
	static inline float stop_time{ 1.f };

private:
	float timer;
};

//-----------------------------------------------------------------------------------------------------------

class PatrolAI final
	: public RootBase
{
private:
	using AI_Control = Variant<Optional<MoveAI>, Optional<StopAI>, Optional<FindAI>>;

public:
	PatrolAI(const bool executable, const uint64_t priority_number, size_t& way_point_count,
		std::deque<WayPoint>* way_points, const uint16_t kind_node, const float* enemy_angle);
	~PatrolAI() noexcept = default;

	PatrolAI(const PatrolAI&) = delete;
	auto& operator=(const PatrolAI&) = delete;

	PatrolAI(PatrolAI && _rt) noexcept
		: RootBase(std::move(_rt)), change_mode(_rt.change_mode), patroling_mode(_rt.patroling_mode),
		patrol_modes(std::move(_rt.patrol_modes)), way_point_count(_rt.way_point_count),
		way_points(_rt.way_points), end_number(_rt.end_number), enemy_angle_y(_rt.enemy_angle_y)
	{ }
	auto& operator=(PatrolAI && _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			RootBase::operator=(move(_rt));

			change_mode = _rt.change_mode;
			patroling_mode = _rt.patroling_mode;
			patrol_modes = move(_rt.patrol_modes);
			way_point_count = _rt.way_point_count;
			way_points = _rt.way_points;
			end_number = _rt.end_number;
			enemy_angle_y = _rt.enemy_angle_y;
		}

		return (*this);
	}

public:
	void Update(VF3& enemy_pos, float elapsed_time, Node * node = nullptr) override;

private:
	void InitModeData();
	bool IsPlayerRayHit(const VF3 & enemy_pos, const VF3 & player_pos);
#if false
	template<class _Ty, size_t _Idx, typename Type = std::remove_reference<_Ty>::type,
		class... _ArgTypes>
		void BuildAINode(std::vector<std::pair<bool, uint64_t>> & init_data, const _ArgTypes &... args)
	{
		static_assert(std::is_base_of<Type, RootBase>(), "Not Base Class");

		const auto& data{ init_data.at(_Idx) };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<_Idx>(std::make_optional<Type>(data.first, data.second, args...));
}
#endif

private:
	std::deque<AI_Control> patrol_modes;
	AI_Control* patroling_mode;

	std::deque<WayPoint>* way_points;
	const float* enemy_angle_y;
	bool change_mode;
	uint16_t end_number;
	size_t& way_point_count;

public:
	static inline float hit_distance{ 5.f };
	static inline float hit_angle{ 3.14f / 4.f };
};
