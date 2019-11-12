#include "ObjectManager.h"
#include "GameScene.h"
#include "KDL.h"
#include "ImVecHelper.h"
#include "LoadAllFileName.h"
#include "Time.h"

ObjectManager::ObjectManager()
	: pl_handle(-1), start_handle(-1), goal_handle(-1),
	enm_handle(-1), glass_handle(-1), glass_broken_handle(-1), wall_handle(-1), warp_handle(-1),
	door_handle(-1), key_handle(-1),
	select_enm(nullptr), select_waypoint(nullptr), masu_pos(0.f, 0.f, 0.f)
{
	now_move_object = edit_mode_first = false;
}

ObjectManager::~ObjectManager() noexcept
{
	std::vector<int*> erase_handle{
		// SingleObjets
		&pl_handle,
		&start_handle,
		&goal_handle,

		// multiple_objects
		&enm_handle,
		&glass_handle,
		&glass_broken_handle,
		&wall_handle,
		&warp_handle,
		&door_handle,
		&key_handle,
		&waypoint_handle,
		&arrow_handle
	};

	assert(GetObjectKindsNum() == erase_handle.size() && "�폜����n���h����ݒ肵�Y��Ă���");

	// �폜����
	for (auto handle : erase_handle)
	{
		// ���Q�͂Ȃ��̂ł������Ă���
		GMLIB->DeleteModel(*handle);
		GMLIB->DeleteBoardImageHandle(*handle);
	}
}

// �X�V
void ObjectManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	// �e�I�u�W�F�N�g�X�V
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

			auto& plane{ objects.GetChangeObjects<Plane>() };

			std::for_each(plane.begin(), plane.end(), [](auto& data) { data.InversionProc(); });
		}
		else if (!first && player && !WarpHoleCheck())
		{
			first = true;
		}
	}

	edit_mode_first = true;

	// �n�ʂɃv���[���[���W���Z�b�g
	if (const auto& player{ objects.GetObjects<Player>() }; player)
		Plane::SetPlPosition(player->pos);
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
	auto fbx_paths{ GetAllFileName("Data\\Model\\Game") };

	// �t�@�C�������œǂݍ��ނ̂Łi�A���t�@�x�b�g���j
	const std::vector<int*> load_handle{
		&arrow_handle,
		&door_handle,
		&enm_handle,
		&goal_handle,
		&key_handle,
		&pl_handle,
		&start_handle,
		&wall_handle,
		&warp_handle,
		&waypoint_handle,
	};

	fbx_paths.erase(std::remove_if(fbx_paths.begin(), fbx_paths.end(),
		[](const auto& path) { return path.extension().string() != ".fbx"; }), fbx_paths.end());

	assert(fbx_paths.size() == load_handle.size() && "�ǂݍ��ݐ�or�ݒ萔���s��");

	// FBX�ǂݍ���
	for (size_t i = 0, length = fbx_paths.size(); i < length; i++)
	{
		if (!GMLIB->LoadModel(fbx_paths[i], *load_handle[i]))
			assert(!"�ǂݍ��ݎ��s");

		(*load_count)++;
	}

	// �e�N�X�`���ǂݍ���
	{
		if (!GMLIB->LoadBoadImage(L"Data\\Texture\\Glass.png", glass_handle))
			assert(!"�ǂݍ��ݎ��s");

		(*load_count)++;

		if (!GMLIB->LoadBoadImage(L"Data\\Texture\\GlassBroken.png", glass_broken_handle))
			assert(!"�ǂݍ��ݎ��s");

		(*load_count)++;
	}

	// variant�I�u�W�F�N�g�\�z
	{
		// SingleObjets�\�z
		objects.BuildVariant<Player>({ pl_handle });
		objects.BuildVariant<Start>({ start_handle });
		objects.BuildVariant<Goal>({ goal_handle });

		// multiple_objects�\�z
		objects.BuildVariant<Enemy>({ enm_handle, waypoint_handle, arrow_handle });
		objects.BuildVariant<Plane>({ glass_handle, glass_broken_handle });
		objects.BuildVariant<Wall>({ wall_handle });
		objects.BuildVariant<WarpHole>({ warp_handle });
		objects.BuildVariant<Door>({ door_handle });
		objects.BuildVariant<Key>({ key_handle });
	}

	Enemy::SetNodeData(node);
}

// �����o��---------------------------------------------------------------------------------------------------
void ObjectManager::Output(std::ofstream& ofs)
{
	// �p�X�̐ݒ�
#if _DEBUG
	node.PathSetting(objects);
#endif

	// �I�u�W�F�N�g���ۑ�
	ofs << objects;
}

// �ǂݍ���---------------------------------------------------------------------------------------------------
void ObjectManager::Input(std::ifstream& ifs)
{
	// �I�u�W�F�N�g���擾
	ifs >> objects;

	// �p�X�̐ݒ�
#if !_DEBUG
	node.PathSetting(objects);
#endif
}

std::ofstream& operator<<(std::ofstream& ofs, const Object& data)
{
	using namespace std::string_literals;

	// single_objects
	data.SingleObjVisit([&](const auto& dt)
		{
			OutputComment(ofs, typeid(decltype(dt)).name(), "name", "", 2u);

			const bool exist{ dt };

			ofs << F_OUT(exist);

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

			ofs << F_OUT(size);

			for (auto& obj : dt)
			{
				ofs << obj;
			}

			ofs << std::endl;
			ofs << std::endl;
			ofs << std::endl;
		});

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
				if constexpr (std::is_same<Enemy, decltype(dt.front())>())
				{
					auto& back{ data.AddObjects<decltype(dt[i])>() };

					ifs >> back;
				}
				else
				{
					auto& back{ data.AddObjects<decltype(dt[i])>() };

					ifs >> back;
				}
			}
		});

	return ifs;
}

// �p�X���̐ݒ�--------------------------------------------------------------------------------------------
void Node::PathSetting(Object& objects)
{
	auto& planes{ objects.GetChangeObjects<Plane>() };      // ��
	auto& doors{ objects.GetChangeObjects<Door>() };        // �h�A
	auto& warphole{ objects.GetChangeObjects<WarpHole>() }; // ���[�v�z�[��

	// ������
	data.clear();

	auto PositionSetting{ [&](const auto& set_obj) { std::for_each(set_obj.cbegin(), set_obj.cend(),
		[&](const auto& obj) { data.emplace_back(obj.pos); }); } };

	// �m�[�h��ǉ������W���Z�b�g����
	PositionSetting(planes);
	//PositionSetting(doors);
	PositionSetting(warphole);

	// ���̎��͂̏����擾
	for (size_t i = 0, length{ planes.size() }; i < length; i++)
	{
		auto& base_node{ data[i] };

		VF3 left{ base_node.pos }, right{ base_node.pos }, up{ base_node.pos }, down{ base_node.pos };

		right.x -= 1.f;
		left.x += 1.f;
		up.z -= 1.f;
		down.z += 1.f;

		for (size_t j = 0; j < length; j++)
		{
			auto& other_node{ data[j] };

			// �������g�Ȃ�Ƃ΂�
			if (i == j)	continue;

#if false
			// ���i���݂��Ȃ��Ȃ�ǉ��j
			if (!base_node.plane_index[Dir::Left] && left == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Left) = make_optional(j);

				// ���Α����ǉ�
				if (!other_node.plane_index[Dir::Right])
					other_node.plane_index.at(Dir::Right) = make_optional(i);

				continue;
			}

			// �E�i���݂��Ȃ��Ȃ�ǉ��j
			if (!base_node.plane_index[Dir::Right] && right == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Right) = make_optional(j);

				// ���Α����ǉ�
				if (!other_node.plane_index[Dir::Left])
					other_node.plane_index.at(Dir::Left) = make_optional(i);

				continue;
			}

			// ��i���݂��Ȃ��Ȃ�ǉ��j
			if (!base_node.plane_index[Dir::Up] && up == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Up) = make_optional(j);

				// ���Α����ǉ�
				if (!other_node.plane_index[Dir::Down])
					other_node.plane_index.at(Dir::Down) = make_optional(i);

				continue;
}

			// ���i���݂��Ȃ��Ȃ�ǉ��j
			if (!base_node.plane_index[Dir::Down] && down == other_plane.pos)
			{
				base_node.plane_index.at(Dir::Down) = make_optional(j);

				// ���Α����ǉ�
				if (!other_node.plane_index[Dir::Up])
					other_node.plane_index.at(Dir::Up) = make_optional(i);

				continue;
			}
#else
			const std::array<VF3, 4u> poses{ left, right, up, down };

			auto ConnectNodeCheck{ [&](const VF3& pos)
			{ return (other_node.pos == pos && ConnectNode(&base_node, &other_node)); } };

			// �e�����Ƀm�[�h���������A����������ڑ�����
			if (std::any_of(poses.begin(), poses.end(), [&](const VF3& pos)
				{ return ConnectNodeCheck(pos); }))
				continue;
#endif
		}
	}
}

NodeData* Node::PathFindingDijkstra(const VF3& base_pos, const VF3& target_pos, int64_t* processing_time)
{
#if false
	using Dir = NodeData::DirInfo;
	using std::make_pair;

	assert(nodes && !nodes->empty() && "�n�ʂ����݂��Ȃ�");

	// �T���ς݃t���O������
	std::for_each(nodes->begin(), nodes->end(), [](auto& plane) { plane.is_finded = false; });

	size_t index_count_num{ 0u };

	// ������
	for (size_t i = 0u, length = nodes->size(); i < length; i++)
	{
		auto& plane{ nodes->at(i) };

		plane.is_finded = false;
		plane.cache_data.clear();

		// ��{���W�ɑ��݂��鏰���擾����
		if (plane.pos == base_pos) index_count_num = i;
	}

	// �ŏ��ɁA�T�����ƂɂȂ�m�[�h��ǉ��i�������g�j
	//nodes->at(index_count_num).cache_data.emplace_back(nodes->at(index_count_num).pos);

	Deque<Plane*> save_planes;

	while (true)
	{
		// �i��{���W����̋����A�ړI�n�ւ̋����j�A�Y���l
		std::deque<std::pair<std::pair<float, float>, uint32_t>> save_dis;

		bool is_end{ false };

		auto& next_plane{ nodes->at(index_count_num) };

		// ���̏����ړI�̃p�X�̏ꍇ�͔���
		if (next_plane.pos == target_pos)	is_end = true;

		save_planes.emplace_back(&next_plane);

		for (auto s_plane : save_planes)
		{
			// ���̏��̋ߗׂ̏��ŖړI�̏��Ɉ�ԋ߂�����T��
			for (auto& index : s_plane->plane_index)
			{
				using Math::EasyDistance;

				const uint32_t index_num{ *(index.second) };

				// ���̕����ɏ��͑��݂��Ȃ�
				if (!index.second)	continue;

				auto& dir_plane{ nodes->at(index_num) };

				// ���̕����̏��͊��ɒT���ς�
				if (dir_plane.is_finded)	continue;

				// ���̕����̊�{���W����̋����ƃS�[���ւ̋��������߁A�Y���l��ǉ�
				save_dis.emplace_back(
					make_pair(make_pair(EasyDistance(base_pos, dir_plane.pos),
						EasyDistance(target_pos, dir_plane.pos)), *(index.second)));
			}

			// ���̃}�X��T���ς݂Ƃ���
			s_plane->is_finded = true;
		}

		// �i��{���W����̋����A���v�����j�A�Y���l
		decltype(save_dis) near_dis{ save_dis };

		// ���v�������ŏ��l�݂̂ɂ���
		{
			// ���v�����ɕϊ�
			for (auto& n_dis : near_dis)
				n_dis.first.second = n_dis.first.first + n_dis.first.second;

			// ���v�����̏����ɕ��ёւ���
			std::stable_sort(near_dis.begin(), near_dis.end(), [](auto& dis1, auto& dis2)
				{ return (dis1.first.second) < (dis2.first.second); });

			const float nearest_dis{ near_dis.front().first.second };

			// ���v�����̍ŏ��l���傫���ꍇ�͏���
			near_dis.erase(std::remove_if(near_dis.begin(), near_dis.end(), [nearest_dis](const auto& dis)
				{ return dis.first.second > nearest_dis; }), near_dis.end());
		}

		// ��{���W����̋�������������I��
		{
			assert(near_dis.size() != 0u && "�s���ȃT�C�Y");

			// ��{���W����̋�������ԉ�������T��
			const auto get_max_element{ *std::max_element(near_dis.cbegin(), near_dis.cend(),
				[&](auto& dis1, auto& dis2) { return dis1.first < dis2.first; }) };

			const auto& before_plane{ nodes->at(index_count_num) };

			// ���̒T�����̃C���f�b�N�X������
			index_count_num = get_max_element.second;

			auto& after_cache_data{ nodes->at(index_count_num).cache_data };

			// �T�����ƂɂȂ�m�[�h��ǉ�
			after_cache_data = before_plane.cache_data;

			// �T�����ƂɂȂ�m�[�h��ǉ��i�������g�j
			after_cache_data.emplace_back(before_plane.pos);
	}

		if (is_end)		break;
}
#else
#define DATA_SET	data.begin(), data.end()

	Timer time;

	if(processing_time)
		time.Start();

	std::for_each(DATA_SET, [](auto& dt) { dt.Reset(); });

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == base_pos); }) && "��{���W���m�[�h��ɑ��݂��Ȃ�");

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == target_pos); }) && "�ړI���W���m�[�h��ɑ��݂��Ȃ�");

	std::deque<NodeData*> work1, work2;

	auto* currLevel{ &work1 };
	auto* nextLevel{ &work2 };

	auto goal{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == target_pos); }) };
	auto start{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == base_pos); }) };

	//�S�[������v�Z����̂ŁA�R�X�g0���Z�b�g���Čv�Z�ς݂Ƃ���
	goal->cost = 0;

	//�������K�w�̃m�[�h�Z�b�g(�S�[���m�[�h���Z�b�g����)
	currLevel->emplace_back(&(*goal));

	std::list<Node*>::iterator it;
	int nodeCost;

	while (currLevel->size())
	{
		for (auto* it : *currLevel)
		{
			for (auto& it_cnct : it->coonect_node)
			{
				using Math::EasyDistance;

				nodeCost = it->cost + it_cnct.cost;

				//���T���m�[�h���邢�͍ŒZ���[�g���X�V�ł���ꍇ
				if (auto* node = it_cnct.next_node; (!node->is_finded || nodeCost < node->cost))
				{
					//�o�H�R�X�g�ƃS�[���֌��������߂̃m�[�h���Z�b�g
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//���Ɍ�������K�w�̃��X�g�ɓo�^
				nextLevel->emplace_back(it_cnct.next_node);
			}
		}

		//���X�g�����ւ��Ď��̊K�w����������
		std::swap(nextLevel, currLevel);

		nextLevel->clear();//�N���A����
	}

	if (processing_time)
	{
		time.End();
		*processing_time = time.GetMicroTimer();
	}

	return &(*start);

#endif
}

NodeData& Node::PathFindingAstar(const VF3& base_pos, const VF3& target_pos, int64_t* processing_time)
{
#if false
	using Dir = NodeData::DirInfo;
	using std::make_pair;

	assert(nodes && !nodes->empty() && "�n�ʂ����݂��Ȃ�");

	// �T���ς݃t���O������
	std::for_each(nodes->begin(), nodes->end(), [](auto& plane) { plane.is_finded = false; });

	size_t index_count_num{ 0u };

	// ������
	for (size_t i = 0u, length = nodes->size(); i < length; i++)
	{
		auto& plane{ nodes->at(i) };

		plane.is_finded = false;
		plane.cache_data.clear();

		// ��{���W�ɑ��݂��鏰���擾����
		if (plane.pos == base_pos) index_count_num = i;
	}

	// �ŏ��ɁA�T�����ƂɂȂ�m�[�h��ǉ��i�������g�j
	//nodes->at(index_count_num).cache_data.emplace_back(nodes->at(index_count_num).pos);

	Deque<Plane*> save_planes;

	while (true)
	{
		// �i��{���W����̋����A�ړI�n�ւ̋����j�A�Y���l
		std::deque<std::pair<std::pair<float, float>, uint32_t>> save_dis;

		bool is_end{ false };

		auto& next_plane{ nodes->at(index_count_num) };

		// ���̏����ړI�̃p�X�̏ꍇ�͔���
		if (next_plane.pos == target_pos)	is_end = true;

		save_planes.emplace_back(&next_plane);

		for (auto s_plane : save_planes)
		{
			// ���̏��̋ߗׂ̏��ŖړI�̏��Ɉ�ԋ߂�����T��
			for (auto& index : s_plane->plane_index)
			{
				using Math::EasyDistance;

				const uint32_t index_num{ *(index.second) };

				// ���̕����ɏ��͑��݂��Ȃ�
				if (!index.second)	continue;

				auto& dir_plane{ nodes->at(index_num) };

				// ���̕����̏��͊��ɒT���ς�
				if (dir_plane.is_finded)	continue;

				// ���̕����̊�{���W����̋����ƃS�[���ւ̋��������߁A�Y���l��ǉ�
				save_dis.emplace_back(
					make_pair(make_pair(EasyDistance(base_pos, dir_plane.pos),
						EasyDistance(target_pos, dir_plane.pos)), *(index.second)));
			}

			// ���̃}�X��T���ς݂Ƃ���
			s_plane->is_finded = true;
		}

		// �i��{���W����̋����A���v�����j�A�Y���l
		decltype(save_dis) near_dis{ save_dis };

		// ���v�������ŏ��l�݂̂ɂ���
		{
			// ���v�����ɕϊ�
			for (auto& n_dis : near_dis)
				n_dis.first.second = n_dis.first.first + n_dis.first.second;

			// ���v�����̏����ɕ��ёւ���
			std::stable_sort(near_dis.begin(), near_dis.end(), [](auto& dis1, auto& dis2)
				{ return (dis1.first.second) < (dis2.first.second); });

			const float nearest_dis{ near_dis.front().first.second };

			// ���v�����̍ŏ��l���傫���ꍇ�͏���
			near_dis.erase(std::remove_if(near_dis.begin(), near_dis.end(), [nearest_dis](const auto& dis)
				{ return dis.first.second > nearest_dis; }), near_dis.end());
		}

		// ��{���W����̋�������������I��
		{
			assert(near_dis.size() != 0u && "�s���ȃT�C�Y");

			// ��{���W����̋�������ԉ�������T��
			const auto get_max_element{ *std::max_element(near_dis.cbegin(), near_dis.cend(),
				[&](auto& dis1, auto& dis2) { return dis1.first < dis2.first; }) };

			const auto& before_plane{ nodes->at(index_count_num) };

			// ���̒T�����̃C���f�b�N�X������
			index_count_num = get_max_element.second;

			auto& after_cache_data{ nodes->at(index_count_num).cache_data };

			// �T�����ƂɂȂ�m�[�h��ǉ�
			after_cache_data = before_plane.cache_data;

			// �T�����ƂɂȂ�m�[�h��ǉ��i�������g�j
			after_cache_data.emplace_back(before_plane.pos);
		}

		if (is_end)		break;
	}
#else
#define DATA_SET	data.begin(), data.end()

	Timer time;

	if (processing_time)
		time.Start();

	std::for_each(DATA_SET, [](auto& dt) { dt.Reset(); });

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == base_pos); }) && "��{���W���m�[�h��ɑ��݂��Ȃ�");

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == target_pos); }) && "�ړI���W���m�[�h��ɑ��݂��Ȃ�");

	std::deque<NodeData*> work1, work2;

	auto* currLevel{ &work1 };
	auto* nextLevel{ &work2 };

	auto goal{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == target_pos); }) };
	auto start{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == base_pos); }) };

	//�S�[������v�Z����̂ŁA�R�X�g0���Z�b�g���Čv�Z�ς݂Ƃ���
	goal->cost = 0;

	//�������K�w�̃m�[�h�Z�b�g(�S�[���m�[�h���Z�b�g����)
	currLevel->emplace_back(&(*goal));

	std::list<Node*>::iterator it;
	int nodeCost;

	while (currLevel->size())
	{
		for (auto* it : *currLevel)
		{
#if false
			for (auto& connect : it->coonect_node)
			{
				nodeCost = it->cost + connect.cost;

				//���T���m�[�h���邢�͍ŒZ���[�g���X�V�ł���ꍇ
				if (auto* node = connect.next_node; (!node->is_finded || nodeCost < node->cost))
				{
					//�o�H�R�X�g�ƃS�[���֌��������߂̃m�[�h���Z�b�g
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//���Ɍ�������K�w�̃��X�g�ɓo�^
				nextLevel->emplace_back(connect.next_node);
			}
#else
			// �m�[�h�f�[�^�A(�X�^�[�g����̋����A�S�[���ւ̋���)
			std::deque<std::pair<NodeData*, std::pair<float, float>>> connect_nodes;

			// ���ёւ��p�ɍ\�z
			for (NodeData::NodeConnect& node : it->coonect_node)
			{
				constexpr float Fmax{ (std::numeric_limits<float>::max)() };

				connect_nodes.emplace_back(node.next_node, std::make_pair(Fmax, Fmax));
			}

			// ���v�������ŏ��l�݂̂ɂ���
			{
				// ���v�����ɕϊ�
				for (auto& c_node : connect_nodes)
				{
					using Math::EasyDistance;

					const auto next_node{ c_node.first };
					const float start_dis{ EasyDistance(next_node->pos, start->pos) };
					const float goal_dis{ EasyDistance(next_node->pos, goal->pos) };

					c_node.second = std::make_pair(start_dis, goal_dis);
				}

				// ���v�����̏����ɕ��ёւ���
				std::stable_sort(connect_nodes.begin(), connect_nodes.end(), [](auto& dis1, auto& dis2)
					{
						float total_dis1{ dis1.second.first + dis1.second.second };
						float total_dis2{ dis2.second.first + dis2.second.second };

						return (total_dis1) < (total_dis2);
					});

				const float nearest_dis
				{ connect_nodes.front().second.first + connect_nodes.front().second.second };

				// ���v�����̍ŏ��l���傫���ꍇ�͏���
				connect_nodes.erase(std::remove_if(
					connect_nodes.begin(), connect_nodes.end(), [nearest_dis](const auto& dis)
					{ return dis.second.first + dis.second.second > nearest_dis; }), connect_nodes.end());
			}

			// ��{���W����̋�������������I��
			{
				assert(connect_nodes.size() != 0u && "�s���ȃT�C�Y");

				// ��{���W����̋�������ԉ�������T��
				const auto get_max_element{ *std::max_element(connect_nodes.cbegin(), connect_nodes.cend(),
					[&](auto& dis1, auto& dis2) { return dis1.second.first < dis2.second.first; }) };

				nodeCost = get_max_element.first->cost + get_max_element.first->cost;

				//���T���m�[�h���邢�͍ŒZ���[�g���X�V�ł���ꍇ
				if (auto* node = get_max_element.first; (!node->is_finded || nodeCost < node->cost))
				{
					//�o�H�R�X�g�ƃS�[���֌��������߂̃m�[�h���Z�b�g
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//���Ɍ�������K�w�̃��X�g�ɓo�^
				nextLevel->emplace_back(get_max_element.first);
			}
#endif
		}

		//���X�g�����ւ��Ď��̊K�w����������
		std::swap(nextLevel, currLevel);

		nextLevel->clear();//�N���A����
	}

	if (processing_time)
	{
		time.End();
		*processing_time = time.GetMicroTimer();
	}

	return *start;

#endif
}
