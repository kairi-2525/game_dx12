#include "scene.h"


void SceneTitle::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	board = std::make_unique<KDL::DX12::Geometric_Board>(p_app, "./data/images/PNG.png");
}

void SceneTitle::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	camera = 
		std::make_unique<KDL::TOOL::Camera>(
			KDL::TOOL::Camera(
				{ 0.f, 10.f, -10.f },		//eye
				{ 0.f, 0.f, 0.f },			//focus
				{ 0.f, 1.f, 0.f },			//up
				DirectX::XMConvertToRadians(30.f),
				p_app->GetViewport().Width / p_app->GetViewport().Height,
				{ 0.1f, 1000.f }
			)
		);
	angle = 0;
}

void SceneTitle::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const auto* input = p_window->GetInput();
	if (input->IsTrgKey(KDL::KEY_INPUTS::Enter))
	{
		if (input->IsHitKey(KDL::KEY_INPUTS::LeftControl))
			ChangeScene<SceneMain>();		//メインスレッドでシーン切り替え
		else
			SetNextScene<SceneMain>();	//別スレッドでシーン切り替え
	}

	const float elpased_time = p_window->GetElapsedTime();
	const float radian_180 = DirectX::XMConvertToRadians(180.f);
	angle += DirectX::XMConvertToRadians(90.f) * elpased_time;
	while (angle > radian_180)
		angle -= radian_180 * 2;
}

void SceneTitle::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const KDL::COLOR4F clear_color = { 0.f, 0.f, 0.f, 1.f };
	p_app->ClearBackBuffer(clear_color);

	const float scale = 4.f;
	const KDL::FLOAT3 position = { 0.f, 0.f, 0.f };
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale, scale, scale);
		R = DirectX::XMMatrixRotationRollPitchYaw(0.f, angle, 0.f);
		T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		W = S * R * T;
	}
	DirectX::XMFLOAT4X4 wvp, w;
	DirectX::XMStoreFloat4x4(&w, W);
	camera->CreateUpdateWorldViewProjection(&wvp, W);

	board->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, { 0, 0, 1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f });
}

void SceneTitle::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{

}