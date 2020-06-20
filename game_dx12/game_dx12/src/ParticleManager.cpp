#include "ParticleManager.h"
#include "LoadAllFileName.h"
#include "AlgorithmHelper.h"
#include "XMFLOAT_Hlper.h"
#include "GameScene.h"

namespace
{
	namespace Exec = std::execution;
	using Path = std::filesystem::path;
	constexpr bool UseMultiThread{ true };
}

Particle::Particle(const VF3& pos, const VF3& scale, const VF3& angle, const VF4& color)
	: exist(true), timer(0.f), world_mat()
{}

void Particle::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	pos += speed;

	// �s��v�Z
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

void Particle::Draw(KDL::Window* p_window, KDL::DX12::App* p_app,
	std::unordered_map<Particle::Type, KDL::DX12::Geometric_Board>& board)
{
	using GS = SceneGame;

	DirectX::XMFLOAT4X4 wvp;
	auto&& W{ DirectX::XMLoadFloat4x4(&world_mat) };
	GS::camera->CreateUpdateWorldViewProjection(&wvp, W);

	board.at(type).AddCommand(p_app->GetCommandList(), p_app, wvp, world_mat, LightDir, color);
}

//------------------------------------------------------------------------------------------------------------------------

bool ParticleManager::Init(KDL::DX12::App* p_app, const KDL::TOOL::Camera* camela)
{
	using std::make_unique;

	constexpr const char* const RemoveExtension{ ".png" };

	this->camela = camela;

	auto&& file_paths{ GetAllFileName("./data/images/Particles") };

	// png�ȊO�̃t�@�C���`�����폜
	Erase_Remove_If(file_paths, [&](const Path& path) { return(path.extension() != RemoveExtension); }, Exec::par);

	assert(static_cast<size_t>(Particle::Type::Max) == file_paths.size() &&
		"�摜�t�@�C���ƃW�I���g���{�[�h�̌��̕s��v");

	bool is_success{ true };

	// �e�p�[�e�B�N���p�̉摜��ǂݍ���
	for (size_t i = 0, length = file_paths.size(); i < length; i++)
	{
		try
		{
			is_success &= board.try_emplace(static_cast<Particle::Type>(i), p_app, file_paths.at(i)).second;
		}
		catch ([[maybe_unused]] const std::exception& exc)
		{
			return false;
		}
	}

	assert(is_success && "board�̍\�z���s");

	return true;
}

bool ParticleManager::Uninit()
{
	try
	{
		board.clear();
		particles.clear();
		camela = nullptr;
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

void ParticleManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// �}���`�X���b�h���g���ꍇ
	if constexpr (UseMultiThread)
	{
		// �X�V
		For_Each(particles, [&](Particle& particle) { particle.Update(p_window, p_app); }, Exec::par);

		// ���݃t���O�������ȃp�[�e�B�N�����폜
		Erase_Remove(particles, false, Exec::par);
	}
	else
	{
		// �X�V
		NormalAlgorithm::For_Each(particles, [&](Particle& particle) { particle.Update(p_window, p_app); });

		// ���݃t���O�������ȃp�[�e�B�N�����폜
		NormalAlgorithm::Erase_Remove(particles, false);
	}
}

void ParticleManager::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	for (auto& particle : particles)
	{
		particle.Draw(p_window, p_app, board);
	}
}