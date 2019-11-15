#include <vector>

#include "ImGuiSeting.h"
#include "PatrolAI.h"
#include "ObjectManager.h"
#include "Enemy.h"
#include "XMFLOAT_Hlper.h"
#include "GameScene.h"

#if !_DEBUG
#include "MessageBox.h"
#endif

PatrolAI::PatrolAI(const bool executable, const uint64_t priority_number, size_t& way_point_count,
	std::deque<WayPoint>* way_points, const uint16_t kind_node, const float* enemy_angle)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
	change_mode(true), patroling_mode(nullptr), way_point_count(way_point_count), way_points(way_points),
	end_number((std::numeric_limits<uint16_t>::max)()), enemy_angle_y(enemy_angle)
{
	using std::get;

	static const std::vector<std::tuple<bool, uint64_t, uint16_t>> init_data
	{
		// ���s�\�A�D�揇�ʁA���
		{ true	   , 2		, 1 },  // �ړ�
		{ true	   , 3		, 2 },  // ��~
		{ false	   , 1		, 3 },  // ����
	};

	assert(init_data.size() == 3u && "�������f�[�^�̒ǉ����Y��");

	// �ړ���ԍ\�z
	{
		const auto& data{ init_data.front() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<0>(std::make_optional<MoveAI>(
			get<0>(data), get<1>(data), way_point_count, way_points, get<2>(data)));
	}

	// ��~��ԍ\�z
	{
		const auto& data{ init_data.at(1) };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<1>(std::make_optional<StopAI>(get<0>(data), get<1>(data), get<2>(data)));
	}

	// ������ԍ\�z
	{
		const auto& data{ init_data.back() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<2>(std::make_optional<FindAI>(get<0>(data), get<1>(data), get<2>(data)));
	}
}

void PatrolAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
	using std::visit;

	// �O���FindAI�m�[�h���I�����Ă�����
	if (end_number == 3u && change_mode)
	{
		// �p�X�����������ʁA�p�X���������Ȃ��Ȃ�U����Ԃֈڂ�Ȃ�
		if (node->PathFindingDijkstra(enemy_pos, (*Enemy::player)->pos).empty())
		{
			end_number = (std::numeric_limits<uint16_t>::max)();
			change_mode = true;

			return;
		}

		// ���̃��[�h���I��
		is_mode_end = true;

		// �D��x��������
		priority_number -= RootBase::SubStructNumber;
	}
	// ���[�h�ύX����
	else if (change_mode)
	{
		change_mode = false;

		InitModeData();

		std::vector<AI_Control*> temp;
		std::vector<std::pair<bool, uint64_t>> init_data;

		// �D��x�Ǝ��s�\��Ԏ擾
		for (auto& p_mode : patrol_modes)
		{
			constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

			uint64_t priority_num
			{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
			bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };

			init_data.emplace_back(is_executable, MaxNum - priority_num);
		}

		auto BuildObjctPtr{ [&](size_t pos)
		{
			if (const auto& data{ init_data.at(pos) }; data.first)
			{
				auto& back{ temp.emplace_back() };

				back = &patrol_modes.at(pos);
			}
		} };

		// �ړ���ԍ\�z�i���s�\�Ȃ�j
		BuildObjctPtr(0u);

		// ��~��ԍ\�z�i���s�\�Ȃ�j
		BuildObjctPtr(1u);

		// ������ԍ\�z�i���s�\�Ȃ�j
		BuildObjctPtr(2u);

		// ���s�\�ȃm�[�h�ōł��D��x�������m�[�h��I������
		patroling_mode = { (*std::max_element(temp.begin(), temp.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode2) };

				return (rv1 < rv2);
			})) };
	}
	// �X�V����
	else if (patroling_mode)
	{
		constexpr uint16_t InitNumber{ (std::numeric_limits<uint16_t>::max)() };

		end_number = InitNumber;

		uint16_t kinds_num{ 0 };

		visit([&](auto& mode)
			{
				// �X�V
				mode->Update(enemy_pos, elapsed_time, node);

				// ���̃��[�h�̍X�V���I�����m�F����
				change_mode = mode->GetModeEnd();

				kinds_num = mode->GetKindNode();

				// ���̃��[�h�̏I�����Ɏ�ނ�ۑ�
				if (change_mode)
					end_number = kinds_num;
			}, *patroling_mode);

#if _DEBUG
		if (!(*Enemy::player))
		{
			assert(!"�v���C���[��z�u���Ă��Ȃ��i���s�j");
			SceneGame::execution_quick_exit = true;
			return;
		}
#else
		if (!(*Enemy::player) && Message(L"�v���C���[��z�u���Ă��Ȃ�", L"�G�f�B�^�[�ֈڍs����ZE��") == BoxReturn::Yes_Ok)
		{
			SceneGame::execution_quick_exit = true;
			return;
		}
#endif

		// �����Ƃ̓����蔻��iFind�m�[�h�������j
		if (!change_mode && kinds_num != 3 &&
			IsPlayerRayHit(enemy_pos, (*Enemy::player)->pos))
		{
			// Find�m�[�h�����s�\��Ԃ�
			visit([](auto& mode) { mode->SetExecutable(true); }, patrol_modes.back());

			// ���[�h�ύX�\��
			change_mode = true;
		}
	}
}

bool PatrolAI::IsPlayerRayHit(const VF3& enemy_pos, const VF3& player_pos)
{
	namespace DX = DirectX;
	using Math::AdjEqual;
	using VF2 = DX::XMFLOAT2;
	using Vec2sub::MakeVector2;

	// �G�������������ɂ��Ȃ�
	if (Math::Distance(enemy_pos, player_pos) > hit_distance)	return false;

	DX::XMVECTOR enm_vec{ DX::XMVectorSet(0.f, 0.f, 0.f, 0.f) };

	// �G�̔����͈�
	{
		constexpr float AdjRadY{ Math::PAI<float> / 2.f };

		const auto e_pos{ MakeVector2<float>(enemy_pos.x, enemy_pos.z) };
		const auto p_pos{ MakeVector2<float>(player_pos.x, player_pos.z) };

		const auto enm_to_pl_vec{ ToNormalizeXMVECTOR(e_pos - p_pos) };

		// �E
		if (AdjEqual(*enemy_angle_y, AdjRadY))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ +1.f, 0.f }) };

			enm_vec = InitVec;
		}
		// ��
		else if (AdjEqual(*enemy_angle_y, -AdjRadY))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ -1.f, 0.f }) };

			enm_vec = InitVec;
		}
		// ��
		else if (AdjEqual(*enemy_angle_y, AdjRadY * 0.f))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ 0.f, +1.f }) };

			enm_vec = InitVec;
		}
		// ��
		else if (AdjEqual(*enemy_angle_y, AdjRadY * 2.f))
		{
			static const auto InitVec{ ToXMVECTOR(VF2{ 0.f, -1.f }) };

			enm_vec = InitVec;
		}

		// �G����ɂ����v���[���[�̊p�x
		const float radian{ std::acos(ToXMFLOAT1(DX::XMVector2Dot(enm_to_pl_vec, enm_vec))) };

		// �G�������͈͓��ɂ��Ȃ�
		if (radian > hit_angle)		return false;
	}

	for (auto& wall : *Enemy::walls)
	{
		constexpr float WallDivSize{ 0.6f };

		// �F���͈͊O�͏������Ȃ�
		if (Math::Distance(wall.pos, enemy_pos) > hit_distance)	continue;

		const auto e_pos{ MakeVector2<float>(enemy_pos.x, enemy_pos.z) };
		const auto w_pos{ MakeVector2<float>(wall.pos.x, wall.pos.z) };

		const auto& enm_wall_vec{ ToNormalizeXMVECTOR(-(w_pos - e_pos)) };

		// �G����ɂ����ǂ̊p�x
		const float radian{ std::acos(ToXMFLOAT1(DX::XMVector2Dot(enm_wall_vec, enm_vec))) };

		// �ǂ������͈͓��ɂ��Ȃ�
		if (radian > hit_angle)		continue;

		const auto point{ ToXMVECTOR(w_pos) };
		const auto line_point1{ ToXMVECTOR(VF2{ player_pos.x, player_pos.z }) };
		const auto line_point2{ ToXMVECTOR(VF2{ enemy_pos.x, enemy_pos.z }) };

		// �G�Ǝ��@�̐��ƕǂ̍ŒZ����
		const float dis{ ToXMFLOAT1(DX::XMVector2LinePointDistance(line_point1, line_point2, point)) };

		// �ŒZ�������ǂ����ƌ��Ȃ������̔��a�����Z��
		if (dis < WallDivSize)	return false;
	}

	return true;
}

void PatrolAI::InitModeData()
{
	using std::visit;
	using std::get;

	std::vector<std::tuple<bool, uint64_t, uint16_t>> init_data;

	// �D�揇�ʂƎ��s�\��Ԃ��擾��
	for (auto& p_mode : patrol_modes)
	{
		constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

		uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
		bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };
		uint16_t kinds{ visit([](const auto& mode) { return mode->GetKindNode(); }, p_mode) };

		init_data.emplace_back(is_executable, MaxNum - priority_num, kinds);
	}

	// ���[�h������
	patrol_modes.clear();

	// ���[�h���Đ���
	{
		// �ړ���ԍ\�z
		{
			const auto& data{ init_data.front() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<0>(std::make_optional<MoveAI>(
				get<0>(data), get<1>(data), way_point_count, way_points, get<2>(data)));
		}

		// ��~��ԍ\�z
		{
			const auto& data{ init_data.at(1) };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<1>(std::make_optional<StopAI>(get<0>(data), get<1>(data), get<2>(data)));
		}

		// ������ԍ\�z
		{
			const auto& data{ init_data.back() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<2>(std::make_optional<FindAI>(get<0>(data), get<1>(data), get<2>(data)));
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void MoveAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "MoveAI");

	ImGui::SliderFloat(u8"�ړ����x", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// ���̃m�[�h�ɗ��čŏ�
	if (is_first && way_points)
	{
		// �E�F�C�|�C���g�������G�͂��̃m�[�h�ł͉������Ȃ�
		if (way_points->empty())
		{
			is_mode_end = true;
			return;
		}

		// ���ڈȍ~
		is_first = false;

		// �덷���C������
		enemy_pos = Round(enemy_pos);

		// ���������J�E���g��߂�
		if (way_point_count >= way_points->size()) way_point_count = 0u;

		// �E�F�C�|�C���g�̃J�E���g�ɍ��������W��ڕW�n�_�ɂ���
		target_pos = way_points->at(way_point_count).pos;

		// �p�X�������A���ʂ��擾
		node_data = (node->PathFindingDijkstra(enemy_pos, target_pos));
		node_array_count = 0u;

		// �p�X�𔭌��o�����ꍇ
		if (!node_data.empty())
		{
			// ���̃p�X�m�[�h�̈ʒu���擾
			halfway_path = node_data[node_array_count].pos;

			// ���̃p�X�m�[�h�����擾
			node_array_count++;
		}
		// �܏��H�i�p�X�𔭌��ł����j
		else
		{
#if _DEBUG
			assert(!"�E�F�C�|�C���g�̐ݒu�ʒu�����������i���s�j");
			SceneGame::execution_quick_exit = true;
			return;
#else
			std::wstring text{ L"�E�F�C�|�C���g�̐ݒu�ʒu������������I�I\n" };

			text += std::to_wstring(way_point_count + 1);
			text += L"�ڂ̃E�F�C�|�C���g�̐ݒu�ʒu�܂ōs���Ȃ���I�I";

			if (Message(text.data(), L"�G�f�B�^�[�ֈڍs����ZE��") == BoxReturn::Yes_Ok)
			{
				SceneGame::execution_quick_exit = true;
				return;
			}
#endif
		}
	}
	// ���ڈȍ~
	else if (!node_data.empty())
	{
		constexpr float DisErrorRange{ 0.01f };  // �����̌덷

		// ���̃E�F�C�|�C���g�ɓ��B
		if (Math::Distance(enemy_pos, target_pos) < DisErrorRange)
		{
			// ���̃��[�h���I��
			is_mode_end = true;

			// �D��x��������
			priority_number -= RootBase::SubStructNumber;

			// ���̃E�F�C�|�C���g���w������
			way_point_count++;

			// �덷���C������
			enemy_pos = Round(enemy_pos);
		}
		// ���̃p�X�m�[�h�ɓG������
		else if (Math::Distance(halfway_path, enemy_pos) < DisErrorRange)
		{
			// ���̃p�X�m�[�h�̈ʒu���擾
			halfway_path = node_data[node_array_count].pos;

			// ���̃p�X�m�[�h�����擾
			node_array_count++;

			// �덷���C������
			enemy_pos = Round(enemy_pos);
		}
		// �i�s��
		else
		{
			const auto speed_vec{ ToNormalizeXMVECTOR(halfway_path - enemy_pos) };
			const VF3 speed{ ToXMFLOAT3(speed_vec) * move_speed * elapsed_time };

			enemy_pos += speed;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void StopAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"��~����", &stop_time, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	if (timer += elapsed_time; timer > stop_time)
	{
		is_mode_end = true;

		// �D��x��������
		priority_number -= RootBase::SubStructNumber;
	}
}

//-----------------------------------------------------------------------------------------------------------

void FindAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"��~����", &stop_time, 0.f, 10.f, "%.0f");

	ImGui::End();
#endif

	// ���񂾂�
	if (timer == 0.f)
	{
		using SG = SceneGame;

		int handle = SG::audio->CreatePlayHandle(SG::se_whistle, 0.f, false, false, 0.f, 0.f, 0, false, false);
		SG::audio->Play(SG::se_whistle, handle, 0.01f, 0.2f, false);
	}

	// ��莞�Ԍ�
	if (timer += elapsed_time; timer > stop_time)
	{
		// ���̃��[�h���I��
		is_mode_end = true;
		executable = false;
	}
}