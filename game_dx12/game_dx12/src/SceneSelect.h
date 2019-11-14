#pragma once
#include "scene.h"
#include "KDL_Dx12/Sprite.h"

#include <filesystem>
#include <vector>
#include <unordered_map>

class SceneSelect :
	public SceneBase
{
private:
	using Path = std::filesystem::path;
	using VF4 = DirectX::XMFLOAT4;
	using VF3 = DirectX::XMFLOAT3;
	using VF2 = DirectX::XMFLOAT2;

public:
	SceneSelect()
		: select_num(0)
	{}
	~SceneSelect() noexcept = default;
	SceneSelect(const SceneSelect&) = delete;
	SceneSelect(SceneSelect&&) noexcept = delete;
	auto& operator=(const SceneSelect&) = delete;
	auto& operator=(SceneSelect&&) noexcept = delete;

public:
	void Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);

private:
	std::vector<Path> file_names;
	std::unordered_map<std::string, std::unique_ptr<KDL::DX12::Sprite_Image>> sprites;
	std::unique_ptr<KDL::DX12::Sprite_Image> bg_sprite;
	int select_num;
};

