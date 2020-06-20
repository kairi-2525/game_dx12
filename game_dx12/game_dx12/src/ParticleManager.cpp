#include "ParticleManager.h"
#include "LoadAllFileName.h"
#include "AlgorithmHelper.h"

namespace
{
	namespace Exec = std::execution;
	using Path = std::filesystem::path;
}

Particle::Particle()
{
}

bool ParticleManager::Init(KDL::DX12::App* p_app)
{
	using std::make_unique;

	constexpr const char* const RemoveExtension{ ".png" };

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
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

void ParticleManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
}
