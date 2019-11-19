#include "GameScene.h"
//#include "scene.h"
#include "Math.h"
#include "KDL.h"
#include "ImVecHelper.h"
#include "SceneSelect.h"
#include "LoadAllFileName.h"
#include <string>

//todo KDL_USE_IMGUI USE_IMGUI�̑���

void SceneGame::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	namespace fs = std::filesystem;

	// ������
	object_manager.emplace();
	file_flg = false;
	camera_angle = { Math::ToRadian(-80.f), 0.f, 0.f };
	camera_dis = 50.f;
	FadeTimeInit();

	execution_quick_exit = false;
	masu_pos = Fill3(0.f);
	edit_mode = false;
	enm_edit_mode = false;
	is_save = false;
	back_world_mode = false;
	init_scene = false;

	// �\�z
	{
		// variant�I�u�W�F�N�g�\�z
		{
			// SingleObjets�\�z
			object_manager->GetObjectData().BuildVariant<Player>();
			object_manager->GetObjectData().BuildVariant<Start>();
			object_manager->GetObjectData().BuildVariant<Goal>();

			// multiple_objects�\�z
			object_manager->GetObjectData().BuildVariant<Enemy>();
			object_manager->GetObjectData().BuildVariant<Plane>();
			object_manager->GetObjectData().BuildVariant<Wall>();
			object_manager->GetObjectData().BuildVariant<WarpHole>();
			object_manager->GetObjectData().BuildVariant<Door>();
			object_manager->GetObjectData().BuildVariant<Key>();
		}
	}

	// �ꎞ�t�@�C�������݂��Ă���Ȃ�A�폜
	//if (fs::exists(TempFileDir) && !std::filesystem::remove(TempFileDir))
	//	assert(!"�ꎞ�t�@�C���̍폜�Ɏ��s");

	camera =
		std::make_unique<KDL::TOOL::Camera>(
			KDL::TOOL::Camera(
				{ 0.f, 0.f, -30.f },		//eye
				{ 0.f, 0.f, 0.f },			//focus
				{ 0.f, 1.f, 0.f },			//up
				DirectX::XMConvertToRadians(30.f),
				p_app->GetViewport().Width / p_app->GetViewport().Height,
				{ 0.1f, 1000.f }
			)
			);

	camera->SetPosition(CreateRotationPos(camera_angle, 30.f));

	Enemy::player = &object_manager->GetObjectData<Player>();
	Enemy::walls = &object_manager->GetObjectData<Wall>();

	Input(open_file_path);

	// �X�^�[�g�n�_�ɃZ�b�g����
	if (auto& player{ object_manager->GetObjectData().GetChangeObjects<Player>() }; player)
	{
		player->pos = object_manager->GetObjectData<Start>()->pos;
	}

	auto* audio = p_window->GetAudio();
	audio->Stop(bgm_handle, bgm_handle_p, 1.0f);
	bgm_handle_p = audio->CreatePlayHandle(bgm_handle, 0.f, true, false, 0.f, 0.f, 0, false, false);
	audio->Play(bgm_handle, bgm_handle_p, 0.01f, 0.2f, false);
}

void SceneGame::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	namespace fs = std::filesystem;

	object_manager->UnInitialize();

	object_manager = std::nullopt;
	camera = nullptr;

	// �ꎞ�t�@�C�������݂��Ă���Ȃ�A�폜
	//if (fs::exists(TempFileDir) && !std::filesystem::remove(TempFileDir))
	//	assert(!"�ꎞ�t�@�C���̍폜�Ɏ��s");

	auto* audio = p_window->GetAudio();
	audio->Delete(bgm_handle);
	audio->Delete(se_break);
	audio->Delete(se_crack);
	audio->Delete(se_door);
	audio->Delete(se_waap);
	audio->Delete(se_whistle);
	audio = nullptr;
}

void SceneGame::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;

	// ���[�h�ύX����
#if false
	{
		static bool save_f1{ false };

		// ���[�h�ύX
		ModeChange(&save_f1, p_scene_mgr, p_window, p_app);

		// �m�F��ʒ��͍X�V���Ȃ�
		if (save_f1)	return;
	}
#endif

#ifdef KDL_USE_IMGUI
#if false
	// ���C�����j���[
	if (edit_mode)
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu(u8"�t�@�C��"))
		{
			if (ImGui::MenuItem(u8"�ۑ�", "CTRL+S"))				file_flg.is_save = true;
			//if (ImGui::MenuItem(u8"�ʖ��ۑ�", "CTRL+SHIFT+S"))			file_flg.is_save_as = true;
			if (ImGui::MenuItem(u8"�J��", "CTRL+O"))				file_flg.is_open = true;
			//if (ImGui::MenuItem(u8"�폜"	, "CTRL+SHIFT+ALT+DELETE"))	file_flg.is_delete  = true;
			//if (ImGui::MenuItem(u8"����"	, "CTRL+ALT+C"))			file_flg.is_close   = true;
			//if (ImGui::MenuItem(u8"���O�ύX", "CTRL+R"))				file_flg.is_rename  = true;
			//if (ImGui::MenuItem(u8"�R�s�["	, "CTRL+SHIFT+C"))			file_flg.is_copy    = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"�ҏW"))
		{
			//ImGui::MenuItem(u8"�R�s�[", "CTRL+C", nullptr, false);
			//ImGui::MenuItem(u8"�J�b�g", "CTRL+X", nullptr, false);
			//ImGui::MenuItem(u8"�y�[�X�g", "CTRL+V", nullptr, false);
			//ImGui::MenuItem("ReDo", "CTRL+Z", nullptr, false);
			//ImGui::MenuItem("UnDo", "CTRL+Y", nullptr, false);

			//ImGui::Separator();

			ImGui::MenuItem(u8"�ݒu", u8"���N���b�N", nullptr, false);
			ImGui::MenuItem(u8"�ړ�", u8"�z�C�[���N���b�N(hold)", nullptr, false);
			ImGui::MenuItem(u8"�폜", u8"�E�N���b�N", nullptr, false);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"�J����"))
		{
			ImGui::MenuItem(u8"�E", "D", nullptr, false);
			ImGui::MenuItem(u8"��", "S", nullptr, false);
			ImGui::MenuItem(u8"��", "W", nullptr, false);
			ImGui::MenuItem(u8"��", "S", nullptr, false);
			ImGui::MenuItem(u8"�g��", "PGUP", nullptr, false);
			ImGui::MenuItem(u8"�k��", "PGDN", nullptr, false);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImguiTool::BeginShowTempWindow({ 0.f, 50.f }, LabelName.data());

	// ���E���\��
	if (!enm_edit_mode)  /// �G�ҏW���[�h�ȊO�\��
	{
		const auto vp{ p_app->GetViewport() };
		const VF2 s_size{ vp.Width, vp.Height };

		ImguiTool::BeginShowTempWindow({ s_size.x - 120.f, 50.f }, u8"���E��");

		ImGui::TextColored({ IMGUI_RED }, back_world_mode ? u8"�����E" : u8"�\���E");

		// �ҏW���[�h���������R�ɕύX�ł���悤��
		if (edit_mode && ImGui::Button(u8"�؂�ւ�"))	back_world_mode ^= true;

		ImGui::End();
	}

	// ���[�h�\��
	{
		static const ImVec4 Color{ IMGUI_RED };

		// �ʒu����
		{
			constexpr VF2 w_size{ 150.f, 60.f };
			const auto vp{ p_app->GetViewport() };
			const VF2 s_size{ vp.Width, vp.Height };

			ImguiTool::BeginShowTempWindow(s_size - w_size, u8"���[�h�\��");
		}

		// TextColored�\���i�F�Œ�j
		auto Text{ [&](const char* text, auto... contents)
		{ ImGui::TextColored(Color, text, contents...); } };

		// �e���[�h��\��
		if (!edit_mode) Text(u8"���s���[�h");

		else if (edit_mode && !enm_edit_mode) Text(u8"�ҏW���[�h");

		else Text(u8"�G�ҏW���[�h");

		ImGui::End();
	}
#endif
#endif

	audio = p_window->GetAudio();
	object_manager->Update(p_window, p_app);

#if false

	if (edit_mode) file_flg.Update(p_scene_mgr, p_window, p_app);

	FileOperateUpdate();

	edit_mode ? EditModeUpdate(p_scene_mgr, p_window, p_app) : NormalModeUpdate(p_scene_mgr, p_window, p_app);
#else

	NormalModeUpdate(p_scene_mgr, p_window, p_app);

	const auto* input{ p_window->GetInput() };

	if (input->IsTrgKey(Keys::Back))
	{
		fadeout_timer += 2.f;
	}

	if (fadeout_timer >= GoalFadeTime)
	{
		SetNextScene<SceneSelect>();	//�ʃX���b�h�ŃV�[���؂�ւ�
	}

#endif
#ifdef KDL_USE_IMGUI
#if false
	ImGui::End();
#endif
#endif
}

// �`��
void SceneGame::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const KDL::COLOR4F clear_color = { AQUA, 1.f };
	p_app->ClearBackBuffer(clear_color);

	// �w�i
	{
		constexpr VF3 Scale{ 107.f, 1.f, 100.f };
		constexpr VF4 Color{ WHITE, 1.f };
		constexpr VF2 TexPos{ 0.f, 0.f };
		constexpr VF2 TexScale{ 1.f, 1.f };

		const VF3 pos{ camera->GetTarget() };

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
			R = DirectX::XMMatrixRotationRollPitchYaw(0.f, 3.1415f, 0.f);
			T = DirectX::XMMatrixTranslation(pos.x, -50.f, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		camera->CreateUpdateWorldViewProjection(&wvp, W);

		if (back_world_mode)
			sand_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale);
		else
			snow_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale);
	}

	object_manager->Draw(p_window, p_app);

	// �`���[�g���A�����b�Z�[�W�{�b�N�X
	if (SceneSelect::GetIsTutrialMode())  // �`���[�g���A���X�e�[�W�Ȃ�
	{
		using Math::ToRadian;

		constexpr VF4 Color{ WHITE, 1.f };
		constexpr VF2 TexPos{ 0.f, 0.f };
		constexpr VF2 TexScale{ 1.f, 1.f };
		constexpr VF3 Scale{ 5.f, 5.f, 5.f };
		constexpr VF3 Angle{ ToRadian(-40.f), ToRadian(180.f), 0.f };
		constexpr std::array<VF3, 4> pos{ {
			{ 5.5f, 3.f, -3.f},
			{ 0.f, 3.f, -3.f },
			{ -10.5f, 3.f, -3.f },
			{ -8.f, 3.f, 2.f } } };

		auto& pl_pos{ object_manager->GetObjectData<Player>()->pos };

		if (pl_pos.x >= 1.f)
		{
			for (size_t i = 0, length = tutorial_board.size() - 2u; i < length; i++)
			{
				DirectX::XMMATRIX W;
				{
					DirectX::XMMATRIX S, R, T;

					S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
					R = DirectX::XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);
					T = DirectX::XMMatrixTranslation(pos[i].x, pos[i].y, pos[i].z);

					W = S * R * T;
				}

				DirectX::XMFLOAT4X4 wvp, w;
				DirectX::XMStoreFloat4x4(&w, W);
				camera->CreateUpdateWorldViewProjection(&wvp, W);

				tutorial_board[i]->AddCommand(
					p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale, 0, false, false);
			}
		}
		else
		{
			for (size_t i = 2, length = tutorial_board.size(); i < length; i++)
			{
				DirectX::XMMATRIX W;
				{
					DirectX::XMMATRIX S, R, T;

					S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
					R = DirectX::XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);
					T = DirectX::XMMatrixTranslation(pos[i].x, pos[i].y, pos[i].z);

					W = S * R * T;
				}

				DirectX::XMFLOAT4X4 wvp, w;
				DirectX::XMStoreFloat4x4(&w, W);
				camera->CreateUpdateWorldViewProjection(&wvp, W);

				tutorial_board[i]->AddCommand(
					p_app->GetCommandList(), p_app, wvp, w, LightDir, Color, TexPos, TexScale, 0, false, false);
			}
		}
	}

	// �G���^�[�L�[�Ō���A�o�b�N�X�y�[�X�L�[�Ń^�C�g����
	{
		constexpr DirectX::XMFLOAT4 Color{ BLACK, 1.f };
		constexpr int BM{ static_cast<int>(KDL::DX12::BLEND_STATE::ALPHA) };
		constexpr float BaseY{ 25.f };

		static const VF2 Size{ 25.f, 50.f };

		static auto vp{ p_app->GetViewport() };
		static VF2 s_size{ vp.Width, vp.Height };

		font_sprite->AddTextCommands(p_app->GetCommandList(), p_app, "Retry : Enter",
			Fill2(BaseY), Size, Fill2(0.f), Fill2(0.f), Fill2(1.f), 0.f,
			Color, Color, Color, Color, BM);

		font_sprite->AddTextCommands(p_app->GetCommandList(), p_app, "Select : Back Space",
			{ s_size.x - 500.f, BaseY }, Size, Fill2(0.f), Fill2(0.f), Fill2(1.f), 0.f,
			Color, Color, Color, Color, BM);
	}

	// �t�F�[�h�A�E�g
	if (fadeout_timer > 0.0)
	{
		const double timer{ Easing::InCubic<double>(fadeout_timer, GoalFadeTime) };

		FadeOutDraw(p_app, &timer);

		fadeout_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	// �t�F�[�h�C��
	if (fadein_timer < BaseFadeTimeMax)
	{
		const double timer{ Easing::OutCubic<double>(fadein_timer, BaseFadeTimeMax) };

		FadeInDraw(p_app, &timer);

		fadein_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	// �I���O���b�g
#if false
	if (edit_mode)
	{
		constexpr float Scale{ 1.f };
		constexpr VF4 Color{ RED, 1.f };

		VF3 pos{ masu_pos };
		pos.y = 0.6f;

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(Scale, Scale, Scale);
			R = DirectX::XMMatrixRotationRollPitchYaw(3.14f / 2.f, 0.f, 0.f);
			T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		camera->CreateUpdateWorldViewProjection(&wvp, W);

		grit_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, { RED, 1.f }, { 0.f, 0.f }, { 0.f, 0.f });
	}

	// �O���b�g��
	{
		constexpr VF3 Pos{ 0.5f, 0.25f, 0.5f };
		constexpr VF2 Scale{ 1000.f, 1000.f };
		constexpr VF3 Angle{ 3.14f / 2.f, 0.f, 0.f };
		constexpr VF4 Color{ GRAY, 1.f };
		constexpr VF2 TexPos{ 0.f, 0.f };

		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(Scale.x, Scale.y, 1.f);
			R = DirectX::XMMatrixRotationRollPitchYaw(3.14f / 2.f, 0.f, 0.f);
			T = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
			W = S * R * T;
		}
		DirectX::XMFLOAT4X4 wvp, w;
		DirectX::XMStoreFloat4x4(&w, W);
		camera->CreateUpdateWorldViewProjection(&wvp, W);

		grit_board->AddCommand(p_app->GetCommandList(), p_app, wvp, w, LightDir, { WHITE, 1.f }, { 0.f, 0.f }, Scale);
	}
#endif
}

// �ʏ탂�[�h�X�V
void SceneGame::NormalModeUpdate(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;
	using SM = SceneManager;

#ifdef KDL_USE_IMGUI
#if false
	ImGui::SliderAngle(u8"�J�����ʒuX", &camera_angle.x, -89.f, -1.f);
	ImGui::SliderAngle(u8"�J�����ʒuY", &camera_angle.y, 0.f, 360.f);
	ImGui::SliderFloat(u8"�J��������", &camera_dis, 10.f, 150.f, "%.0f");

	ImGui::Separator();

	ImGui::Text(u8"Enter �� ���X�^�[�g���႟�`");
	ImGui::Text(u8"F1 �� �ҏW���[�h���႟�`");
#endif
#endif
	// ���g���C�@�\
	if (const auto* input{ p_window->GetInput() };
		input->IsTrgKey(Keys::Enter) || init_scene)
	{
		Initialize(p_scene_mgr, p_window, p_app);

		return;
	}

	// �I����ʂ�
	if (execution_quick_exit && Math::AdjEqual(fadeout_timer, 0.0))
	{
		fadeout_timer += static_cast<double>(p_window->GetElapsedTime());
	}

	if (fadeout_timer >= GoalFadeTime)
	{
		SetNextScene<SceneSelect>();	//�ʃX���b�h�ŃV�[���؂�ւ�
	}

	// �J�����̍X�V
#if false
	if (const auto& player{ object_manager->GetObjectData<Player>() }; player)
	{
		camera->SetPosition(CreateRotationPos(camera_angle, camera_dis, player->pos));
		camera->SetTarget(player->pos);
	}
	// �v���C���[�����݂��Ȃ��Ȃ猴�_���^�[�Q�b�g�ɂ���
	else
	{
		constexpr VF3 BasePos{ 0.f, 0.f, 0.f };

		camera->SetPosition(CreateRotationPos(camera_angle, camera_dis, player->pos));
		camera->SetTarget(BasePos);
	}
#else
	if (const auto& player{ object_manager->GetObjectData<Player>() }; player)
	{
		static double adj_dis_timer{ 0.f };

		if (object_manager->GetIsGoal())
		{
			adj_dis_timer += p_window->GetElapsedTime();
		}
		else
		{
			adj_dis_timer = 0.f;
		}

		camera->SetPosition(CreateRotationPos(camera_angle,
			camera_dis - (Easing::OutCirc(
								adj_dis_timer,
								GoalFadeTime,
								static_cast<double>(camera_dis - 1.5))), player->pos));

		camera->SetTarget(player->pos);
	}
#endif
}

// �ҏW���[�h�X�V
void SceneGame::EditModeUpdate(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;

	auto camera_pos{ camera->GetPosition() };
	auto camera_target{ camera->GetTarget() };
	const auto* input = p_window->GetInput();

	// �}�E�X�֌W
	{
		// �}�E�X���W�擾
		mouse_pos = { static_cast<float>(input->GetMousePos().x), static_cast<float>(input->GetMousePos().y) };

		// ���[�J�����W�ɕϊ�
		masu_pos = MouseToLocalPos(mouse_pos, p_app);

		// �}�X���W�ɕϊ�
		masu_pos = Round(masu_pos);
	}

	static float camera_dis{ 50.f };
	static float cam_speed{ 10.f };

	auto KeyInput{ [&](Keys key1, Keys key2)
	{ return (input->IsHitKey(key1) || input->IsHitKey(key2)); } };

	// �J����
	if (!(input->IsHitKey({ Keys::LeftControl }) && input->IsHitKey({ Keys::RightControl })))
	{
#ifdef KDL_USE_IMGUI
		if (ImGui::GetIO().WantCaptureKeyboard)	return;
#endif

		constexpr VF3 CamAngle{ -3.14f * 0.5f, 0.f, 0.f };
		const float Movement{ cam_speed * p_window->GetElapsedTime() };

		if (KeyInput(Keys::Left, Keys::A))
			camera_target.x += Movement;

		else if (KeyInput(Keys::Right, Keys::D))
			camera_target.x -= Movement;

		if (KeyInput(Keys::Up, Keys::W))
			camera_target.z -= Movement;

		else if (KeyInput(Keys::Down, Keys::S))
			camera_target.z += Movement;

		if (input->IsTrgKey(Keys::PageUp))
			camera_dis -= Movement;

		if (input->IsTrgKey(Keys::PageDown))
			camera_dis += Movement;

		camera_dis = std::clamp(camera_dis, 10.f, 150.f);

		camera_pos = camera_target;

		camera_pos.y += camera_dis;

		camera->SetPosition(CreateRotationPos(CamAngle, camera_dis, camera_target));
	}

#ifdef KDL_USE_IMGUI
	ImGui::Text(u8"�J�������W�F%.01f, %.01f", camera_pos.x, camera_pos.z);
	ImGui::SliderFloat(u8"�J��������", &camera_dis, 10.f, 150.f, "%.0f");
	ImGui::SliderFloat(u8"�J�������x", &cam_speed, 5.f, 50.f, "%.0f");

	ImGui::Separator();

	ImGui::Text(u8"�� %u, �� %u, �G %u, ���[�v�z�[�� %u,\n�h�A %u, �J�M %u",
		object_manager->GetObjectData<Plane>().size(),
		object_manager->GetObjectData<Wall>().size(),
		object_manager->GetObjectData<Enemy>().size(),
		object_manager->GetObjectData<WarpHole>().size(),
		object_manager->GetObjectData<Door>().size(),
		object_manager->GetObjectData<Key>().size());

	ImGui::Separator();

	ImGui::Text(u8"�}�E�X���W : %.01f, %.01f", mouse_pos.x, mouse_pos.y);
	ImGui::Text(u8"�}�X���W : %.01f, %.01f", masu_pos.x, masu_pos.z);

	ImGui::Separator();

	if (!enm_edit_mode) ImGui::Text(u8"F1 �� ���s���[�h���႟�`");
	ImGui::Text(u8"F2 �� %s�ҏW���[�h���႟�`", (enm_edit_mode ? u8" " : u8"�G"));

	ImGui::Separator();
	ImGui::Text(u8"�J����Ă���t�@�C���F%s", open_file_path.stem().u8string().data());
	ImGui::Text(u8"�ۑ�����Ă�%s", is_save ? u8"��" : u8"�Ȃ�");
#endif
#endif
}

// ���[�h�ύX����
void SceneGame::ModeChange(bool* save_f1, SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;

	const auto* input = p_window->GetInput();

	// �ҏW���[�h�������s���[�h
	if (!enm_edit_mode)
	{
		const bool confirm_f1{ input->IsTrgKey(Keys::F1) };

		if (confirm_f1)	*save_f1 = true;

		// �m�F���
		if (*save_f1)
		{
			// ���s���[�h�ֈڍs��
			if (edit_mode)
			{
				auto& start{ object_manager->GetObjectData<Start>() };
				auto& player{ object_manager->GetObjectData<Player>() };

				// �v���C���[�ƃX�^�[�g�n�_�����݂���Ƃ������m�F��ʂ��o��
				if (player && start)
				{
#ifdef KDL_USE_IMGUI && false
					// �ʒu����
					{
						static VF2 size{ 100.f, 100.f };
						const auto vp{ p_app->GetViewport() };
						const VF2 s_size{ vp.Width, vp.Height };

						const auto& s_div_size{ (s_size / 2.f) };
						const auto pos{ s_div_size - size };

						ImguiTool::BeginShowTempWindow(pos, u8"�m�F���");

						auto temp{ ToXMFLOAT(ImGui::GetWindowSize()) };

						size = temp / 2.f;
					}

					auto [normal, debug] { ImguiTool::YesNoButton(u8"�ʏ탂�[�h", u8"�f�o�b�N���[�h") };
					ImguiTool::ShowHelp(u8"�ʏ탂�[�h�̓v���C���[���X�^�[�g�n�_����J�n���܂��B\n"
						u8"�f�o�b�N���[�h�̓v���C���[��u�����n�_����J�n���܂��B");

					// �I��������
					if (normal || debug)
					{
						*save_f1 = false;
						edit_mode ^= true;
						back_world_mode = false;

						if (!Output(TempFileDir, true))
							assert(!"�ꎞ�t�@�C���̍쐬�Ɏ��s");

						// �ʏ탂�[�h���̂݃v���C���[���X�^�[�g�n�_��
						if (normal)
							player->pos = start->pos;
					}

					ImGui::End();
#endif
				}
				// �v���C���[�ƃX�^�[�g�n�_�̕Е�Or���������݂��Ȃ�
				else
				{
					edit_mode ^= true;
					back_world_mode = false;
					*save_f1 = false;

					if (!Output(TempFileDir, true))
						assert(!"�ꎞ�t�@�C���̍쐬�Ɏ��s");
				}
			}
			// �ҏW��ʂֈڍs��
			else
			{
				namespace fs = std::filesystem;

				*save_f1 = false;
				edit_mode ^= true;
				back_world_mode = false;

				// �ꎞ�t�@�C�������݂��Ă���
				if (fs::exists(TempFileDir))
				{
					// �ǂݍ��ޑO�ɃI�u�W�F�N�g��S�폜
					object_manager->GetObjectData().AllObjectClear();

					// �ǂݍ����
					if (!Input(TempFileDir, true))
						assert(!"�ꎞ�t�@�C���̓ǂݍ��݂Ɏ��s");

					// �ꎞ�t�@�C�����폜
					if (!std::filesystem::remove(TempFileDir))
						assert(!"�ꎞ�t�@�C���̍폜�Ɏ��s");

					auto& planes{ object_manager->GetObjectData().GetChangeObjects<Plane>() };

					std::for_each(planes.begin(), planes.end(), [](auto& obj) { obj.InitHP(); });
				}
			}
		}
	}

	// �G�ҏW���[�h�����ҏW���[�hor���s���[�h
	if (edit_mode && input->IsTrgKey(Keys::F2))
	{
		object_manager->InitSelectObject();
		enm_edit_mode ^= true;
	}
#endif
}

// �}�E�X���W���烍�[�J�����W�ւ̕ϊ�
DirectX::XMFLOAT3 SceneGame::MouseToLocalPos(const VF2& mouse_pos, KDL::DX12::App* p_app)
{
	constexpr std::array<VF3, 3u> Points  // �v���[���̂R�_
	{ { { -1000.f, 0.f, 1000.f }, { 1000.f, 0.f, 1000.f }, { 1000.f, 0.f, -1000.f } } };

	const auto vp{ p_app->GetViewport() };
	const VF2 s_size{ vp.Width, vp.Height };

	// �^��ϊ����ĕԂ�
	return TransformMouseToPlanePos(s_size, mouse_pos, Points[0], Points[1], Points[2]);
}

// �ǂݍ���
void SceneGame::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// (�L�E�ցE`)�m��񂪂�
	// �}���`�X���b�h��Com�I�u�W�F�N�g������ł���Ɓi���m�ɂ͕ʃX���b�h����Com�I�u�W�F�N�g�𐶐����鎞�H�j�A
	// �������Ȃ��Ƃ����Ȃ��炵��
	// �X���b�h���ɂ��̊֐����Ăяo���K�v������
	// https://teratail.com/questions/165187
	// https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex
	// https://social.msdn.microsoft.com/Forums/ja-JP/4ea73ec0-e90d-4759-8517-260f03621362/createinstance123910x800401f0123981245612521125401236430330299831237712?forum=vcgeneralja
	//if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) assert(!"COM���C�u�����́A���̃X���b�h�Ŋ��ɏ���������Ă���");

	// �|���摜
	{
		auto Load{ [&](auto& board, const Path& path)
		{ board = std::make_unique<KDL::DX12::Geometric_Board_S>(p_app, path, 100); } };

		std::vector<std::pair<decltype(snow_board)*, std::string>> load_board{
			{ &snow_board, "data\\images\\Snow.png" },
			{ &sand_board, "data\\images\\Sand.png" },
		};

		for (auto& board : load_board)
		{
			Load(*board.first, board.second);

			load_count++;
		}

		const auto tutorial_name{ GetAllFileName("data\\images\\Tutorial") };

		for (auto& name : tutorial_name)
		{
			tutorial_board.emplace_back(
				std::make_unique<KDL::DX12::Geometric_Board_S>(p_app, name, 100u));
		}
	}

	// ���y
	{
		auto audio = p_window->GetAudio();

		bgm_handle_p = 0;

		std::vector<std::pair<int*, std::string>> load_music{
			{ &bgm_handle, "./data/sounds/BGM.wav" },
			{ &se_break, "./data/sounds/break.wav" },
			{ &se_crack, "./data/sounds/crack.wav" },
			{ &se_door, "./data/sounds/door.wav" },
			{ &se_waap, "./data/sounds/warp.wav" },
			{ &se_whistle, "./data/sounds/whistle.wav" },
			{ &se_goal, "./data/sounds/goal.wav" },
		};

		for (auto& music : load_music)
		{
			*music.first = audio->Load(music.second);
			load_count++;
		}
	}

	font_sprite = std::make_unique< KDL::DX12::Sprite_Image>(p_app, "data\\fonts\\font3.png", 100u);

	FadeBoxInit(p_app);

	object_manager->Load(&load_count, p_window, p_app);

	// �v���Z�X�I�����ɂ��̊֐����Ăяo��
	//CoUninitialize();
}

void SceneGame::FileDataFlg::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
#if false
	using Keys = KDL::KEY_INPUTS;

	const auto* input = p_window->GetInput();

	auto KeyInput{ [&](Keys key1, Keys key2)
	{ return (input->IsHitKey(key1) || input->IsHitKey(key2)); } };

	const bool press_ctrl{ KeyInput(Keys::LeftControl, Keys::RightControl) };
	const bool press_shift{ KeyInput(Keys::LeftShift, Keys::RightShift) };
	//const bool press_alt{ KeyInput(Keys::LeftAlt, Keys::RightAlt ) };
	//const bool down_c{ input->IsTrgKey(Keys::C) };
	const bool down_s{ input->IsTrgKey(Keys::S) };

	// �R���g���[���L�[�������Ă��Ȃ��Ȃ�Ƃ΂�
	if (!press_ctrl)	return;

	// �ۑ�
	if (down_s && !press_shift)
		is_save = true;

	// �ʖ��ۑ�
	//else if (press_shift && down_s)
	//	is_save_as = true;

	// �J��
	else if (input->IsTrgKey(Keys::O))
		is_open = true;

	// �폜
	//else if (press_shift && press_alt && GMLIB->IsKeyPress(Key::Delete))
	//	is_delete = true;

	// ����
	//else if (press_alt && down_c)
	//	is_close = true;

	// ���O�ύX
	//else if (GMLIB->IsKeyPress(Key::R))
	//	is_rename = true;

	// �R�s�[
	//else if (press_shift && down_c)
	//	is_copy = true;
#endif
}

// �����o��
bool SceneGame::Output(const Path& output_path, const bool temp_file)
{
#if false
	// �ꎞ�t�@�C���̓I�u�W�F�N�g�����o��
	if (temp_file)
	{
		if (std::ofstream ofs{ output_path }; ofs)
		{
			object_manager->Output(ofs);
		}
		else
			return false;
	}
	else
	{
		if (std::ofstream ofs{ CameraFileDir }; ofs)
		{
			ofs << camera_angle << F_OUT(camera_dis) << std::endl;
		}
		else
			return false;

		// �ҏW���[�h�̎��̂݃I�u�W�F�N�g�����o��
		if (edit_mode)
		{
			if (std::ofstream ofs{ output_path }; ofs)
			{
				object_manager->Output(ofs);
			}
			else
				return false;
}
	}
#endif

	return true;
}

// �ǂݍ���
bool SceneGame::Input(const Path& input_path, const bool temp_file)
{
	namespace fs = std::filesystem;

#if false
	// �ꎞ�t�@�C���̓I�u�W�F�N�g��������
	if (temp_file)
	{
		if (std::ifstream ifs{ input_path }; ifs)
		{
			object_manager->Input(ifs);
		}
		else
			return false;
	}
	else
	{
		if (std::ifstream ifs{ CameraFileDir }; ifs)
		{
			ifs >> camera_angle >> camera_dis;
		}
		else
			return false;

		// �ҏW���[�h�̎��̂݃I�u�W�F�N�g�������
		if (edit_mode)
		{
			if (std::ifstream ifs{ input_path }; ifs)
			{
				object_manager->Input(ifs);
			}
			else
				return false;
		}
		// ���s���[�h
		else
		{
			// �ꎞ�t�@�C�������݂���Ȃ�A�ꎞ�t�@�C���̕���D�悷��
			if (fs::exists(TempFileDir))
			{
				if (std::ifstream ifs{ TempFileDir }; ifs)
				{
					object_manager->Input(ifs);
				}
				else
					return false;

				// �t�@�C�����J����Ă���
				if (!open_file_path.empty())
				{
					// �ꎞ�t�@�C���̓��e���X�e�[�W�ɃR�s�[
					fs::copy_file(TempFileDir, input_path, fs::copy_options::overwrite_existing);
				}
			}
			// �ꎞ�t�@�C�������݂��Ȃ��i�N�����ď���j
			else
			{
				if (std::ifstream ifs{ input_path }; ifs)
				{
					object_manager->Input(ifs);
				}
				else
					return false;
			}
		}
	}
#else
	if (std::ifstream ifs{ CameraFileDir }; ifs)
	{
		ifs >> camera_angle >> camera_dis;
	}
	else
		return false;

	if (std::ifstream ifs{ input_path }; ifs)
	{
		object_manager->Input(ifs);
	}
	else
		return false;

#endif
	return true;
}