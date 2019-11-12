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
	RootBase(const bool executable, const uint64_t priority_number)
		: executable(executable), priority_number(priority_number), is_mode_end(false), node_data(nullptr),
		halfway_path(0.f, 0.f, 0.f), target_pos(0.f, 0.f, 0.f)
	{}
	virtual ~RootBase() noexcept = default;

	RootBase(const RootBase&) = delete;
	auto& operator=(const RootBase&) = delete;

	RootBase(RootBase&& _rt) noexcept
		: priority_number(_rt.priority_number), executable(_rt.executable), is_mode_end(_rt.is_mode_end),
		node_data(nullptr), halfway_path(_rt.halfway_path), target_pos(std::move(_rt.target_pos))
	{ }
	auto& operator=(RootBase&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			priority_number = _rt.priority_number;
			executable = _rt.executable;
			is_mode_end = _rt.is_mode_end;
			node_data = _rt.node_data;
			halfway_path = _rt.halfway_path;
			target_pos = move(_rt.target_pos);
		}

		return (*this);
	}

public:
	static constexpr uint64_t SubStructNumber{ 10llu };

public:
	uint64_t GetPriorityNumber() const noexcept{ return priority_number; };
	bool GetExecutable() const noexcept{ return executable; };
	bool GetModeEnd() const noexcept{ return is_mode_end; };
	void SetExecutable(const bool executable) noexcept { this->executable = executable; }

public:
	virtual void Update(VF3& enemy_pos, Node* node = nullptr) = 0;

protected:
	uint64_t priority_number; // 優先順位
	bool executable;          // 実行可能か
	bool is_mode_end;         // 実行モードの終了かどうか
	NodeData* node_data;	  // ノードデータ
	VF3 halfway_path;         // パス検索した際の途中のパス
public:
	VF3 target_pos;           // 目標地点
};
