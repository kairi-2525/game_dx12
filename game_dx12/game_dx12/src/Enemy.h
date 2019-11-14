#pragma once

#include <deque>
#include <optional>

#include "AI_Manager.h"
#include "Obj3d.h"
#include "NodeData.h"

//------------------------------------------------------------------------------------------------------
// 矢印
//------------------------------------------------------------------------------------------------------

class Arrow final
	: public Obj3D
{
public:
	Arrow(const VF3& pos);
	Arrow() = default;
	~Arrow() noexcept = default;

	Arrow(const Arrow& _rt)
	{
		Obj3D::operator=(_rt);
	}
	auto& operator=(const Arrow& _rt)
	{
		Obj3D::operator=(_rt);

		return (*this);
	}

	Arrow(Arrow&& _rt) noexcept
		: Obj3D(std::move(_rt))
	{}
	auto& operator=(Arrow&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));
		}

		return (*this);
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;

public:
	static void SetModelHandle(const int handle) noexcept
	{
		model_handle = handle;
	}

private:
	static inline int model_handle{ -1 };
};

//------------------------------------------------------------------------------------------------------
// ウェイポイント
//------------------------------------------------------------------------------------------------------

class WayPoint final
	: public Obj3D
{
public:
	WayPoint(const VF3& pos);
	WayPoint()
		: is_back(false)
	{}
	~WayPoint() noexcept = default;

	WayPoint(const WayPoint& _rt)
	{
		Obj3D::operator=(_rt);

		is_back = _rt.is_back;
	}
	auto& operator=(const WayPoint& _rt)
	{
		Obj3D::operator=(_rt);

		is_back = _rt.is_back;

		return (*this);
	}

	WayPoint(WayPoint&& _rt) noexcept
		: Obj3D(std::move(_rt)), is_back(_rt.is_back)
	{}
	auto& operator=(WayPoint&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));

			is_back = _rt.is_back;
		}

		return (*this);
	}

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const WayPoint& data)
	{
		ofs << data.pos << std::endl << std::endl;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, WayPoint& data)
	{
		ifs >> data.pos;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;

private:
	bool is_back;
};

//------------------------------------------------------------------------------------------------------
// 敵
//------------------------------------------------------------------------------------------------------

struct Node;
class AI_Manager;
class RootBase;
class Player;
class Wall;

class Enemy final
	: public Obj3D
{
private:
	template<class _Ty>
	using Deque = std::deque<_Ty>;

public:
	Enemy();
	~Enemy() noexcept = default;

	Enemy(const Enemy& _rt)
	{
		Obj3D::operator=(_rt);

		way_points = _rt.way_points;
		arrows = _rt.arrows;
		is_select = _rt.is_select;
		next_target_pos = _rt.next_target_pos;
		//ai = _rt.ai;
	}
	auto& operator=(const Enemy& _rt)
	{
		Obj3D::operator=(_rt);

		way_points = _rt.way_points;
		arrows = _rt.arrows;
		is_select = _rt.is_select;
		next_target_pos = _rt.next_target_pos;
		//ai = _rt.ai;

		return (*this);
	}

	Enemy(Enemy&& _rt) noexcept
		: Obj3D(std::move(_rt)), is_select(_rt.is_select)
	{
		using std::move;

		way_points.swap(_rt.way_points);
		arrows.swap(_rt.arrows);
		next_target_pos.swap(_rt.next_target_pos);
		//ai = move(_rt.ai);
	}
	auto& operator=(Enemy&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));

			way_points.swap(_rt.way_points);
			arrows.swap(_rt.arrows);
			next_target_pos.swap(_rt.next_target_pos);
			is_select = _rt.is_select;
			//ai = move(_rt.ai);
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 0u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Enemy& data);
	friend std::ifstream& operator>>(std::ifstream& ifs, Enemy& data);

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	// ウェイポイントの追加
	auto& AddWayPoint(const VF3& masu)
	{
		// 追加初回
		if (way_points.empty())
		{
			auto& rv{ way_points.emplace_back(masu) };

			way_points.emplace_back(pos);

			return rv;
		}
		else
		{
			way_points.pop_back();

			auto& rv{ way_points.emplace_back(masu) };

			way_points.emplace_back(pos);

			return rv;
		}
	}
	// ウェイポイントの削除
	void EraseWayPoint(const VF3& masu)
	{
		size_t count{ 0u };

		for (auto& wp : way_points)
		{
			if (wp.pos == masu)
				wp.EndExist();
			else
				count++;
		}

		// 残ったウェイポイントが一つ（自分自身）しかないので
		if (count <= 1u)
			way_points.clear();  // 全削除する
	}

public:
	void SetIsSelect(const bool selected) noexcept { is_select = selected; }
	auto& GetWayPoint() noexcept { return way_points; }
	void SetNode(Node& node) noexcept { this->node = &node; }

private:
	Deque<WayPoint> way_points;
	Deque<Arrow> arrows;
	Deque<VF3> next_target_pos;
	bool is_select;
	std::optional<AI_Manager> ai;
	VF3 before_pos;
	Node* node;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> model;
	static inline const std::deque<Wall>* walls{ nullptr };
	static inline const std::shared_ptr<Player>* player{ nullptr };
};