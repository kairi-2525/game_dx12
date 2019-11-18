#include "SceneSelect.h"
#include "GameScene.h"
#include "LoadAllFileName.h"
#include "ImGuiSeting.h"
#include "ImVecHelper.h"
#include "TitleScene.h"
#include "Easing.h"

void SceneSelect::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// ファイルを読み込み
	file_names = GetAllFileName(SceneGame::SaveFileDir);

	// チュートリアルを最初に持ってくる
	{
		auto tutorial{ std::find_if(file_names.begin(), file_names.end(),
			[](const Path& name) { return name.string().find("tutorial") != std::string::npos; }) };

		assert(tutorial != file_names.end() && "チュートリアルファイルが見つからない");

		file_names.emplace(file_names.begin(), *tutorial);
		file_names.pop_back();
	}

	// 背景読み込み
	bg_sprite.emplace_back(std::make_unique< KDL::DX12::Sprite_Image>(p_app, "data\\images\\Snow.png", 100u));
	bg_sprite.emplace_back(std::make_unique< KDL::DX12::Sprite_Image>(p_app, "data\\images\\Sand.png", 100u));

	arrow_sprite = std::make_unique< KDL::DX12::Sprite_Image>(p_app, "data\\images\\Arrow.png", 2u);

	auto audio = p_window->GetAudio();
	bgm_handle = audio->Load("./data/sounds/BGM.wav");
	bgm_handle_p = 0;
	se_select = audio->Load("./data/sounds/select.wav");
	se_decision = audio->Load("./data/sounds/decision.wav");

	font_sprite = std::make_unique< KDL::DX12::Sprite_Image>(p_app, "data\\fonts\\font3.png", 100u);

	FadeBoxInit(p_app);
}

void SceneSelect::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	bg_timer = 0.0;
	arrow_timer = 0.0;
	select_num = 0;
	FadeTimeInit();
	is_tutrial_mode = false;

	auto* audio = p_window->GetAudio();
	audio->Stop(bgm_handle, bgm_handle_p, 1.0f);
	bgm_handle_p = audio->CreatePlayHandle(bgm_handle, 0.f, true, false, 0.f, 0.f, 0, false, false);
	audio->Play(bgm_handle, bgm_handle_p, 0.01f, 0.2f, false);
}

void SceneSelect::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;

	static std::optional<bool> is_enter;

	const auto* input{ p_window->GetInput() };
	auto audio = p_window->GetAudio();

	// 移動
	if (file_names.size() - 1 > select_num && (input->IsTrgKey(Keys::Down) || input->IsTrgKey(Keys::S)))
	{
		select_num++;
		arrow_timer = 0.f;
		int handle = audio->CreatePlayHandle(se_select, 0.f, false, false, 0.f, 0.f, 0, false, false);
		audio->Play(se_select, handle, 0.01f, 0.2f, false);
	}
	else if (select_num > 0 && (input->IsTrgKey(Keys::Up) || input->IsTrgKey(Keys::W)))
	{
		select_num--;
		arrow_timer = 0.f;
		int handle = audio->CreatePlayHandle(se_select, 0.f, false, false, 0.f, 0.f, 0, false, false);
		audio->Play(se_select, handle, 0.01f, 0.2f, false);
	}

	if (input->IsTrgKey(Keys::Back))
	{
		is_enter.emplace(false);

		int handle = audio->CreatePlayHandle(se_decision, 0.f, false, false, 0.f, 0.f, 0, false, false);
		audio->Play(se_decision, handle, 0.01f, 1.f, false);

		fadeout_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	if (input->IsTrgKey(Keys::Enter))
	{
		is_enter.emplace(true);

		int handle = audio->CreatePlayHandle(se_decision, 0.f, false, false, 0.f, 0.f, 0, false, false);
		audio->Play(se_decision, handle, 0.01f, 1.f, false);

		fadeout_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	// 決定か戻るが押された
	if (is_enter)
	{
		constexpr double AlphaRate{ 2.0 };

		if (fadeout_timer >= BaseFadeTimeMax)
		{
			if (*is_enter)
			{
				SceneGame::open_file_path = file_names.at(select_num);

				SetNextScene<SceneLoad>();	//別スレッドでシーン切り替え

				is_enter = std::nullopt;
			}
			else
			{
				SetNextScene<SceneTitle>();	//別スレッドでシーン切り替え

				is_enter = std::nullopt;
			}

			if (const auto& name{ file_names[select_num].string() }; name.find("tutorial") != std::string::npos)
				is_tutrial_mode = true;
			else
				is_tutrial_mode = false;
		}

		fadeout_timer += static_cast<double>(p_window->GetElapsedTime())* AlphaRate;
	}

	bg_timer += static_cast<double>(p_window->GetElapsedTime());
	arrow_timer += static_cast<double>(p_window->GetElapsedTime());
	fadein_timer += static_cast<double>(p_window->GetElapsedTime());
}

void SceneSelect::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const KDL::COLOR4F clear_color = { 0.f, 0.f, 0.f, 1.f };
	p_app->ClearBackBuffer(clear_color);

	constexpr int BM{ static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA) };

	static auto vp{ p_app->GetViewport() };
	static VF2 s_size{ vp.Width, vp.Height };

	const VF2 DivSize{ s_size / 2.f };

	// 背景
	{
		constexpr double Alpha_Rate{ 0.5 };

		{
			const VF4 color{ WHITE, ((std::sinf(bg_timer * Alpha_Rate) + 1.f) / 2.f) };

			bg_sprite.front()->AddCommand(p_app->GetCommandList(), p_app, Fill2(0.f), s_size, Fill2(0.f),
				Fill2(1.f), 0.f, color, color, color, color, BM);
		}

		{
			const VF4 color{ WHITE, 1.f - ((std::sinf(bg_timer * Alpha_Rate) + 1.f) / 2.f) };

			bg_sprite.back()->AddCommand(p_app->GetCommandList(), p_app, Fill2(0.f), s_size, Fill2(0.f),
				Fill2(1.f), 0.f, color, color, color, color, BM);
		}
	}

	// 矢印
	{
		constexpr double AlphaRate{ 2.0 };
		constexpr float MoveRate{ 20.f };
		constexpr VF4 Color{ WHITE, 1.f };

#ifdef KDL_USE_IMGUI
		//ImguiTool::BeginShowTempWindow({ 0.f, 0.f }, "test");

		//ImguiHeler::SliderFloat(u8"座標", &base_pos, 0.f, 1920.f, "%.0f");

		//ImGui::End();
#endif

		// 上
		if (select_num != 0)
		{
			const VF2 pos
			{ DivSize.x - 55.f, (DivSize.y - 350.f) + (1.f - std::sinf(arrow_timer * AlphaRate) * MoveRate) };

			arrow_sprite->AddCommand(p_app->GetCommandList(), p_app, pos, { 200.f, 100.f }, Fill2(0.f),
				Fill2(1.f), 0.f, Color, Color, Color, Color, BM);
		}

		// 下
		if (select_num != file_names.size() - 1u)
		{
			const auto spr_size{ arrow_sprite->GetSize() };
			const VF2 pos{ DivSize.x, (vp.Height - 450.f) + (std::sinf(arrow_timer * AlphaRate) * MoveRate) };

			arrow_sprite->AddCommand(p_app->GetCommandList(), p_app, pos, { 200.f, 100.f }, spr_size / 2.f,
				Fill2(1.f), Math::PAI<float>, Color, Color, Color, Color, BM);
		}
	}

	//todo エンターキーで決定、バックスペースキーでタイトルへ
	{
		constexpr VF4 Color{ BLACK, 1.f };
		static const VF2 Size{ 25.f, 50.f };

		font_sprite->AddTextCommands(p_app->GetCommandList(), p_app, "Press Enter...",
			{ DivSize.x - 100.f, s_size.y - Size.y - 50.f }, Size, Fill2(0.f), Fill2(0.f), Fill2(1.f), 0.f,
			Color, Color, Color, Color, BM);

		font_sprite->AddTextCommands(p_app->GetCommandList(), p_app, "Title : Back Space",
			Fill2(25.f), Size, Fill2(0.f), Fill2(0.f), Fill2(1.f), 0.f,
			Color, Color, Color, Color, BM);
	}

#if false
	if (select_num == 0)
	{
		static VF2 adj{ -200.f, -200.f };

		font_sprites.at("tutorial")->AddCommand(p_app->GetCommandList(), p_app, DivSize + adj, Fill2(500.f), Fill2(0.f),
			Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);
	}
	else if (select_num < file_names.size())
	{
		auto name = file_names.at(select_num).string();

		// ステージという名前が見つかれば
		if (name.find("stage") != std::string::npos)
		{
			{
				static VF2 adj{ -300.f, -200.f };

				font_sprites.at("Stage")->AddCommand(p_app->GetCommandList(), p_app, DivSize + adj, Fill2(500.f), Fill2(0.f),
					Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);
			}

			if (select_num < 10)
			{
				static VF2 adj{ 100.f, -200.f };

				font_sprites.at(std::to_string(select_num))->
					AddCommand(p_app->GetCommandList(), p_app, DivSize + adj, Fill2(500.f), Fill2(0.f),
						Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);
			}
			else
			{
				// 十の位
				{
					auto temp{ std::to_string(select_num) };

					static VF2 adj{ 100.f, -200.f };

					temp.pop_back();

					font_sprites.at(temp)->
						AddCommand(p_app->GetCommandList(), p_app, DivSize + adj, Fill2(500.f), Fill2(0.f),
							Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);
				}

				// 一の位
				{
					static VF2 adj{ 150.f, -200.f };

					auto temp{ std::to_string(select_num) };

					temp.erase(temp.begin());

					font_sprites.at(temp)->
						AddCommand(p_app->GetCommandList(), p_app, DivSize + adj, Fill2(500.f), Fill2(0.f),
							Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);
				}
			}
		}
	}
#else
	// フォント
	{
		constexpr VF4 Color{ BLACK, 1.f };

		auto f_size{ font_sprite->GetSize() };
		VF2 f_div_size{ f_size.x + 400.f, f_size.y };

		font_sprite->AddTextCommands(p_app->GetCommandList(), p_app, file_names[select_num].stem().string(),
			DivSize - (f_div_size / 2.f), Fill2(100.f), Fill2(0.f), Fill2(0.f), Fill2(1.f), 0.f,
			Color, Color, Color, Color, BM);
}
#endif

	// フェードアウト
	if (fadeout_timer > 0.0)
	{
		const double timer{ Easing::InQuint(fadeout_timer, BaseFadeTimeMax) };

		FadeOutDraw(p_app, &timer);
	}

	// フェードイン
	if (fadein_timer < BaseFadeTimeMax)
	{
		const double timer{ Easing::InQuint(fadein_timer, BaseFadeTimeMax) };

		FadeInDraw(p_app, &timer);

		fadein_timer += static_cast<double>(p_window->GetElapsedTime());
	}
}

void SceneSelect::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	auto* audio = p_window->GetAudio();
	audio->Delete(bgm_handle);
	audio->Delete(se_decision);
	audio->Delete(se_select);
}