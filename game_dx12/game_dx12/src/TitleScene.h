#pragma once

//#define REFACTED_TITLE

#include "TitleModel.h"

#include <unordered_map>
#include <string>

#ifdef REFACTED_TITLE
#include <optional>
#include <atomic>

#include "ObjectManager.h"
#endif

#ifndef REFACTED_TITLE
class SceneTitle : public SceneBase
{
private:
	static inline const std::unordered_map<GROUND_TYPE, std::string> grounds_list =
	{
		{GROUND_TYPE::Air, "Air"},
		{GROUND_TYPE::Sand, "SandPlane"},
		{GROUND_TYPE::SandBroken, "SandBrokenPlane"},
		{GROUND_TYPE::Snow, "Snow"},
		{GROUND_TYPE::SnowBroken, "SnowBroken"},
		{GROUND_TYPE::WarpHole, "WarpSuna"},
		{GROUND_TYPE::SandWall, "SandWall"},
		{GROUND_TYPE::SnowWall, "SnowWall"}
	};
	static inline const UINT BLOCK_NUM = 20;
	static inline const float BLOCK_SIZE = 1.0f;
	static inline float WARP_HOLE_SCALE = 1.f / 6;
	static inline float WALL_SCALE = 1.f / 6;
private:
	KDL::FLOAT3 light_dir;
	std::unique_ptr<KDL::TOOL::Camera> camera;
	Model<KDL::DX12::Mesh_FBX> player;
	std::unordered_map<GROUND_TYPE, MenyModels<KDL::DX12::Mesh_FBX, Ground>> grounds;
	std::unordered_map<GROUND_TYPE, MenyModels<KDL::DX12::Mesh_FBX, Ground>> drop_grounds;
	MenyModels<KDL::DX12::Mesh_FBX, Ground> warp_hole;
	MenyModels<KDL::DX12::Mesh_FBX> snows;
	MenyModels<KDL::DX12::Mesh_FBX> sands;
	std::unique_ptr<KDL::DX12::Mesh_FBX> warp_hole_off;
	std::unique_ptr<KDL::DX12::Sprite_Image> snow_bg;
	std::unique_ptr<KDL::DX12::Sprite_Image> sand_bg;
	Model<KDL::DX12::Mesh_FBX> title_parallel;
	Model<KDL::DX12::Mesh_FBX> title_labyrinth;
	std::unique_ptr<KDL::DX12::Sprite_Image> font_sprite;
	bool snow;
	struct Sounds
	{
		int bgm, bgm_p;
		int se_break;
		int se_crack;
		int se_warp;
		int se_decision;
	} sounds;
public:
	void Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);

	void BreakGrounds(KDL::Window* p_window);
	void RemoveGrounds(KDL::Window* p_window);
	void FlipGrounds(KDL::Window* p_window);
};

#else

class SceneTitle : public SceneBase
{
private:
	std::optional<ObjectManager> object_manager;
	std::unique_ptr<KDL::TOOL::Camera> camera;
	struct Sounds
	{
		int bgm, bgm_p;
		int se_break;
		int se_crack;
		int se_warp;
		int se_decision;
	} sounds;
public:
	~SceneTitle();
	void Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
};

#endif