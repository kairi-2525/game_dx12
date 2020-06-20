#pragma once

#include <unordered_map>

#include "KDL_Dx12/Primitive.h"
#include "Singleton.h"

class Particle
{
private:
	using VF2 = DirectX::XMFLOAT2;
	using VF3 = DirectX::XMFLOAT3;
	using VF4 = DirectX::XMFLOAT4;

public:
	Particle();
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

private:
	VF3 pos, scale, angle;
	VF4 color;
	float timer;
};


class ParticleManager final
{
private:
	using VF2 = DirectX::XMFLOAT2;
	using VF3 = DirectX::XMFLOAT3;

public:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	auto& operator=(const ParticleManager&) = delete;
	ParticleManager(ParticleManager&&) = delete;
	auto& operator=(ParticleManager&&) = delete;

public:
	bool Init(KDL::DX12::App* p_app);
	bool Uninit();
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Set();

private:
	std::unordered_map<Particle::Type, KDL::DX12::Geometric_Board> board;
};

#define pParticleManager	(Singleton<ParticleManager>::GetInstancePtr())