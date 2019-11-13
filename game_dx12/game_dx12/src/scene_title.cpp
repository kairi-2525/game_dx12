#include "TitleScene.h"
#include "GameScene.h"

void SceneTitle::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	player.model = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, "./data/models/player.fbx", 1);

	for (auto& it : grounds_list)
	{
		grounds[it.first].model = std::make_unique<KDL::DX12::Geometric_Board>(p_app, ("./data/images/Game/" + it.second + ".png"), 20);
	}
}

void SceneTitle::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	camera =
		std::make_unique<KDL::TOOL::Camera>(
			KDL::TOOL::Camera(
				{ 0.f, 1.f, -5.f },		//eye
				{ 0.f, 1.f, 0.f },			//focus
				{ 0.f, 1.f, 0.f },			//up
				DirectX::XMConvertToRadians(30.f),
				p_app->GetViewport().Width / p_app->GetViewport().Height,
				{ 0.1f, 1000.f }
			)
		);
	light_dir = KDL::FLOAT3{ 0.f, -1.f, 1.f };
	light_dir /= light_dir.Length();
	player.position = KDL::FLOAT3{ 0.f, 0.8f, 0.f };
	player.rotate = KDL::FLOAT3{ 0.f, DirectX::XMConvertToRadians(-90.f), 0.f };
	player.scale = KDL::FLOAT3{ 0.1f, 0.1f, 0.1f };
	for (auto& it : grounds)
		it.second.data.clear();
	const float size = 1.0f;
	float set_pos = -static_cast<float>(BLOCK_NUM) / 2;
	for (UINT i = 0u; i < BLOCK_NUM; i++)
	{
		auto& it = grounds[rand() % 2 == 0 ? GROUND_TYPE::Snow : GROUND_TYPE::SnowBroken].data.emplace_back();
		it.position = KDL::FLOAT3{ set_pos + size / 2, 0, 0};
		it.rotate = KDL::FLOAT3{ 0, 0, 0 };
		it.scale = KDL::FLOAT3{ 1.f, 1.f, 1.f };
		set_pos += 1.f;
	}
}

void SceneTitle::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const auto* input = p_window->GetInput();
	const float elpased_time = p_window->GetElapsedTime();
	if (input->IsTrgKey(KDL::KEY_INPUTS::Enter))
	{
		SetNextScene<SceneLoad>();	//別スレッドでシーン切り替え
	}
	if (input->IsTrgKey(KDL::KEY_INPUTS::Space))
	{
		Initialize(p_scene_mgr, p_window, p_app);
	}
	const float speed = 1.f;
	const float end_line = -10.f;
	for (auto& g : grounds)
	{
		for (auto& it : g.second.data)
		{
			it.position.x -= speed * elpased_time;
			if (it.position.x < end_line)
			{
				it = g.second.data.back();
				g.second.data.pop_back();
			}
		}
	}
	{
		const float break_line = -0.5f;

		for (auto& it : grounds[GROUND_TYPE::Sand].data)
		{
			if (!it.changed && it.position.x < break_line)
			{
				it.changed = true;
				grounds[GROUND_TYPE::SandBroken].data.emplace_back(it);
				it = grounds[GROUND_TYPE::Sand].data.back();
				grounds[GROUND_TYPE::Sand].data.pop_back();
			}
		}
		for (auto& it : grounds[GROUND_TYPE::Snow].data)
		{
			if (!it.changed && it.position.x < break_line)
			{
				it.changed = true;
				grounds[GROUND_TYPE::SnowBroken].data.emplace_back(it);
				it = grounds[GROUND_TYPE::Snow].data.back();
				grounds[GROUND_TYPE::Snow].data.pop_back();
			}
		}
		for (auto& it : grounds[GROUND_TYPE::SandBroken].data)
		{
			if (!it.changed && it.position.x < break_line)
			{
				it = grounds[GROUND_TYPE::SandBroken].data.back();
				grounds[GROUND_TYPE::SandBroken].data.pop_back();
			}
		}
		for (auto& it : grounds[GROUND_TYPE::SnowBroken].data)
		{
			if (!it.changed && it.position.x < break_line)
			{
				it = grounds[GROUND_TYPE::SnowBroken].data.back();
				grounds[GROUND_TYPE::SnowBroken].data.pop_back();
			}
		}
	}
}

void SceneTitle::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const KDL::COLOR4F clear_color = { 0.f, 0.f, 0.f, 1.f };
	p_app->ClearBackBuffer(clear_color);
	if (FAILED(player.Draw(p_app, camera.get(), light_dir)))
	{
		p_scene_mgr->Exit();
		return;
	}
	for (auto& it : grounds)
	{
		if (FAILED(it.second.Draw(p_app, camera.get(), light_dir)))
		{
			p_scene_mgr->Exit();
			return;
		}
	}
}

void SceneTitle::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{

}