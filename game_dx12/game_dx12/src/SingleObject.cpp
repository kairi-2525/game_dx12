#include "SingleObject.h"
#include "GameLib.h"
#include "GameScene.h"

//------------------------------------------------------------------------------------------------------
// 自機
//------------------------------------------------------------------------------------------------------

Player::Player(const Deque<Plane>& planes, const Deque<WarpHole>& warphole, const SharedPtr<Start>& start,
	const SharedPtr<Goal>& goal, const Deque<Door>& doors, const Deque<Key>& keys)
	: Obj3D(), planes(planes), warphole(warphole), start(start), goal(goal), doors(doors), keys(keys)
{
	scale = { 0.005f, 0.005f, 0.005f };
}

void Player::Update()
{
	using GS = GameScene;

	// 操作
	if (!GS::GetEditMode())	Move();

	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}

#if USE_IMGUI
	ImguiTool::BeginShowTempWindow({ 0.f, 0.f }, GameScene::LabelName.data());

	ImGui::Text(u8"自機座標 : %.01f, %.01f", pos.x, pos.z);

	ImguiTool::Spacing(5u, true);

	ImGui::End();
#endif
}

void Player::Move()
{
	using Keys = KeyData::Keys;

	constexpr float Movement{ 1.f };

	bool is_move{ false };

	speed.Clear();

	if (GMLIB->isKeyDown({ Keys::Left, Keys::A }))
	{
		speed.x += Movement;
		is_move = true;
	}
	else if (GMLIB->isKeyDown({ Keys::Right, Keys::D }))
	{
		speed.x -= Movement;
		is_move = true;
	}
	else if (GMLIB->isKeyDown({ Keys::Up, Keys::W }))
	{
		speed.z -= Movement;
		is_move = true;
	}
	else if (GMLIB->isKeyDown({ Keys::Down, Keys::S }))
	{
		speed.z += Movement;
		is_move = true;
	}

	// 動いた上で進めるかの判定
	if (is_move && isMoveObjectCheck())		pos += speed;
}

void Player::Draw() const
{
	GMLIB->DrawModel(model_handle, pos, scale, angle, GameScene::LightDir, { AQUA, 1.f });
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

void Start::Update()
{
	using GS = GameScene;

	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
}

void Start::Draw() const
{
	GMLIB->DrawModel(model_handle, pos, scale, angle, GameScene::LightDir, color);
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

void Goal::Update()
{
	using GS = GameScene;

	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
}

void Goal::Draw() const
{
	const VF4 color{ RED, (GameScene::back_world_mode == is_back_world ? 1.f : 0.5f) };

	GMLIB->DrawModel(model_handle, pos, scale, angle, GameScene::LightDir, color);
}
