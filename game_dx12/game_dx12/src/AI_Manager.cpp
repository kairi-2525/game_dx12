#include <vector>

#include "Enemy.h"
#include "AI_Manager.h"

AI_Manager::AI_Manager(std::deque<WayPoint>* way_points)
	: change_mode(true), running_mode(nullptr), target_pos(0.f, 0.f, 0.f), way_points(way_points),
	way_point_count(0u)
{
	static const std::vector<std::pair<bool, uint64_t>> init_data
	{
		// 実行可能、優先順位
		{ true	   , 1 },  // 巡回
		{ true	   , 2 },  // 攻撃
	};

	assert(init_data.size() == run_modes.size() && "初期化データの追加し忘れ");

	// 巡回状態構築
	{
		const auto& data{ init_data.front() };

		run_modes.front().emplace<0>(
			std::make_optional<PatrolAI>(data.first, data.second, way_point_count, way_points));
	}

	// 攻撃状態構築
	{
		const auto& data{ init_data.back() };

		run_modes.at(1).emplace<1>(std::make_optional<AttackAI>(data.first, data.second));
	}
}

void AI_Manager::Update(Enemy& enemy, float elapsed_time, Node* node)
{
	using std::visit;

	// モード変更処理
	if (change_mode)
	{
		change_mode = false;

		InitModeData();

		running_mode = { &(*std::max_element(run_modes.begin(), run_modes.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode2) };

				return (rv1 < rv2);
			})) };
	}
	else if (running_mode)
	{

		visit([&](auto& mode)
			{
				// 更新し
				mode->Update(enemy.pos, elapsed_time, node);

				// そのモードの更新が終了を確認する
				change_mode = mode->GetModeEnd();

			}, *running_mode);
	}
}

void AI_Manager::InitModeData()
{
	using std::visit;

	std::vector<std::pair<bool, uint64_t>> init_data;

	// 優先順位と実行可能状態を取得し
	for (auto& p_mode : run_modes)
	{
		constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

		uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
		bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };

		init_data.emplace_back(is_executable, MaxNum - priority_num);
	}

	// モードを消去
	for (auto& r_mode : run_modes)
	{
		std::visit([](auto& mode) { mode = std::nullopt; }, r_mode);
	}

	// モードを再生成
	{
		// 巡回状態構築
		{
			const auto& data{ init_data.front() };

			run_modes.front().emplace<0>(
				std::make_optional<PatrolAI>(data.first, data.second, way_point_count, way_points));
		}

		// 攻撃状態構築
		{
			const auto& data{ init_data.back() };

			run_modes.at(1).emplace<1>(std::make_optional<AttackAI>(data.first, data.second));
		}
	}
}
