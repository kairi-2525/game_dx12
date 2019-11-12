#include <vector>

#include "ImGui/ImGuiSeting.h"
#include "GameLib.h"
#include "PatrolAI.h"
#include "ObjectManager.h"
#include "Enemy.h"
#include "XMFLOAT_Hlper.h"
#if !_DEBUG
#include "MessageBox.h"
#endif

PatrolAI::PatrolAI(const bool executable, const uint64_t priority_number, size_t& way_point_count,
	std::deque<WayPoint>* way_points)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number), change_mode(true),
	patroling_mode(nullptr), way_point_count(way_point_count), way_points(way_points)
{
	static const std::vector<std::pair<bool, uint64_t>> init_data
	{
		// 実行可能、優先順位
		{ true	   , 2 },  // 移動
		{ true	   , 3 },  // 停止
		{ false	   , 1 },  // 発見
	};

	assert(init_data.size() == 3u && "初期化データの追加し忘れ");

	// 移動状態構築
	{
		const auto& data{ init_data.front() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<0>(std::make_optional<MoveAI>(data.first, data.second, way_point_count, way_points));
	}

	// 停止状態構築
	{
		const auto& data{ init_data.at(1) };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<1>(std::make_optional<StopAI>(data.first, data.second));
	}

	// 発見状態構築
	{
		const auto& data{ init_data.back() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<2>(std::make_optional<FindAI>(data.first, data.second));
	}
}

void PatrolAI::Update(VF3& enemy_pos, Node* node)
{
	using std::visit;

	if (GMLIB->isKeyDown(KeyData::Keys::P))
	{
		// このモードを終了
		is_mode_end = true;

		// 優先度を下げる
		priority_number -= RootBase::SubStructNumber;
	}

	// モード変更処理
	if (change_mode)
	{
		change_mode = false;

		InitModeData();

		std::vector<AI_Control*> temp;
		std::vector<std::pair<bool, uint64_t>> init_data;

		// 優先度と実行可能状態取得
		for (auto& p_mode : patrol_modes)
		{
			constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

			uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
			bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };

			init_data.emplace_back(is_executable, MaxNum - priority_num);
		}

		auto BuildObjctPtr{ [&](size_t pos)
		{
			if (const auto& data{ init_data.at(pos) }; data.first)
			{
				auto& back{ temp.emplace_back() };

				back = &patrol_modes.at(pos);
			}
		} };

		// 移動状態構築（実行可能なら）
		BuildObjctPtr(0u);

		// 停止状態構築（実行可能なら）
		BuildObjctPtr(1u);

		// 発見状態構築（実行可能なら）
		BuildObjctPtr(2u);

		// 実行可能なノードで最も優先度が高いノードを選択する
		patroling_mode = { (*std::max_element(temp.begin(), temp.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode2) };

				return (rv1 < rv2);
			})) };
	}
	// 更新処理
	else if (patroling_mode)
	{
		visit([&](auto& mode)
			{
				// 更新
				mode->Update(enemy_pos, node);

				// そのモードの更新が終了を確認する
				change_mode = mode->GetModeEnd();
			}, *patroling_mode);
	}
}

void PatrolAI::InitModeData()
{
	using std::visit;

	std::vector<std::pair<bool, uint64_t>> init_data;

	// 優先順位と実行可能状態を取得し
	for (auto& p_mode : patrol_modes)
	{
		constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

		uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
		bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };

		init_data.emplace_back(is_executable, MaxNum - priority_num);
	}

	// モードを消去
	patrol_modes.clear();

	// モードを再生成
	{
		// 移動状態構築
		{
			const auto& data{ init_data.front() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<0>(std::make_optional<MoveAI>(data.first, data.second, way_point_count,
				way_points));

			//BuildAINode<MoveAI, 0u>(init_data, way_point_count, way_points);
		}

		// 停止状態構築
		{
			const auto& data{ init_data.at(1) };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<1>(std::make_optional<StopAI>(data.first, data.second));
		}

		// 発見状態構築
		{
			const auto& data{ init_data.back() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<2>(std::make_optional<FindAI>(data.first, data.second));
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void MoveAI::Update(VF3& enemy_pos, Node* node)
{
#if USE_IMGUI
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "MoveAI");

	ImGui::SliderFloat(u8"移動速度", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// このノードに来て最初
	if (is_first && way_points)
	{
		// 二回目以降
		is_first = false;

		// 誤差分修正する
		enemy_pos = Round(enemy_pos);

		// 一周したらカウントを戻す
		if (way_point_count >= way_points->size()) way_point_count = 0u;

		// ウェイポイントのカウントに合った座標を目標地点にする
		target_pos = way_points->at(way_point_count).pos;

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
#if _DEBUG
			assert(!"ウェイポイントの設置位置がおかしい");
#else
			if (Message(L"ウェイポイントの設置位置がおかしい", L"終了します") == BoxReturn::Yes_Ok)
			{
				exit(EXIT_SUCCESS);
			}
#endif
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

			// 次のウェイポイントを指し示す
			way_point_count++;

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
			const auto speed_vec{ ToNormalizeXMVECTOR(halfway_path - enemy_pos) };
			const VF3 speed{ ToXMFLOAT3(speed_vec) * move_speed * GMLIB->GetElapsedTime() };

			enemy_pos += speed;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void StopAI::Update(VF3& enemy_pos, Node* node)
{
#if USE_IMGUI
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"停止時間", &stop_time, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	if (timer += GMLIB->GetElapsedTime(); timer > stop_time)
	{
		is_mode_end = true;

		// 優先度を下げる
		priority_number -= RootBase::SubStructNumber;
	}
}

//-----------------------------------------------------------------------------------------------------------

void FindAI::Update(VF3& enemy_pos, Node* node)
{
#if USE_IMGUI
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"停止時間", &stop_time, 0.f, 10.f, "%.0f");

	ImGui::End();
#endif

	if (timer += GMLIB->GetElapsedTime(); timer > stop_time)
	{
		is_mode_end = true;

		// 優先度を下げる
		priority_number -= RootBase::SubStructNumber;
	}
}