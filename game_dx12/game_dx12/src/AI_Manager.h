#pragma once
#include <variant>
#include <optional>
#include <deque>

#include "PatrolAI.h"
#include "AttackAI.h"


class Enemy;
struct Node;
class WayPoint;

class AI_Manager final
{
private:
	using AI_Control = std::variant<std::optional<PatrolAI>, std::optional<AttackAI>>;

public:
	AI_Manager(std::deque<WayPoint>* way_points);
	~AI_Manager() noexcept = default;
	AI_Manager(const AI_Manager&) = delete;
	auto& operator=(const AI_Manager&) = delete;

	AI_Manager(AI_Manager&& _rt) noexcept
		: way_points(_rt.way_points)
	{
		run_modes.swap(_rt.run_modes);
		change_mode = _rt.change_mode;
		running_mode = _rt.running_mode;
		target_pos = _rt.target_pos;
		way_point_count = _rt.way_point_count;
	}
	auto& operator=(AI_Manager&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			run_modes.swap(_rt.run_modes);
			way_points = _rt.way_points;
			change_mode = _rt.change_mode;
			running_mode = _rt.running_mode;
			target_pos = _rt.target_pos;
			way_point_count = _rt.way_point_count;
		}

		return (*this);
	}

public:
	void Update(Enemy& enemy, Node* node);
private:
	void InitModeData();

private:
	std::array<AI_Control, 2u> run_modes;
	AI_Control* running_mode;
	std::deque<WayPoint>* way_points;
	size_t way_point_count;
	bool change_mode;
	DirectX::XMFLOAT3 target_pos;
};