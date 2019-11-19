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
	SceneSelect() = default;
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
	static bool GetIsTutrialMode() noexcept { return is_tutrial_mode; }

private:
	std::vector<Path> file_names;
	std::vector<std::unique_ptr<KDL::DX12::Sprite_Image>> bg_sprite;
	std::unique_ptr<KDL::DX12::Sprite_Image> arrow_sprite;
	std::unique_ptr<KDL::DX12::Sprite_Image> font_sprite;

	static inline int select_num;
	double bg_timer;
	double arrow_timer;
	int bgm_handle;
	int bgm_handle_p;
	int se_select;
	int se_decision;

	static inline bool is_tutrial_mode;
};

