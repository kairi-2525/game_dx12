#include "MultipleObject.h"
#include "KDL.h"
#include "GameScene.h"

//------------------------------------------------------------------------------------------------------
// �n��
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
		// �܂������Ă���
		if (hp != 0)
		{
			if (pl_pos == pos)
				pl_stand = true;
			else
			{
				pl_stand = false;
				pl_stand_first = false;
			}

			// �v���[���[�������ɏ��񂾂���������
			if (pl_stand && !pl_stand_first)
			{
				hp--;
				pl_stand_first = true;
			}
		}
		// ����ł��邪������̂�h�~����ׁA�v���[���[������Ă������
		else
		{
			if (hp == 0u && pl_pos != pos)	is_dead = true;
		}
	}
#else
	// �܂������Ă���
	if (hp != 0)
	{
		if (pl_pos == pos)
			pl_stand = true;
		else
		{
			pl_stand = false;
			pl_stand_first = false;
		}

		// �v���[���[�������ɏ��񂾂���������
		if (pl_stand && !pl_stand_first)
		{
			hp--;
			pl_stand_first = true;
		}
	}
	// ����ł��邪������̂�h�~����ׁA�v���[���[������Ă������
	else
	{
		if (hp == 0u && pl_pos != pos)	is_dead = true;
	}
#endif

	// �ҏW���[�h�̂�
#if false
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()))
	{
		// �F����
		is_dead ? color = { BLACK, 1.f } : color = { WHITE, 1.f };

		// �ړ����Ȃ�
		if (is_move_select) pos = GS::GetMasuPos();  // ���W���}�X���W�ɂ���
	}
#endif
}

void Plane::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// �ҏW���[�h���̂�
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
// ��
//------------------------------------------------------------------------------------------------------

Wall::Wall()
{
	scale = { 0.005f, 0.005f, 0.005f };
}

void Wall::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

#if false
	// �ҏW���[�h�̂�
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // ���W���}�X���W�ɂ���
	}
#endif
}

void Wall::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir);
}

//------------------------------------------------------------------------------------------------------
// ���[�v�z�[��
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
	// �ҏW���[�h�̂�
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // ���W���}�X���W�ɂ���
	}
#endif
}

void WarpHole::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const VF4 color{ WHITE, (SceneGame::back_world_mode == is_back_world ? 1.f : 0.5f) };

	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, color);
}

//------------------------------------------------------------------------------------------------------
// ��
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
	// �ҏW���[�h�̂�
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // ���W���}�X���W�ɂ���
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
// �h�A
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
	// �ҏW���[�h�̂�
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // ���W���}�X���W�ɂ���
	}
#endif
}

void Door::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const VF4 color{ OLIVE, (SceneGame::back_world_mode == is_back_world ? 1.f : 0.5f) };

	GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, color);
}
