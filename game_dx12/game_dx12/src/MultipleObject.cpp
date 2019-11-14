#include "MultipleObject.h"
#include "KDL.h"
#include "GameScene.h"

//------------------------------------------------------------------------------------------------------
// �n��
//------------------------------------------------------------------------------------------------------

Plane::Plane()
	: hp(HpMax), pl_stand(false), is_dead(false)
{
	angle.x = 3.14f * 0.5f;

	pl_pos = Fill3((std::numeric_limits<float>::max)());
}

Plane::Plane(const uint32_t hp)
	: hp(hp), pl_stand(false), is_dead(false)
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
		// ���@����ɂ���
		if (pl_pos == pos)
			pl_stand = true;
		// ��ɂ��Ȃ�
		else
		{
			if (pl_stand)
				hp--;

			pl_stand = false;
		}
	}
	// ����ł��邪������̂�h�~����ׁA�v���[���[������Ă������
	else
	{
		if (hp == 0u && pl_pos != pos)
			is_dead = true;
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
		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale.x, scale.y, 1.f);
			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

		if (!is_dead)
		{
			auto Draw{ [&](auto& obj)
			{ obj->AddCommand(p_app->GetCommandList(), p_app, wvp, w, GS::LightDir, { WHITE, 1.f }); } };

			if (hp == HpMax)
			{
				if (GS::back_world_mode)
					Draw(sand_boad);
				else
					Draw(snow_boad);
			}
			else
			{
				if (GS::back_world_mode)
					Draw(sand_broken_boad);
				else
					Draw(snow_broken_boad);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------
// ��
//------------------------------------------------------------------------------------------------------

Wall::Wall()
{
	scale = Fill3(0.16f);
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
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, { WHITE, 1.f }); } };

	if (GS::back_world_mode)
		Draw(sand_model);
	else
		Draw(snow_model);
}

//------------------------------------------------------------------------------------------------------
// ���[�v�z�[��
//------------------------------------------------------------------------------------------------------

WarpHole::WarpHole(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = Fill3(0.005f);
}

WarpHole::WarpHole()
	: is_back_world(false)
{
	scale = Fill3(0.005f);
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
	using GS = SceneGame;

	const VF4 color{ WHITE, (GS::back_world_mode == is_back_world ? 1.f : 0.5f) };

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
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, color); } };

	Draw(model);
}

//------------------------------------------------------------------------------------------------------
// ��
//------------------------------------------------------------------------------------------------------

Key::Key(const bool is_back_world)
	: is_back_world(is_back_world)
{
	scale = Fill3(0.16f);
}

Key::Key()
	: is_back_world(false)
{
	scale = Fill3(0.16f);
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
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, color); } };

	Draw(model);
}

//------------------------------------------------------------------------------------------------------
// �h�A
//------------------------------------------------------------------------------------------------------

Door::Door(const bool is_back_world)
	: is_open(false), is_back_world(is_back_world)
{
	scale = Fill3(0.16f);
}

Door::Door()
	: is_open(false), is_back_world(false)
{
	scale = Fill3(0.16f);
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
	using GS = SceneGame;

	const VF4 color{ OLIVE, (SceneGame::back_world_mode == is_back_world ? 1.f : 0.5f) };

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
	{ obj->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, color); } };

	Draw(model);
}
