#include <vector>

#include "ImGui/ImGuiSeting.h"
#include "GameLib.h"
#include "PatrolAI.h"
#include "ObjectManager.h"
#include "Enemy.h"
#include "XMFLOAT_Hlper.h"
#if !_DEBUG
#include "MessageBox.h"
#endif

PatrolAI::PatrolAI(const bool executable, const uint64_t priority_number, size_t& way_point_count,
	std::deque<WayPoint>* way_points)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number), change_mode(true),
	patroling_mode(nullptr), way_point_count(way_point_count), way_points(way_points)
{
	static const std::vector<std::pair<bool, uint64_t>> init_data
	{
		// ���s�\�A�D�揇��
		{ true	   , 2 },  // �ړ�
		{ true	   , 3 },  // ��~
		{ false	   , 1 },  // ����
	};

	assert(init_data.size() == 3u && "�������f�[�^�̒ǉ����Y��");

	// �ړ���ԍ\�z
	{
		const auto& data{ init_data.front() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<0>(std::make_optional<MoveAI>(data.first, data.second, way_point_count, way_points));
	}

	// ��~��ԍ\�z
	{
		const auto& data{ init_data.at(1) };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<1>(std::make_optional<StopAI>(data.first, data.second));
	}

	// ������ԍ\�z
	{
		const auto& data{ init_data.back() };

		auto& back{ patrol_modes.emplace_back() };

		back.emplace<2>(std::make_optional<FindAI>(data.first, data.second));
	}
}

void PatrolAI::Update(VF3& enemy_pos, Node* node)
{
	using std::visit;

	if (GMLIB->isKeyDown(KeyData::Keys::P))
	{
		// ���̃��[�h���I��
		is_mode_end = true;

		// �D��x��������
		priority_number -= RootBase::SubStructNumber;
	}

	// ���[�h�ύX����
	if (change_mode)
	{
		change_mode = false;

		InitModeData();

		std::vector<AI_Control*> temp;
		std::vector<std::pair<bool, uint64_t>> init_data;

		// �D��x�Ǝ��s�\��Ԏ擾
		for (auto& p_mode : patrol_modes)
		{
			constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

			uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
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
		visit([&](auto& mode)
			{
				// �X�V
				mode->Update(enemy_pos, node);

				// ���̃��[�h�̍X�V���I�����m�F����
				change_mode = mode->GetModeEnd();
			}, *patroling_mode);
	}
}

void PatrolAI::InitModeData()
{
	using std::visit;

	std::vector<std::pair<bool, uint64_t>> init_data;

	// �D�揇�ʂƎ��s�\��Ԃ��擾��
	for (auto& p_mode : patrol_modes)
	{
		constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

		uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
		bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };

		init_data.emplace_back(is_executable, MaxNum - priority_num);
	}

	// ���[�h������
	patrol_modes.clear();

	// ���[�h���Đ���
	{
		// �ړ���ԍ\�z
		{
			const auto& data{ init_data.front() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<0>(std::make_optional<MoveAI>(data.first, data.second, way_point_count,
				way_points));

			//BuildAINode<MoveAI, 0u>(init_data, way_point_count, way_points);
		}

		// ��~��ԍ\�z
		{
			const auto& data{ init_data.at(1) };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<1>(std::make_optional<StopAI>(data.first, data.second));
		}

		// ������ԍ\�z
		{
			const auto& data{ init_data.back() };

			auto& back{ patrol_modes.emplace_back() };

			back.emplace<2>(std::make_optional<FindAI>(data.first, data.second));
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void MoveAI::Update(VF3& enemy_pos, Node* node)
{
#if USE_IMGUI
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "MoveAI");

	ImGui::SliderFloat(u8"�ړ����x", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// ���̃m�[�h�ɗ��čŏ�
	if (is_first && way_points)
	{
		// ���ڈȍ~
		is_first = false;

		// �덷���C������
		enemy_pos = Round(enemy_pos);

		// ���������J�E���g��߂�
		if (way_point_count >= way_points->size()) way_point_count = 0u;

		// �E�F�C�|�C���g�̃J�E���g�ɍ��������W��ڕW�n�_�ɂ���
		target_pos = way_points->at(way_point_count).pos;

		// �p�X�������A���ʂ��擾
		node_data = node->PathFindingDijkstra(enemy_pos, target_pos);

		// �p�X�𔭌��o�����ꍇ
		if (node_data)
		{
			// ���̃p�X�m�[�h�̈ʒu���擾
			halfway_path = node_data->to_goal->pos;

			assert(node_data && node_data->to_goal && "����ł���");

			// ���̃p�X�m�[�h�����擾
			node_data = node_data->to_goal;
		}
		// �܏��H�i�p�X�𔭌��ł����j
		else
		{
#if _DEBUG
			assert(!"�E�F�C�|�C���g�̐ݒu�ʒu����������");
#else
			if (Message(L"�E�F�C�|�C���g�̐ݒu�ʒu����������", L"�I�����܂�") == BoxReturn::Yes_Ok)
			{
				exit(EXIT_SUCCESS);
			}
#endif
		}
	}
	// ���ڈȍ~
	else if (node_data)
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
			assert(node_data && node_data->to_goal && "����ł���");

			// ���̃p�X�m�[�h�̈ʒu���擾
			halfway_path = node_data->to_goal->pos;

			// ���̃p�X�m�[�h�����擾
			node_data = node_data->to_goal;

			// �덷���C������
			enemy_pos = Round(enemy_pos);
		}
		// �i�s��
		else
		{
			const auto speed_vec{ ToNormalizeXMVECTOR(halfway_path - enemy_pos) };
			const VF3 speed{ ToXMFLOAT3(speed_vec) * move_speed * GMLIB->GetElapsedTime() };

			enemy_pos += speed;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void StopAI::Update(VF3& enemy_pos, Node* node)
{
#if USE_IMGUI
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"��~����", &stop_time, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	if (timer += GMLIB->GetElapsedTime(); timer > stop_time)
	{
		is_mode_end = true;

		// �D��x��������
		priority_number -= RootBase::SubStructNumber;
	}
}

//-----------------------------------------------------------------------------------------------------------

void FindAI::Update(VF3& enemy_pos, Node* node)
{
#if USE_IMGUI
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "StopAI");

	ImGui::SliderFloat(u8"��~����", &stop_time, 0.f, 10.f, "%.0f");

	ImGui::End();
#endif

	if (timer += GMLIB->GetElapsedTime(); timer > stop_time)
	{
		is_mode_end = true;

		// �D��x��������
		priority_number -= RootBase::SubStructNumber;
	}
}