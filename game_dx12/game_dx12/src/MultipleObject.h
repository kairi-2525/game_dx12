#pragma once

#include "Obj3d.h"

//------------------------------------------------------------------------------------------------------
// �n��
//------------------------------------------------------------------------------------------------------

class Plane final
	: public Obj3D
{
public:
	Plane();
	Plane(const uint32_t hp);
	~Plane() noexcept;

	Plane(const Plane& _rt)
	{
		Obj3D::operator=(_rt);

		hp = _rt.hp;
		pl_stand = _rt.pl_stand;
		is_dead = _rt.is_dead;
		drop_scale = _rt.drop_scale;
		is_drop = _rt.is_drop;
	}
	auto& operator=(const Plane& _rt)
	{
		Obj3D::operator=(_rt);

		hp = _rt.hp;
		pl_stand = _rt.pl_stand;
		is_dead = _rt.is_dead;
		drop_scale = _rt.drop_scale;
		is_drop = _rt.is_drop;

		return (*this);
	}

	Plane(Plane&& _rt) noexcept
		: Obj3D(std::move(_rt)), hp(_rt.hp), pl_stand(_rt.pl_stand), is_dead(_rt.is_dead), drop_scale(_rt.drop_scale),
		is_drop(_rt.is_drop)
	{}
	auto& operator=(Plane&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));

			hp = _rt.hp;
			pl_stand = _rt.pl_stand;
			is_dead = _rt.is_dead;
			drop_scale = _rt.drop_scale;
			is_drop = _rt.is_drop;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 1u };
	static constexpr float DeathDropColorScale{ 0.8f };	//���̐F��  80%�@�ŏo��
	static constexpr float DeathDropTime{ 1.5f };		//0.5�b�ŏ�����
	static constexpr float DeathDropLength{ -5.f };	//DeathDropTime�b�� y �� -0.5 ��������
private:
	static constexpr size_t HpMax{ 2u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Plane& data)
	{
		using std::endl;

		ofs << data.pos << F_OUT(data.hp) << endl << endl;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Plane& data)
	{
		ifs >> data.pos >> data.hp;

		data.CheckHP();

		return ifs;
	}
private:
	void StartDrop();
	void DropUpdate(float elapsed_time);
public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void InversionProc() noexcept  // ���]����
	{
		// �O�̈�
		pl_stand = false;

		if (hp == 0u)
		{
			hp = HpMax;
			is_dead = false;
			drop_scale = 1.f;
			is_drop = false;
		}
		else if (hp == HpMax)
		{
			hp = 0u;
			is_dead = true;
			drop_scale = 1.f;
		}
	}
	void CheckHP() { if (hp == 0u) is_dead = true; }
public:
	static void SetPlPosition(const VF3& pos) noexcept { pl_pos = pos; }
	uint16_t GetHP() const noexcept { return hp; }
	bool GetIsDead() const noexcept { return is_dead; }
	void InitHP() noexcept { pl_stand = false; }


	// �߂�l 0�@�������Ă��Ȃ��B 1 ���Ɨ����؂����B�i��������ԁj
	float GetDropScale() const noexcept { return is_dead ? std::min<float>(1.f, drop_scale) : 0.f; }
	// Drop���̍��W��Obj3D::pos����͎擾�ł��Ȃ��B
	DirectX::XMFLOAT3 GetDropPos() const noexcept { return { pos.x, pos.y + DeathDropLength * GetDropScale(), pos.z }; }
private:
	uint16_t hp;
	bool pl_stand;
	bool is_dead;
	bool is_drop;
	float drop_scale;
	static inline VF3 pl_pos;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> sand_model;
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> sand_broken_model;
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> snow_model;
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> snow_broken_model;
};

//------------------------------------------------------------------------------------------------------
// ��
//------------------------------------------------------------------------------------------------------

class Wall final
	: public Obj3D
{
public:
	Wall();
	~Wall() noexcept = default;

	Wall(const Wall& _rt)
	{
		Obj3D::operator=(_rt);
	}
	auto& operator=(const Wall& _rt)
	{
		Obj3D::operator=(_rt);

		return (*this);
	}

	Wall(Wall&& _rt) noexcept
		: Obj3D(std::move(_rt))
	{}
	auto& operator=(Wall&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 2u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Wall& data)
	{
		ofs << data.pos << std::endl;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Wall& data)
	{
		ifs >> data.pos;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> snow_model;
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> sand_model;
};

//------------------------------------------------------------------------------------------------------
// ���[�v�z�[��
//------------------------------------------------------------------------------------------------------

class WarpHole final
	: public Obj3D
{
public:
	WarpHole(const bool is_back_world);
	WarpHole();
	~WarpHole() noexcept = default;

	WarpHole(const WarpHole& _rt)
	{
		Obj3D::operator=(_rt);

		is_back_world = _rt.is_back_world;
	}
	auto& operator=(const WarpHole& _rt)
	{
		Obj3D::operator=(_rt);

		is_back_world = _rt.is_back_world;

		return (*this);
	}

	WarpHole(WarpHole&& _rt) noexcept
		: Obj3D(std::move(_rt)), is_back_world(_rt.is_back_world)
	{}
	auto& operator=(WarpHole&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));

			is_back_world = _rt.is_back_world;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 3u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const WarpHole& data)
	{
		ofs << data.pos << F_OUT(data.is_back_world) << std::endl;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, WarpHole& data)
	{
		ifs >> data.pos >> data.is_back_world;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;

public:
	bool GetBackWorld() const noexcept { return is_back_world; }

private:
	bool is_back_world;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> warp_sand;
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> warp_snow;
};

//------------------------------------------------------------------------------------------------------
// ��
//------------------------------------------------------------------------------------------------------

class Key final
	: public Obj3D
{
public:
	Key(const bool is_back_world);
	Key();
	~Key() noexcept = default;

	Key(const Key& _rt)
	{
		Obj3D::operator=(_rt);

		is_back_world = _rt.is_back_world;
	}
	auto& operator=(const Key& _rt)
	{
		Obj3D::operator=(_rt);

		is_back_world = _rt.is_back_world;

		return (*this);
	}

	Key(Key&& _rt) noexcept
		: Obj3D(std::move(_rt)), is_back_world(_rt.is_back_world)
	{}
	auto& operator=(Key&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));

			is_back_world = _rt.is_back_world;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 4u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Key& data)
	{
		ofs << data.pos << F_OUT(data.is_back_world) << std::endl;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Key& data)
	{
		ifs >> data.pos >> data.is_back_world;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;

public:
	bool GetBackWorld() const noexcept { return is_back_world; }

private:
	bool is_back_world;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> model;
};

//------------------------------------------------------------------------------------------------------
// �h�A
//------------------------------------------------------------------------------------------------------

class Door final
	: public Obj3D
{
public:
	Door(const bool is_back_world);
	Door();
	~Door() noexcept;

	Door(const Door& _rt)
	{
		Obj3D::operator=(_rt);

		is_open = _rt.is_open;
		is_back_world = _rt.is_back_world;
	}
	auto& operator=(const Door& _rt)
	{
		Obj3D::operator=(_rt);

		is_open = _rt.is_open;
		is_back_world = _rt.is_back_world;

		return (*this);
	}

	Door(Door&& _rt) noexcept
		: Obj3D(std::move(_rt)), is_open(_rt.is_open), is_back_world(_rt.is_back_world)
	{}
	auto& operator=(Door&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			Obj3D::operator=(move(_rt));
			is_open = _rt.is_open;
			is_back_world = _rt.is_back_world;
		}

		return (*this);
	}

public:
	static constexpr size_t IndexNumber{ 5u };

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Door& data)
	{
		ofs << data.pos << data.angle << F_OUT(data.is_back_world) << std::endl << std::endl;

		return ofs;
	}
	friend std::ifstream& operator>>(std::ifstream& ifs, Door& data)
	{
		ifs >> data.pos >> data.angle >> data.is_back_world;

		return ifs;
	}

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) override;
	bool GetBackWorld() const noexcept { return is_back_world; }
	void OpenDoor() noexcept { is_open = true; }

public:
	bool is_open;

private:
	bool is_back_world;

public:
	static inline std::unique_ptr<KDL::DX12::Mesh_FBX> model;
};
