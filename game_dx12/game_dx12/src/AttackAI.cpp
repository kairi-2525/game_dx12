#include <vector>

#include "NodeData.h"
#include "AttackAI.h"
#include "ObjectManager.h"

AttackAI::AttackAI(const bool executable, const uint64_t priority_number)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number),
	attackking_mode(nullptr), change_mode(std::move(decltype(change_mode){ false, false })),
	mode_count(0u)
{
	static const std::vector<std::pair<bool, uint64_t>> init_data
	{
		// 実行可能、優先順位
		{ true	   , 1 },  // 移動
		{ true	   , 2 },  // 停止
	};

	assert(init_data.size() == attack_modes.size() && "初期化データの追加し忘れ");

	// 移動状態構築
	{
		const auto& data{ init_data.front() };

		attack_modes.front().emplace<0>(std::make_optional<ChaseAI>(data.first, data.second));
	}

	// 停止状態構築
	{
		const auto& data{ init_data.back() };

		attack_modes.front().emplace<1>(std::make_optional<GoBackAI>(data.first, data.second));
	}
}

void AttackAI::Update(VF3& enemy_pos, const float elapsed_time, Node* node)
{
	using std::visit;

	// モード変更処理
	if (std::any_of(change_mode.begin(), change_mode.end(), [](const bool mode) { return (mode); }))
	{
		// 次の履歴を差す
		mode_count++;

		attackking_mode = { &(*std::max_element(attack_modes.begin(), attack_modes.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode2) };

				return (rv1 < rv2);
			})) };
	}
	else if (attackking_mode)
	{
		visit([&](auto& mode)
			{
				// 更新し
				mode->Update(enemy_pos, elapsed_time, node);

				// そのモードの更新が終了を確認する
				change_mode.at(mode_count) = mode->GetModeEnd();

			}, *attackking_mode);
	}
}

//-----------------------------------------------------------------------------------------------------------

void ChaseAI::Update(VF3& enemy_pos, const float elapsed_time, Node* node)
{
#if false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "MoveAI");

	ImGui::SliderFloat(u8"移動速度", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// このノードに来て最初
	if (is_first)
	{
		// 二回目以降
		is_first = false;

		// 誤差分修正する
		enemy_pos = Round(enemy_pos);

		// ウェイポイントのカウントに合った座標を目標地点にする
		//target_pos = way_points->at(way_point_count).pos;

		// パス検索し、結果を取得
		node_data = node->PathFindingDijkstra(enemy_pos, target_pos);

		// パスを発見出来た場合
		if (node_data)
		{
			// 次のパスノードの位置を取得
			halfway_path = node_data->to_goal->pos;

			assert(node_data && node_data->to_goal && "死んでいる");

			// 次のパスノード情報を取得
			node_data = node_data->to_goal;
		}
		// 袋小路（パスを発見できず）
		else
		{
			is_mode_end = true;
		}
	}
	// 二回目以降
	else if (node_data)
	{
		constexpr float DisErrorRange{ 0.01f };  // 距離の誤差

		// 次のウェイポイントに到達
		if (Math::Distance(enemy_pos, target_pos) < DisErrorRange)
		{
			// このモードを終了
			is_mode_end = true;

			// 優先度を下げる
			priority_number -= RootBase::SubStructNumber;

			// 誤差分修正する
			enemy_pos = Round(enemy_pos);
		}
		// 次のパスノードに敵が来る
		else if (Math::Distance(halfway_path, enemy_pos) < DisErrorRange)
		{
			assert(node_data && node_data->to_goal && "死んでいる");

			// 次のパスノードの位置を取得
			halfway_path = node_data->to_goal->pos;

			// 次のパスノード情報を取得
			node_data = node_data->to_goal;

			// 誤差分修正する
			enemy_pos = Round(enemy_pos);
		}
		// 進行中
		else
		{
			//const auto speed_vec{ ToNormalizeXMVECTOR(halfway_path - enemy_pos) };
			//const VF3 speed{ ToXMFLOAT3(speed_vec) * move_speed * GMLIB->GetElapsedTime() };

			//enemy_pos += speed;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void GoBackAI::Update(VF3& enemy_pos, const float elapsed_time, Node* node)
{
}
