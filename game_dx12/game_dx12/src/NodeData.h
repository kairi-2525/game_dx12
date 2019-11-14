#pragma once

#include <deque>
#include <DirectXMath.h>
#include <algorithm>

#include "FileStreamHelpter.h"
#include "XMFLOAT_Hlper.h"

class NodeData final
{
private:
	using VF4 = DirectX::XMFLOAT4;
	using VF3 = DirectX::XMFLOAT3;
	using VF2 = DirectX::XMFLOAT2;

public:
	NodeData(const VF3& pos)
		: is_finded(false), cost(0u), to_goal(nullptr), pos(pos)
	{}
	~NodeData() noexcept = default;

	NodeData(const NodeData& _rt)
		: coonect_node(_rt.coonect_node), cost(_rt.cost), is_finded(_rt.is_finded), pos(_rt.pos), to_goal(_rt.to_goal)
	{}
	auto& operator=(const NodeData& _rt)
	{
		coonect_node = _rt.coonect_node;
		cost = _rt.cost;
		is_finded = _rt.is_finded;
		pos = _rt.pos;
		to_goal = _rt.to_goal;

		return (*this);
	}

	NodeData(NodeData&& _rt) noexcept
	{
		coonect_node.swap(_rt.coonect_node);
		cost = _rt.cost;
		is_finded = _rt.is_finded;
		pos = std::move(_rt.pos);
		to_goal = _rt.to_goal;
	}
	auto& operator=(NodeData&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			coonect_node.swap(_rt.coonect_node);
			cost = _rt.cost;
			is_finded = _rt.is_finded;
			pos = move(_rt.pos);
			to_goal = _rt.to_goal;
		}

		return (*this);
	}

public:
	struct NodeConnect
	{
		NodeConnect(NodeData* node, const uint16_t cost)
			: next_node(node), cost(cost)
		{}
		NodeConnect()
			: next_node(nullptr), cost(0u)
		{}
		~NodeConnect() noexcept = default;

		NodeData* next_node;  // �ړ���m�[�h
		uint16_t cost;   // �ړ��ɂ�����R�X�g
	};

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const NodeData& data)
	{
		ofs << data.pos;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, NodeData& data)
	{
		ifs >> data.pos;

		return ifs;
	}

public:
	void NodeDataInit()
	{
		coonect_node.clear();
		pos = Fill3(0.f);
		cost = 0u;
		is_finded = false;
		to_goal = nullptr;
	}
	void Reset()
	{
		cost = 0u;
		to_goal = nullptr;
		is_finded = false;
	}

public:
	std::deque<NodeConnect> coonect_node;  // �ڑ����Ă���m�[�h
	VF3 pos;           // ���W
	uint16_t cost;     // �T���ɗv�����R�X�g
	bool is_finded;    // �T���ς݂��ǂ���
	NodeData* to_goal; // �S�[���ւ̍ŒZ���[�g�ɂȂ���m�[�h
};

struct Object;

struct Node
{
	using VF3 = DirectX::XMFLOAT3;

	Node() = default;
	~Node() noexcept = default;

	Node(const Node& _rt) = delete;
	Node(Node&& _rt) noexcept
	{
		data.swap(_rt.data);
	}

	auto& operator=(const Node& _rt)
	{
		data = _rt.data;

		return (*this);
	}
	auto& operator=(Node&& _rt) noexcept
	{
		data.swap(_rt.data);

		return (*this);
	}

	// �p�X���̐ݒ�
	void PathSetting(Object& objects);
	// �p�X�̔����inullptr�̏ꍇ�͑܏��H��ԁj
	[[nodiscard]] std::deque<NodeData> PathFindingDijkstra(
		const VF3& base_pos, const VF3& target_pos, int64_t* processing_time = nullptr);
	// �p�X�̔���
	[[nodiscard]] NodeData& PathFindingAstar(
		const VF3& base_pos, const VF3& target_pos, int64_t* processing_time = nullptr);
	// �m�[�h�̐ڑ�
	[[nodiscard]] bool ConnectNode(NodeData* n1, NodeData* n2)
	{
		auto& connect_node{ n1->coonect_node };

		for (auto& node : connect_node)
		{
			//�o�^�ς݂łȂ���
			if (node.next_node == n2)	return false;
		}

		constexpr uint16_t Cost{ 1u };  // �������ɃR�X�g�͂P

		//�ڑ�
		n1->coonect_node.emplace_back(n2, Cost);
		n2->coonect_node.emplace_back(n1, Cost);

		return true;
	}

	std::deque<NodeData> data;
};
