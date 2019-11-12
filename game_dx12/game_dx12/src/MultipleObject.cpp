#include "MultipleObject.h"
#include "KDL.h"
#include "GameScene.h"

//------------------------------------------------------------------------------------------------------
// 地面
//------------------------------------------------------------------------------------------------------

Plane::Plane()
	: hp(HpMax), pl_stand(false), pl_stand_first(false), is_dead(false)
{
	angle.x = 3.14f * 0.5f;

	pl_pos = Fill3((std::numeric_limits<float>::max)());
}

Plane::Plane(const uint32_t hp)
	: hp(hp), pl_stand(false), pl_stand_first(false), is_dead(false)
{
	angle.x = 3.14f * 0.5f;

	pl_pos = Fill3((std::numeric_limits<float>::max)());
}

void Plane::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	if (!GS::GetEditMode())
	{
		// まだ生きている
		if (hp != 0)
		{
			if (pl_pos == pos)
				pl_stand = true;
			else
			{
				pl_stand = false;
				pl_stand_first = false;
			}

			// プレーヤーがいる上に初回だけ処理する
			if (pl_stand && !pl_stand_first)
			{
				hp--;
				pl_stand_first = true;
			}
		}
		// 死んでいるが消えるのを防止する為、プレーヤーが離れてから消す
		else
		{
			if (hp == 0u && pl_pos != pos)	is_dead = true;
		}
	}
#else
	// まだ生きている
	if (hp != 0)
	{
		if (pl_pos == pos)
			pl_stand = true;
		else
		{
			pl_stand = false;
			pl_stand_first = false;
		}

		// プレーヤーがいる上に初回だけ処理する
		if (pl_stand && !pl_stand_first)
		{
			hp--;
			pl_stand_first = true;
		}
	}
	// 死んでいるが消えるのを防止する為、プレーヤーが離れてから消す
	else
	{
		if (hp == 0u && pl_pos != pos)	is_dead = true;
	}
#endif

	// 編集モードのみ
#if false
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()))
	{
		// 色調整
		is_dead ? color = { BLACK, 1.f } : color = { WHITE, 1.f };

		// 移動中なら
		if (is_move_select) pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
#endif
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
#if !_DEBUG
		if (!is_dead)
			GMLIB->DrawBoardImage(hp == HpMax ? model_handle : broken_model_handle, pos,
				{ scale.x, scale.y }, angle, GS::LightDir);
#else
		if (!is_dead)
			GMLIB->DrawBoardImage(hp == HpMax ? model_handle : broken_model_handle, pos,
				{ scale.x, scale.y }, angle, GS::LightDir, color);
#endif
	}
}

//------------------------------------------------------------------------------------------------------
// 壁
//------------------------------------------------------------------------------------------------------

Wall::Wall()
{
	scale = { 0.005f, 0.005f, 0.005f };
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
	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir);
}

//------------------------------------------------------------------------------------------------------
// ワープホール
//------------------------------------------------------------------------------------------------------

WarpHole::WarpHole(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = { 0.005f, 0.005f, 0.005f };
}

WarpHole::WarpHole()
	: is_back_world(false)
{
	scale = { 0.005f, 0.005f, 0.005f };
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
	const VF4 color{ WHITE, (SceneGame::back_world_mode == is_back_world ? 1.f : 0.5f) };

	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, color);
}

//------------------------------------------------------------------------------------------------------
// 鍵
//------------------------------------------------------------------------------------------------------

Key::Key(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = { 0.005f, 0.005f, 0.005f };
}

Key::Key()
	: is_back_world(false)
{
	scale = { 0.005f, 0.005f, 0.005f };
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
	VF4 color;

	if (SceneGame::back_world_mode == is_back_world)
		color = { YELLOW, 1.f };
	else
		color = { GREEN, 0.5f };

	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, color);
}

//------------------------------------------------------------------------------------------------------
// ドア
//------------------------------------------------------------------------------------------------------

Door::Door(const bool is_back_world)
	: is_open(false), is_back_world(is_back_world)
{
	scale = { 0.005f, 0.005f, 0.005f };
}

Door::Door()
	: is_open(false), is_back_world(false)
{
	scale = { 0.005f, 0.005f, 0.005f };
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
	const VF4 color{ OLIVE, (SceneGame::back_world_mode == is_back_world ? 1.f : 0.5f) };

	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, color);
}
