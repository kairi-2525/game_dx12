#include "SceneSelect.h"
#include "GameScene.h"
#include "LoadAllFileName.h"

void SceneSelect::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	// ƒtƒ@ƒCƒ‹‚ğ“Ç‚İ‚İ
	file_names = GetAllFileName(SceneGame::SaveFileDir);
	auto fonts{ GetAllFileName("data\\images\\Select") };

	// ‰æ‘œ“Ç‚İ‚İ
	for (auto& name : file_names)
	{
		sprites.emplace(name.string(), std::make_unique<KDL::DX12::Sprite_Image>(p_app, name, 10u));
	}
}

void SceneSelect::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	select_num = 0;
}

void SceneSelect::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using Keys = KDL::KEY_INPUTS;

	const auto* input{ p_window->GetInput() };

	// ˆÚ“®
	if (file_names.size() - 1 >= select_num && input->IsTrgKey(Keys::Down))
	{
		select_num++;
	}
	else if (select_num > 0 && input->IsTrgKey(Keys::Up))
	{
		select_num--;
	}


}

void SceneSelect::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
}

void SceneSelect::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
}
