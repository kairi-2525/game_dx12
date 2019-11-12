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
