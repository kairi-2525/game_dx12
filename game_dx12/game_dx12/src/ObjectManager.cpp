#include "ObjectManager.h"
#include "GameScene.h"
#include "KDL.h"
#include "ImVecHelper.h"
#include "LoadAllFileName.h"
#include "Time.h"
#include "Random.h"

ObjectManager::ObjectManager()
	: select_enm(nullptr), select_waypoint(nullptr), masu_pos(0.f, 0.f, 0.f), is_goal(false), timer(0.0)
{
	now_move_object = edit_mode_first = false;
}

// �X�V
void ObjectManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	// �e�I�u�W�F�N�g�X�V
	if (!is_goal)
	{
		// �P��I�u�W�F�N�g
		objects.SingleObjVisit([&](auto& object) { if (object) object->Update(p_window, p_app); });

		// �����I�u�W�F�N�g
		objects.MultiObjVisit([&](auto& objects) { for (auto& obj : objects) obj.Update(p_window, p_app); });
	}

	masu_pos = GS::GetMasuPos();

#if false
	// ���[�h�ɂ���čX�V�𕪂���
	GS::GetEditMode() ? EditModeUpdate(p_window, p_app) : NormalModeUpdate(p_window, p_app);

	// �I�u�W�F�N�g�폜
	{
		auto EraseObject{ [](auto& data) { data.erase(std::remove_if(
			data.begin(), data.end(), [](const auto& dt) { return (!dt); }), data.end()); } };

		// �����I�u�W�F�N�g
		objects.MultiObjVisit(EraseObject);

		// �P��I�u�W�F�N�g
		objects.SingleObjVisit([](auto& object) { if (object && !(*object)) object = nullptr; });
	}
#else
	NormalModeUpdate(p_window, p_app);
#endif
}

// �`��
void ObjectManager::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// �P��I�u�W�F�N�g
	objects.SingleObjVisit([&](auto& object) { if (object) object->Draw(p_window, p_app); });

	// �����I�u�W�F�N�g
	objects.MultiObjVisit([&](auto& objects) { for (auto& obj : objects) obj.Draw(p_window, p_app); });

	for (auto& effect : effects)
	{
		using GS = SceneGame;

		constexpr int BS{ static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA) };

		auto& angle{ effect.angle };
		auto& pos{ effect.pos };

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			if (SceneGame::back_world_mode)
				S = DirectX::XMMatrixScaling(0.2f, 0.2f, 0.2f);
			else
				S = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);

			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			T = DirectX::XMMatrixTranslation(pos.x, pos.y - 0.5f, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

		if (SceneGame::back_world_mode)
		{
			sand_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, GS::LightDir, { WHITE, 1.f }, BS, false, true, false);
		}
		else
		{
			crystal_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, GS::LightDir, { WHITE, 1.f }, BS, false, true, false);
		}
	}
}

// �ҏW���[�h�X�V
void ObjectManager::EditModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;
	using Buttons = KDL::MOUSE_INPUTS;
	using GS = SceneGame;

#ifdef KDL_USE_IMGUI
#if false
	auto input{ p_window->GetInput() };

	// �ҏW���[�h
	if (!GS::GetEnmEditMode())
	{
		// �ύX��̏���
		if (edit_mode_first)
		{
			edit_mode_first = false;

			auto& enemys{ objects.GetChangeObjects<Enemy>() };

			// �S�Ă̓G�̑I������������
			for (auto& enm : enemys)
				enm.SetIsSelect(false);
		}

		ImGui::Text(u8"���N���b�N�@�@�@      �F����");
		ImGui::Text(u8"�E�N���b�N�@�@�@      �F�폜");
		ImGui::Text(u8"�z�C�[���N���b�N(hold)�F�ړ�");

		ImguiTool::Spacing(5u, true);

		// �ǉ�
		if (!now_move_object)  // �ړ����͒ǉ��o���Ȃ�
		{
			static const char* AddObjectNames[]
			{ u8"��", u8"��", u8"�G", u8"���@", u8"���[�v�z�[��", u8"�X�^�[�g", u8"�S�[��", u8"�h�A", u8"�J�M", "None(�ړ����Ȃǂɐݒu����ƕ֗�)" };
			static int select_item{ -1 };
			static int hp{ 2 };

			ImGui::Combo(u8"��������I�u�W�F�N�g", &select_item, AddObjectNames, IM_ARRAYSIZE(AddObjectNames));

			// �n�ʂ̂�
			if (select_item == 0) ImGui::SliderInt(u8"HP�ݒ�", &hp, 0, 2);

			ImguiTool::Spacing(2u, true);

			if (select_item != -1)
			{
				GS::is_save = false;

				// �E�N���b�N�Œǉ�
				if (!ImGui::GetIO().WantCaptureMouse && input->IsHitMouse(Buttons::leftButton))
				{
					const VF3& masu_pos{ GS::GetMasuPos() };

					// �I��������ނɂ���Đ�������
					switch (select_item)
					{
					case 0:		// ��
					{
						// �S�ẴI�u�W�F�N�g�Əd�Ȃ��Ă��Ȃ�
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Plane>(hp) };

							back.CheckHP();

							back.pos = masu_pos;
						}

						break;
					}
					case 1:		// ��
					{
						// �S�ẴI�u�W�F�N�g�Əd�Ȃ��Ă��Ȃ�
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Wall>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 2:		// �G
					{
						// �������ɏd�Ȃ��Ă��āA�G�Ǝ��@�ƌ��ɏd�Ȃ��Ă��Ȃ�
						if (AnyOfNotExceptPlaneObjSamePos())
						{
							auto& back{ objects.AddObjects<Enemy>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 3:		// ���@
					{
						// �������ɏd�Ȃ��Ă��āA�G�Ǝ��@�ƌ��ɏd�Ȃ��Ă��Ȃ�
						if (AnyOfNotExceptPlaneObjSamePos())
						{
							auto& back{ objects.AddObjects<Player>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 4:		// ���[�v�]�[��
					{
						// �S�ẴI�u�W�F�N�g�Əd�Ȃ��Ă��Ȃ�
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<WarpHole>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					case 5:		// �X�^�[�g
					{
						// �S�ẴI�u�W�F�N�g�Əd�Ȃ��Ă��Ȃ�
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Start>() };

							back.pos = masu_pos;
						}

						break;
					}
					case 6:		// �S�[��
					{
						// �S�ẴI�u�W�F�N�g�Əd�Ȃ��Ă��Ȃ�
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Goal>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					case 7:		// �h�A
					{
						// �S�ẴI�u�W�F�N�g�Əd�Ȃ��Ă��Ȃ�
						if (AnyOfNotAllObjSamePos())
						{
							auto& back{ objects.AddObjects<Door>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					case 8:		// ��
					{
						// �������ɏd�Ȃ��Ă��āA�G�Ǝ��@�ƌ��ɏd�Ȃ��Ă��Ȃ�
						if (AnyOfNotExceptPlaneObjSamePos())
						{
							auto& back{ objects.AddObjects<Key>(GS::back_world_mode) };

							back.pos = masu_pos;
						}

						break;
					}
					}
				}
			}
		}

		// �폜
		// Imgui��̃E�B���h�E�ɏd�Ȃ��Ă��炸�A�E�N���b�N����������
		if (!now_move_object && !ImGui::GetIO().WantCaptureMouse &&  // �ړ����͍폜�ł��Ȃ�
			input->IsTrgMouse(Buttons::rightButton))
		{
			auto DeleteObjectS{ [&](auto& object)
				{ if (object && object->pos == GS::GetMasuPos()) object->EndExist(); } };
			auto DeleteObjectM{ [&](auto& data)
			{ for (auto& dt : data) { if (dt.pos == GS::GetMasuPos()) dt.EndExist(); }; } };

			static bool deleted{ false };
			static VF3 save_pos{ GS::GetMasuPos() };
			const VF3 masu_pos{ GS::GetMasuPos() };

			// �n�ʂƂ܂Ƃ߂ď�����̂�h�����߁i�N���b�N���ď��񂩈Ⴄ�}�X�Ɉړ��������j
			if (input->IsTrgMouse(Buttons::rightButton) || save_pos != masu_pos)
			{
				auto& keys{ objects.GetChangeObjects<Key>() };
				auto& player{ objects.GetChangeObjects<Player>() };
				auto& enemys{ objects.GetChangeObjects<Enemy>() };

				// ������
				save_pos = masu_pos;
				deleted = false;

				// ���@
				if (player && player->pos == GS::GetMasuPos())
				{
					player->EndExist();
					deleted = true;
				}

				// ��
				for (auto& dt : keys)
				{
					if (!deleted && dt.pos == GS::GetMasuPos())
					{
						dt.EndExist();
						deleted = true;
					}
				}

				// �G
				for (auto& dt : enemys)
				{
					if (!deleted && dt.pos == GS::GetMasuPos())
					{
						dt.EndExist(); deleted = true;
					}
				}

				if (deleted) GS::is_save = false;
			}

			// ���ɏ�����Ă��Ȃ����
			if (!deleted)
			{
				GS::is_save = false;
				save_pos = masu_pos;
				deleted = false;

				// �P��I�u�W�F�N�g
				objects.SingleObjVisit(DeleteObjectS);

				// �����I�u�W�F�N�g
				objects.MultiObjVisit(DeleteObjectM);
			}
		}

		// �ړ�
		if (!ImGui::GetIO().WantCaptureMouse) MoveObject(p_window, p_app);
	}
	// �G�ҏW���[�h���͒ǉ��E�폜�E�ړ��͏o���Ȃ�
	else
		EnmEditModeUpdate(p_window, p_app);
#endif
#endif
}

// �G�ҏW���[�h�X�V
void ObjectManager::EnmEditModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;
	using Buttons = KDL::MOUSE_INPUTS;
	using GS = SceneGame;

	auto input{ p_window->GetInput() };

	edit_mode_first = true;

	const auto& masu_pos{ GS::GetMasuPos() };

	{
		auto& enemys{ objects.GetChangeObjects<Enemy>() };

		// �G�����݂��Ȃ��Ȃ珈�����Ȃ�
		if (enemys.empty())	return;

		// ���N���b�N
		if (input->IsTrgMouse(Buttons::leftButton))
		{
			select_waypoint = nullptr;

			// �I�������G������
			auto find{ std::find_if(enemys.begin(), enemys.end(),
				[&](const auto& enm) { return (enm.pos == masu_pos); }) };

			// �S�Ă̑I��������
			for (auto& enm : enemys) enm.SetIsSelect(false);
			select_enm = nullptr;

			// �I�������}�X�Əd�Ȃ��Ă���G�𔭌�����
			if (find != enemys.end())
			{
				find->SetIsSelect(true);  // �I������Ă���
				select_enm = &(*find);
				GS::is_save = false;
			}
		}
	}

	// �I�΂�Ă��鎞
	if (select_enm)
	{
		static VF3 save_wp_pos;

		// Enter�L�[
		if (input->IsHitKey(Keys::Enter))
		{
			// �����Ă��珉��
			if (input->IsTrgKey(Keys::Enter))
			{
				auto& waypoint{ select_enm->GetWayPoint() };

				auto FindWayPoint{ [&]()
				{ return std::find_if(waypoint.begin(), waypoint.end(), [&](auto& wp)
					{ return (wp.pos == masu_pos); }); } };

				auto WayPointNotSamePos{ [&]()
				{ return std::all_of(waypoint.cbegin(), waypoint.cend(), [&](const auto& wp)
					{ return (wp.pos != masu_pos); }); } };

				// ����������
				select_waypoint = nullptr;
				Clear(save_wp_pos);

				// �ǉ�
				// �E�F�C�|�C���g��I�����Ă��炸�A�������ɏd�Ȃ��Ă��āA�G�Ǝ��@�ƌ��ɏd�Ȃ��Ă��Ȃ�
				if (!select_waypoint && AnyOfNotExceptPlaneObjSamePos() && WayPointNotSamePos())
				{
					select_enm->AddWayPoint(masu_pos);
				}
				// �ړ�
				// �I�������}�X��ɃE�F�C�|�C���g�𔭌�����
				else if (auto itr{ FindWayPoint() }; itr != waypoint.cend())
				{
					select_waypoint = &(*itr);
					save_wp_pos = masu_pos;
				}
			}
			// ���ڈȍ~
			else
			{
				// �E�F�C�|�C���g��I�����Ă���Ȃ�A�ʒu���X�V����
				if (select_waypoint) select_waypoint->pos = masu_pos;
			}
		}
		// �E�F�C�|�C���g��I�����āAEnter�L�[�b�����u��
		else if (select_waypoint && !input->IsHitKey(Keys::Enter))
		{
			// �������ɏd�Ȃ��Ă��āA�G�Ǝ��@�ƌ��ɏd�Ȃ��Ă��Ȃ�
			if (AnyOfNotExceptPlaneObjSamePos())
			{
				select_waypoint = nullptr;  // �I��������
				Clear(save_wp_pos);
			}
			else
			{
				// �ʒu�����Ƃɖ߂��Ă���A�I��������
				select_waypoint->pos = save_wp_pos;
				select_waypoint = nullptr;
				Clear(save_wp_pos);
			}
		}
		// �폜
		else if (input->IsTrgKey(Keys::Back))
		{
			// �G��I�����Ă��Ȃ��Ȃ�폜�i�����I�ɃE�F�C�|�C���g�G���g�ɐݒ肵�Ă���ׁj
			if (masu_pos != select_enm->pos)
				select_enm->EraseWayPoint(masu_pos);
		}
	}

#ifdef KDL_USE_IMGUI
	const auto vp{ p_app->GetViewport() };
	const VF2 s_size{ vp.Width, vp.Height };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, u8"�G�ҏW���[�h");

	ImGui::Text(u8"�ҏW����G�����N���b�N");
	ImGui::Text(u8"�K���ȏ������N���b�N�őI������");
	ImGui::Text(u8"�v���v���ɐݒu���邾���ō\���܂���B\n"
		u8"�����ł����Ɍ������܂�");

	ImguiTool::Spacing(3u, true);

	ImGui::Text(u8"�E�F�C�|�C���g���");

	ImGui::Separator();

	ImGui::Text(u8"Enter       : �ǉ�");
	ImGui::Text(u8"Enter(Hold) : �ړ�");
	ImGui::Text(u8"BackSpace   : �폜");

	ImguiTool::Spacing(3u, true);

	if (select_enm)
	{
		const auto& waypoints{ select_enm->GetWayPoint() };

		// �E�F�C�|�C���g�����݂��Ȃ��Ȃ牽�����Ȃ�
		if (!waypoints.empty())
		{
			ImGui::Text(u8"�E�F�C�|�C���g�����F%u��", waypoints.size() - 1);

			for (size_t i = 0u, length = waypoints.size(); i < length; i++)
			{
				if (i == length - 1u)	break;  // �Ō�͉������Ȃ�

				constexpr float AdjPosY{ -15.f };

				const auto& wp{ waypoints[i] };
				auto vp{ p_app->GetViewport() };

				VF2 pos{ GS::TransformLocalToScreen({ vp.Width, vp.Height }, wp.pos) };

				pos.y += AdjPosY;

				const std::string label{ "Number" + std::to_string(i) };

				ImguiTool::BeginShowTempWindow(pos, label.data(), false);

				ImGui::Text(u8"���� : %u", i + 1u);

				ImGui::End();
			}
		}
	}

	ImGui::End();
#endif
#endif
}

// �ʏ탂�[�h�X�V
void ObjectManager::NormalModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;
	using Keys = KDL::KEY_INPUTS;

	// �����E���[�h�ݒ�
	{
		static bool first{ true };

		bool& back_world_mode{ GS::back_world_mode };
		const auto& warphole{ objects.GetObjects<WarpHole>() };
		const auto& player{ objects.GetObjects<Player>() };

		auto WarpHoleCheck{ [&]()
		{ return std::any_of(warphole.cbegin(), warphole.cend(), [&](const auto& wh)
			{ if (back_world_mode == wh.GetBackWorld())
				return (wh.pos == player->pos); else return false; }); } };

		// ���[�v�z�[���ɓ������ď���
		if (first && player && WarpHoleCheck())
		{
			first = false;
			back_world_mode ^= true;

			using SG = SceneGame;

			int handle = SG::audio->CreatePlayHandle(SG::se_waap, 0.f, false, false, 0.f, 0.f, 0, false, false);
			SG::audio->Play(SG::se_waap, handle, 0.01f, Volume / 2.f, false);

			auto& plane{ objects.GetChangeObjects<Plane>() };

			std::for_each(plane.begin(), plane.end(), [](auto& data) { data.InversionProc(); });
		}
		else if (!first && player && !WarpHoleCheck())
		{
			first = true;
		}
	}

	// �G�t�F�N�g�p
	{
		// ����
		{
			static RndDoubleMaker random_inc_rate{ 1.0, 0.001 };

			static double set_inc_rate{ random_inc_rate.GetRnd() };

			if (timer > set_inc_rate)
			{
				constexpr float BasePosY{ 20.f };

				static RndDoubleMaker rand_pos{ 80.0, -80.0 };
				static RndDoubleMaker rand_ang{ 3.14, -3.14 };

				auto& c_pos{ SceneGame::camera->GetPosition() };

				effects.emplace_back(
					VF3{ rand_pos.GetRnd<float>() + c_pos.x, c_pos.y + 5.f, rand_pos.GetRnd<float>() + c_pos.z },
					VF3{ rand_ang.GetRnd<float>(), rand_ang.GetRnd<float>(), rand_ang.GetRnd<float>() });

				set_inc_rate = random_inc_rate.GetRnd();
			}

			timer += p_window->GetElapsedTime();
		}

		// �X�V
		{
			for (auto& effect : effects)
			{
				// ���W
				{
					constexpr float Speed{ 2.f };

					effect.pos.y -= Speed * p_window->GetElapsedTime();
				}

				// �p�x
				{
					constexpr float Speed{ Math::ToRadian(10.f) };

					effect.angle.x -= Speed * p_window->GetElapsedTime();
					effect.angle.y -= Speed * p_window->GetElapsedTime();
					effect.angle.z -= Speed * p_window->GetElapsedTime();
				}
			}
		}

		// �폜
		effects.erase(std::remove_if(effects.begin(), effects.end(), [](Effect& ef) { return ef.pos.y < -50.f; }),
			effects.end());
	}

	edit_mode_first = true;

	if (auto& player{ objects.GetChangeObjects<Player>() }; player)
	{
		// �v���[���[�ƓG�̔���
		{
			constexpr float AdjstDistance{ 1.f };

			auto& enemies{ objects.GetObjects<Enemy>() };

			for (auto& enm : enemies)
			{
				const float dis{ Math::Distance(enm.pos, player->pos) };

				// ��苗���܂ŋ߂Â�����Q�[���I�[�o�[
				if (dis < AdjstDistance)
				{
					SceneGame::init_scene = true;
				}
			}
		}

		// �S�[���Ǝ��@�̔���
		if (auto& goal{ objects.GetObjects<Goal>() }; goal)
		{
			using SG = SceneGame;

			// �������E���œ������W�ɂȂ�����V�[���؂�ւ������ɓ���
			if ((goal->GetIsBackWorld() == SceneGame::back_world_mode) && (goal->pos == player->pos)
				&& !SG::audio->IsPlay(SG::se_goal, SG::p_se_goal))
			{
				SG::p_se_goal =
					SG::audio->CreatePlayHandle(SG::se_goal, 0.f, false, false, 0.f, 0.f, 0, false, false);
				SG::audio->Play(SG::se_goal, SG::p_se_goal, 0.01f, Volume, false);

				is_goal = true;
				SceneGame::execution_quick_exit = true;
			}
		}

		// ���Ǝ��@�̔���
		{
			auto& keys{ objects.GetChangeObjects<Key>() };

			for (auto& key : keys)
			{
				if (key.GetBackWorld() != SceneGame::back_world_mode)	continue;

				if (key.pos == player->pos)
				{
					player->key_num++;
					key.EndExist();

					break;
				}
			}
		}

		// �h�A�Ǝ��@�̔���
		if (auto input{ p_window->GetInput() }; input->IsHitAnyKey())
		{
			using Vec2sub::MakeVector2;
			using Math::AdjEqual;

			constexpr float AdjstDistance{ 1.4f };
			constexpr float AdjRadY{ Math::PAI<float> / 2.f };

			auto& doors{ objects.GetChangeObjects<Door>() };

			for (auto& door : doors)
			{
				//if (door.GetBackWorld() != SceneGame::back_world_mode)	continue;

				const float dis{ Math::Distance(door.pos, player->pos) };

				// �v���[���[���h�A�ɐڐG���A���������Ă���
				if (door.pos == player->pos && player->key_num > 0)
				{
					player->key_num--;
					door.OpenDoor();

					break;
				}
				// �v���[���[���h�A�ɐڐG���A���������Ă��Ȃ�
				else if (door.pos == player->pos && (player->key_num == 0 || door.is_open))
				{
					const float ang_y{ player->angle.y };
					VF3& p_pos{ player->pos };

					// �����Ă�������Ƌt�����ɖ߂�

					// ��
					if (AdjEqual(ang_y, -AdjRadY))
					{
						p_pos.x -= 1.f;
					}
					// �E
					else if (AdjEqual(ang_y, AdjRadY))
					{
						p_pos.x += 1.f;
					}
					// ��
					else if (AdjEqual(ang_y, 0.f))
					{
						p_pos.z += 1.f;
					}
					// ��
					else if (AdjEqual(ang_y, AdjRadY * 2.f))
					{
						p_pos.z -= 1.f;
					}
				}
			}
		}
	}

	// �n�ʂɃv���[���[���W���Z�b�g
	if (const auto& player{ objects.GetObjects<Player>() }; player)
		Plane::SetPlPosition(player->pos);

	// �I�u�W�F�N�g�폜
	{
		auto EraseObject{ [](auto& data) { data.erase(std::remove_if(
			data.begin(), data.end(), [](const auto& dt) { return (!dt); }), data.end()); } };

		// �����I�u�W�F�N�g
		objects.MultiObjVisit(EraseObject);

		// �P��I�u�W�F�N�g
		objects.SingleObjVisit([](auto& object) { if (object && !(*object)) object = nullptr; });
	}
}

// �I�u�W�F�N�g�̈ړ�
void ObjectManager::MoveObject(KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Buttons = KDL::MOUSE_INPUTS;
	using GS = SceneGame;

	static VF3 save_pos;

	const auto& masu_pos{ GS::GetMasuPos() };
	auto input{ p_window->GetInput() };

	// ���N���b�N���������i�͂ށj
	if (input->IsTrgMouse(Buttons::middleButton))
	{
		bool is_finded{ false };

		auto AnyOfSamePos{ [&]()
		{ return (IsSamePosObject<Player>() ||
				 IsSamePosObject<Enemy>() ||
				 IsSamePosObject<Key>()); } };

		// ������
		objects.select_m_obj = std::nullopt;
		objects.select_s_obj = std::nullopt;

		// ���@�E�G�E���̂����ꂩ�ɏd�Ȃ��Ă���
		if (AnyOfSamePos())
		{
			// ���@
			if (auto& player{ objects.GetChangeObjects<Player>() }; player)
			{
				if (player->pos == masu_pos)
				{
					player->is_move_select = true;
					is_finded = true;
					save_pos = masu_pos;
					objects.select_s_obj.emplace(&(*player));
				}
			}

			// �G
			for (auto& enemy : objects.GetChangeObjects<Enemy>())
			{
				// ���Ɍ������Ă���̂łƂ΂�
				if (is_finded)	break;

				if (enemy.pos == masu_pos)
				{
					enemy.is_move_select = true;
					is_finded = true;
					save_pos = masu_pos;
					objects.select_m_obj = &enemy;
				}
			}

			// ��
			for (auto& key : objects.GetChangeObjects<Key>())
			{
				// ���Ɍ������Ă���̂łƂ΂�
				if (is_finded)	break;

				if (key.pos == masu_pos)
				{
					key.is_move_select = true;
					is_finded = true;
					save_pos = masu_pos;
					objects.select_m_obj = &key;
				}
			}
		}
		// �d�Ȃ��Ă��Ȃ��̂ő�������
		else
		{
			// single_objects
			objects.SingleObjVisit([&](auto& data)
				{
					if (data && (data->pos == masu_pos))
					{
						data->is_move_select = true;
						is_finded = true;
						save_pos = masu_pos;
						objects.select_s_obj = &(*data);
					}
				});

			// multiple_objects
			objects.MultiObjVisit([&](auto& data)
				{
					for (auto& dt : data)
					{
						// ���Ɍ������Ă���̂łƂ΂�
						if (is_finded)	break;

						if (dt.pos == masu_pos)
						{
							is_finded = true;
							dt.is_move_select = true;
							save_pos = masu_pos;
							objects.select_m_obj = &dt;
						}
					}
				});
		}

		// �����E�������Ŕ��f����
		now_move_object = is_finded;
	}
	// ���N���b�N���������i�z�u�j
	else if (GMLIB->isMouseUp(Buttons::middleButton))
	{
		bool is_finded{ false };
		size_t count{ 0u };

		now_move_object = false;

		objects.SingleObjVisit([&](const auto& data)
			{ if (data && data->pos == masu_pos) count++; });

		objects.MultiObjVisit([&](const auto& data)
			{ count += std::count_if(data.cbegin(), data.cend(), [&](const auto& dt)
				{ return (dt.pos == masu_pos); }); });

		// �z�u��̃}�X�ɉ����ݒu����Ă��Ȃ�
		if (count <= 1u)  // ���������������l
		{
			auto HaveKeyEnmPl{ [&]()
			{ return (objects.GetSelectObjectPtr<Player>() ||
					  objects.GetSelectObjectPtr<Enemy>() ||
					  objects.GetSelectObjectPtr<Key>()); } };

			// �z�u�悪���Ŏ������g�͌��E���@�E�G
			const bool is_plane_up_only{ HaveKeyEnmPl() };

			// single_objects��ێ�
			if (objects.select_s_obj)
			{
				// �I���������i�������A�������g�����@�Ȃ猳�̏ꏊ�֖߂��j
				std::visit([is_plane_up_only](auto& data)
					{
						if (data)
						{
							data->is_move_select = false;
							if (is_plane_up_only) data->pos = save_pos;
						}
					}, *objects.select_s_obj);

				objects.select_s_obj = std::nullopt;
			}
			// multiple_objects��ێ�
			else if (objects.select_m_obj)
			{
				// �I���������i�������A�������g�����E�G�Ȃ猳�̏ꏊ�֖߂��j
				std::visit([is_plane_up_only](auto& data)
					{
						if (data)
						{
							data->is_move_select = false;
							if (is_plane_up_only) data->pos = save_pos;
						}
					}, *objects.select_m_obj);

				objects.select_m_obj = std::nullopt;
			}
		}
		// �z�u��̃}�X�ɉ���������
		else
		{
			// �z�u�悪���Ŏ������g�͌��E���@�E�G
			{
				auto RePlaceObject{ [&]()
				{ return (IsSamePosObject<Plane>() &&
					!objects.GetSelectObjectPtr<Plane>()); } };

				auto HaveKeyEnmPl{ [&]()
				{ return (objects.GetSelectObjectPtr<Player>() ||
						  objects.GetSelectObjectPtr<Enemy>() ||
						  objects.GetSelectObjectPtr<Key>()); } };

				// �z�u�悪���Ŏ������g�͌��E���@�E�G�i�ݒu�\�j
				if (RePlaceObject() && HaveKeyEnmPl())
				{
					// single_objects��ێ�
					if (objects.select_s_obj)
					{
						// �I��������
						std::visit([](auto& data) { data->is_move_select = false; }, *objects.select_s_obj);

						objects.select_s_obj = std::nullopt;
					}
					// multiple_objects��ێ�
					else if (objects.select_m_obj)
					{
						// �I��������
						std::visit([](auto& data) { data->is_move_select = false; }, *objects.select_m_obj);

						objects.select_m_obj = std::nullopt;
					}

					return;  // ����ȏ�͑S�Đݒu�s�\�Ȃ̂ňȍ~�������Ȃ�
				}
			}

			// ��L�p�^�[���ȊO�͑S�Đݒu�s�\�ƂȂ�
			{
				// single_objects��ێ�
				if (objects.select_s_obj)
				{
					// �I��������
					std::visit([](auto& data) { data->is_move_select = false; data->pos = save_pos; },
						*objects.select_s_obj);

					objects.select_s_obj = std::nullopt;
				}
				// multiple_objects��ێ�
				else
				{
					// �I��������
					std::visit([](auto& data) { data->is_move_select = false; data->pos = save_pos; },
						*objects.select_m_obj);

					objects.select_m_obj = std::nullopt;
				}
			}
		}
	}
#endif
}

// �ǂݍ���
void ObjectManager::Load(std::atomic<size_t>* load_count, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// ���f���ǂݍ���
	{
		auto fbx_paths{ GetAllFileName("data\\models\\Game") };

		// �t�@�C�������œǂݍ��ނ̂Łi�A���t�@�x�b�g���j
		const std::vector<decltype(&Door::model)> load_models{
			&Door::model,
			&Enemy::model,
			&Goal::model,
			&Key::model,
			&Player::model,
			&Plane::sand_broken_model,
			&Plane::sand_model,
			&Wall::sand_model,
			&Plane::snow_model,
			&Plane::snow_broken_model,
			&Wall::snow_model,
			&Start::model,
			&WarpHole::warp_sand,
			&WarpHole::warp_snow,
		};

		// FBX�ȊO�̃t�@�C�������폜
		fbx_paths.erase(std::remove_if(fbx_paths.begin(), fbx_paths.end(),
			[](const auto& path) { return path.extension().string() != ".fbx"; }), fbx_paths.end());

		assert(fbx_paths.size() == load_models.size() && "�ǂݍ��ݐ�or�ݒ萔���s��");

		// FBX�ǂݍ���
		for (size_t i = 0, length = fbx_paths.size(); i < length; i++)
		{
			assert(!(*load_models[i]) && "���ɓǂݍ��܂�Ă���");

			*load_models[i] = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, fbx_paths[i], 1000u);

			(*load_count)++;
		}
	}

	// �e�N�X�`���ǂݍ���
	{
		auto png_paths{ GetAllFileName("data\\images\\Game") };

		// �t�@�C�������œǂݍ��ނ̂Łi�A���t�@�x�b�g���j
		std::vector<decltype(&crystal_board)> load_textures{
			&crystal_board,
			&Goal::board,
			&sand_board,
		};

		// �摜�ȊO�̃t�@�C�������폜
		png_paths.erase(std::remove_if(png_paths.begin(), png_paths.end(),
			[](const auto& path) { return path.extension().string() != ".png"; }), png_paths.end());

		assert(png_paths.size() == load_textures.size() && "�ǂݍ��ݐ�or�ݒ萔���s��");

		// �摜�ǂݍ���
		for (size_t i = 0, length = load_textures.size(); i < length; i++)
		{
			*load_textures[i] = std::make_unique<KDL::DX12::Geometric_Board>(p_app, png_paths[i], 10000u);

			(*load_count)++;
		}
	}
}

// �������
void ObjectManager::UnInitialize()
{
	const std::vector<decltype(Door::model)*> release_models{
		&Door::model,
		&Enemy::model,
		&Goal::model,
		&Key::model,
		&Player::model,
		&Plane::sand_broken_model,
		&Plane::sand_model,
		&Wall::sand_model,
		&Plane::snow_model,
		&Plane::snow_broken_model,
		&Wall::snow_model,
		&Start::model,
		&WarpHole::warp_sand,
		&WarpHole::warp_snow,
	};

	const std::vector<decltype(crystal_board)*> release_textures{
		&Goal::board,
		&crystal_board,
		&sand_board,
	};

	auto Clear{ [](auto& load_data) { std::for_each(load_data.begin(), load_data.end(), [](auto& data)
		{ *data = nullptr; }); } };

	Clear(release_models);
	Clear(release_textures);
}

// �����o��---------------------------------------------------------------------------------------------------
void ObjectManager::Output(std::ofstream& ofs)
{
	// �I�u�W�F�N�g���ۑ�
	ofs << objects;

	// �p�X�̐ݒ�
	node.PathSetting(objects);

	auto& enemy{ objects.GetChangeObjects<Enemy>() };

	for (auto& enm : enemy)
		enm.SetNode(node);
}

// �ǂݍ���---------------------------------------------------------------------------------------------------
void ObjectManager::Input(std::ifstream& ifs)
{
	// �I�u�W�F�N�g���擾
	ifs >> objects;

	// �p�X�̐ݒ�
	node.PathSetting(objects);

	auto& enemy{ objects.GetChangeObjects<Enemy>() };

	for (auto& enm : enemy)
		enm.SetNode(node);
}

std::ofstream& operator<<(std::ofstream& ofs, const Object& data)
{
	using namespace std::string_literals;

	// single_objects
	data.SingleObjVisit([&](const auto& dt)
		{
			OutputComment(ofs, typeid(decltype(dt)).name(), "name", "", 2u);

			const bool exist{ dt };

			ofs << F_OUT(exist) << std::endl;

			if (exist) { ofs << (*dt); }

			ofs << std::endl;
			ofs << std::endl;
			ofs << std::endl;
		});

	// multiple_objects
	data.MultiObjVisit([&](const auto& dt)
		{
			OutputComment(ofs, typeid(decltype(dt)).name(), "Class", "", 2u);

			const size_t size{ dt.size() };

			ofs << F_OUT(size) << std::endl;

			if (size != 0)
			{
				for (auto& obj : dt)
				{
					ofs << obj;
				}

				ofs << std::endl;
				ofs << std::endl;
			}

			ofs << std::endl;
		});

	// �G��AI
	{
		const std::vector<float*> ai_out
		{
			&MoveAI::move_speed,
			nullptr,
			&StopAI::stop_time,
			nullptr,
			&FindAI::stop_time,
			nullptr,
			&PatrolAI::hit_distance,
			&PatrolAI::hit_angle,
			nullptr,
			&ChaseAI::move_speed,
			&ChaseAI::max_chase_time,
			nullptr,
			&GoBackAI::move_speed,
		};

		OutputComment(ofs, "�GAI���", "", "", 2u);
		for (auto& out : ai_out)
		{
			if (out)
				ofs << F_OUT(*out);
			else
				ofs << std::endl;
		}
	}

	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, Object& data)
{
	// single_objects
	data.SingleObjVisit([&](auto& dt)
		{
			// �R�����g�擾
			InputComment(ifs);

			bool exist{ false };

			ifs >> exist;

			InputComment(ifs);

			if (exist)
			{
				auto& back{ data.AddObjects<decltype(*dt)>() };

				ifs >> back;
			}
		});

	// multiple_objects
	data.MultiObjVisit([&](auto& dt)
		{
			// �R�����g�擾
			InputComment(ifs);

			size_t size{ 0u };

			ifs >> size;

			for (size_t i = 0; i < size; i++)
			{
				auto& back{ data.AddObjects<decltype(dt[i])>() };

				ifs >> back;
			}
		});

	// �G��AI
	{
		const std::vector<float*> ai_in
		{
			&MoveAI::move_speed,
			&StopAI::stop_time,
			&FindAI::stop_time,
			&PatrolAI::hit_distance,
			&PatrolAI::hit_angle,
			&ChaseAI::move_speed,
			&ChaseAI::max_chase_time,
			&GoBackAI::move_speed,
		};

		InputComment(ifs);
		for (auto& in : ai_in)
		{
			ifs >> (*in);
		}
	}

	return ifs;
}