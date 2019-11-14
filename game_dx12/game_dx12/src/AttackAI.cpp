#include <vector>

#include "NodeData.h"
#include "AttackAI.h"
#include "ObjectManager.h"
#include "ImGuiSeting.h"
#include "Enemy.h"

AttackAI::AttackAI(const bool executable, const uint64_t priority_number, const uint16_t kind_node,
	std::deque<WayPoint>* way_points)
	: RootBase(executable, (std::numeric_limits<uint64_t>::max)() - priority_number, kind_node),
	attackking_mode(nullptr), mode_state(std::move(decltype(mode_state){ false, false })),
	mode_count(-1), change_mode(true)
{
	using std::get;

	static const std::vector<std::tuple<bool, uint64_t, uint16_t>> init_data
	{
		// ���s�\�A�D�揇�ʁA���
		{ true	   , 1u		, 1 },  // �ǐ�
		{ true	   , 2u		, 2 },  // �߂�
	};

	assert(init_data.size() == attack_modes.size() && "�������f�[�^�̒ǉ����Y��");

	// �ǐՏ�ԍ\�z
	{
		const auto& data{ init_data.front() };

		attack_modes.front().emplace<0>(std::make_optional<ChaseAI>(
			get<0>(data), get<1>(data), get<2>(data)));
	}

	// �߂��ԍ\�z
	{
		const auto& data{ init_data.back() };

		attack_modes.back().emplace<1>(std::make_optional<GoBackAI>(
			get<0>(data), get<1>(data), get<2>(data), way_points));
	}
}

void AttackAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
	using std::visit;

#define	MODE_SET	mode_state.cbegin(), mode_state.cend(), [](const bool mode) { return (mode); }

	// �S�Ẵm�[�h���I��
	if (std::all_of(MODE_SET))
	{
		// ���̃��[�h���I��
		is_mode_end = true;

		// �D��x��������
		priority_number -= RootBase::SubStructNumber;
	}
	// ���[�h�ύX����
	else if (change_mode)
	{
		// ���̗���������
		mode_count++;

		change_mode = false;

		std::vector<AI_Control*> temp;
		std::vector<std::pair<bool, uint64_t>> init_data;

		// �D��x�Ǝ��s�\��Ԏ擾
		for (auto& p_mode : attack_modes)
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

				back = &attack_modes.at(pos);
			}
		} };

		// �ړ���ԍ\�z�i���s�\�Ȃ�j
		BuildObjctPtr(0u);

		// ��~��ԍ\�z�i���s�\�Ȃ�j
		BuildObjctPtr(1u);

		// ���s�\�ȃm�[�h�ōł��D��x�������m�[�h��I������
		attackking_mode = *std::max_element(temp.begin(), temp.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, *mode2) };

				return (rv1 < rv2);
			});
	}
	// �X�V
	else if (attackking_mode)
	{
		visit([&](auto& mode)
			{
				// �X�V��
				mode->Update(enemy_pos, elapsed_time, node);

				// ���̃��[�h�̍X�V���I�����m�F����
				change_mode = mode_state.at(mode_count) = mode->GetModeEnd();
			}, *attackking_mode);
	}
}

//-----------------------------------------------------------------------------------------------------------

void ChaseAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "ChaseAI");

	ImGui::SliderFloat(u8"�ړ����x", &move_speed, 0.f, 10.f, "%.1f");
	ImGui::SliderFloat(u8"�ő�ǐՎ���", &max_chase_time, 0.f, 60.f, "%.1f");

	ImGui::End();
#endif

	// ���̃m�[�h�ɗ��čŏ�
	if (is_first)
	{
		assert(Enemy::player && "std::shared_ptr<Player>* player��player�����Z�b�g���Y��");

		// ���ڈȍ~
		is_first = false;

		target_pos = (*Enemy::player)->pos;

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
			is_mode_end = true;
		}
	}
	// ���ڈȍ~
	else if (!node_data.empty())
	{
		constexpr float DisErrorRange{ 0.01f };  // �����̌덷

		// �ő�ǐՎ��Ԃ�����
		if (second_timer > max_chase_time)
		{
			is_mode_end = true;
			executable = false;
		}
		// ���̃p�X�m�[�h�ɓG������
		else if (Math::Distance(halfway_path, enemy_pos) < DisErrorRange)
		{
			target_pos = (*Enemy::player)->pos;

			// �p�X�������A���ʂ��擾
			node_data = (node->PathFindingDijkstra(enemy_pos, target_pos));
			node_array_count = 0u;

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

	timer += elapsed_time;
	second_timer += elapsed_time;
}

//-----------------------------------------------------------------------------------------------------------

void GoBackAI::Update(VF3& enemy_pos, float elapsed_time, Node* node)
{
	using Math::Pow2;

#if USE_IMGUI && false
	auto s_size{ GMLIB->GetScreenSize() };

	ImguiTool::BeginShowTempWindow({ 0.f, s_size.y / 2.f }, "GoBackAI");

	ImGui::SliderFloat(u8"�ړ����x", &move_speed, 0.f, 10.f, "%.1f");

	ImGui::End();
#endif

	// ���̃m�[�h�ɗ��čŏ�
	if (is_first)
	{
		// ���ڈȍ~
		is_first = false;

		assert(!way_points->empty() && "�E�F�C�|�C���g���ݒu����Ă��Ȃ�");

		// ��r�����v�Z�֐�
		auto SquareDis{ [](const VF3& pos1, const VF3& pos2)
		{ return Pow2(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y) + Pow2<float>(pos2.z - pos1.z); } };

		// ��ԋ߂��E�F�C�|�C���g��ڕW�n�_�ɐݒ�
		target_pos = (*std::min_element(way_points->begin(), way_points->end(),
			[&](const WayPoint& wp1, const WayPoint& wp2)
			{ return (SquareDis(wp1.pos, enemy_pos) < SquareDis(wp2.pos, enemy_pos)); })).pos;

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
			is_mode_end = true;
		}
	}
	// ���ڈȍ~
	else if (!node_data.empty())
	{
		constexpr float DisErrorRange{ 0.01f };  // �����̌덷

		// �ړI�̃E�F�C�|�C���g�ɓ��B
		if (Math::Distance(enemy_pos, target_pos) < DisErrorRange)
		{
			// ���̃��[�h���I��
			is_mode_end = true;

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
