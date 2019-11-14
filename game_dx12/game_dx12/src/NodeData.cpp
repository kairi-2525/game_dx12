#include <list>

#include "NodeData.h"
#include "ObjectManager.h"
#include "Time.h"

// パス情報の設定--------------------------------------------------------------------------------------------
void Node::PathSetting(Object& objects)
{
	auto& planes{ objects.GetChangeObjects<Plane>() };      // 床
	auto& doors{ objects.GetChangeObjects<Door>() };        // ドア
	auto& warphole{ objects.GetChangeObjects<WarpHole>() }; // ワープホール

	// 初期化
	data.clear();

	auto PositionSetting{ [&](const auto& set_obj) { std::for_each(set_obj.cbegin(), set_obj.cend(),
		[&](const auto& obj) { data.emplace_back(obj.pos); }); } };

	// ノードを追加し座標をセットする
	PositionSetting(planes);
	//PositionSetting(doors);
	PositionSetting(warphole);

	// 床の周囲の情報を取得
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

			// 自分自身ならとばす
			if (i == j)	continue;

			const std::array<VF3, 4u> poses{ left, right, up, down };

			auto ConnectNodeCheck{ [&](const VF3& pos)
			{ return (other_node.pos == pos && ConnectNode(&base_node, &other_node)); } };

			// 各方向にノードを検索し、発見したら接続する
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

	// 比較距離計算関数
	auto SquareDis{ [](const VF3& pos1, const VF3& pos2)
	{ return Pow2(pos2.x - pos1.x) + Pow2<float>(pos2.y - pos1.y) + Pow2<float>(pos2.z - pos1.z); } };

	// 座標が誤差の範囲に収まらないならば、target_posの最も近いノードを探し、そのノードの位置に修正する
	t_pos = (*std::min_element(DATA_SET, [&](const NodeData& dt1, const NodeData& dt2)
		{ return (SquareDis(dt1.pos, t_pos) < SquareDis(dt2.pos, t_pos)); })).pos;

	// 座標が誤差の範囲に収まらないならば、base_posの最も近いノードを探し、そのノードの位置に修正する
	b_pos = (*std::min_element(DATA_SET, [&](const NodeData& dt1, const NodeData& dt2)
		{ return (SquareDis(dt1.pos, b_pos) < SquareDis(dt2.pos, b_pos)); })).pos;

	assert(std::any_of(DATA_SET, [&](const NodeData& dt)
		{ return (dt.pos == t_pos); }) && "基本座標がノード上に存在しない");

	assert(std::any_of(DATA_SET, [&](const NodeData& dt)
		{ return (dt.pos == t_pos); }) && "目的座標がノード上に存在しない");

	std::deque<NodeData*> work1, work2;

	auto& currLevel{ work1 };
	auto& nextLevel{ work2 };

	auto goal{ std::find_if(DATA_SET, [&](const NodeData& dt) { return (dt.pos == t_pos); }) };
	auto start{ std::find_if(DATA_SET, [&](const NodeData& dt) { return (dt.pos == b_pos); }) };

	//ゴールから計算するので、コスト0をセットして計算済みとする
	goal->cost = 0;

	//検索第一階層のノードセット(ゴールノードをセットする)
	currLevel.emplace_back(&(*goal));

	int nodeCost;

	while (currLevel.size())
	{
		for (NodeData* it : currLevel)
		{
			for (auto& it_cnct : it->coonect_node)
			{
				nodeCost = it->cost + it_cnct.cost;

				//未探索ノードあるいは最短ルートを更新できる場合
				if (NodeData* node = it_cnct.next_node;
					(!node->is_finded || nodeCost < node->cost))
				{
					//経路コストとゴールへ向かうためのノードをセット
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//次に検索する階層のリストに登録
				nextLevel.emplace_back(it_cnct.next_node);
			}
		}

		//リストを入れ替えて次の階層を検索する
		std::swap(nextLevel, currLevel);

		nextLevel.clear();//クリアする
	}

	if (processing_time)
	{
		time.End();
		*processing_time = time.GetMicroTimer();
	}

	// ゴール以降のデータは必要ない
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

	assert(nodes && !nodes->empty() && "地面が存在しない");

	// 探索済みフラグを解除
	std::for_each(nodes->begin(), nodes->end(), [](auto& plane) { plane.is_finded = false; });

	size_t index_count_num{ 0u };

	// 初期化
	for (size_t i = 0u, length = nodes->size(); i < length; i++)
	{
		auto& plane{ nodes->at(i) };

		plane.is_finded = false;
		plane.cache_data.clear();

		// 基本座標に存在する床を取得する
		if (plane.pos == base_pos) index_count_num = i;
	}

	// 最初に、探索もとになるノードを追加（自分自身）
	//nodes->at(index_count_num).cache_data.emplace_back(nodes->at(index_count_num).pos);

	Deque<Plane*> save_planes;

	while (true)
	{
		// （基本座標からの距離、目的地への距離）、添え値
		std::deque<std::pair<std::pair<float, float>, uint32_t>> save_dis;

		bool is_end{ false };

		auto& next_plane{ nodes->at(index_count_num) };

		// 次の床が目的のパスの場合は発見
		if (next_plane.pos == target_pos)	is_end = true;

		save_planes.emplace_back(&next_plane);

		for (auto s_plane : save_planes)
		{
			// その床の近隣の床で目的の床に一番近い床を探す
			for (auto& index : s_plane->plane_index)
			{
				using Math::EasyDistance;

				const uint32_t index_num{ *(index.second) };

				// その方向に床は存在しない
				if (!index.second)	continue;

				auto& dir_plane{ nodes->at(index_num) };

				// その方向の床は既に探索済み
				if (dir_plane.is_finded)	continue;

				// その方向の基本座標からの距離とゴールへの距離を求め、添え値を追加
				save_dis.emplace_back(
					make_pair(make_pair(EasyDistance(base_pos, dir_plane.pos),
						EasyDistance(target_pos, dir_plane.pos)), *(index.second)));
			}

			// このマスを探索済みとする
			s_plane->is_finded = true;
		}

		// （基本座標からの距離、合計距離）、添え値
		decltype(save_dis) near_dis{ save_dis };

		// 合計距離を最小値のみにする
		{
			// 合計距離に変換
			for (auto& n_dis : near_dis)
				n_dis.first.second = n_dis.first.first + n_dis.first.second;

			// 合計距離の昇順に並び替える
			std::stable_sort(near_dis.begin(), near_dis.end(), [](auto& dis1, auto& dis2)
				{ return (dis1.first.second) < (dis2.first.second); });

			const float nearest_dis{ near_dis.front().first.second };

			// 合計距離の最小値より大きい場合は消す
			near_dis.erase(std::remove_if(near_dis.begin(), near_dis.end(), [nearest_dis](const auto& dis)
				{ return dis.first.second > nearest_dis; }), near_dis.end());
		}

		// 基本座標からの距離が遠い物を選択
		{
			assert(near_dis.size() != 0u && "不正なサイズ");

			// 基本座標からの距離が一番遠い床を探す
			const auto get_max_element{ *std::max_element(near_dis.cbegin(), near_dis.cend(),
				[&](auto& dis1, auto& dis2) { return dis1.first < dis2.first; }) };

			const auto& before_plane{ nodes->at(index_count_num) };

			// 次の探す床のインデックスを決定
			index_count_num = get_max_element.second;

			auto& after_cache_data{ nodes->at(index_count_num).cache_data };

			// 探索もとになるノードを追加
			after_cache_data = before_plane.cache_data;

			// 探索もとになるノードを追加（自分自身）
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
		{ return (dt.pos == base_pos); }) && "基本座標がノード上に存在しない");

	assert(std::any_of(DATA_SET, [&](const auto& dt)
		{ return (dt.pos == target_pos); }) && "目的座標がノード上に存在しない");

	std::deque<NodeData*> work1, work2;

	auto* currLevel{ &work1 };
	auto* nextLevel{ &work2 };

	auto goal{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == target_pos); }) };
	auto start{ std::find_if(DATA_SET, [&](const auto& dt) { return (dt.pos == base_pos); }) };

	//ゴールから計算するので、コスト0をセットして計算済みとする
	goal->cost = 0;

	//検索第一階層のノードセット(ゴールノードをセットする)
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

				//未探索ノードあるいは最短ルートを更新できる場合
				if (auto* node = connect.next_node; (!node->is_finded || nodeCost < node->cost))
				{
					//経路コストとゴールへ向かうためのノードをセット
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//次に検索する階層のリストに登録
				nextLevel->emplace_back(connect.next_node);
	}
#else
			// ノードデータ、(スタートからの距離、ゴールへの距離)
			std::deque<std::pair<NodeData*, std::pair<float, float>>> connect_nodes;

			// 並び替え用に構築
			for (NodeData::NodeConnect& node : it->coonect_node)
			{
				constexpr float Fmax{ (std::numeric_limits<float>::max)() };

				connect_nodes.emplace_back(node.next_node, std::make_pair(Fmax, Fmax));
			}

			// 合計距離を最小値のみにする
			{
				// 合計距離に変換
				for (auto& c_node : connect_nodes)
				{
					using Math::EasyDistance;

					const auto next_node{ c_node.first };
					const float start_dis{ EasyDistance(next_node->pos, start->pos) };
					const float goal_dis{ EasyDistance(next_node->pos, goal->pos) };

					c_node.second = std::make_pair(start_dis, goal_dis);
				}

				// 合計距離の昇順に並び替える
				std::stable_sort(connect_nodes.begin(), connect_nodes.end(), [](auto& dis1, auto& dis2)
					{
						float total_dis1{ dis1.second.first + dis1.second.second };
						float total_dis2{ dis2.second.first + dis2.second.second };

						return (total_dis1) < (total_dis2);
					});

				const float nearest_dis
				{ connect_nodes.front().second.first + connect_nodes.front().second.second };

				// 合計距離の最小値より大きい場合は消す
				connect_nodes.erase(std::remove_if(
					connect_nodes.begin(), connect_nodes.end(), [nearest_dis](const auto& dis)
					{ return dis.second.first + dis.second.second > nearest_dis; }), connect_nodes.end());
			}

			// 基本座標からの距離が遠い物を選択
			{
				assert(connect_nodes.size() != 0u && "不正なサイズ");

				// 基本座標からの距離が一番遠い床を探す
				const auto get_max_element{ *std::max_element(connect_nodes.cbegin(), connect_nodes.cend(),
					[&](auto& dis1, auto& dis2) { return dis1.second.first < dis2.second.first; }) };

				nodeCost = get_max_element.first->cost + get_max_element.first->cost;

				//未探索ノードあるいは最短ルートを更新できる場合
				if (auto* node = get_max_element.first; (!node->is_finded || nodeCost < node->cost))
				{
					//経路コストとゴールへ向かうためのノードをセット
					node->cost = nodeCost;
					node->to_goal = it;
					node->is_finded = true;
				}
				else
					continue;

				//次に検索する階層のリストに登録
				nextLevel->emplace_back(get_max_element.first);
			}
#endif
		}

		//リストを入れ替えて次の階層を検索する
		std::swap(nextLevel, currLevel);

		nextLevel->clear();//クリアする
	}

	if (processing_time)
	{
		time.End();
		*processing_time = time.GetMicroTimer();
	}

	return *start;

#endif
}