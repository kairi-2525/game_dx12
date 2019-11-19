#pragma once

#include <deque>
#include <memory>

#include "Obj3d.h"
#include "KDL_Dx12/Sprite.h"

//------------------------------------------------------------------------------------------------------
// 自機
//------------------------------------------------------------------------------------------------------

class Plane;
class WarpHole;
class Start;
class Goal;
class Door;
class Key;

class Player final
	: public Obj3D
{
private:
	template<class _Ty>
	using Deque = std::deque<_Ty>;
	template<class _Ty>
	using SharedPtr = std::shared_ptr<_Ty>;

public:
	Player(const Deque<Plane>& planes, const Deque<WarpHole>& warphole, const SharedPtr<Start>& start,
		const SharedPtr<Goal>& goal, const Deque<Door>& doors, const Deque<Key>& keys);
	~Player() noexcept = default;

	Player(const Player&) = delete;
	auto& operator=(const Player&) = delete;

	Player(Player&& _rt) noexcept = delete;
	auto& operator=(Player&& _rt) noexcept = delete;

public:
	static constexpr size_t IndexNumber{ 0u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Player& data)
	{
		ofs << data.pos;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Player& data)
	{
		ifs >> data.pos;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;

private:
	bool isMoveObjectCheck();
	void Move(KDL::Window* p_window, KDL::DX12::App* p_app);

private:
	const Deque<Plane>& planes;
	const Deque<WarpHole>& warphole;
	const SharedPtr<Start>& start;
	const SharedPtr<Goal>& goal;
	const Deque<Door>& doors;
	const Deque<Key>& keys;

public:
	uint16_t key_num;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> model;
	static inline bool is_move{ false };
};

//------------------------------------------------------------------------------------------------------
// スタート
//------------------------------------------------------------------------------------------------------

class Start final
	: public Obj3D
{
public:
	Start();
	~Start() noexcept = default;

	Start(const Start&) = delete;
	auto& operator=(const Start&) = delete;

	Start(Start&&) = delete;
	auto& operator=(Start&&) = delete;

public:
	static constexpr size_t IndexNumber{ 1u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Start& data)
	{
		ofs << data.pos;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Start& data)
	{
		ifs >> data.pos;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> model;
};

//------------------------------------------------------------------------------------------------------
// ゴール
//------------------------------------------------------------------------------------------------------

class Goal final
	: public Obj3D
{
public:
	Goal(const bool is_back_world);
	Goal();
	~Goal() noexcept = default;

	Goal(const Goal&) = delete;
	auto& operator=(const Goal&) = delete;

	Goal(Goal&&) = delete;
	auto& operator=(Goal&&) = delete;

public:
	static constexpr size_t IndexNumber{ 2u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Goal& data)
	{
		ofs << data.pos << F_OUT(data.is_back_world);

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Goal& data)
	{
		ifs >> data.pos >> data.is_back_world;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	bool GetIsBackWorld() const noexcept { return is_back_world; }

private:
	bool is_back_world;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> model;
	static inline std::unique_ptr<KDL::DX12::Geometric_Board> board;
};
