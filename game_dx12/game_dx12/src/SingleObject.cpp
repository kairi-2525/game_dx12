#include "SingleObject.h"
#include "KDL.h"
#include "GameScene.h"
#include "ImVecHelper.h"

//------------------------------------------------------------------------------------------------------
// 自機
//------------------------------------------------------------------------------------------------------

Player::Player(const Deque<Plane>& planes, const Deque<WarpHole>& warphole, const SharedPtr<Start>& start,
	const SharedPtr<Goal>& goal, const Deque<Door>& doors, const Deque<Key>& keys)
	: Obj3D(), planes(planes), warphole(warphole), start(start), goal(goal), doors(doors), keys(keys), key_num(0)
{
	scale = Fill3(0.166f);
}

void Player::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// 操作
	if (!GS::GetEditMode())	Move(p_window, p_app);

	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}

#ifdef KDL_USE_IMGUI
	ImguiTool::BeginShowTempWindow({ 0.f, 0.f }, SceneGame::LabelName.data());

	ImGui::Text(u8"自機座標 : %.01f, %.01f", pos.x, pos.z);

	ImguiTool::Spacing(5u, true);

	ImGui::End();
#endif
#else
	Move(p_window, p_app);
#endif
}

void Player::Move(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;

	constexpr float Movement{ 1.f };
	constexpr float AdjRadY{ Math::PAI<float> / 2.f };

	float& ang{ angle.y };
	auto input{ p_window->GetInput() };

	is_move = false;
	speed.Clear();

	auto KeyInput{ [&](Keys key1, Keys key2) {
		return input->IsTrgKey(key1) || input->IsTrgKey(key2);
	} };

	// 左
	if (KeyInput( Keys::Left, Keys::A ))
	{
		speed.x += Movement;
		is_move = true;
		ang = -AdjRadY;
	}
	// 右
	else if (KeyInput( Keys::Right, Keys::D ))
	{
		speed.x -= Movement;
		is_move = true;
		ang = AdjRadY;
	}
	// 上
	else if (KeyInput( Keys::Up, Keys::W ))
	{
		speed.z -= Movement;
		is_move = true;
		ang = 0.f;
	}
	// 下
	else if (KeyInput( Keys::Down, Keys::S ))
	{
		speed.z += Movement;
		is_move = true;
		ang = AdjRadY * 2.f;
	}

	// 動いた上で進めるかの判定
	if (is_move && isMoveObjectCheck())
	{
		pos += speed;
	}
	else
		is_move = false;
}

void Player::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(0.f, angle.y, 0.f);
		T = DirectX::XMMatrixTranslation(pos.x, pos.y + 0.5f, pos.z);
		W = S * R * T;
	}
	DirectX::XMFLOAT4X4 wvp, w;
	DirectX::XMStoreFloat4x4(&w, W);
	GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

	auto Draw{ [&](auto& obj)
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, { WHITE, 1.f },
				static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA)); } };

	Draw(model);
}

bool Player::isMoveObjectCheck()
{
	bool is_dead{ false };

	const auto next_pos{ pos + speed };

	auto FindData{ [&next_pos](const auto& data) { return std::find_if(data.cbegin(), data.cend(),
		[&](const auto& dt) { return(dt.pos == next_pos); }); } };

	const auto find_plane{ FindData(planes) };

	// 進んだ先に地面を発見した場合
	if (find_plane != planes.cend())
	{
		is_dead = find_plane->GetIsDead();
	}

	auto AnyOfSamePos{ [&next_pos](const auto& data) { return (std::any_of(data.cbegin(), data.cend(),
		[&](const auto& dt) { return(dt.pos == next_pos); })); } };
	auto SamePos{ [&next_pos](const auto& data)
	{ if (!data) return false; return (data->pos == next_pos); } };

	bool is_plane_pos{ AnyOfSamePos(planes) };
	bool is_warphole{ AnyOfSamePos(warphole) };
	bool is_start{ SamePos(start) };
	bool is_goal{ SamePos(goal) };
	bool is_doors{ AnyOfSamePos(doors) };

	// 生きている床かワープホールの時だけ進む
	return ((!is_dead && is_plane_pos) || (is_warphole || is_start || is_goal || is_doors));
}

//------------------------------------------------------------------------------------------------------
// スタート
//------------------------------------------------------------------------------------------------------

Start::Start()
{
	scale = Fill3(0.166f);
	angle.y = 3.1415f;
	color = { WHITE, 1.f };
}

void Start::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using GS = SceneGame;

	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
#endif
}

void Start::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(0.f, angle.y, 0.f);
		T = DirectX::XMMatrixTranslation(pos.x, pos.y - 0.5f, pos.z);
		W = S * R * T;
	}
	DirectX::XMFLOAT4X4 wvp, w;
	DirectX::XMStoreFloat4x4(&w, W);
	GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

	auto Draw{ [&](auto& obj)
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, { WHITE, 1.f },
				static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA)); } };

	Draw(model);
}

//------------------------------------------------------------------------------------------------------
// ゴール
//------------------------------------------------------------------------------------------------------

Goal::Goal(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = Fill3(0.15f);
	angle.y = 3.1415f;
}

Goal::Goal()
	: is_back_world(false)
{
	scale = Fill3(0.15f);
	angle.y = 3.1415f;
}

void Goal::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using GS = SceneGame;

	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
#endif
}

void Goal::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	VF4 color{ WHITE, 1.f };

	// モデル
	//if (GS::back_world_mode == is_back_world)

	// 板ポリ
	{
		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(1.f, 1.f, 1.f);
			R = DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f);
			T = DirectX::XMMatrixTranslation(pos.x, -0.5f, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

		auto Draw{ [&](auto& obj)
		{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, color); } };

		Draw(board);
	}

	{
		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			R = DirectX::XMMatrixRotationRollPitchYaw(0.f, angle.y, 0.f);
			T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

		if (GS::back_world_mode != is_back_world)
			color.w = BackWorldAlpha;
		auto Draw{ [&](auto& obj)
		{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, color); } };

		Draw(model);
	}
}
