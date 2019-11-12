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
	void Update() override;
	void Draw() const override;

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
		ofs << data.pos;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, WayPoint& data)
	{
		ifs >> data.pos;

		return ifs;
	}

public:
	void Update() override;
	void Draw() const override;

public:
	static void SetModelHandle(const int handle) noexcept
	{
		model_handle = handle;
	}

private:
	bool is_back;

	static inline int model_handle{ -1 };
};

//------------------------------------------------------------------------------------------------------
// 敵
//------------------------------------------------------------------------------------------------------

struct Node;
class AI_Manager;
class RootBase;
class Player;

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
	friend std::ofstream& operator<<(std::ofstream& ofs, const Enemy& data)
	{
		ofs << data.pos << std::endl << std::endl;

		if (size_t size{ data.way_points.size() }; size > 0u)
		{
			ofs << F_OUT(size);

			for (auto& wp : data.way_points)
			{
				ofs << wp;
			}
		}

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Enemy& data)
	{
		size_t wp_size{ 0u };

		ifs >> data.pos >> wp_size;

		if (wp_size != 0u)
		{
			data.way_points.resize(wp_size);

			for (auto& wp : data.way_points)
			{
				ifs >> wp;
			}
		}

		return ifs;
	}

public:
	void Update() override;
	void Draw() const override;
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
	static void SetModelHandle(const std::initializer_list<int>& handle) noexcept
	{
		assert(handle.size() == 3u && "不正なハンドル数");

		auto itr{ handle.begin() };

		model_handle = *(itr);

		itr++;

		WayPoint::SetModelHandle(*itr);

		itr++;

		Arrow::SetModelHandle(*itr);
	}
	void SetIsSelect(const bool selected) noexcept { is_select = selected; }
	auto& GetWayPoint() noexcept { return way_points; }
	static void SetNodeData(Node& set_planes) noexcept { node = &set_planes; }

private:
	Deque<WayPoint> way_points;
	Deque<Arrow> arrows;
	Deque<VF3> next_target_pos;
	bool is_select;
	std::optional<NodeData> path_result;
	std::optional<NodeData> path_goal;
	std::optional<AI_Manager> ai;

	static inline Node* node;
	static inline int model_handle{ -1 };

public:
	static inline std::shared_ptr<Player>* player{ nullptr };
};