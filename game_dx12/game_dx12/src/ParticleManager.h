#pragma once

#include <unordered_map>
#include <deque>
#include <optional>

#include "KDL_Dx12/Primitive.h"
#include "Singleton.h"
#include "KDL_Tool/Camera.h"
#include "ColorDef.h"
#include "XMFLOAT_Math.h"
#include "Random.h"

class Particle
{
private:
	using VF2 = DirectX::XMFLOAT2;
	using VF3 = DirectX::XMFLOAT3;
	using VF4 = DirectX::XMFLOAT4;
	using VF4X4 = DirectX::XMFLOAT4X4;

public:
	static constexpr float GravityAcceleration{ 9.80665f }; // 重力加速度
	static constexpr VF4 LightDir{ 0.f, 1.f, 0.f, 1.f };
public:
	enum class Type : UINT8
	{
		Circle,
		Square,
		Star,
		Triangle,
		Max,
	};

public:
	Particle(const VF3& pos, const float max_timer, const Type type, const float speed, const VF3& move_vec,
		const VF3& scale, const VF3& angle, const VF4& color);
	~Particle() = default;
	Particle(const Particle&) = delete;
	auto& operator=(const Particle&) = delete;
	Particle(Particle&& _rt) noexcept
	{
		using std::move;

		pos = move(_rt.pos);
		scale = move(_rt.scale);
		angle = move(_rt.angle);
		move_vec = move(_rt.move_vec);
		color = move(_rt.color);
		timer = (_rt.timer);
		gravity_scale = (_rt.gravity_scale);
		speed = (_rt.speed);
		max_timer = (_rt.max_timer);
		angular_velocity = (_rt.angular_velocity);
		exist = (_rt.exist);
		is_gravity = (_rt.is_gravity);
		is_angular_velocity = (_rt.is_angular_velocity);
		type = (_rt.type);
		world_mat = move(_rt.world_mat);
		random_maker = move(_rt.random_maker);
	}
	auto& operator=(Particle&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			pos = move(_rt.pos);
			scale = move(_rt.scale);
			angle = move(_rt.angle);
			move_vec = move(_rt.move_vec);
			color = move(_rt.color);
			timer = (_rt.timer);
			gravity_scale = (_rt.gravity_scale);
			speed = (_rt.speed);
			max_timer = (_rt.max_timer);
			angular_velocity = (_rt.angular_velocity);
			exist = (_rt.exist);
			is_gravity = (_rt.is_gravity);
			is_angular_velocity = (_rt.is_angular_velocity);
			type = (_rt.type);
			random_maker = move(_rt.random_maker);
		}
		return (*this);
	}

public:
	operator bool() const noexcept { return exist; }

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(class KDL::TOOL::Camera* camera, KDL::Window* p_window, KDL::DX12::App* p_app,
		std::unordered_map<Particle::Type, KDL::DX12::Geometric_Board>& board);

	auto GetType() const noexcept { return type; }
	void GravityOn(const float gravity_scale) noexcept
	{
		is_gravity = true;
		this->gravity_scale = gravity_scale;
	}
	void AngularVelocityOn(const float angular_velocity);

public:
	VF3 pos, scale, angle;
private:
	VF3 move_vec;
	VF4 color;
	float timer, gravity_scale, speed, max_timer, angular_velocity;
	bool exist, is_gravity, is_angular_velocity;
	Type type;
	VF4X4 world_mat;
	std::optional<RndDoubleMaker> random_maker;
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
	bool Init(KDL::DX12::App* p_app, class KDL::TOOL::Camera* camela);
	bool Uninit();
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app);

	// 重力・各速度無し版
	const Particle& Set(
		const VF3& pos, const float max_timer, const Particle::Type type, const float speed, const VF3& move_vec,
		const VF3& scale = Fill3(1.f), const VF3& angle = Fill3(0.f), const VF4& color = { C_WHITE, 1.f });
	// 重力・各速度あり版
	const Particle& AngulurSet(
		const VF3& pos, const float max_timer, const Particle::Type type, const float speed, const VF3& move_vec,
		const float angular_velocity,
		const VF3& scale = Fill3(1.f), const VF3& angle = Fill3(0.f), const VF4& color = { C_WHITE, 1.f });
	// 重力あり・各速度無し版
	const Particle& GravitySet(
		const VF3& pos, const float max_timer, const Particle::Type type, const float speed, const VF3& move_vec,
		const float gravity_scale,
		const VF3& scale = Fill3(1.f), const VF3& angle = Fill3(0.f), const VF4& color = { C_WHITE, 1.f });
	// 重力無し・各速度あり版
	const Particle& Angulur_Gravity_Set(
		const VF3& pos, const float max_timer, const Particle::Type type, const float speed, const VF3& move_vec,
		const float angular_velocity, const float gravity_scale,
		const VF3& scale = Fill3(1.f), const VF3& angle = Fill3(0.f), const VF4& color = { C_WHITE, 1.f });

	const auto& GetParticleArr() const noexcept { return (particles); }
	const auto& GetParticleAt(const size_t pos) const noexcept { return (particles.at(pos)); }
	size_t GetParticleSize() const noexcept { return (particles.size()); }
	void Clear() { particles.clear(); }

private:
	std::unordered_map<Particle::Type, KDL::DX12::Geometric_Board> board;
	std::deque<Particle> particles;
	class KDL::TOOL::Camera* camera{ nullptr };
};

#define pParticleManager	(Singleton<ParticleManager>::GetInstancePtr())