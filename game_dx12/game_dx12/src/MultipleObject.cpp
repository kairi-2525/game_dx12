#include "MultipleObject.h"
#include "KDL.h"
#include "GameScene.h"

//------------------------------------------------------------------------------------------------------
// 地面
//------------------------------------------------------------------------------------------------------

Plane::Plane()
	: hp(HpMax), pl_stand(false), is_dead(false)
{
	//angle.x = 3.14f * 0.5f;

	pl_pos = Fill3((std::numeric_limits<float>::max)());
}

Plane::Plane(const uint32_t hp)
	: hp(hp), pl_stand(false), is_dead(false)
{
	//angle.x = 3.14f * 0.5f;

	pl_pos = Fill3((std::numeric_limits<float>::max)());
}

Plane::~Plane() noexcept
{
}

void Plane::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	// まだ生きている
	if (hp != 0)
	{
		// 自機が上にいる
		if (pl_pos == pos)
			pl_stand = true;

		if (Player::is_move)
		{
			if (pl_stand)
			{
				using SG = SceneGame;

				hp--;

				if (hp == 0)
				{
					int handle = SG::audio->CreatePlayHandle(SG::se_break, 0.f, false, false, 0.f, 0.f, 0, false, false);
					SG::audio->Play(SG::se_break, handle, 0.01f, 0.2f, false);
				}
				else
				{

					int handle = SG::audio->CreatePlayHandle(SG::se_crack, 0.f, false, false, 0.f, 0.f, 0, false, false);
					SG::audio->Play(SG::se_crack, handle, 0.01f, 0.2f, false);
				}
			}

			pl_stand = false;
		}
	}
	// 死んでいるが消えるのを防止する為、プレーヤーが離れてから消す
	else
	{
		if (hp == 0u && pl_pos != pos)
		{
			is_dead = true;
		}
	}
}

void Plane::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// 編集モード時のみ
	if (GS::GetEditMode())
	{
		GMLIB->DrawBoardImage(hp == (HpMax - 1u) ? broken_model_handle : model_handle, pos,
			{ scale.x, scale.y }, angle, GS::LightDir, color);
	}
	else
#endif
	{
		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale.x, scale.y, 1.f);
			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			T = DirectX::XMMatrixTranslation(pos.x, pos.y - 0.5f, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

		if (!is_dead)
		{
			auto Draw{ [&](auto& obj)
			{ obj->AddCommand(p_app->GetCommandList(), p_app, wvp, w, GS::LightDir, { WHITE, 1.f },
				static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA)); } };

			if (hp == HpMax)
			{
				if (GS::back_world_mode)
					Draw(sand_board);
				else
					Draw(snow_board);
			}
			else
			{
				if (GS::back_world_mode)
					Draw(sand_broken_board);
				else
					Draw(snow_broken_board);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------
// 壁
//------------------------------------------------------------------------------------------------------

Wall::Wall()
{
	scale = Fill3(0.166f);
}

void Wall::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
#endif
}

void Wall::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

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

	auto Draw{ [&](auto& obj)
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, { WHITE, 1.f },
				static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA)); } };

	if (GS::back_world_mode)
		Draw(sand_model);
	else
		Draw(snow_model);
}

//------------------------------------------------------------------------------------------------------
// ワープホール
//------------------------------------------------------------------------------------------------------

WarpHole::WarpHole(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = Fill3(0.166f);
}

WarpHole::WarpHole()
	: is_back_world(false)
{
	scale = Fill3(0.166f);
}

void WarpHole::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
#endif
}

void WarpHole::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	decltype(WarpOn)* draw_obj{ nullptr };

	if (is_back_world)
		draw_obj = &WarpOff;
	else
		draw_obj = &WarpOn;

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

	auto Draw{ [&](auto& obj)
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, { WHITE, 1.f },
				static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA)); } };

	Draw(*draw_obj);
}

//------------------------------------------------------------------------------------------------------
// 鍵
//------------------------------------------------------------------------------------------------------

Key::Key(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = Fill3(0.166f);
}

Key::Key()
	: is_back_world(false)
{
	scale = Fill3(0.166f);
}

void Key::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
#endif
}

void Key::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	if (SceneGame::back_world_mode == is_back_world)
		color.w = 1.f;
	else
		color.w = 0.5f;

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

	auto Draw{ [&](auto& obj)
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, color,
				static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA)); } };

	Draw(model);
}

//------------------------------------------------------------------------------------------------------
// ドア
//------------------------------------------------------------------------------------------------------

Door::Door(const bool is_back_world)
	: is_open(false), is_back_world(is_back_world)
{
	scale = Fill3(0.166f);
}

Door::Door()
	: is_open(false), is_back_world(false)
{
	scale = Fill3(0.166f);
}

Door::~Door() noexcept
{
	using SG = SceneGame;

	int handle = SG::audio->CreatePlayHandle(SG::se_door, 0.f, false, false, 0.f, 0.f, 0, false, false);
	SG::audio->Play(SG::se_door, handle, 0.01f, 0.2f, false);
}

void Door::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
#endif
}

void Door::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	if (is_open)	return;

	//const VF4 color{ WHITE, (SceneGame::back_world_mode == is_back_world ? 1.f : 0.5f) };

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

	auto Draw{ [&](auto& obj)
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, color,
				static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA)); } };

	Draw(model);
}