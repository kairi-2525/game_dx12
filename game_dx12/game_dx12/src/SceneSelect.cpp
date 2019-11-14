#include "SceneSelect.h"
#include "GameScene.h"
#include "LoadAllFileName.h"
#include "XMFLOAT_Hlper.h"
#include "ImGuiSeting.h"
#include "ImVecHelper.h"

void SceneSelect::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// ファイルを読み込み
	file_names = GetAllFileName(SceneGame::SaveFileDir);
	const auto fonts{ GetAllFileName("data\\images\\Select") };

	// 背景読み込み
	bg_sprite = std::make_unique< KDL::DX12::Sprite_Image>(p_app, "data\\images\\Snow.png", 1u);

	// 画像読み込み
	for (auto& name : fonts)
	{
		sprites.emplace(name.stem().string(), std::make_unique<KDL::DX12::Sprite_Image>(p_app, name, 10u));
	}
}

void SceneSelect::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	select_num = 0;
}

void SceneSelect::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;

	const auto* input{ p_window->GetInput() };

	// 移動
	if (file_names.size() - 1 > select_num && input->IsTrgKey(Keys::Down))
	{
		select_num++;
	}
	else if (select_num > 0 && input->IsTrgKey(Keys::Up))
	{
		select_num--;
	}

	if (input->IsTrgKey(Keys::Enter))
	{
		SceneGame::open_file_path = file_names.at(select_num);

		SetNextScene<SceneLoad>();	//別スレッドでシーン切り替え
	}
}

void SceneSelect::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const KDL::COLOR4F clear_color = { 0.f, 0.f, 0.f, 1.f };
	p_app->ClearBackBuffer(clear_color);

	constexpr int BM{ static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA) };

	static auto vp{ p_app->GetViewport() };
	static VF2 s_size{ vp.Width, vp.Height };

	const VF2 Pos{ s_size / 2.f };

	//ImguiTool::BeginShowTempWindow({ 0.f, 0.f }, "test");

	//ImguiHeler::InputFloat(u8"座標", &adj);

	//ImGui::End();

	//bg_sprite->AddCommand(p_app->GetCommandList(), p_app, Fill2(0.f), s_size, Fill2(0.f),
	//	Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);

	if (select_num == 0)
	{
		static VF2 adj{ -200.f, -200.f };

		sprites.at("tutorial")->AddCommand(p_app->GetCommandList(), p_app, Pos + adj, Fill2(500.f), Fill2(0.f),
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

				sprites.at("Stage")->AddCommand(p_app->GetCommandList(), p_app, Pos + adj, Fill2(500.f), Fill2(0.f),
					Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);
			}

			{
				static VF2 adj{ 100.f, -200.f };

				sprites.at(std::to_string(select_num))->
					AddCommand(p_app->GetCommandList(), p_app, Pos + adj, Fill2(500.f), Fill2(0.f),
						Fill2(1.f), 0.f, Fill4(1.f), Fill4(1.f), Fill4(1.f), Fill4(1.f), BM);
			}
		}
	}
}

void SceneSelect::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
}
