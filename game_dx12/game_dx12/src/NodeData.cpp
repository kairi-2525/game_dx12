#include <list>

#include "NodeData.h"
#include "ObjectManager.h"
#include "Time.h"

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
	for (size_t i = 0, length{ data.size() }; i < length; i++)
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

			const std::array<VF3, 4u> poses{ left, right, up, down };

			auto ConnectNodeCheck{ [&](const VF3& pos)
			{ return (other_node.pos == pos && ConnectNode(&base_node, &other_node)); } };

			// �e�����Ƀm�[�h���������A����������ڑ�����
			if (std::any_of(poses.begin(), poses.end(), [&](const VF3& pos)
				{ return ConnectNodeCheck(pos); }))
				continue;
		}
	}
}

std::deque<NodeData> Node::PathFindingDijkstra(const VF3& base_pos, const VF3& target_pos, int64_t* processing_time)
{
#define DATA_SET	data.begin(), data.end()
	using Math::Pow2;

	Timer time;

	if (processing_time)
		time.Start();

	std::for_each(DATA_SET, [](NodeData& dt) { dt.Reset(); });

	VF3 t_pos{ target_pos };
	VF3 b_pos{ base_pos };

	// ��r�����v�Z�֐�
	auto SquareDis{ [](const VF3& pos1, const VF3& pos2)
	{ return Pow2(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y) + Pow2<float>(pos2.z - pos1.z); } };

	// ���W���덷�͈̔͂Ɏ��܂�Ȃ��Ȃ�΁Atarget_pos�̍ł��߂��m�[�h��T���A���̃m�[�h�̈ʒu�ɏC������
	t_pos = (*std::min_element(DATA_SET, [&](const NodeData& dt1, const NodeData& dt2)
		{ return (SquareDis(dt1.pos, t_pos) < SquareDis(dt2.pos, t_pos)); })).pos;

	// ���W���덷�͈̔͂Ɏ��܂�Ȃ��Ȃ�΁Abase_pos�̍ł��߂��m�[�h��T���A���̃m�[�h�̈ʒu�ɏC������
	b_pos = (*std::min_element(DATA_SET, [&](const NodeData& dt1, const NodeData& dt2)
		{ return (SquareDis(dt1.pos, b_pos) < SquareDis(dt2.pos, b_pos)); })).pos;

	assert(std::any_of(DATA_SET, [&](const NodeData& dt)
		{ return (dt.pos == t_pos); }) && "��{���W���m�[�h��ɑ��݂��Ȃ�");

	assert(std::any_of(DATA_SET, [&](const NodeData& dt)
		{ return (dt.pos == t_pos); }) && "�ړI���W���m�[�h��ɑ��݂��Ȃ�");

	std::deque<NodeData*> work1, work2;

	auto& currLevel{ work1 };
	auto& nextLevel{ work2 };

	auto goal{ std::find_if(DATA_SET, [&](const NodeData& dt) { return (dt.pos == t_pos); }) };
	auto start{ std::find_if(DATA_SET, [&](const NodeData& dt) { return (dt.pos == b_pos); }) };

	//�S�[������v�Z����̂ŁA�R�X�g0���Z�b�g���Čv�Z�ς݂Ƃ���
	goal->cost = 0;

	//�������K�w�̃m�[�h�Z�b�g(�S�[���m�[�h���Z�b�g����)
	currLevel.emplace_back(&(*goal));

	int nodeCost;

	while (currLevel.size())
	{
		for (NodeData* it : currLevel)
		{
			for (auto& it_cnct : it->coonect_node)
			{
				nodeCost = it->cost + it_cnct.cost;

				//���T���m�[�h���邢�͍ŒZ���[�g���X�V�ł���ꍇ
				if (NodeData* node = it_cnct.next_node;
					(!node->is_finded || nodeCost < node->cost))
				{
					//�o�H�R�X�g�ƃS�[���֌��������߂̃m�[�h���Z�b�g
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//���Ɍ�������K�w�̃��X�g�ɓo�^
				nextLevel.emplace_back(it_cnct.next_node);
			}
		}

		//���X�g�����ւ��Ď��̊K�w����������
		std::swap(nextLevel, currLevel);

		nextLevel.clear();//�N���A����
	}

	if (processing_time)
	{
		time.End();
		*processing_time = time.GetMicroTimer();
	}

	// �S�[���ȍ~�̃f�[�^�͕K�v�Ȃ�
	goal->to_goal = nullptr;

	std::deque<NodeData> rv;

	NodeData* temp{ &(*start) };

	while (temp)
	{
		rv.emplace_back(temp->pos);

		temp = (temp->to_goal);
	}

	rv.pop_front();

	return rv;
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