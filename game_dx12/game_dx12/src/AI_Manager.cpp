#include <vector>

#include "Enemy.h"
#include "AI_Manager.h"

AI_Manager::AI_Manager(std::deque<WayPoint>* way_points)
	: change_mode(true), running_mode(nullptr), target_pos(0.f, 0.f, 0.f), way_points(way_points),
	way_point_count(0u)
{
	static const std::vector<std::pair<bool, uint64_t>> init_data
	{
		// ���s�\�A�D�揇��
		{ true	   , 1 },  // ����
		{ true	   , 2 },  // �U��
	};

	assert(init_data.size() == run_modes.size() && "�������f�[�^�̒ǉ����Y��");

	// �����ԍ\�z
	{
		const auto& data{ init_data.front() };

		run_modes.front().emplace<0>(
			std::make_optional<PatrolAI>(data.first, data.second, way_point_count, way_points));
	}

	// �U����ԍ\�z
	{
		const auto& data{ init_data.back() };

		run_modes.at(1).emplace<1>(std::make_optional<AttackAI>(data.first, data.second));
	}
}

void AI_Manager::Update(Enemy& enemy, float elapsed_time, Node* node)
{
	using std::visit;

	// ���[�h�ύX����
	if (change_mode)
	{
		change_mode = false;

		InitModeData();

		running_mode = { &(*std::max_element(run_modes.begin(), run_modes.end(),
			[](const auto& mode1, const auto& mode2)
			{
				uint64_t rv1{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode1) };
				uint64_t rv2{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, mode2) };

				return (rv1 < rv2);
			})) };
	}
	else if (running_mode)
	{

		visit([&](auto& mode)
			{
				// �X�V��
				mode->Update(enemy.pos, elapsed_time, node);

				// ���̃��[�h�̍X�V���I�����m�F����
				change_mode = mode->GetModeEnd();

			}, *running_mode);
	}
}

void AI_Manager::InitModeData()
{
	using std::visit;

	std::vector<std::pair<bool, uint64_t>> init_data;

	// �D�揇�ʂƎ��s�\��Ԃ��擾��
	for (auto& p_mode : run_modes)
	{
		constexpr uint64_t MaxNum{ (std::numeric_limits<uint64_t>::max)() };

		uint64_t priority_num{ visit([](const auto& mode) { return mode->GetPriorityNumber(); }, p_mode) };
		bool is_executable{ visit([](const auto& mode) { return mode->GetExecutable(); }, p_mode) };

		init_data.emplace_back(is_executable, MaxNum - priority_num);
	}

	// ���[�h������
	for (auto& r_mode : run_modes)
	{
		std::visit([](auto& mode) { mode = std::nullopt; }, r_mode);
	}

	// ���[�h���Đ���
	{
		// �����ԍ\�z
		{
			const auto& data{ init_data.front() };

			run_modes.front().emplace<0>(
				std::make_optional<PatrolAI>(data.first, data.second, way_point_count, way_points));
		}

		// �U����ԍ\�z
		{
			const auto& data{ init_data.back() };

			run_modes.at(1).emplace<1>(std::make_optional<AttackAI>(data.first, data.second));
		}
	}
}
