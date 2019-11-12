#include "ObjectManager.h"
#include "GameScene.h"
#include "KDL.h"
#include "ImVecHelper.h"
#include "LoadAllFileName.h"
#include "Time.h"

ObjectManager::ObjectManager()
	: pl_handle(-1), start_handle(-1), goal_handle(-1),
	enm_handle(-1), glass_handle(-1), glass_broken_handle(-1), wall_handle(-1), warp_handle(-1),
	door_handle(-1), key_handle(-1),
	select_enm(nullptr), select_waypoint(nullptr), masu_pos(0.f, 0.f, 0.f)
{
	now_move_object = edit_mode_first = false;
}

ObjectManager::~ObjectManager() noexcept
{
	std::vector<int*> erase_handle{
		// SingleObjets
		&pl_handle,
		&start_handle,
		&goal_handle,

		// multiple_objects
		&enm_handle,
		&glass_handle,
		&glass_broken_handle,
		&wall_handle,
		&warp_handle,
		&door_handle,
		&key_handle,
		&waypoint_handle,
		&arrow_handle
	};

	assert(GetObjectKindsNum() == erase_handle.size() && "削除するハンドルを設定し忘れている");

	// 削除する
	for (auto handle : erase_handle)
	{
		// 実害はないのでこうしている
		GMLIB->DeleteModel(*handle);
		GMLIB->DeleteBoardImageHandle(*handle);
	}
}

// 更新
void ObjectManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	// 各オブジェクト更新
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

			auto& plane{ objects.GetChangeObjects<Plane>() };

			std::for_each(plane.begin(), plane.end(), [](auto& data) { data.InversionProc(); });
		}
		else if (!first && player && !WarpHoleCheck())
		{
			first = true;
		}
	}

	edit_mode_first = true;

	// 地面にプレーヤー座標をセット
	if (const auto& player{ objects.GetObjects<Player>() }; player)
		Plane::SetPlPosition(player->pos);
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
	auto fbx_paths{ GetAllFileName("Data\\Model\\Game") };

	// ファイル走査で読み込むので（アルファベット順）
	const std::vector<int*> load_handle{
		&arrow_handle,
		&door_handle,
		&enm_handle,
		&goal_handle,
		&key_handle,
		&pl_handle,
		&start_handle,
		&wall_handle,
		&warp_handle,
		&waypoint_handle,
	};

	fbx_paths.erase(std::remove_if(fbx_paths.begin(), fbx_paths.end(),
		[](const auto& path) { return path.extension().string() != ".fbx"; }), fbx_paths.end());

	assert(fbx_paths.size() == load_handle.size() && "読み込み数or設定数が不正");

	// FBX読み込み
	for (size_t i = 0, length = fbx_paths.size(); i < length; i++)
	{
		if (!GMLIB->LoadModel(fbx_paths[i], *load_handle[i]))
			assert(!"読み込み失敗");

		(*load_count)++;
	}

	// テクスチャ読み込み
	{
		if (!GMLIB->LoadBoadImage(L"Data\\Texture\\Glass.png", glass_handle))
			assert(!"読み込み失敗");

		(*load_count)++;

		if (!GMLIB->LoadBoadImage(L"Data\\Texture\\GlassBroken.png", glass_broken_handle))
			assert(!"読み込み失敗");

		(*load_count)++;
	}

	// variantオブジェクト構築
	{
		// SingleObjets構築
		objects.BuildVariant<Player>({ pl_handle });
		objects.BuildVariant<Start>({ start_handle });
		objects.BuildVariant<Goal>({ goal_handle });

		// multiple_objects構築
		objects.BuildVariant<Enemy>({ enm_handle, waypoint_handle, arrow_handle });
		objects.BuildVariant<Plane>({ glass_handle, glass_broken_handle });
		objects.BuildVariant<Wall>({ wall_handle });
		objects.BuildVariant<WarpHole>({ warp_handle });
		objects.BuildVariant<Door>({ door_handle });
		objects.BuildVariant<Key>({ key_handle });
	}

	Enemy::SetNodeData(node);
}

// 書き出し---------------------------------------------------------------------------------------------------
void ObjectManager::Output(std::ofstream& ofs)
{
	// パスの設定
#if _DEBUG
	node.PathSetting(objects);
#endif

	// オブジェクト情報保存
	ofs << objects;
}

// 読み込み---------------------------------------------------------------------------------------------------
void ObjectManager::Input(std::ifstream& ifs)
{
	// オブジェクト情報取得
	ifs >> objects;

	// パスの設定
#if !_DEBUG
	node.PathSetting(objects);
#endif
}

std::ofstream& operator<<(std::ofstream& ofs, const Object& data)
{
	using namespace std::string_literals;

	// single_objects
	data.SingleObjVisit([&](const auto& dt)
		{
			OutputComment(ofs, typeid(decltype(dt)).name(), "name", "", 2u);

			const bool exist{ dt };

			ofs << F_OUT(exist);

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

			ofs << F_OUT(size);

			for (auto& obj : dt)
			{
				ofs << obj;
			}

			ofs << std::endl;
			ofs << std::endl;
			ofs << std::endl;
		});

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
				if constexpr (std::is_same<Enemy, decltype(dt.front())>())
				{
					auto& back{ data.AddObjects<decltype(dt[i])>() };

					ifs >> back;
				}
				else
				{
					auto& back{ data.AddObjects<decltype(dt[i])>() };

					ifs >> back;
				}
			}
		});

	return ifs;
}

// パス情報の設定--------------------------------------------------------------------------------------------
void Node::PathSetting(Object& objects)
{
	auto& planes{ objects.GetChangeObjects<Plane>() };      // 床
	auto& doors{ objects.GetChangeObjects<Door>() };        // ドア
	auto& warphole{ objects.GetChangeObjects<WarpHole>() }; // ワープホール

	// 初期化
	data.clear();

	auto PositionSetting{ [&](const auto& set_obj) { std::for_each(set_obj.cbegin(), set_obj.cend(),
		[&](const auto& obj) { data.emplace_back(obj.pos); }); } };

	// ノードを追加し座標をセットする
	PositionSetting(planes);
	//PositionSetting(doors);
	PositionSetting(warphole);

	// 床の周囲の情報を取得
	for (size_t i = 0, length{ planes.size() }; i < length; i++)
	{
		auto& base_node{ data[i] };

		VF3 left{ base_node.pos }, right{ base_node.pos }, up{ base_node.pos }, down{ base_node.pos };

		right.x -= 1.f;
		left.x += 1.f;
		up.z -= 1.f;
		down.z += 1.f;

		for (size_t j = 0; j < length; j++)
		{
			auto& other_node{ data[j] };

			// 自分自身ならとばす
			if (i == j)	continue;

#if false
			// 左（存在しないなら追加）
			if (!base_node.plane_index[Dir::Left] && left == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Left) = make_optional(j);

				// 反対側も追加
				if (!other_node.plane_index[Dir::Right])
					other_node.plane_index.at(Dir::Right) = make_optional(i);

				continue;
			}

			// 右（存在しないなら追加）
			if (!base_node.plane_index[Dir::Right] && right == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Right) = make_optional(j);

				// 反対側も追加
				if (!other_node.plane_index[Dir::Left])
					other_node.plane_index.at(Dir::Left) = make_optional(i);

				continue;
			}

			// 上（存在しないなら追加）
			if (!base_node.plane_index[Dir::Up] && up == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Up) = make_optional(j);

				// 反対側も追加
				if (!other_node.plane_index[Dir::Down])
					other_node.plane_index.at(Dir::Down) = make_optional(i);

				continue;
}

			// 下（存在しないなら追加）
			if (!base_node.plane_index[Dir::Down] && down == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Down) = make_optional(j);

				// 反対側も追加
				if (!other_node.plane_index[Dir::Up])
					other_node.plane_index.at(Dir::Up) = make_optional(i);

				continue;
			}
#else
			const std::array<VF3, 4u> poses{ left, right, up, down };

			auto ConnectNodeCheck{ [&](const VF3& pos)
			{ return (other_node.pos == pos && ConnectNode(&base_node, &other_node)); } };

			// 各方向にノードを検索し、発見したら接続する
			if (std::any_of(poses.begin(), poses.end(), [&](const VF3& pos)
				{ return ConnectNodeCheck(pos); }))
				continue;
#endif
		}
	}
}

NodeData* Node::PathFindingDijkstra(const VF3& base_pos, const VF3& target_pos, int64_t* processing_time)
{
#if false
	using Dir = NodeData::DirInfo;
	using std::make_pair;

	assert(nodes && !nodes->empty() && "地面が存在しない");

	// 探索済みフラグを解除
	std::for_each(nodes->begin(), nodes->end(), [](auto& plane) { plane.is_finded = false; });

	size_t index_count_num{ 0u };

	// 初期化
	for (size_t i = 0u, length = nodes->size(); i < length; i++)
	{
		auto& plane{ nodes->at(i) };

		plane.is_finded = false;
		plane.cache_data.clear();

		// 基本座標に存在する床を取得する
		if (plane.pos == base_pos) index_count_num = i;
	}

	// 最初に、探索もとになるノードを追加（自分自身）
	//nodes->at(index_count_num).cache_data.emplace_back(nodes->at(index_count_num).pos);

	Deque<Plane*> save_planes;

	while (true)
	{
		// （基本座標からの距離、目的地への距離）、添え値
		std::deque<std::pair<std::pair<float, float>, uint32_t>> save_dis;

		bool is_end{ false };

		auto& next_plane{ nodes->at(index_count_num) };

		// 次の床が目的のパスの場合は発見
		if (next_plane.pos == target_pos)	is_end = true;

		save_planes.emplace_back(&next_plane);

		for (auto s_plane : save_planes)
		{
			// その床の近隣の床で目的の床に一番近い床を探す
			for (auto& index : s_plane->plane_index)
			{
				using Math::EasyDistance;

				const uint32_t index_num{ *(index.second) };

				// その方向に床は存在しない
				if (!index.second)	continue;

				auto& dir_plane{ nodes->at(index_num) };

				// その方向の床は既に探索済み
				if (dir_plane.is_finded)	continue;

				// その方向の基本座標からの距離とゴールへの距離を求め、添え値を追加
				save_dis.emplace_back(
					make_pair(make_pair(EasyDistance(base_pos, dir_plane.pos),
						EasyDistance(target_pos, dir_plane.pos)), *(index.second)));
			}

			// このマスを探索済みとする
			s_plane->is_finded = true;
		}

		// （基本座標からの距離、合計距離）、添え値
		decltype(save_dis) near_dis{ save_dis };

		// 合計距離を最小値のみにする
		{
			// 合計距離に変換
			for (auto& n_dis : near_dis)
				n_dis.first.second = n_dis.first.first + n_dis.first.second;

			// 合計距離の昇順に並び替える
			std::stable_sort(near_dis.begin(), near_dis.end(), [](auto& dis1, auto& dis2)
				{ return (dis1.first.second) < (dis2.first.second); });

			const float nearest_dis{ near_dis.front().first.second };

			// 合計距離の最小値より大きい場合は消す
			near_dis.erase(std::remove_if(near_dis.begin(), near_dis.end(), [nearest_dis](const auto& dis)
				{ return dis.first.second > nearest_dis; }), near_dis.end());
		}

		// 基本座標からの距離が遠い物を選択
		{
			assert(near_dis.size() != 0u && "不正なサイズ");

			// 基本座標からの距離が一番遠い床を探す
			const auto get_max_element{ *std::max_element(near_dis.cbegin(), near_dis.cend(),
				[&](auto& dis1, auto& dis2) { return dis1.first < dis2.first; }) };

			const auto& before_plane{ nodes->at(index_count_num) };

			// 次の探す床のインデックスを決定
			index_count_num = get_max_element.second;

			auto& after_cache_data{ nodes->at(index_count_num).cache_data };

			// 探索もとになるノードを追加
			after_cache_data = before_plane.cache_data;

			// 探索もとになるノードを追加（自分自身）
			after_cache_data.emplace_back(before_plane.pos);
	}

		if (is_end)		break;
}
#else
#define DATA_SET	data.begin(), data.end()

	Timer time;

	if(processing_time)
		time.Start();

	std::for_each(DATA_SET, [](auto& dt) { dt.Reset(); });

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == base_pos); }) && "基本座標がノード上に存在しない");

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == target_pos); }) && "目的座標がノード上に存在しない");

	std::deque<NodeData*> work1, work2;

	auto* currLevel{ &work1 };
	auto* nextLevel{ &work2 };

	auto goal{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == target_pos); }) };
	auto start{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == base_pos); }) };

	//ゴールから計算するので、コスト0をセットして計算済みとする
	goal->cost = 0;

	//検索第一階層のノードセット(ゴールノードをセットする)
	currLevel->emplace_back(&(*goal));

	std::list<Node*>::iterator it;
	int nodeCost;

	while (currLevel->size())
	{
		for (auto* it : *currLevel)
		{
			for (auto& it_cnct : it->coonect_node)
			{
				using Math::EasyDistance;

				nodeCost = it->cost + it_cnct.cost;

				//未探索ノードあるいは最短ルートを更新できる場合
				if (auto* node = it_cnct.next_node; (!node->is_finded || nodeCost < node->cost))
				{
					//経路コストとゴールへ向かうためのノードをセット
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//次に検索する階層のリストに登録
				nextLevel->emplace_back(it_cnct.next_node);
			}
		}

		//リストを入れ替えて次の階層を検索する
		std::swap(nextLevel, currLevel);

		nextLevel->clear();//クリアする
	}

	if (processing_time)
	{
		time.End();
		*processing_time = time.GetMicroTimer();
	}

	return &(*start);

#endif
}

NodeData& Node::PathFindingAstar(const VF3& base_pos, const VF3& target_pos, int64_t* processing_time)
{
#if false
	using Dir = NodeData::DirInfo;
	using std::make_pair;

	assert(nodes && !nodes->empty() && "地面が存在しない");

	// 探索済みフラグを解除
	std::for_each(nodes->begin(), nodes->end(), [](auto& plane) { plane.is_finded = false; });

	size_t index_count_num{ 0u };

	// 初期化
	for (size_t i = 0u, length = nodes->size(); i < length; i++)
	{
		auto& plane{ nodes->at(i) };

		plane.is_finded = false;
		plane.cache_data.clear();

		// 基本座標に存在する床を取得する
		if (plane.pos == base_pos) index_count_num = i;
	}

	// 最初に、探索もとになるノードを追加（自分自身）
	//nodes->at(index_count_num).cache_data.emplace_back(nodes->at(index_count_num).pos);

	Deque<Plane*> save_planes;

	while (true)
	{
		// （基本座標からの距離、目的地への距離）、添え値
		std::deque<std::pair<std::pair<float, float>, uint32_t>> save_dis;

		bool is_end{ false };

		auto& next_plane{ nodes->at(index_count_num) };

		// 次の床が目的のパスの場合は発見
		if (next_plane.pos == target_pos)	is_end = true;

		save_planes.emplace_back(&next_plane);

		for (auto s_plane : save_planes)
		{
			// その床の近隣の床で目的の床に一番近い床を探す
			for (auto& index : s_plane->plane_index)
			{
				using Math::EasyDistance;

				const uint32_t index_num{ *(index.second) };

				// その方向に床は存在しない
				if (!index.second)	continue;

				auto& dir_plane{ nodes->at(index_num) };

				// その方向の床は既に探索済み
				if (dir_plane.is_finded)	continue;

				// その方向の基本座標からの距離とゴールへの距離を求め、添え値を追加
				save_dis.emplace_back(
					make_pair(make_pair(EasyDistance(base_pos, dir_plane.pos),
						EasyDistance(target_pos, dir_plane.pos)), *(index.second)));
			}

			// このマスを探索済みとする
			s_plane->is_finded = true;
		}

		// （基本座標からの距離、合計距離）、添え値
		decltype(save_dis) near_dis{ save_dis };

		// 合計距離を最小値のみにする
		{
			// 合計距離に変換
			for (auto& n_dis : near_dis)
				n_dis.first.second = n_dis.first.first + n_dis.first.second;

			// 合計距離の昇順に並び替える
			std::stable_sort(near_dis.begin(), near_dis.end(), [](auto& dis1, auto& dis2)
				{ return (dis1.first.second) < (dis2.first.second); });

			const float nearest_dis{ near_dis.front().first.second };

			// 合計距離の最小値より大きい場合は消す
			near_dis.erase(std::remove_if(near_dis.begin(), near_dis.end(), [nearest_dis](const auto& dis)
				{ return dis.first.second > nearest_dis; }), near_dis.end());
		}

		// 基本座標からの距離が遠い物を選択
		{
			assert(near_dis.size() != 0u && "不正なサイズ");

			// 基本座標からの距離が一番遠い床を探す
			const auto get_max_element{ *std::max_element(near_dis.cbegin(), near_dis.cend(),
				[&](auto& dis1, auto& dis2) { return dis1.first < dis2.first; }) };

			const auto& before_plane{ nodes->at(index_count_num) };

			// 次の探す床のインデックスを決定
			index_count_num = get_max_element.second;

			auto& after_cache_data{ nodes->at(index_count_num).cache_data };

			// 探索もとになるノードを追加
			after_cache_data = before_plane.cache_data;

			// 探索もとになるノードを追加（自分自身）
			after_cache_data.emplace_back(before_plane.pos);
		}

		if (is_end)		break;
	}
#else
#define DATA_SET	data.begin(), data.end()

	Timer time;

	if (processing_time)
		time.Start();

	std::for_each(DATA_SET, [](auto& dt) { dt.Reset(); });

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == base_pos); }) && "基本座標がノード上に存在しない");

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == target_pos); }) && "目的座標がノード上に存在しない");

	std::deque<NodeData*> work1, work2;

	auto* currLevel{ &work1 };
	auto* nextLevel{ &work2 };

	auto goal{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == target_pos); }) };
	auto start{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == base_pos); }) };

	//ゴールから計算するので、コスト0をセットして計算済みとする
	goal->cost = 0;

	//検索第一階層のノードセット(ゴールノードをセットする)
	currLevel->emplace_back(&(*goal));

	std::list<Node*>::iterator it;
	int nodeCost;

	while (currLevel->size())
	{
		for (auto* it : *currLevel)
		{
#if false
			for (auto& connect : it->coonect_node)
			{
				nodeCost = it->cost + connect.cost;

				//未探索ノードあるいは最短ルートを更新できる場合
				if (auto* node = connect.next_node; (!node->is_finded || nodeCost < node->cost))
				{
					//経路コストとゴールへ向かうためのノードをセット
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//次に検索する階層のリストに登録
				nextLevel->emplace_back(connect.next_node);
			}
#else
			// ノードデータ、(スタートからの距離、ゴールへの距離)
			std::deque<std::pair<NodeData*, std::pair<float, float>>> connect_nodes;

			// 並び替え用に構築
			for (NodeData::NodeConnect& node : it->coonect_node)
			{
				constexpr float Fmax{ (std::numeric_limits<float>::max)() };

				connect_nodes.emplace_back(node.next_node, std::make_pair(Fmax, Fmax));
			}

			// 合計距離を最小値のみにする
			{
				// 合計距離に変換
				for (auto& c_node : connect_nodes)
				{
					using Math::EasyDistance;

					const auto next_node{ c_node.first };
					const float start_dis{ EasyDistance(next_node->pos, start->pos) };
					const float goal_dis{ EasyDistance(next_node->pos, goal->pos) };

					c_node.second = std::make_pair(start_dis, goal_dis);
				}

				// 合計距離の昇順に並び替える
				std::stable_sort(connect_nodes.begin(), connect_nodes.end(), [](auto& dis1, auto& dis2)
					{
						float total_dis1{ dis1.second.first + dis1.second.second };
						float total_dis2{ dis2.second.first + dis2.second.second };

						return (total_dis1) < (total_dis2);
					});

				const float nearest_dis
				{ connect_nodes.front().second.first + connect_nodes.front().second.second };

				// 合計距離の最小値より大きい場合は消す
				connect_nodes.erase(std::remove_if(
					connect_nodes.begin(), connect_nodes.end(), [nearest_dis](const auto& dis)
					{ return dis.second.first + dis.second.second > nearest_dis; }), connect_nodes.end());
			}

			// 基本座標からの距離が遠い物を選択
			{
				assert(connect_nodes.size() != 0u && "不正なサイズ");

				// 基本座標からの距離が一番遠い床を探す
				const auto get_max_element{ *std::max_element(connect_nodes.cbegin(), connect_nodes.cend(),
					[&](auto& dis1, auto& dis2) { return dis1.second.first < dis2.second.first; }) };

				nodeCost = get_max_element.first->cost + get_max_element.first->cost;

				//未探索ノードあるいは最短ルートを更新できる場合
				if (auto* node = get_max_element.first; (!node->is_finded || nodeCost < node->cost))
				{
					//経路コストとゴールへ向かうためのノードをセット
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//次に検索する階層のリストに登録
				nextLevel->emplace_back(get_max_element.first);
			}
#endif
		}

		//リストを入れ替えて次の階層を検索する
		std::swap(nextLevel, currLevel);

		nextLevel->clear();//クリアする
	}

	if (processing_time)
	{
		time.End();
		*processing_time = time.GetMicroTimer();
	}

	return *start;

#endif
}
