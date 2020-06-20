#pragma once

#include <unordered_map>
#include <deque>

#include "KDL_Dx12/Primitive.h"
#include "Singleton.h"
#include "KDL_Tool/Camera.h"

class Particle
{
private:
	using VF2 = DirectX::XMFLOAT2;
	using VF3 = DirectX::XMFLOAT3;
	using VF4 = DirectX::XMFLOAT4;
	using VF4X4 = DirectX::XMFLOAT4X4;

public:
	static constexpr float GravityAcceleration{ 9.80665f }; // èdóÕâ¡ë¨ìx
	static constexpr VF4 LightDir{ 0.f, 1.f, 0.f, 1.f };

public:
	Particle(const VF3& pos, const VF3& scale, const VF3& angle, const VF4& color);
	//Particle(const VF3& pos, const VF3& scale, const VF3& angle, const VF4& color);
	~Particle() = default;
	Particle(const Particle&) = delete;
	auto& operator=(const Particle&) = delete;
	Particle(Particle&& _rt) noexcept
	{
		using std::move;

		pos = move(_rt.pos);
		scale = move(_rt.scale);
		angle = move(_rt.angle);
		color = move(_rt.color);
		timer = (_rt.timer);
	}
	auto& operator=(Particle&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			pos = move(_rt.pos);
			scale = move(_rt.scale);
			angle = move(_rt.angle);
			color = move(_rt.color);
			timer = (_rt.timer);
		}
		return (*this);
	}

public:
	enum class Type : UINT8
	{
		Circle,
		Square,
		Star,
		Triangle,
		Max,
	};

	operator bool() const noexcept { return exist; }

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app,
		std::unordered_map<Particle::Type, KDL::DX12::Geometric_Board>& board);

	auto GetType() const noexcept { return type; }

public:
	VF3 pos, scale, angle;
private:
	VF3 speed, move_vec;
	VF4 color;
	float timer, gravity_scale;
	bool exist, is_gravity;
	Type type;
	VF4X4 world_mat;
};

class ParticleManager final
{
private:
	using VF2 = DirectX::XMFLOAT2;
	using VF3 = DirectX::XMFLOAT3;
	using VF4 = DirectX::XMFLOAT4;

public:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	auto& operator=(const ParticleManager&) = delete;
	ParticleManager(ParticleManager&&) = delete;
	auto& operator=(ParticleManager&&) = delete;

public:
	bool Init(KDL::DX12::App* p_app, const class KDL::TOOL::Camera* camela);
	bool Uninit();
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Set();
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app);

	const auto& GetParticleArr() const noexcept { return (particles); }
	size_t GetParticleSize() const noexcept { return (particles.size()); }

private:
	std::unordered_map<Particle::Type, KDL::DX12::Geometric_Board> board;
	std::deque<Particle> particles;
	const class KDL::TOOL::Camera* camela{ nullptr };
};

#define pParticleManager	(Singleton<ParticleManager>::GetInstancePtr())