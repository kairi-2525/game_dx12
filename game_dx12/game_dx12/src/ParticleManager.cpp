#include "ParticleManager.h"
#include "LoadAllFileName.h"
#include "AlgorithmHelper.h"
#include "XMFLOAT_Hlper.h"
#include "GameScene.h"

namespace
{
	namespace Exec = std::execution;
	using Path = std::filesystem::path;

	constexpr bool UseMultiThread{ true }; // マルチスレッドを使うかどうか
}

// 重力無し版
Particle::Particle(const VF3& pos, const float max_timer, const Type type, const float speed, const VF3& move_vec,
	const VF3& scale, const VF3& angle, const VF4& color)
	: pos(pos), max_timer(max_timer), type(type), speed(speed), move_vec(move_vec), scale(scale), angle(angle),
	color(color)
{
	exist = true;
	timer = 0.f;
	world_mat = {};
	is_gravity = false;
	gravity_scale = 0.f;
	is_angular_velocity = false;
	angular_velocity = 0.f;
}

void Particle::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const float dt{ p_window->GetElapsedTime() };

	if (is_gravity) move_vec.y -= (GravityAcceleration * gravity_scale * dt);

	//todo: 今はとりあえずやめておく
	//if (is_angular_velocity)
	//{
	//	assert(random_maker);

	//	angle.x += random_maker->GetRnd<float>() * dt;
	//	angle.y += random_maker->GetRnd<float>() * dt;
	//	angle.z += random_maker->GetRnd<float>() * dt;
	//}

	timer += dt;
	//move_vec = VectorNormalize(move_vec); // 正規化
	pos += (move_vec * speed * dt);

	color.w = 1.f - (timer / max_timer); // 徐々に透明に

	if (timer >= max_timer)	exist = false; // 最大時間を超えたら削除

	// 行列計算
	{
		DirectX::XMMATRIX W;
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;

		DirectX::XMStoreFloat4x4(&world_mat, W);
	}
}

void Particle::Draw(KDL::TOOL::Camera* camera, KDL::Window* p_window, KDL::DX12::App* p_app,
	std::unordered_map<Particle::Type, KDL::DX12::Geometric_Board>& board)
{
	DirectX::XMFLOAT4X4 wvp;
	auto&& W{ DirectX::XMLoadFloat4x4(&world_mat) };
	camera->CreateUpdateWorldViewProjection(&wvp, W);

	board.at(type).AddCommand(p_app->GetCommandList(), p_app, wvp, world_mat, LightDir, color);
}

void Particle::AngularVelocityOn(const float angular_velocity)
{
	is_angular_velocity = true;

	this->angular_velocity = ::fabsf(angular_velocity);
	random_maker.emplace(+this->angular_velocity, -this->angular_velocity);
}

//------------------------------------------------------------------------------------------------------------------------

bool ParticleManager::Init(KDL::DX12::App* p_app, KDL::TOOL::Camera* camera)
{
	using std::make_unique;

	constexpr const char* const RemoveExtension{ ".png" };

	particles.clear();

	// 既に読み込んでいる
	if (!board.empty())	return true;

	this->camera = camera;

	auto&& file_paths{ GetAllFileName("./data/images/Particles") };

	// png以外のファイル形式を削除
	Erase_Remove_If(file_paths, [&](const Path& path) { return(path.extension() != RemoveExtension); }, Exec::par);

	assert(static_cast<size_t>(Particle::Type::Max) == file_paths.size() &&
		"画像ファイルとジオメトリボードの個数の不一致");

	bool is_success{ true };

	// 各パーティクル用の画像を読み込む
	for (size_t i = 0, length = file_paths.size(); i < length; i++)
	{
		try
		{
			is_success &= board.try_emplace(static_cast<Particle::Type>(i), p_app, file_paths.at(i), 10000u).second;
		}
		catch ([[maybe_unused]] const std::exception& exc)
		{
			return false;
		}
	}

	assert(is_success && "boardの構築失敗");

	return true;
}

bool ParticleManager::Uninit()
{
	try
	{
		board.clear();
		particles.clear();
		camera = nullptr;
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

void ParticleManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// マルチスレッドを使う場合
	if constexpr (UseMultiThread)
	{
		// 更新
		For_Each(particles, [&](Particle& particle) { particle.Update(p_window, p_app); }, Exec::par);

		// 存在フラグが無効なパーティクルを削除
		Erase_Remove(particles, false, Exec::par);
	}
	else
	{
		// 更新
		NormalAlgorithm::For_Each(particles, [&](Particle& particle) { particle.Update(p_window, p_app); });

		// 存在フラグが無効なパーティクルを削除
		NormalAlgorithm::Erase_Remove(particles, false);
	}
}

void ParticleManager::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	for (auto& particle : particles)
	{
		particle.Draw(camera, p_window, p_app, board);
	}
}

const Particle& ParticleManager::Set(const VF3& pos, const float max_timer, const Particle::Type type,
	const float speed,
	const VF3& move_vec, const VF3& scale, const VF3& angle, const VF4& color)
{
	particles.emplace_back(pos, max_timer, type, speed, move_vec, scale, angle, color);

	return particles.back();
}

const Particle& ParticleManager::AngulurSet(const VF3& pos, const float max_timer, const Particle::Type type,
	const float speed,
	const VF3& move_vec, const float angular_velocity, const VF3& scale, const VF3& angle, const VF4& color)
{
	Set(pos, max_timer, type, speed, move_vec, scale, angle, color);

	particles.back().AngularVelocityOn(angular_velocity);

	return particles.back();
}

const Particle& ParticleManager::GravitySet(const VF3& pos, const float max_timer, const Particle::Type type,
	const float speed,
	const VF3& move_vec, const float gravity_scale, const VF3& scale, const VF3& angle, const VF4& color)
{
	Set(pos, max_timer, type, speed, move_vec, scale, angle, color);

	particles.back().GravityOn(gravity_scale);

	return particles.back();
}

const Particle& ParticleManager::Angulur_Gravity_Set(const VF3& pos, const float max_timer, const Particle::Type type,
	const float speed, const VF3& move_vec, const float angular_velocity, const float gravity_scale,
	const VF3& scale, const VF3& angle, const VF4& color)
{
	Set(pos, max_timer, type, speed, move_vec, scale, angle, color);

	particles.back().AngularVelocityOn(angular_velocity);
	particles.back().GravityOn(gravity_scale);

	return particles.back();
}
