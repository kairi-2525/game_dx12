#include <vector>

#include "ImGuiSeting.h"
#include "PatrolAI.h"
#include "ObjectManager.h"
#include "Enemy.h"
#include "XMFLOAT_Hlper.h"
#include "GameScene.h"

#if !_DEBUG
#include "MessageBox.h"
#endif

PatrolAI::PatrolAI(const bool executable, const uint64_t priority_number, size_t& way_point_count,
	std::deque<WayPoint>* way_points, const uint16_t kind_node, const float* enemy_angle)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
	change_mode(true), patroling_mode(nullptr), way_point_count(way_point_count), way_points(way_points),
	end_number((std::numeric_limits<uint16_t>::max)()), enemy_angle_y(enemy_angle)
{
	using std::get;

	static const std::vector<std::tuple<bool, uint64_t, uint16_t>> init_data
	{
		// 実行可能、優先順位、種類
		{ true	   , 2		, 1 },  // 移動
		{ true	   , 3		, 2 },  // 停止
		{ false	   , 1		, 3 },  // 発見
	};

	assert(init_data.size() == 3u && "初期化データの追加し忘れ");

	// 移動状態構築
	{
		const auto& data{ init_data.front() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<0>(std::make_optional<MoveAI>(
			get<0>(data), get<1>(data), way_point_count, way_points, get<2>(data)));
	}

	// 停止状態構築
	{
		const auto& data{ init_data.at(1) };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<1>(std::make_optional<StopAI>(get<0>(data), get<1>(data), get<2>(data)));
	}

	// 発見状態構築
	{
		const auto& data{ init_data.back() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<2>(std::make_optional<FindAI>(get<0>(data), get<1>(data), get<2>(data)));
	}
}

void PatrolAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
	using std::visit;

	// 前回のFindAIノードが終了してした時
	if (end_number == 3u && change_mode)
	{
		// パス検索した結果、パスを見つけられないなら攻撃状態へ移らない
		if (node->PathFindingDijkstra(enemy_pos, (*Enemy::player)->pos).empty())
		{
			end_number = (std::numeric_limits<uint16_t>::max)();
			change_mode = true;

			return;
		}

		// このモードを終了
		is_mode_end = true;

		// 優先度を下げる
		priority_number -= RootBase::SubStructNumber;
	}
	// モード変更処理
	else if (change_mode)
	{
		change_mode = false;

		InitModeData();

		std::vector<AI_Control*> temp;
		std::vector<std::pair<bool, uint64_t>> init_data;

		// 優先度と実行可能状態取得
		for (auto& p_mode : patrol_modes)
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
		constexpr uint16_t InitNumber{ (std::numeric_limits<uint16_t>::max)() };

		end_number = InitNumber;

		uint16_t kinds_num{ 0 };

		visit([&](auto& mode)
			{
				// 更新
				mode->Update(enemy_pos, elapsed_time, node);

				// そのモードの更新が終了を確認する
				change_mode = mode->GetModeEnd();

				kinds_num = mode->GetKindNode();

				// そのモードの終了時に種類を保存
				if (change_mode)
					end_number = kinds_num;
			}, *patroling_mode);

#if _DEBUG
		if (!(*Enemy::player))
		{
			assert(!"プレイヤーを配置していない（続行可）");
			SceneGame::execution_quick_exit = true;
			return;
		}
#else
		if (!(*Enemy::player) && Message(L"プレイヤーを配置していない", L"エディターへ移行するZE☆") == BoxReturn::Yes_Ok)
		{
			SceneGame::execution_quick_exit = true;
			return;
		}
#endif

		// 自棄との当たり判定（Findノードを除く）
		if (!change_mode && kinds_num != 3 &&
			IsPlayerRayHit(enemy_pos, (*Enemy::player)->pos))
		{
			// Findノードを実行可能状態へ
			visit([](auto& mode) { mode->SetExecutable(true); }, patrol_modes.back());

			// モード変更可能に
			change_mode = true;
		}
	}
}

bool PatrolAI::IsPlayerRayHit(const VF3& enemy_pos, const VF3& player_pos)
{
	namespace DX = DirectX;
	using Math::AdjEqual;
	using VF2 = DX::XMFLOAT2;
	using Vec2sub::MakeVector2;

	// 敵が反応距離内にいない
	if (Math::Distance(enemy_pos, player_pos) > hit_distance)	return false;

	DX::XMVECTOR enm_vec{ DX::XMVectorSet(0.f, 0.f, 0.f, 0.f) };

	// 敵の反応範囲
	{
		constexpr float AdjRadY{ Math::PAI<float> / 2.f };

		const auto e_pos{ MakeVector2<float>(enemy_pos.x, enemy_pos.z) };
		const auto p_pos{ MakeVector2<float>(player_pos.x, player_pos.z) };

		const auto enm_to_pl_vec{ ToNormalizeXMVECTOR(e_pos - p_pos) };

		// 右
		if (AdjEqual(*enemy_angle_y, AdjRadY))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ +1.f, 0.f }) };

			enm_vec = InitVec;
		}
		// 左
		else if (AdjEqual(*enemy_angle_y, -AdjRadY))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ -1.f, 0.f }) };

			enm_vec = InitVec;
		}
		// 上
		else if (AdjEqual(*enemy_angle_y, AdjRadY * 0.f))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ 0.f, +1.f }) };

			enm_vec = InitVec;
		}
		// 下
		else if (AdjEqual(*enemy_angle_y, AdjRadY * 2.f))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ 0.f, -1.f }) };

			enm_vec = InitVec;
		}

		// 敵を基準にしたプレーヤーの角度
		const float radian{ std::acos(ToXMFLOAT1(DX::XMVector2Dot(enm_to_pl_vec, enm_vec))) };

		// 敵が反応範囲内にいない
		if (radian > hit_angle)		return false;
	}

	for (auto& wall : *Enemy::walls)
	{
		constexpr float WallDivSize{ 0.6f };

		// 認識範囲外は処理しない
		if (Math::Distance(wall.pos, enemy_pos) > hit_distance)	continue;

		const auto e_pos{ MakeVector2<float>(enemy_pos.x, enemy_pos.z) };
		const auto w_pos{ MakeVector2<float>(wall.pos.x, wall.pos.z) };

		const auto& enm_wall_vec{ ToNormalizeXMVECTOR(-(w_pos - e_pos)) };

		// 敵を基準にした壁の角度
		const float radian{ std::acos(ToXMFLOAT1(DX::XMVector2Dot(enm_wall_vec, enm_vec))) };

		// 壁が反応範囲内にいない
		if (radian > hit_angle)		continue;

		const auto point{ ToXMVECTOR(w_pos) };
		const auto line_point1{ ToXMVECTOR(VF2{ player_pos.x, player_pos.z }) };
		const auto line_point2{ ToXMVECTOR(VF2{ enemy_pos.x, enemy_pos.z }) };

		// 敵と自機の線と壁の最短距離
		const float dis{ ToXMFLOAT1(DX::XMVector2LinePointDistance(line_point1, line_point2, point)) };

		// 最短距離が壁を球と見なした時の半径よりも短い
		if (dis < WallDivSize)	return false;
	}

	return true;
}

void PatrolAI::InitModeData()
{
	using std::visit;
	using std::get;

	std::vector<std::tuple<bool, uint64_t, uint16_t>> init_data;

	// 優先順位と実行可能状態を取得し
	for (auto& p_mode : patrol_modes)
	{
		constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

		uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
		bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };
		uint16_t kinds{ visit([](const auto& mode) { return mode->GetKindNode(); }, p_mode) };

		init_data.emplace_back(is_executable, MaxNum - priority_num, kinds);
	}

	// モードを消去
	patrol_modes.clear();

	// モードを再生成
	{
		// 移動状態構築
		{
			const auto& data{ init_data.front() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<0>(std::make_optional<MoveAI>(
				get<0>(data), get<1>(data), way_point_count, way_points, get<2>(data)));
		}

		// 停止状態構築
		{
			const auto& data{ init_data.at(1) };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<1>(std::make_optional<StopAI>(get<0>(data), get<1>(data), get<2>(data)));
		}

		// 発見状態構築
		{
			const auto& data{ init_data.back() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<2>(std::make_optional<FindAI>(get<0>(data), get<1>(data), get<2>(data)));
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void MoveAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "MoveAI");

	ImGui::SliderFloat(u8"移動速度", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// このノードに来て最初
	if (is_first && way_points)
	{
		// ウェイポイントが無い敵はこのノードでは何もしない
		if (way_points->empty())
		{
			is_mode_end = true;
			return;
		}

		// 二回目以降
		is_first = false;

		// 誤差分修正する
		enemy_pos = Round(enemy_pos);

		// 一周したらカウントを戻す
		if (way_point_count >= way_points->size()) way_point_count = 0u;

		// ウェイポイントのカウントに合った座標を目標地点にする
		target_pos = way_points->at(way_point_count).pos;

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
#if _DEBUG
			assert(!"ウェイポイントの設置位置がおかしい（続行可）");
			SceneGame::execution_quick_exit = true;
			return;
#else
			std::wstring text{ L"ウェイポイントの設置位置がおかしいよ！！\n" };

			text += std::to_wstring(way_point_count + 1);
			text += L"個目のウェイポイントの設置位置まで行けないよ！！";

			if (Message(text.data(), L"エディターへ移行するZE☆") == BoxReturn::Yes_Ok)
			{
				SceneGame::execution_quick_exit = true;
				return;
			}
#endif
		}
	}
	// 二回目以降
	else if (!node_data.empty())
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

//-----------------------------------------------------------------------------------------------------------

void StopAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"停止時間", &stop_time, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	if (timer += elapsed_time; timer > stop_time)
	{
		is_mode_end = true;

		// 優先度を下げる
		priority_number -= RootBase::SubStructNumber;
	}
}

//-----------------------------------------------------------------------------------------------------------

void FindAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"停止時間", &stop_time, 0.f, 10.f, "%.0f");

	ImGui::End();
#endif

	// 初回だけ
	if (timer == 0.f)
	{
		using SG = SceneGame;

		int handle = SG::audio->CreatePlayHandle(SG::se_whistle, 0.f, false, false, 0.f, 0.f, 0, false, false);
		SG::audio->Play(SG::se_whistle, handle, 0.01f, 0.2f, false);
	}

	// 一定時間後
	if (timer += elapsed_time; timer > stop_time)
	{
		// このモードを終了
		is_mode_end = true;
		executable = false;
	}
}