#include "SingleObject.h"
#include "KDL.h"
#include "GameScene.h"
#include "ImVecHelper.h"

//------------------------------------------------------------------------------------------------------
// 自機
//------------------------------------------------------------------------------------------------------

Player::Player(const Deque<Plane>& planes, const Deque<WarpHole>& warphole, const SharedPtr<Start>& start,
	const SharedPtr<Goal>& goal, const Deque<Door>& doors, const Deque<Key>& keys)
	: Obj3D(), planes(planes), warphole(warphole), start(start), goal(goal), doors(doors), keys(keys)
{
	scale = { 0.005f, 0.005f, 0.005f };
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

	bool is_move{ false };
	auto input{ p_window->GetInput() };
	speed.Clear();

	auto KeyInput{ [&](Keys key1, Keys key2) {
		return input->IsTrgKey(key1) || input->IsTrgKey(key2);
	} };

	if (KeyInput( Keys::Left, Keys::A ))
	{
		speed.x += Movement;
		is_move = true;
	}
	else if (KeyInput( Keys::Right, Keys::D ))
	{
		speed.x -= Movement;
		is_move = true;
	}
	else if (KeyInput( Keys::Up, Keys::W ))
	{
		speed.z -= Movement;
		is_move = true;
	}
	else if (KeyInput( Keys::Down, Keys::S ))
	{
		speed.z += Movement;
		is_move = true;
	}

	// 動いた上で進めるかの判定
	if (is_move && isMoveObjectCheck())		pos += speed;
}

void Player::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, { AQUA, 1.f });
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
	scale = { 0.005f, 0.005f, 0.005f };
	color = { BLUE, 1.f };
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
	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, color);
}

//------------------------------------------------------------------------------------------------------
// ゴール
//------------------------------------------------------------------------------------------------------

Goal::Goal(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = { 0.005f, 0.005f, 0.005f };
}

Goal::Goal()
	: is_back_world(false)
{
	scale = { 0.005f, 0.005f, 0.005f };
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
	const VF4 color{ RED, (SceneGame::back_world_mode == is_back_world ? 1.f : 0.5f) };

	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, color);
}
