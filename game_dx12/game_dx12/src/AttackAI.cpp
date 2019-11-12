#include <vector>

#include "NodeData.h"
#include "AttackAI.h"
#include "ObjectManager.h"

AttackAI::AttackAI(const bool executable, const uint64_t priority_number)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number),
	attackking_mode(nullptr), change_mode(std::move(decltype(change_mode){ false, false })),
	mode_count(0u)
{
	static const std::vector<std::pair<bool, uint64_t>> init_data
	{
		// ���s�\�A�D�揇��
		{ true	   , 1 },  // �ړ�
		{ true	   , 2 },  // ��~
	};

	assert(init_data.size() == attack_modes.size() && "�������f�[�^�̒ǉ����Y��");

	// �ړ���ԍ\�z
	{
		const auto& data{ init_data.front() };

		attack_modes.front().emplace<0>(std::make_optional<ChaseAI>(data.first, data.second));
	}

	// ��~��ԍ\�z
	{
		const auto& data{ init_data.back() };

		attack_modes.front().emplace<1>(std::make_optional<GoBackAI>(data.first, data.second));
	}
}

void AttackAI::Update(VF3& enemy_pos, const float elapsed_time, Node* node)
{
	using std::visit;

	// ���[�h�ύX����
	if (std::any_of(change_mode.begin(), change_mode.end(), [](const bool mode) { return (mode); }))
	{
		// ���̗���������
		mode_count++;

		attackking_mode = { &(*std::max_element(attack_modes.begin(), attack_modes.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode2) };

				return (rv1 < rv2);
			})) };
	}
	else if (attackking_mode)
	{
		visit([&](auto& mode)
			{
				// �X�V��
				mode->Update(enemy_pos, elapsed_time, node);

				// ���̃��[�h�̍X�V���I�����m�F����
				change_mode.at(mode_count) = mode->GetModeEnd();

			}, *attackking_mode);
	}
}

//-----------------------------------------------------------------------------------------------------------

void ChaseAI::Update(VF3& enemy_pos, const float elapsed_time, Node* node)
{
#if false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "MoveAI");

	ImGui::SliderFloat(u8"�ړ����x", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// ���̃m�[�h�ɗ��čŏ�
	if (is_first)
	{
		// ���ڈȍ~
		is_first = false;

		// �덷���C������
		enemy_pos = Round(enemy_pos);

		// �E�F�C�|�C���g�̃J�E���g�ɍ��������W��ڕW�n�_�ɂ���
		//target_pos = way_points->at(way_point_count).pos;

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
			is_mode_end = true;
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
			//const auto speed_vec{ ToNormalizeXMVECTOR(halfway_path - enemy_pos) };
			//const VF3 speed{ ToXMFLOAT3(speed_vec) * move_speed * GMLIB->GetElapsedTime() };

			//enemy_pos += speed;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

void GoBackAI::Update(VF3& enemy_pos, const float elapsed_time, Node* node)
{
}
