#include "ObjectManager.h"
#include "GameScene.h"
#include "KDL.h"
#include "ImVecHelper.h"
#include "LoadAllFileName.h"
#include "Time.h"
#include "Random.h"

ObjectManager::ObjectManager()
	: select_enm(nullptr), select_waypoint(nullptr), masu_pos(0.f, 0.f, 0.f), is_goal(false), timer(0.0)
{
	now_move_object = edit_mode_first = false;
}

// 更新
void ObjectManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	// 各オブジェクト更新
	if (!is_goal)
	{
		// 単一オブジェクト
		objects.SingleObjVisit([&](auto& object) { if (object) object->Update(p_window, p_app); });

		// 複数オブジェクト
		objects.MultiObjVisit([&](auto& objects) { for (auto& obj : objects) obj.Update(p_window, p_app); });
	}

	masu_pos = GS::GetMasuPos();

#if false
	// モードによって更新を分ける
	GS::GetEditMode() ? EditModeUpdate(p_window, p_app) : NormalModeUpdate(p_window, p_app);

	// オブジェクト削除
	{
		auto EraseObject{ [](auto& data) { data.erase(std::remove_if(
			data.begin(), data.end(), [](const auto& dt) { return (!dt); }), data.end()); } };

		// 複数オブジェクト
		objects.MultiObjVisit(EraseObject);

		// 単一オブジェクト
		objects.SingleObjVisit([](auto& object) { if (object && !(*object)) object = nullptr; });
	}
#else
	NormalModeUpdate(p_window, p_app);
#endif
}

// 描画
void ObjectManager::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// 単一オブジェクト
	objects.SingleObjVisit([&](auto& object) { if (object) object->Draw(p_window, p_app); });

	// 複数オブジェクト
	objects.MultiObjVisit([&](auto& objects) { for (auto& obj : objects) obj.Draw(p_window, p_app); });

	for (auto& effect : effects)
	{
		using GS = SceneGame;

		constexpr int BS{ static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA) };

		auto& angle{ effect.angle };
		auto& pos{ effect.pos };

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			if (SceneGame::back_world_mode)
				S = DirectX::XMMatrixScaling(0.2f, 0.2f, 0.2f);
			else
				S = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);

			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			T = DirectX::XMMatrixTranslation(pos.x, pos.y - 0.5f, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

		if (SceneGame::back_world_mode)
		{
			sand_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, GS::LightDir, { WHITE, 1.f }, BS, false, true, false);
		}
		else
		{
			crystal_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, GS::LightDir, { WHITE, 1.f }, BS, false, true, false);
		}
	}
}

// 編集モード更新
void ObjectManager::EditModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;
	using Buttons = KDL::MOUSE_INPUTS;
	using GS = SceneGame;

#ifdef KDL_USE_IMGUI
#if false
	auto input{ p_window->GetInput() };

	// 編集モード
	if (!GS::GetEnmEditMode())
	{
		// 変更後の初回
		if (edit_mode_first)
		{
			edit_mode_first = false;

			auto& enemys{ objects.GetChangeObjects<Enemy>() };

			// 全ての敵の選択を解除する
			for (auto& enm : enemys)
				enm.SetIsSelect(false);
		}

		ImGui::Text(u8"左クリック　　　      ：生成");
		ImGui::Text(u8"右クリック　　　      ：削除");
		ImGui::Text(u8"ホイールクリック(hold)：移動");

		ImguiTool::Spacing(5u, true);

		// 追加
		if (!now_move_object)  // 移動中は追加出来ない
		{
			static const char* AddObjectNames[]
			{ u8"床", u8"壁", u8"敵", u8"自機", u8"ワープホール", u8"スタート", u8"ゴール", u8"ドア", u8"カギ", "None(移動時などに設置すると便利)" };
			static int select_item{ -1 };
			static int hp{ 2 };

			ImGui::Combo(u8"生成するオブジェクト", &select_item, AddObjectNames, IM_ARRAYSIZE(AddObjectNames));

			// 地面のみ
			if (select_item == 0) ImGui::SliderInt(u8"HP設定", &hp, 0, 2);

			ImguiTool::Spacing(2u, true);

			if (select_item != -1)
			{
				GS::is_save = false;

				// 右クリックで追加
				if (!ImGui::GetIO().WantCaptureMouse && input->IsHitMouse(Buttons::leftButton))
				{
					const VF3& masu_pos{ GS::GetMasuPos() };

					// 選択した種類によって生成する
					switch (select_item)
					{
					case 0:		// 床
					{
						// 全てのオブジェクトと重なっていない
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Plane>(hp) };

							back.CheckHP();

							back.pos = masu_pos;
						}

						break;
					}
					case 1:		// 壁
					{
						// 全てのオブジェクトと重なっていない
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Wall>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 2:		// 敵
					{
						// 床だけに重なっていて、敵と自機と鍵に重なっていない
						if (AnyOfNotExceptPlaneObjSamePos())
						{
							auto& back{ objects.AddObjects<Enemy>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 3:		// 自機
					{
						// 床だけに重なっていて、敵と自機と鍵に重なっていない
						if (AnyOfNotExceptPlaneObjSamePos())
						{
							auto& back{ objects.AddObjects<Player>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 4:		// ワープゾーン
					{
						// 全てのオブジェクトと重なっていない
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<WarpHole>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					case 5:		// スタート
					{
						// 全てのオブジェクトと重なっていない
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Start>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 6:		// ゴール
					{
						// 全てのオブジェクトと重なっていない
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Goal>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					case 7:		// ドア
					{
						// 全てのオブジェクトと重なっていない
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Door>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					case 8:		// 鍵
					{
						// 床だけに重なっていて、敵と自機と鍵に重なっていない
						if (AnyOfNotExceptPlaneObjSamePos())
						{
							auto& back{ objects.AddObjects<Key>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					}
				}
			}
		}

		// 削除
		// Imgui上のウィンドウに重なっておらず、右クリックを押した時
		if (!now_move_object && !ImGui::GetIO().WantCaptureMouse &&  // 移動中は削除できない
			input->IsTrgMouse(Buttons::rightButton))
		{
			auto DeleteObjectS{ [&](auto& object)
				{ if (object && object->pos == GS::GetMasuPos()) object->EndExist(); } };
			auto DeleteObjectM{ [&](auto& data)
			{ for (auto& dt : data) { if (dt.pos == GS::GetMasuPos()) dt.EndExist(); }; } };

			static bool deleted{ false };
			static VF3 save_pos{ GS::GetMasuPos() };
			const VF3 masu_pos{ GS::GetMasuPos() };

			// 地面とまとめて消えるのを防ぐため（クリックして初回か違うマスに移動した時）
			if (input->IsTrgMouse(Buttons::rightButton) || save_pos != masu_pos)
			{
				auto& keys{ objects.GetChangeObjects<Key>() };
				auto& player{ objects.GetChangeObjects<Player>() };
				auto& enemys{ objects.GetChangeObjects<Enemy>() };

				// 初期化
				save_pos = masu_pos;
				deleted = false;

				// 自機
				if (player && player->pos == GS::GetMasuPos())
				{
					player->EndExist();
					deleted = true;
				}

				// 鍵
				for (auto& dt : keys)
				{
					if (!deleted && dt.pos == GS::GetMasuPos())
					{
						dt.EndExist();
						deleted = true;
					}
				}

				// 敵
				for (auto& dt : enemys)
				{
					if (!deleted && dt.pos == GS::GetMasuPos())
					{
						dt.EndExist(); deleted = true;
					}
				}

				if (deleted) GS::is_save = false;
			}

			// 既に消されていなければ
			if (!deleted)
			{
				GS::is_save = false;
				save_pos = masu_pos;
				deleted = false;

				// 単一オブジェクト
				objects.SingleObjVisit(DeleteObjectS);

				// 複数オブジェクト
				objects.MultiObjVisit(DeleteObjectM);
			}
		}

		// 移動
		if (!ImGui::GetIO().WantCaptureMouse) MoveObject(p_window, p_app);
	}
	// 敵編集モード時は追加・削除・移動は出来ない
	else
		EnmEditModeUpdate(p_window, p_app);
#endif
#endif
}

// 敵編集モード更新
void ObjectManager::EnmEditModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;
	using Buttons = KDL::MOUSE_INPUTS;
	using GS = SceneGame;

	auto input{ p_window->GetInput() };

	edit_mode_first = true;

	const auto& masu_pos{ GS::GetMasuPos() };

	{
		auto& enemys{ objects.GetChangeObjects<Enemy>() };

		// 敵が存在しないなら処理しない
		if (enemys.empty())	return;

		// 左クリック
		if (input->IsTrgMouse(Buttons::leftButton))
		{
			select_waypoint = nullptr;

			// 選択した敵を検索
			auto find{ std::find_if(enemys.begin(), enemys.end(),
				[&](const auto& enm) { return (enm.pos == masu_pos); }) };

			// 全ての選択を解除
			for (auto& enm : enemys) enm.SetIsSelect(false);
			select_enm = nullptr;

			// 選択したマスと重なっている敵を発見した
			if (find != enemys.end())
			{
				find->SetIsSelect(true);  // 選択されている
				select_enm = &(*find);
				GS::is_save = false;
			}
		}
	}

	// 選ばれている時
	if (select_enm)
	{
		static VF3 save_wp_pos;

		// Enterキー
		if (input->IsHitKey(Keys::Enter))
		{
			// 押してから初回
			if (input->IsTrgKey(Keys::Enter))
			{
				auto& waypoint{ select_enm->GetWayPoint() };

				auto FindWayPoint{ [&]()
				{ return std::find_if(waypoint.begin(), waypoint.end(), [&](auto& wp)
					{ return (wp.pos == masu_pos); }); } };

				auto WayPointNotSamePos{ [&]()
				{ return std::all_of(waypoint.cbegin(), waypoint.cend(), [&](const auto& wp)
					{ return (wp.pos != masu_pos); }); } };

				// 初期化する
				select_waypoint = nullptr;
				Clear(save_wp_pos);

				// 追加
				// ウェイポイントを選択しておらず、床だけに重なっていて、敵と自機と鍵に重なっていない
				if (!select_waypoint && AnyOfNotExceptPlaneObjSamePos() && WayPointNotSamePos())
				{
					select_enm->AddWayPoint(masu_pos);
				}
				// 移動
				// 選択したマス上にウェイポイントを発見した
				else if (auto itr{ FindWayPoint() }; itr != waypoint.cend())
				{
					select_waypoint = &(*itr);
					save_wp_pos = masu_pos;
				}
			}
			// 二回目以降
			else
			{
				// ウェイポイントを選択しているなら、位置を更新する
				if (select_waypoint) select_waypoint->pos = masu_pos;
			}
		}
		// ウェイポイントを選択して、Enterキー話した瞬間
		else if (select_waypoint && !input->IsHitKey(Keys::Enter))
		{
			// 床だけに重なっていて、敵と自機と鍵に重なっていない
			if (AnyOfNotExceptPlaneObjSamePos())
			{
				select_waypoint = nullptr;  // 選択を解除
				Clear(save_wp_pos);
			}
			else
			{
				// 位置をもとに戻してから、選択を解除
				select_waypoint->pos = save_wp_pos;
				select_waypoint = nullptr;
				Clear(save_wp_pos);
			}
		}
		// 削除
		else if (input->IsTrgKey(Keys::Back))
		{
			// 敵を選択していないなら削除（内部的にウェイポイント敵自身に設定している為）
			if (masu_pos != select_enm->pos)
				select_enm->EraseWayPoint(masu_pos);
		}
	}

#ifdef KDL_USE_IMGUI
	const auto vp{ p_app->GetViewport() };
	const VF2 s_size{ vp.Width, vp.Height };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, u8"敵編集モード");

	ImGui::Text(u8"編集する敵を左クリック");
	ImGui::Text(u8"適当な所を左クリックで選択解除");
	ImGui::Text(u8"要所要所に設置するだけで構いません。\n"
		u8"自動でそこに向かいます");

	ImguiTool::Spacing(3u, true);

	ImGui::Text(u8"ウェイポイント情報");

	ImGui::Separator();

	ImGui::Text(u8"Enter       : 追加");
	ImGui::Text(u8"Enter(Hold) : 移動");
	ImGui::Text(u8"BackSpace   : 削除");

	ImguiTool::Spacing(3u, true);

	if (select_enm)
	{
		const auto& waypoints{ select_enm->GetWayPoint() };

		// ウェイポイントが存在しないなら何もしない
		if (!waypoints.empty())
		{
			ImGui::Text(u8"ウェイポイント総数：%u個", waypoints.size() - 1);

			for (size_t i = 0u, length = waypoints.size(); i < length; i++)
			{
				if (i == length - 1u)	break;  // 最後は何もしない

				constexpr float AdjPosY{ -15.f };

				const auto& wp{ waypoints[i] };
				auto vp{ p_app->GetViewport() };

				VF2 pos{ GS::TransformLocalToScreen({ vp.Width, vp.Height }, wp.pos) };

				pos.y += AdjPosY;

				const std::string label{ "Number" + std::to_string(i) };

				ImguiTool::BeginShowTempWindow(pos, label.data(), false);

				ImGui::Text(u8"順番 : %u", i + 1u);

				ImGui::End();
			}
		}
	}

	ImGui::End();
#endif
#endif
}

// 通常モード更新
void ObjectManager::NormalModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;
	using Keys = KDL::KEY_INPUTS;

	// 裏世界モード設定
	{
		static bool first{ true };

		bool& back_world_mode{ GS::back_world_mode };
		const auto& warphole{ objects.GetObjects<WarpHole>() };
		const auto& player{ objects.GetObjects<Player>() };

		auto WarpHoleCheck{ [&]()
		{ return std::any_of(warphole.cbegin(), warphole.cend(), [&](const auto& wh)
			{ if (back_world_mode == wh.GetBackWorld())
				return (wh.pos == player->pos); else return false; }); } };

		// ワープホールに当たって初回
		if (first && player && WarpHoleCheck())
		{
			first = false;
			back_world_mode ^= true;

			using SG = SceneGame;

			int handle = SG::audio->CreatePlayHandle(SG::se_waap, 0.f, false, false, 0.f, 0.f, 0, false, false);
			SG::audio->Play(SG::se_waap, handle, 0.01f, Volume / 2.f, false);

			auto& plane{ objects.GetChangeObjects<Plane>() };

			std::for_each(plane.begin(), plane.end(), [](auto& data) { data.InversionProc(); });
		}
		else if (!first && player && !WarpHoleCheck())
		{
			first = true;
		}
	}

	// エフェクト用
	{
		// 生成
		{
			static RndDoubleMaker random_inc_rate{ 1.0, 0.001 };

			static double set_inc_rate{ random_inc_rate.GetRnd() };

			if (timer > set_inc_rate)
			{
				constexpr float BasePosY{ 20.f };

				static RndDoubleMaker rand_pos{ 80.0, -80.0 };
				static RndDoubleMaker rand_ang{ 3.14, -3.14 };

				auto& c_pos{ SceneGame::camera->GetPosition() };

				effects.emplace_back(
					VF3{ rand_pos.GetRnd<float>() + c_pos.x, c_pos.y + 5.f, rand_pos.GetRnd<float>() + c_pos.z },
					VF3{ rand_ang.GetRnd<float>(), rand_ang.GetRnd<float>(), rand_ang.GetRnd<float>() });

				set_inc_rate = random_inc_rate.GetRnd();
			}

			timer += p_window->GetElapsedTime();
		}

		// 更新
		{
			for (auto& effect : effects)
			{
				// 座標
				{
					constexpr float Speed{ 2.f };

					effect.pos.y -= Speed * p_window->GetElapsedTime();
				}

				// 角度
				{
					constexpr float Speed{ Math::ToRadian(10.f) };

					effect.angle.x -= Speed * p_window->GetElapsedTime();
					effect.angle.y -= Speed * p_window->GetElapsedTime();
					effect.angle.z -= Speed * p_window->GetElapsedTime();
				}
			}
		}

		// 削除
		effects.erase(std::remove_if(effects.begin(), effects.end(), [](Effect& ef) { return ef.pos.y < -50.f; }),
			effects.end());
	}

	edit_mode_first = true;

	if (auto& player{ objects.GetChangeObjects<Player>() }; player)
	{
		// プレーヤーと敵の判定
		{
			constexpr float AdjstDistance{ 1.f };

			auto& enemies{ objects.GetObjects<Enemy>() };

			for (auto& enm : enemies)
			{
				const float dis{ Math::Distance(enm.pos, player->pos) };

				// 一定距離まで近づいたらゲームオーバー
				if (dis < AdjstDistance)
				{
					SceneGame::init_scene = true;
				}
			}
		}

		// ゴールと自機の判定
		if (auto& goal{ objects.GetObjects<Goal>() }; goal)
		{
			using SG = SceneGame;

			// 同じ世界線で同じ座標になったらシーン切り替え処理に入る
			if ((goal->GetIsBackWorld() == SceneGame::back_world_mode) && (goal->pos == player->pos)
				&& !SG::audio->IsPlay(SG::se_goal, SG::p_se_goal))
			{
				SG::p_se_goal =
					SG::audio->CreatePlayHandle(SG::se_goal, 0.f, false, false, 0.f, 0.f, 0, false, false);
				SG::audio->Play(SG::se_goal, SG::p_se_goal, 0.01f, Volume, false);

				is_goal = true;
				SceneGame::execution_quick_exit = true;
			}
		}

		// 鍵と自機の判定
		{
			auto& keys{ objects.GetChangeObjects<Key>() };

			for (auto& key : keys)
			{
				if (key.GetBackWorld() != SceneGame::back_world_mode)	continue;

				if (key.pos == player->pos)
				{
					player->key_num++;
					key.EndExist();

					break;
				}
			}
		}

		// ドアと自機の判定
		if (auto input{ p_window->GetInput() }; input->IsHitAnyKey())
		{
			using Vec2sub::MakeVector2;
			using Math::AdjEqual;

			constexpr float AdjstDistance{ 1.4f };
			constexpr float AdjRadY{ Math::PAI<float> / 2.f };

			auto& doors{ objects.GetChangeObjects<Door>() };

			for (auto& door : doors)
			{
				//if (door.GetBackWorld() != SceneGame::back_world_mode)	continue;

				const float dis{ Math::Distance(door.pos, player->pos) };

				// プレーヤーがドアに接触し、鍵を持っている
				if (door.pos == player->pos && player->key_num > 0)
				{
					player->key_num--;
					door.OpenDoor();

					break;
				}
				// プレーヤーがドアに接触し、鍵を持っていない
				else if (door.pos == player->pos && (player->key_num == 0 || door.is_open))
				{
					const float ang_y{ player->angle.y };
					VF3& p_pos{ player->pos };

					// 向いている方向と逆向きに戻す

					// 左
					if (AdjEqual(ang_y, -AdjRadY))
					{
						p_pos.x -= 1.f;
					}
					// 右
					else if (AdjEqual(ang_y, AdjRadY))
					{
						p_pos.x += 1.f;
					}
					// 上
					else if (AdjEqual(ang_y, 0.f))
					{
						p_pos.z += 1.f;
					}
					// 下
					else if (AdjEqual(ang_y, AdjRadY * 2.f))
					{
						p_pos.z -= 1.f;
					}
				}
			}
		}
	}

	// 地面にプレーヤー座標をセット
	if (const auto& player{ objects.GetObjects<Player>() }; player)
		Plane::SetPlPosition(player->pos);

	// オブジェクト削除
	{
		auto EraseObject{ [](auto& data) { data.erase(std::remove_if(
			data.begin(), data.end(), [](const auto& dt) { return (!dt); }), data.end()); } };

		// 複数オブジェクト
		objects.MultiObjVisit(EraseObject);

		// 単一オブジェクト
		objects.SingleObjVisit([](auto& object) { if (object && !(*object)) object = nullptr; });
	}
}

// オブジェクトの移動
void ObjectManager::MoveObject(KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Buttons = KDL::MOUSE_INPUTS;
	using GS = SceneGame;

	static VF3 save_pos;

	const auto& masu_pos{ GS::GetMasuPos() };
	auto input{ p_window->GetInput() };

	// 左クリック押した時（掴む）
	if (input->IsTrgMouse(Buttons::middleButton))
	{
		bool is_finded{ false };

		auto AnyOfSamePos{ [&]()
		{ return (IsSamePosObject<Player>() ||
				 IsSamePosObject<Enemy>() ||
				 IsSamePosObject<Key>()); } };

		// 初期化
		objects.select_m_obj = std::nullopt;
		objects.select_s_obj = std::nullopt;

		// 自機・敵・鍵のいずれかに重なっている
		if (AnyOfSamePos())
		{
			// 自機
			if (auto& player{ objects.GetChangeObjects<Player>() }; player)
			{
				if (player->pos == masu_pos)
				{
					player->is_move_select = true;
					is_finded = true;
					save_pos = masu_pos;
					objects.select_s_obj.emplace(&(*player));
				}
			}

			// 敵
			for (auto& enemy : objects.GetChangeObjects<Enemy>())
			{
				// 既に見つかっているのでとばす
				if (is_finded)	break;

				if (enemy.pos == masu_pos)
				{
					enemy.is_move_select = true;
					is_finded = true;
					save_pos = masu_pos;
					objects.select_m_obj = &enemy;
				}
			}

			// 鍵
			for (auto& key : objects.GetChangeObjects<Key>())
			{
				// 既に見つかっているのでとばす
				if (is_finded)	break;

				if (key.pos == masu_pos)
				{
					key.is_move_select = true;
					is_finded = true;
					save_pos = masu_pos;
					objects.select_m_obj = &key;
				}
			}
		}
		// 重なっていないので他を検索
		else
		{
			// single_objects
			objects.SingleObjVisit([&](auto& data)
				{
					if (data && (data->pos == masu_pos))
					{
						data->is_move_select = true;
						is_finded = true;
						save_pos = masu_pos;
						objects.select_s_obj = &(*data);
					}
				});

			// multiple_objects
			objects.MultiObjVisit([&](auto& data)
				{
					for (auto& dt : data)
					{
						// 既に見つかっているのでとばす
						if (is_finded)	break;

						if (dt.pos == masu_pos)
						{
							is_finded = true;
							dt.is_move_select = true;
							save_pos = masu_pos;
							objects.select_m_obj = &dt;
						}
					}
				});
		}

		// 発見・未発見で判断する
		now_move_object = is_finded;
	}
	// 左クリック離した時（配置）
	else if (GMLIB->isMouseUp(Buttons::middleButton))
	{
		bool is_finded{ false };
		size_t count{ 0u };

		now_move_object = false;

		objects.SingleObjVisit([&](const auto& data)
			{ if (data && data->pos == masu_pos) count++; });

		objects.MultiObjVisit([&](const auto& data)
			{ count += std::count_if(data.cbegin(), data.cend(), [&](const auto& dt)
				{ return (dt.pos == masu_pos); }); });

		// 配置先のマスに何も設置されていない
		if (count <= 1u)  // 自分を除いた数値
		{
			auto HaveKeyEnmPl{ [&]()
			{ return (objects.GetSelectObjectPtr<Player>() ||
					  objects.GetSelectObjectPtr<Enemy>() ||
					  objects.GetSelectObjectPtr<Key>()); } };

			// 配置先が無で自分自身は鍵・自機・敵
			const bool is_plane_up_only{ HaveKeyEnmPl() };

			// single_objectsを保持
			if (objects.select_s_obj)
			{
				// 選択を解除（ただし、自分自身が自機なら元の場所へ戻す）
				std::visit([is_plane_up_only](auto& data)
					{
						if (data)
						{
							data->is_move_select = false;
							if (is_plane_up_only) data->pos = save_pos;
						}
					}, *objects.select_s_obj);

				objects.select_s_obj = std::nullopt;
			}
			// multiple_objectsを保持
			else if (objects.select_m_obj)
			{
				// 選択を解除（ただし、自分自身が鍵・敵なら元の場所へ戻す）
				std::visit([is_plane_up_only](auto& data)
					{
						if (data)
						{
							data->is_move_select = false;
							if (is_plane_up_only) data->pos = save_pos;
						}
					}, *objects.select_m_obj);

				objects.select_m_obj = std::nullopt;
			}
		}
		// 配置先のマスに何ががある
		else
		{
			// 配置先が床で自分自身は鍵・自機・敵
			{
				auto RePlaceObject{ [&]()
				{ return (IsSamePosObject<Plane>() &&
					!objects.GetSelectObjectPtr<Plane>()); } };

				auto HaveKeyEnmPl{ [&]()
				{ return (objects.GetSelectObjectPtr<Player>() ||
						  objects.GetSelectObjectPtr<Enemy>() ||
						  objects.GetSelectObjectPtr<Key>()); } };

				// 配置先が床で自分自身は鍵・自機・敵（設置可能）
				if (RePlaceObject() && HaveKeyEnmPl())
				{
					// single_objectsを保持
					if (objects.select_s_obj)
					{
						// 選択を解除
						std::visit([](auto& data) { data->is_move_select = false; }, *objects.select_s_obj);

						objects.select_s_obj = std::nullopt;
					}
					// multiple_objectsを保持
					else if (objects.select_m_obj)
					{
						// 選択を解除
						std::visit([](auto& data) { data->is_move_select = false; }, *objects.select_m_obj);

						objects.select_m_obj = std::nullopt;
					}

					return;  // これ以上は全て設置不可能なので以降処理しない
				}
			}

			// 上記パターン以外は全て設置不可能となる
			{
				// single_objectsを保持
				if (objects.select_s_obj)
				{
					// 選択を解除
					std::visit([](auto& data) { data->is_move_select = false; data->pos = save_pos; },
						*objects.select_s_obj);

					objects.select_s_obj = std::nullopt;
				}
				// multiple_objectsを保持
				else
				{
					// 選択を解除
					std::visit([](auto& data) { data->is_move_select = false; data->pos = save_pos; },
						*objects.select_m_obj);

					objects.select_m_obj = std::nullopt;
				}
			}
		}
	}
#endif
}

// 読み込み
void ObjectManager::Load(std::atomic<size_t>* load_count, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// モデル読み込み
	{
		auto fbx_paths{ GetAllFileName("data\\models\\Game") };

		// ファイル走査で読み込むので（アルファベット順）
		const std::vector<decltype(&Door::model)> load_models{
			&Door::model,
			&Enemy::model,
			&Goal::model,
			&Key::model,
			&Player::model,
			&Plane::sand_broken_model,
			&Plane::sand_model,
			&Wall::sand_model,
			&Plane::snow_model,
			&Plane::snow_broken_model,
			&Wall::snow_model,
			&Start::model,
			&WarpHole::warp_sand,
			&WarpHole::warp_snow,
		};

		// FBX以外のファイル名を削除
		fbx_paths.erase(std::remove_if(fbx_paths.begin(), fbx_paths.end(),
			[](const auto& path) { return path.extension().string() != ".fbx"; }), fbx_paths.end());

		assert(fbx_paths.size() == load_models.size() && "読み込み数or設定数が不正");

		// FBX読み込み
		for (size_t i = 0, length = fbx_paths.size(); i < length; i++)
		{
			assert(!(*load_models[i]) && "既に読み込まれている");

			*load_models[i] = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, fbx_paths[i], 1000u);

			(*load_count)++;
		}
	}

	// テクスチャ読み込み
	{
		auto png_paths{ GetAllFileName("data\\images\\Game") };

		// ファイル走査で読み込むので（アルファベット順）
		std::vector<decltype(&crystal_board)> load_textures{
			&crystal_board,
			&Goal::board,
			&sand_board,
		};

		// 画像以外のファイル名を削除
		png_paths.erase(std::remove_if(png_paths.begin(), png_paths.end(),
			[](const auto& path) { return path.extension().string() != ".png"; }), png_paths.end());

		assert(png_paths.size() == load_textures.size() && "読み込み数or設定数が不正");

		// 画像読み込み
		for (size_t i = 0, length = load_textures.size(); i < length; i++)
		{
			*load_textures[i] = std::make_unique<KDL::DX12::Geometric_Board>(p_app, png_paths[i], 10000u);

			(*load_count)++;
		}
	}
}

// 解放処理
void ObjectManager::UnInitialize()
{
	const std::vector<decltype(Door::model)*> release_models{
		&Door::model,
		&Enemy::model,
		&Goal::model,
		&Key::model,
		&Player::model,
		&Plane::sand_broken_model,
		&Plane::sand_model,
		&Wall::sand_model,
		&Plane::snow_model,
		&Plane::snow_broken_model,
		&Wall::snow_model,
		&Start::model,
		&WarpHole::warp_sand,
		&WarpHole::warp_snow,
	};

	const std::vector<decltype(crystal_board)*> release_textures{
		&Goal::board,
		&crystal_board,
		&sand_board,
	};

	auto Clear{ [](auto& load_data) { std::for_each(load_data.begin(), load_data.end(), [](auto& data)
		{ *data = nullptr; }); } };

	Clear(release_models);
	Clear(release_textures);
}

// 書き出し---------------------------------------------------------------------------------------------------
void ObjectManager::Output(std::ofstream& ofs)
{
	// オブジェクト情報保存
	ofs << objects;

	// パスの設定
	node.PathSetting(objects);

	auto& enemy{ objects.GetChangeObjects<Enemy>() };

	for (auto& enm : enemy)
		enm.SetNode(node);
}

// 読み込み---------------------------------------------------------------------------------------------------
void ObjectManager::Input(std::ifstream& ifs)
{
	// オブジェクト情報取得
	ifs >> objects;

	// パスの設定
	node.PathSetting(objects);

	auto& enemy{ objects.GetChangeObjects<Enemy>() };

	for (auto& enm : enemy)
		enm.SetNode(node);
}

std::ofstream& operator<<(std::ofstream& ofs, const Object& data)
{
	using namespace std::string_literals;

	// single_objects
	data.SingleObjVisit([&](const auto& dt)
		{
			OutputComment(ofs, typeid(decltype(dt)).name(), "name", "", 2u);

			const bool exist{ dt };

			ofs << F_OUT(exist) << std::endl;

			if (exist) { ofs << (*dt); }

			ofs << std::endl;
			ofs << std::endl;
			ofs << std::endl;
		});

	// multiple_objects
	data.MultiObjVisit([&](const auto& dt)
		{
			OutputComment(ofs, typeid(decltype(dt)).name(), "Class", "", 2u);

			const size_t size{ dt.size() };

			ofs << F_OUT(size) << std::endl;

			if (size != 0)
			{
				for (auto& obj : dt)
				{
					ofs << obj;
				}

				ofs << std::endl;
				ofs << std::endl;
			}

			ofs << std::endl;
		});

	// 敵のAI
	{
		const std::vector<float*> ai_out
		{
			&MoveAI::move_speed,
			nullptr,
			&StopAI::stop_time,
			nullptr,
			&FindAI::stop_time,
			nullptr,
			&PatrolAI::hit_distance,
			&PatrolAI::hit_angle,
			nullptr,
			&ChaseAI::move_speed,
			&ChaseAI::max_chase_time,
			nullptr,
			&GoBackAI::move_speed,
		};

		OutputComment(ofs, "敵AI情報", "", "", 2u);
		for (auto& out : ai_out)
		{
			if (out)
				ofs << F_OUT(*out);
			else
				ofs << std::endl;
		}
	}

	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, Object& data)
{
	// single_objects
	data.SingleObjVisit([&](auto& dt)
		{
			// コメント取得
			InputComment(ifs);

			bool exist{ false };

			ifs >> exist;

			InputComment(ifs);

			if (exist)
			{
				auto& back{ data.AddObjects<decltype(*dt)>() };

				ifs >> back;
			}
		});

	// multiple_objects
	data.MultiObjVisit([&](auto& dt)
		{
			// コメント取得
			InputComment(ifs);

			size_t size{ 0u };

			ifs >> size;

			for (size_t i = 0; i < size; i++)
			{
				auto& back{ data.AddObjects<decltype(dt[i])>() };

				ifs >> back;
			}
		});

	// 敵のAI
	{
		const std::vector<float*> ai_in
		{
			&MoveAI::move_speed,
			&StopAI::stop_time,
			&FindAI::stop_time,
			&PatrolAI::hit_distance,
			&PatrolAI::hit_angle,
			&ChaseAI::move_speed,
			&ChaseAI::max_chase_time,
			&GoBackAI::move_speed,
		};

		InputComment(ifs);
		for (auto& in : ai_in)
		{
			ifs >> (*in);
		}
	}

	return ifs;
}