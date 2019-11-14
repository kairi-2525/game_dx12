#pragma once

#include <variant>
#include <numeric>
#include <array>
#include <optional>
#include <DirectXMath.h>
#include <fstream>

#include "NodeData.h"

struct Node;

class RootBase
{
protected:
	using VF3 = DirectX::XMFLOAT3;
	template<class _Ty>
	using Optional = std::optional<_Ty>;
	template<class _Ty, size_t Size>
	using Array = std::array<_Ty, Size>;
	template<class... _Ty>
	using Variant = std::variant<_Ty...>;

public:
	RootBase(const bool executable, const uint64_t priority_number, const uint16_t kind_node)
		: executable(executable), priority_number(priority_number), is_mode_end(false),
		halfway_path(0.f, 0.f, 0.f), target_pos(0.f, 0.f, 0.f), kind_node(kind_node),
		node_array_count(0u)
	{}
	virtual ~RootBase() noexcept = default;

	RootBase(const RootBase&) = delete;
	auto& operator=(const RootBase&) = delete;

	RootBase(RootBase&& _rt) noexcept
		: priority_number(_rt.priority_number), executable(_rt.executable), is_mode_end(_rt.is_mode_end),
		node_data(std::move(_rt.node_data)), halfway_path(_rt.halfway_path), target_pos(std::move(_rt.target_pos)),
		kind_node(_rt.kind_node), node_array_count(_rt.node_array_count)
	{ }
	auto& operator=(RootBase&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			priority_number = _rt.priority_number;
			executable = _rt.executable;
			is_mode_end = _rt.is_mode_end;
			node_data.swap(_rt.node_data);
			halfway_path = _rt.halfway_path;
			target_pos = move(_rt.target_pos);
			kind_node = _rt.kind_node;
			node_array_count = _rt.node_array_count;
		}

		return (*this);
	}

public:
	static constexpr uint64_t SubStructNumber{ 10llu };

public:
	uint64_t GetPriorityNumber() const noexcept { return priority_number; };
	bool GetExecutable() const noexcept { return executable; };
	bool GetModeEnd() const noexcept { return is_mode_end; };
	uint16_t GetKindNode() const noexcept { return kind_node; }
	void SetExecutable(const bool executable) noexcept { this->executable = executable; }

public:
	virtual void Update(VF3& enemy_pos, float elapsed_time, Node* node = nullptr) = 0;

protected:
	uint64_t priority_number; // �D�揇��
	bool executable;          // ���s�\��
	bool is_mode_end;         // ���s���[�h�̏I�����ǂ���
	std::deque<NodeData> node_data; // �m�[�h�f�[�^
	size_t node_array_count;
	VF3 halfway_path;         // �p�X���������ۂ̓r���̃p�X
	uint16_t kind_node;		  // �ǂ̃m�[�h�����ʂ���ׂ̐���
public:
	VF3 target_pos;           // �ڕW�n�_
};
