#include "scene.h"
#include "TitleScene.h"

void SceneMain::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	//mesh = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, "./data/models/BLue Falcon/Blue Falcon.FBX", 10000);
}

void SceneMain::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
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
	angle = 0;
}

void SceneMain::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const KDL::COLOR4F clear_color = { 0.f, 0.f, 0.f, 1.f };
	p_app->ClearBackBuffer(clear_color);

	const auto* input = p_window->GetInput();
	if (input->IsTrgKey(KDL::KEY_INPUTS::Enter))
	{
		if (input->IsHitKey(KDL::KEY_INPUTS::LeftControl))
			ChangeScene<SceneTitle>();		//メインスレッドでシーン切り替え
		else
			SetNextScene<SceneTitle>();	//別スレッドでシーン切り替え
	}

	const float elpased_time = p_window->GetElapsedTime();
	constexpr float radian_180 = DirectX::XMConvertToRadians(180.f);
	angle += DirectX::XMConvertToRadians(90.f) * elpased_time;
	while (angle > radian_180)
		angle -= radian_180 * 2;
}

void SceneMain::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{

	const float scale = 0.05f;
	const KDL::FLOAT3 position = { 0.f, 0.f, 0.f };
	for (UINT i = 0; i < 1; i++)
	{
		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale, scale, scale);
			R = DirectX::XMMatrixRotationRollPitchYaw(0.f, angle, 0.f);
			T = DirectX::XMMatrixTranslation(position.x, position.y + i, position.z + i * 2);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		camera->CreateUpdateWorldViewProjection(&wvp, W);
		//mesh->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, { 0, 0, 1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f });
	}
}

void SceneMain::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{

}