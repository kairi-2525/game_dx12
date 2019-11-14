#include <vector>

#include "NodeData.h"
#include "AttackAI.h"
#include "ObjectManager.h"
#include "ImGuiSeting.h"
#include "Enemy.h"

AttackAI::AttackAI(const bool executable, const uint64_t priority_number, const uint16_t kind_node,
	std::deque<WayPoint>* way_points)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
	attackking_mode(nullptr), mode_state(std::move(decltype(mode_state){ false, false })),
	mode_count(-1), change_mode(true)
{
	using std::get;

	static const std::vector<std::tuple<bool, uint64_t, uint16_t>> init_data
	{
		// 実行可能、優先順位、種類
		{ true	   , 1u		, 1 },  // 追跡
		{ true	   , 2u		, 2 },  // 戻る
	};

	assert(init_data.size() == attack_modes.size() && "初期化データの追加し忘れ");

	// 追跡状態構築
	{
		const auto& data{ init_data.front() };

		attack_modes.front().emplace<0>(std::make_optional<ChaseAI>(
			get<0>(data), get<1>(data), get<2>(data)));
	}

	// 戻る状態構築
	{
		const auto& data{ init_data.back() };

		attack_modes.back().emplace<1>(std::make_optional<GoBackAI>(
			get<0>(data), get<1>(data), get<2>(data), way_points));
	}
}

void AttackAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
	using std::visit;

#define	MODE_SET	mode_state.cbegin(), mode_state.cend(), [](const bool mode) { return (mode); }

	// 全てのノードが終了
	if (std::all_of(MODE_SET))
	{
		// このモードを終了
		is_mode_end = true;

		// 優先度を下げる
		priority_number -= RootBase::SubStructNumber;
	}
	// モード変更処理
	else if (change_mode)
	{
		// 次の履歴を差す
		mode_count++;

		change_mode = false;

		std::vector<AI_Control*> temp;
		std::vector<std::pair<bool, uint64_t>> init_data;

		// 優先度と実行可能状態取得
		for (auto& p_mode : attack_modes)
		{
			constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

			uint64_t priority_num
			{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
			bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };

			init_data.emplace_back(is_executable, MaxNum - priority_num);
		}

		auto BuildObjctPtr{ [&](size_t pos)
		{
			if (const auto& data{ init_data.at(pos) }; data.first)
			{
				auto& back{ temp.emplace_back() };

				back = &attack_modes.at(pos);
			}
		} };

		// 移動状態構築（実行可能なら）
		BuildObjctPtr(0u);

		// 停止状態構築（実行可能なら）
		BuildObjctPtr(1u);

		// 実行可能なノードで最も優先度が高いノードを選択する
		attackking_mode = *std::max_element(temp.begin(), temp.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode2) };

				return (rv1 < rv2);
			});
	}
	// 更新
	else if (attackking_mode)
	{
		visit([&](auto& mode)
			{
				// 更新し
				mode->Update(enemy_pos, elapsed_time, node);

				// そのモードの更新が終了を確認する
				change_mode = mode_state.at(mode_count) = mode->GetModeEnd();
			}, *attackking_mode);
	}
}

//-----------------------------------------------------------------------------------------------------------

void ChaseAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "ChaseAI");

	ImGui::SliderFloat(u8"移動速度", &move_speed, 0.f, 10.f, "%.1f");
	ImGui::SliderFloat(u8"最大追跡時間", &max_chase_time, 0.f, 60.f, "%.1f");

	ImGui::End();
#endif

	// このノードに来て最初
	if (is_first)
	{
		assert(Enemy::player && "std::shared_ptr<Player>* playerにplayer情報をセットし忘れ");

		// 二回目以降
		is_first = false;

		target_pos = (*Enemy::player)->pos;

		// パス検索し、結果を取得
		node_data = (node->PathFindingDijkstra(enemy_pos, target_pos));
		node_array_count = 0u;

		// パスを発見出来た場合
		if (!node_data.empty())
		{
			// 次のパスノードの位置を取得
			halfway_path = node_data[node_array_count].pos;

			// 次のパスノード情報を取得
			node_array_count++;
		}
		// 袋小路（パスを発見できず）
		else
		{
			is_mode_end = true;
		}
	}
	// 二回目以降
	else if (!node_data.empty())
	{
		constexpr float DisErrorRange{ 0.01f };  // 距離の誤差

		// 最大追跡時間を上回る
		if (second_timer > max_chase_time)
		{
			is_mode_end = true;
			executable = false;
		}
		// 次のパスノードに敵が来る
		else if (Math::Distance(halfway_path, enemy_pos) < DisErrorRange)
		{
			target_pos = (*Enemy::player)->pos;

			// パス検索し、結果を取得
			node_data = (node->PathFindingDijkstra(enemy_pos, target_pos));
			node_array_count = 0u;

			// 次のパスノードの位置を取得
			halfway_path = node_data[node_array_count].pos;

			// 次のパスノード情報を取得
			node_array_count++;

			// 誤差分修正する
			enemy_pos = Round(enemy_pos);
		}
		// 進行中
		else
		{
			const auto speed_vec{ ToNormalizeXMVECTOR(halfway_path - enemy_pos) };
			const VF3 speed{ ToXMFLOAT3(speed_vec) * move_speed * elapsed_time };

			enemy_pos += speed;
		}

	}

	timer += elapsed_time;
	second_timer += elapsed_time;
}

//-----------------------------------------------------------------------------------------------------------

void GoBackAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
	using Math::Pow2;

#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "GoBackAI");

	ImGui::SliderFloat(u8"移動速度", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// このノードに来て最初
	if (is_first)
	{
		// 二回目以降
		is_first = false;

		assert(!way_points->empty() && "ウェイポイントが設置されていない");

		// 比較距離計算関数
		auto SquareDis{ [](const VF3& pos1, const VF3& pos2)
		{ return Pow2(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y) + Pow2<float>(pos2.z - pos1.z); } };

		// 一番近いウェイポイントを目標地点に設定
		target_pos = (*std::min_element(way_points->begin(), way_points->end(),
			[&](const WayPoint& wp1, const WayPoint& wp2)
			{ return (SquareDis(wp1.pos, enemy_pos) < SquareDis(wp2.pos, enemy_pos)); })).pos;

		// パス検索し、結果を取得
		node_data = (node->PathFindingDijkstra(enemy_pos, target_pos));
		node_array_count = 0u;

		// パスを発見出来た場合
		if (!node_data.empty())
		{
			// 次のパスノードの位置を取得
			halfway_path = node_data[node_array_count].pos;

			// 次のパスノード情報を取得
			node_array_count++;
		}
		// 袋小路（パスを発見できず）
		else
		{
			is_mode_end = true;
		}
	}
	// 二回目以降
	else if (!node_data.empty())
	{
		constexpr float DisErrorRange{ 0.01f };  // 距離の誤差

		// 目的のウェイポイントに到達
		if (Math::Distance(enemy_pos, target_pos) < DisErrorRange)
		{
			// このモードを終了
			is_mode_end = true;

			// 誤差分修正する
			enemy_pos = Round(enemy_pos);
		}
		// 次のパスノードに敵が来る
		else if (Math::Distance(halfway_path, enemy_pos) < DisErrorRange)
		{
			// 次のパスノードの位置を取得
			halfway_path = node_data[node_array_count].pos;

			// 次のパスノード情報を取得
			node_array_count++;

			// 誤差分修正する
			enemy_pos = Round(enemy_pos);
		}
		// 進行中
		else
		{
			const auto speed_vec{ ToNormalizeXMVECTOR(halfway_path - enemy_pos) };
			const VF3 speed{ ToXMFLOAT3(speed_vec) * move_speed * elapsed_time };

			enemy_pos += speed;
		}
	}
}
