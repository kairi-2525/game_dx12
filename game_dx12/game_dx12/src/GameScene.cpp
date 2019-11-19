#include "GameScene.h"
//#include "scene.h"
#include "Math.h"
#include "KDL.h"
#include "ImVecHelper.h"
#include "SceneSelect.h"
#include "LoadAllFileName.h"
#include <string>

//todo KDL_USE_IMGUI USE_IMGUIの代わり

void SceneGame::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	namespace fs = std::filesystem;

	// 初期化
	object_manager.emplace();
	file_flg = false;
	camera_angle = { Math::ToRadian(-80.f), 0.f, 0.f };
	camera_dis = 50.f;
	FadeTimeInit();

	execution_quick_exit = false;
	masu_pos = Fill3(0.f);
	edit_mode = false;
	enm_edit_mode = false;
	is_save = false;
	back_world_mode = false;
	init_scene = false;

	// 構築
	{
		// variantオブジェクト構築
		{
			// SingleObjets構築
			object_manager->GetObjectData().BuildVariant<Player>();
			object_manager->GetObjectData().BuildVariant<Start>();
			object_manager->GetObjectData().BuildVariant<Goal>();

			// multiple_objects構築
			object_manager->GetObjectData().BuildVariant<Enemy>();
			object_manager->GetObjectData().BuildVariant<Plane>();
			object_manager->GetObjectData().BuildVariant<Wall>();
			object_manager->GetObjectData().BuildVariant<WarpHole>();
			object_manager->GetObjectData().BuildVariant<Door>();
			object_manager->GetObjectData().BuildVariant<Key>();
		}
	}

	// 一時ファイルが存在しているなら、削除
	//if (fs::exists(TempFileDir) && !std::filesystem::remove(TempFileDir))
	//	assert(!"一時ファイルの削除に失敗");

	camera =
		std::make_unique<KDL::TOOL::Camera>(
			KDL::TOOL::Camera(
				{ 0.f, 0.f, -30.f },		//eye
				{ 0.f, 0.f, 0.f },			//focus
				{ 0.f, 1.f, 0.f },			//up
				DirectX::XMConvertToRadians(30.f),
				p_app->GetViewport().Width / p_app->GetViewport().Height,
				{ 0.1f, 1000.f }
			)
			);

	camera->SetPosition(CreateRotationPos(camera_angle, 30.f));

	Enemy::player = &object_manager->GetObjectData<Player>();
	Enemy::walls = &object_manager->GetObjectData<Wall>();

	Input(open_file_path);

	// スタート地点にセットする
	if (auto& player{ object_manager->GetObjectData().GetChangeObjects<Player>() }; player)
	{
		player->pos = object_manager->GetObjectData<Start>()->pos;
	}

	auto* audio = p_window->GetAudio();
	audio->Stop(bgm_handle, bgm_handle_p, 1.0f);
	bgm_handle_p = audio->CreatePlayHandle(bgm_handle, 0.f, true, false, 0.f, 0.f, 0, false, false);
	audio->Play(bgm_handle, bgm_handle_p, 0.01f, 0.2f, false);
}

void SceneGame::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	namespace fs = std::filesystem;

	object_manager->UnInitialize();

	object_manager = std::nullopt;
	camera = nullptr;

	// 一時ファイルが存在しているなら、削除
	//if (fs::exists(TempFileDir) && !std::filesystem::remove(TempFileDir))
	//	assert(!"一時ファイルの削除に失敗");

	auto* audio = p_window->GetAudio();
	audio->Delete(bgm_handle);
	audio->Delete(se_break);
	audio->Delete(se_crack);
	audio->Delete(se_door);
	audio->Delete(se_waap);
	audio->Delete(se_whistle);
	audio = nullptr;
}

void SceneGame::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;

	// モード変更処理
#if false
	{
		static bool save_f1{ false };

		// モード変更
		ModeChange(&save_f1, p_scene_mgr, p_window, p_app);

		// 確認画面中は更新しない
		if (save_f1)	return;
	}
#endif

#ifdef KDL_USE_IMGUI
#if false
	// メインメニュー
	if (edit_mode)
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu(u8"ファイル"))
		{
			if (ImGui::MenuItem(u8"保存", "CTRL+S"))				file_flg.is_save = true;
			//if (ImGui::MenuItem(u8"別名保存", "CTRL+SHIFT+S"))			file_flg.is_save_as = true;
			if (ImGui::MenuItem(u8"開く", "CTRL+O"))				file_flg.is_open = true;
			//if (ImGui::MenuItem(u8"削除"	, "CTRL+SHIFT+ALT+DELETE"))	file_flg.is_delete  = true;
			//if (ImGui::MenuItem(u8"閉じる"	, "CTRL+ALT+C"))			file_flg.is_close   = true;
			//if (ImGui::MenuItem(u8"名前変更", "CTRL+R"))				file_flg.is_rename  = true;
			//if (ImGui::MenuItem(u8"コピー"	, "CTRL+SHIFT+C"))			file_flg.is_copy    = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"編集"))
		{
			//ImGui::MenuItem(u8"コピー", "CTRL+C", nullptr, false);
			//ImGui::MenuItem(u8"カット", "CTRL+X", nullptr, false);
			//ImGui::MenuItem(u8"ペースト", "CTRL+V", nullptr, false);
			//ImGui::MenuItem("ReDo", "CTRL+Z", nullptr, false);
			//ImGui::MenuItem("UnDo", "CTRL+Y", nullptr, false);

			//ImGui::Separator();

			ImGui::MenuItem(u8"設置", u8"左クリック", nullptr, false);
			ImGui::MenuItem(u8"移動", u8"ホイールクリック(hold)", nullptr, false);
			ImGui::MenuItem(u8"削除", u8"右クリック", nullptr, false);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"カメラ"))
		{
			ImGui::MenuItem(u8"右", "D", nullptr, false);
			ImGui::MenuItem(u8"左", "S", nullptr, false);
			ImGui::MenuItem(u8"上", "W", nullptr, false);
			ImGui::MenuItem(u8"下", "S", nullptr, false);
			ImGui::MenuItem(u8"拡大", "PGUP", nullptr, false);
			ImGui::MenuItem(u8"縮小", "PGDN", nullptr, false);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImguiTool::BeginShowTempWindow({ 0.f, 50.f }, LabelName.data());

	// 世界線表示
	if (!enm_edit_mode)  /// 敵編集モード以外表示
	{
		const auto vp{ p_app->GetViewport() };
		const VF2 s_size{ vp.Width, vp.Height };

		ImguiTool::BeginShowTempWindow({ s_size.x - 120.f, 50.f }, u8"世界線");

		ImGui::TextColored({ IMGUI_RED }, back_world_mode ? u8"裏世界" : u8"表世界");

		// 編集モード時だけ自由に変更できるように
		if (edit_mode && ImGui::Button(u8"切り替え"))	back_world_mode ^= true;

		ImGui::End();
	}

	// モード表示
	{
		static const ImVec4 Color{ IMGUI_RED };

		// 位置調整
		{
			constexpr VF2 w_size{ 150.f, 60.f };
			const auto vp{ p_app->GetViewport() };
			const VF2 s_size{ vp.Width, vp.Height };

			ImguiTool::BeginShowTempWindow(s_size - w_size, u8"モード表示");
		}

		// TextColored表示（色固定）
		auto Text{ [&](const char* text, auto... contents)
		{ ImGui::TextColored(Color, text, contents...); } };

		// 各モードを表示
		if (!edit_mode) Text(u8"実行モード");

		else if (edit_mode && !enm_edit_mode) Text(u8"編集モード");

		else Text(u8"敵編集モード");

		ImGui::End();
	}
#endif
#endif

	audio = p_window->GetAudio();
	object_manager->Update(p_window, p_app);

#if false

	if (edit_mode) file_flg.Update(p_scene_mgr, p_window, p_app);

	FileOperateUpdate();

	edit_mode ? EditModeUpdate(p_scene_mgr, p_window, p_app) : NormalModeUpdate(p_scene_mgr, p_window, p_app);
#else

	NormalModeUpdate(p_scene_mgr, p_window, p_app);

	const auto* input{ p_window->GetInput() };

	if (input->IsTrgKey(Keys::Back))
	{
		fadeout_timer += 2.f;
	}

	if (fadeout_timer >= GoalFadeTime)
	{
		SetNextScene<SceneSelect>();	//別スレッドでシーン切り替え
	}

#endif
#ifdef KDL_USE_IMGUI
#if false
	ImGui::End();
#endif
#endif
}

// 描画
void SceneGame::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const KDL::COLOR4F clear_color = { AQUA, 1.f };
	p_app->ClearBackBuffer(clear_color);

	// 背景
	{
		constexpr VF3 Scale{ 107.f, 1.f, 100.f };
		constexpr VF4 Color{ WHITE, 1.f };
		constexpr VF2 TexPos{ 0.f, 0.f };
		constexpr VF2 TexScale{ 1.f, 1.f };

		const VF3 pos{ camera->GetTarget() };

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
			R = DirectX::XMMatrixRotationRollPitchYaw(0.f, 3.1415f, 0.f);
			T = DirectX::XMMatrixTranslation(pos.x, -50.f, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		camera->CreateUpdateWorldViewProjection(&wvp, W);

		if (back_world_mode)
			sand_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale);
		else
			snow_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale);
	}

	object_manager->Draw(p_window, p_app);

	// チュートリアルメッセージボックス
	if (SceneSelect::GetIsTutrialMode())  // チュートリアルステージなら
	{
		using Math::ToRadian;

		constexpr VF4 Color{ WHITE, 1.f };
		constexpr VF2 TexPos{ 0.f, 0.f };
		constexpr VF2 TexScale{ 1.f, 1.f };
		constexpr VF3 Scale{ 5.f, 5.f, 5.f };
		constexpr VF3 Angle{ ToRadian(-40.f), ToRadian(180.f), 0.f };
		constexpr std::array<VF3, 4> pos{ {
			{ 5.5f, 3.f, -3.f},
			{ 0.f, 3.f, -3.f },
			{ -10.5f, 3.f, -3.f },
			{ -8.f, 3.f, 2.f } } };

		auto& pl_pos{ object_manager->GetObjectData<Player>()->pos };

		if (pl_pos.x >= 1.f)
		{
			for (size_t i = 0, length = tutorial_board.size() - 2u; i < length; i++)
			{
				DirectX::XMMATRIX W;
				{
					DirectX::XMMATRIX S, R, T;

					S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
					R = DirectX::XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);
					T = DirectX::XMMatrixTranslation(pos[i].x, pos[i].y, pos[i].z);

					W = S * R * T;
				}

				DirectX::XMFLOAT4X4 wvp, w;
				DirectX::XMStoreFloat4x4(&w, W);
				camera->CreateUpdateWorldViewProjection(&wvp, W);

				tutorial_board[i]->AddCommand(
					p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale, 0, false, false);
			}
		}
		else
		{
			for (size_t i = 2, length = tutorial_board.size(); i < length; i++)
			{
				DirectX::XMMATRIX W;
				{
					DirectX::XMMATRIX S, R, T;

					S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
					R = DirectX::XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);
					T = DirectX::XMMatrixTranslation(pos[i].x, pos[i].y, pos[i].z);

					W = S * R * T;
				}

				DirectX::XMFLOAT4X4 wvp, w;
				DirectX::XMStoreFloat4x4(&w, W);
				camera->CreateUpdateWorldViewProjection(&wvp, W);

				tutorial_board[i]->AddCommand(
					p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale, 0, false, false);
			}
		}
	}

	// エンターキーで決定、バックスペースキーでタイトルへ
	{
		constexpr DirectX::XMFLOAT4 Color{ BLACK, 1.f };
		constexpr int BM{ static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA) };
		constexpr float BaseY{ 25.f };

		static const VF2 Size{ 25.f, 50.f };

		static auto vp{ p_app->GetViewport() };
		static VF2 s_size{ vp.Width, vp.Height };

		font_sprite->AddTextCommands(p_app->GetCommandList(), p_app, "Retry : Enter",
			Fill2(BaseY), Size, Fill2(0.f), Fill2(0.f), Fill2(1.f), 0.f,
			Color, Color, Color, Color, BM);

		font_sprite->AddTextCommands(p_app->GetCommandList(), p_app, "Select : Back Space",
			{ s_size.x - 500.f, BaseY }, Size, Fill2(0.f), Fill2(0.f), Fill2(1.f), 0.f,
			Color, Color, Color, Color, BM);
	}

	// フェードアウト
	if (fadeout_timer > 0.0)
	{
		const double timer{ Easing::InCubic<double>(fadeout_timer, GoalFadeTime) };

		FadeOutDraw(p_app, &timer);

		fadeout_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	// フェードイン
	if (fadein_timer < BaseFadeTimeMax)
	{
		const double timer{ Easing::OutCubic<double>(fadein_timer, BaseFadeTimeMax) };

		FadeInDraw(p_app, &timer);

		fadein_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	// 選択グリット
#if false
	if (edit_mode)
	{
		constexpr float Scale{ 1.f };
		constexpr VF4 Color{ RED, 1.f };

		VF3 pos{ masu_pos };
		pos.y = 0.6f;

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(Scale, Scale, Scale);
			R = DirectX::XMMatrixRotationRollPitchYaw(3.14f / 2.f, 0.f, 0.f);
			T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		camera->CreateUpdateWorldViewProjection(&wvp, W);

		grit_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, { RED, 1.f }, { 0.f, 0.f }, { 0.f, 0.f });
	}

	// グリット線
	{
		constexpr VF3 Pos{ 0.5f, 0.25f, 0.5f };
		constexpr VF2 Scale{ 1000.f, 1000.f };
		constexpr VF3 Angle{ 3.14f / 2.f, 0.f, 0.f };
		constexpr VF4 Color{ GRAY, 1.f };
		constexpr VF2 TexPos{ 0.f, 0.f };

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(Scale.x, Scale.y, 1.f);
			R = DirectX::XMMatrixRotationRollPitchYaw(3.14f / 2.f, 0.f, 0.f);
			T = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		camera->CreateUpdateWorldViewProjection(&wvp, W);

		grit_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, { WHITE, 1.f }, { 0.f, 0.f }, Scale);
	}
#endif
}

// 通常モード更新
void SceneGame::NormalModeUpdate(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;
	using SM = SceneManager;

#ifdef KDL_USE_IMGUI
#if false
	ImGui::SliderAngle(u8"カメラ位置X", &camera_angle.x, -89.f, -1.f);
	ImGui::SliderAngle(u8"カメラ位置Y", &camera_angle.y, 0.f, 360.f);
	ImGui::SliderFloat(u8"カメラ距離", &camera_dis, 10.f, 150.f, "%.0f");

	ImGui::Separator();

	ImGui::Text(u8"Enter で リスタートじゃぁ～");
	ImGui::Text(u8"F1 で 編集モードじゃぁ～");
#endif
#endif
	// リトライ機能
	if (const auto* input{ p_window->GetInput() };
		input->IsTrgKey(Keys::Enter) || init_scene)
	{
		Initialize(p_scene_mgr, p_window, p_app);

		return;
	}

	// 選択画面へ
	if (execution_quick_exit && Math::AdjEqual(fadeout_timer, 0.0))
	{
		fadeout_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	if (fadeout_timer >= GoalFadeTime)
	{
		SetNextScene<SceneSelect>();	//別スレッドでシーン切り替え
	}

	// カメラの更新
#if false
	if (const auto& player{ object_manager->GetObjectData<Player>() }; player)
	{
		camera->SetPosition(CreateRotationPos(camera_angle, camera_dis, player->pos));
		camera->SetTarget(player->pos);
	}
	// プレイヤーが存在しないなら原点をターゲットにする
	else
	{
		constexpr VF3 BasePos{ 0.f, 0.f, 0.f };

		camera->SetPosition(CreateRotationPos(camera_angle, camera_dis, player->pos));
		camera->SetTarget(BasePos);
	}
#else
	if (const auto& player{ object_manager->GetObjectData<Player>() }; player)
	{
		static double adj_dis_timer{ 0.f };

		if (object_manager->GetIsGoal())
		{
			adj_dis_timer += p_window->GetElapsedTime();
		}
		else
		{
			adj_dis_timer = 0.f;
		}

		camera->SetPosition(CreateRotationPos(camera_angle,
			camera_dis - (Easing::OutCirc(
								adj_dis_timer,
								GoalFadeTime,
								static_cast<double>(camera_dis - 1.5))), player->pos));

		camera->SetTarget(player->pos);
	}
#endif
}

// 編集モード更新
void SceneGame::EditModeUpdate(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;

	auto camera_pos{ camera->GetPosition() };
	auto camera_target{ camera->GetTarget() };
	const auto* input = p_window->GetInput();

	// マウス関係
	{
		// マウス座標取得
		mouse_pos = { static_cast<float>(input->GetMousePos().x), static_cast<float>(input->GetMousePos().y) };

		// ローカル座標に変換
		masu_pos = MouseToLocalPos(mouse_pos, p_app);

		// マス座標に変換
		masu_pos = Round(masu_pos);
	}

	static float camera_dis{ 50.f };
	static float cam_speed{ 10.f };

	auto KeyInput{ [&](Keys key1, Keys key2)
	{ return (input->IsHitKey(key1) || input->IsHitKey(key2)); } };

	// カメラ
	if (!(input->IsHitKey({ Keys::LeftControl }) && input->IsHitKey({ Keys::RightControl })))
	{
#ifdef KDL_USE_IMGUI
		if (ImGui::GetIO().WantCaptureKeyboard)	return;
#endif

		constexpr VF3 CamAngle{ -3.14f * 0.5f, 0.f, 0.f };
		const float Movement{ cam_speed * p_window->GetElapsedTime() };

		if (KeyInput(Keys::Left, Keys::A))
			camera_target.x += Movement;

		else if (KeyInput(Keys::Right, Keys::D))
			camera_target.x -= Movement;

		if (KeyInput(Keys::Up, Keys::W))
			camera_target.z -= Movement;

		else if (KeyInput(Keys::Down, Keys::S))
			camera_target.z += Movement;

		if (input->IsTrgKey(Keys::PageUp))
			camera_dis -= Movement;

		if (input->IsTrgKey(Keys::PageDown))
			camera_dis += Movement;

		camera_dis = std::clamp(camera_dis, 10.f, 150.f);

		camera_pos = camera_target;

		camera_pos.y += camera_dis;

		camera->SetPosition(CreateRotationPos(CamAngle, camera_dis, camera_target));
	}

#ifdef KDL_USE_IMGUI
	ImGui::Text(u8"カメラ座標：%.01f, %.01f", camera_pos.x, camera_pos.z);
	ImGui::SliderFloat(u8"カメラ距離", &camera_dis, 10.f, 150.f, "%.0f");
	ImGui::SliderFloat(u8"カメラ速度", &cam_speed, 5.f, 50.f, "%.0f");

	ImGui::Separator();

	ImGui::Text(u8"床 %u, 壁 %u, 敵 %u, ワープホール %u,\nドア %u, カギ %u",
		object_manager->GetObjectData<Plane>().size(),
		object_manager->GetObjectData<Wall>().size(),
		object_manager->GetObjectData<Enemy>().size(),
		object_manager->GetObjectData<WarpHole>().size(),
		object_manager->GetObjectData<Door>().size(),
		object_manager->GetObjectData<Key>().size());

	ImGui::Separator();

	ImGui::Text(u8"マウス座標 : %.01f, %.01f", mouse_pos.x, mouse_pos.y);
	ImGui::Text(u8"マス座標 : %.01f, %.01f", masu_pos.x, masu_pos.z);

	ImGui::Separator();

	if (!enm_edit_mode) ImGui::Text(u8"F1 で 実行モードじゃぁ～");
	ImGui::Text(u8"F2 で %s編集モードじゃぁ～", (enm_edit_mode ? u8" " : u8"敵"));

	ImGui::Separator();
	ImGui::Text(u8"開かれているファイル：%s", open_file_path.stem().u8string().data());
	ImGui::Text(u8"保存されてい%s", is_save ? u8"る" : u8"ない");
#endif
#endif
}

// モード変更処理
void SceneGame::ModeChange(bool* save_f1, SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;

	const auto* input = p_window->GetInput();

	// 編集モード←→実行モード
	if (!enm_edit_mode)
	{
		const bool confirm_f1{ input->IsTrgKey(Keys::F1) };

		if (confirm_f1)	*save_f1 = true;

		// 確認画面
		if (*save_f1)
		{
			// 実行モードへ移行時
			if (edit_mode)
			{
				auto& start{ object_manager->GetObjectData<Start>() };
				auto& player{ object_manager->GetObjectData<Player>() };

				// プレイヤーとスタート地点が存在するときだけ確認画面が出現
				if (player && start)
				{
#ifdef KDL_USE_IMGUI && false
					// 位置調整
					{
						static VF2 size{ 100.f, 100.f };
						const auto vp{ p_app->GetViewport() };
						const VF2 s_size{ vp.Width, vp.Height };

						const auto& s_div_size{ (s_size / 2.f) };
						const auto pos{ s_div_size - size };

						ImguiTool::BeginShowTempWindow(pos, u8"確認画面");

						auto temp{ ToXMFLOAT(ImGui::GetWindowSize()) };

						size = temp / 2.f;
					}

					auto [normal, debug] { ImguiTool::YesNoButton(u8"通常モード", u8"デバックモード") };
					ImguiTool::ShowHelp(u8"通常モードはプレイヤーがスタート地点から開始します。\n"
						u8"デバックモードはプレイヤーを置いた地点から開始します。");

					// 選択したら
					if (normal || debug)
					{
						*save_f1 = false;
						edit_mode ^= true;
						back_world_mode = false;

						if (!Output(TempFileDir, true))
							assert(!"一時ファイルの作成に失敗");

						// 通常モード時のみプレイヤーをスタート地点へ
						if (normal)
							player->pos = start->pos;
					}

					ImGui::End();
#endif
				}
				// プレイヤーとスタート地点の片方Or両方が存在しない
				else
				{
					edit_mode ^= true;
					back_world_mode = false;
					*save_f1 = false;

					if (!Output(TempFileDir, true))
						assert(!"一時ファイルの作成に失敗");
				}
			}
			// 編集画面へ移行時
			else
			{
				namespace fs = std::filesystem;

				*save_f1 = false;
				edit_mode ^= true;
				back_world_mode = false;

				// 一時ファイルが存在している
				if (fs::exists(TempFileDir))
				{
					// 読み込む前にオブジェクトを全削除
					object_manager->GetObjectData().AllObjectClear();

					// 読み込んで
					if (!Input(TempFileDir, true))
						assert(!"一時ファイルの読み込みに失敗");

					// 一時ファイルを削除
					if (!std::filesystem::remove(TempFileDir))
						assert(!"一時ファイルの削除に失敗");

					auto& planes{ object_manager->GetObjectData().GetChangeObjects<Plane>() };

					std::for_each(planes.begin(), planes.end(), [](auto& obj) { obj.InitHP(); });
				}
			}
		}
	}

	// 敵編集モード←→編集モードor実行モード
	if (edit_mode && input->IsTrgKey(Keys::F2))
	{
		object_manager->InitSelectObject();
		enm_edit_mode ^= true;
	}
#endif
}

// マウス座標からローカル座標への変換
DirectX::XMFLOAT3 SceneGame::MouseToLocalPos(const VF2& mouse_pos, KDL::DX12::App* p_app)
{
	constexpr std::array<VF3, 3u> Points  // プレーンの３点
	{ { { -1000.f, 0.f, 1000.f }, { 1000.f, 0.f, 1000.f }, { 1000.f, 0.f, -1000.f } } };

	const auto vp{ p_app->GetViewport() };
	const VF2 s_size{ vp.Width, vp.Height };

	// 型を変換して返す
	return TransformMouseToPlanePos(s_size, mouse_pos, Points[0], Points[1], Points[2]);
}

// 読み込み
void SceneGame::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// (´・ω・`)知らんがな
	// マルチスレッドにComオブジェクトが絡んでくると（正確には別スレッド内でComオブジェクトを生成する時？）、
	// こうしないといけないらしい
	// スレッド事にこの関数を呼び出す必要がある
	// https://teratail.com/questions/165187
	// https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex
	// https://social.msdn.microsoft.com/Forums/ja-JP/4ea73ec0-e90d-4759-8517-260f03621362/createinstance123910x800401f0123981245612521125401236430330299831237712?forum=vcgeneralja
	//if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) assert(!"COMライブラリは、このスレッドで既に初期化されている");

	// 板ポリ画像
	{
		auto Load{ [&](auto& board, const Path& path)
		{ board = std::make_unique<KDL::DX12::Geometric_Board_S>(p_app, path, 100); } };

		std::vector<std::pair<decltype(snow_board)*, std::string>> load_board{
			{ &snow_board, "data\\images\\Snow.png" },
			{ &sand_board, "data\\images\\Sand.png" },
		};

		for (auto& board : load_board)
		{
			Load(*board.first, board.second);

			load_count++;
		}

		const auto tutorial_name{ GetAllFileName("data\\images\\Tutorial") };

		for (auto& name : tutorial_name)
		{
			tutorial_board.emplace_back(
				std::make_unique<KDL::DX12::Geometric_Board_S>(p_app, name, 100u));
		}
	}

	// 音楽
	{
		auto audio = p_window->GetAudio();

		bgm_handle_p = 0;

		std::vector<std::pair<int*, std::string>> load_music{
			{ &bgm_handle, "./data/sounds/BGM.wav" },
			{ &se_break, "./data/sounds/break.wav" },
			{ &se_crack, "./data/sounds/crack.wav" },
			{ &se_door, "./data/sounds/door.wav" },
			{ &se_waap, "./data/sounds/warp.wav" },
			{ &se_whistle, "./data/sounds/whistle.wav" },
			{ &se_goal, "./data/sounds/goal.wav" },
		};

		for (auto& music : load_music)
		{
			*music.first = audio->Load(music.second);
			load_count++;
		}
	}

	font_sprite = std::make_unique< KDL::DX12::Sprite_Image>(p_app, "data\\fonts\\font3.png", 100u);

	FadeBoxInit(p_app);

	object_manager->Load(&load_count, p_window, p_app);

	// プロセス終了時にこの関数を呼び出す
	//CoUninitialize();
}

void SceneGame::FileDataFlg::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;

	const auto* input = p_window->GetInput();

	auto KeyInput{ [&](Keys key1, Keys key2)
	{ return (input->IsHitKey(key1) || input->IsHitKey(key2)); } };

	const bool press_ctrl{ KeyInput(Keys::LeftControl, Keys::RightControl) };
	const bool press_shift{ KeyInput(Keys::LeftShift, Keys::RightShift) };
	//const bool press_alt{ KeyInput(Keys::LeftAlt, Keys::RightAlt ) };
	//const bool down_c{ input->IsTrgKey(Keys::C) };
	const bool down_s{ input->IsTrgKey(Keys::S) };

	// コントロールキーを押していないならとばす
	if (!press_ctrl)	return;

	// 保存
	if (down_s && !press_shift)
		is_save = true;

	// 別名保存
	//else if (press_shift && down_s)
	//	is_save_as = true;

	// 開く
	else if (input->IsTrgKey(Keys::O))
		is_open = true;

	// 削除
	//else if (press_shift && press_alt && GMLIB->IsKeyPress(Key::Delete))
	//	is_delete = true;

	// 閉じる
	//else if (press_alt && down_c)
	//	is_close = true;

	// 名前変更
	//else if (GMLIB->IsKeyPress(Key::R))
	//	is_rename = true;

	// コピー
	//else if (press_shift && down_c)
	//	is_copy = true;
#endif
}

// 書き出し
bool SceneGame::Output(const Path& output_path, const bool temp_file)
{
#if false
	// 一時ファイルはオブジェクトだけ出力
	if (temp_file)
	{
		if (std::ofstream ofs{ output_path }; ofs)
		{
			object_manager->Output(ofs);
		}
		else
			return false;
	}
	else
	{
		if (std::ofstream ofs{ CameraFileDir }; ofs)
		{
			ofs << camera_angle << F_OUT(camera_dis) << std::endl;
		}
		else
			return false;

		// 編集モードの時のみオブジェクト情報を出力
		if (edit_mode)
		{
			if (std::ofstream ofs{ output_path }; ofs)
			{
				object_manager->Output(ofs);
			}
			else
				return false;
}
	}
#endif

	return true;
}

// 読み込み
bool SceneGame::Input(const Path& input_path, const bool temp_file)
{
	namespace fs = std::filesystem;

#if false
	// 一時ファイルはオブジェクトだけ入力
	if (temp_file)
	{
		if (std::ifstream ifs{ input_path }; ifs)
		{
			object_manager->Input(ifs);
		}
		else
			return false;
	}
	else
	{
		if (std::ifstream ifs{ CameraFileDir }; ifs)
		{
			ifs >> camera_angle >> camera_dis;
		}
		else
			return false;

		// 編集モードの時のみオブジェクト情報を入力
		if (edit_mode)
		{
			if (std::ifstream ifs{ input_path }; ifs)
			{
				object_manager->Input(ifs);
			}
			else
				return false;
		}
		// 実行モード
		else
		{
			// 一時ファイルが存在するなら、一時ファイルの方を優先する
			if (fs::exists(TempFileDir))
			{
				if (std::ifstream ifs{ TempFileDir }; ifs)
				{
					object_manager->Input(ifs);
				}
				else
					return false;

				// ファイルが開かれている
				if (!open_file_path.empty())
				{
					// 一時ファイルの内容をステージにコピー
					fs::copy_file(TempFileDir, input_path, fs::copy_options::overwrite_existing);
				}
			}
			// 一時ファイルが存在しない（起動して初回）
			else
			{
				if (std::ifstream ifs{ input_path }; ifs)
				{
					object_manager->Input(ifs);
				}
				else
					return false;
			}
		}
	}
#else
	if (std::ifstream ifs{ CameraFileDir }; ifs)
	{
		ifs >> camera_angle >> camera_dis;
	}
	else
		return false;

	if (std::ifstream ifs{ input_path }; ifs)
	{
		object_manager->Input(ifs);
	}
	else
		return false;

#endif
	return true;
}