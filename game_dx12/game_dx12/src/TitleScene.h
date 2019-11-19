#pragma once

#include "scene.h"
#include <unordered_map>
#include <string>

class SceneTitle : public SceneBase
{
private:
	struct Model_Base
	{
		KDL::FLOAT3 position;
		KDL::FLOAT3 scale;
		KDL::FLOAT3 rotate;
		template<class _T>
		HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir, _T* p_model)
		{
			assert(p_model && "model nullptr");
			assert(p_camera && "camera nullptr");
			DirectX::XMMATRIX W;
			{
				DirectX::XMMATRIX S, R, T;
				S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
				R = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);
				T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
				W = S * R * T;
			}
			DirectX::XMFLOAT4X4 wvp, w;
			DirectX::XMStoreFloat4x4(&w, W);
			p_camera->CreateUpdateWorldViewProjection(&wvp, W);
			HRESULT hr = p_model->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, { light_dir.x, light_dir.y, light_dir.z, 0.f }, { 1.f, 1.f, 1.f, 1.f });
			return hr;
		}
	};
	template<class _T>
	struct Model
		: public Model_Base
	{
		std::unique_ptr<_T> model;
		HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir)
		{
			assert(model && "model nullptr");
			return Model_Base::Draw<_T>(p_app, p_camera, light_dir, model.get());
		}
	};
	template<class _T, class _M = Model_Base>
	struct MenyModels
	{
		std::unique_ptr<_T> model;
		std::vector<_M> data;
		HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir)
		{
			HRESULT hr = S_OK;
			for (auto& it : data)
			{
				hr = it.Model_Base::Draw<_T>(p_app, p_camera, light_dir, model.get());
				if (FAILED(hr)) return hr;
			}
			return hr;
		}
		template<class _TM>
		HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir, _TM* p_model)
		{
			HRESULT hr = S_OK;
			for (auto& it : data)
			{
				hr = it.Model_Base::Draw<_TM>(p_app, p_camera, light_dir, p_model);
				if (FAILED(hr)) return hr;
			}
			return hr;
		}
	};
	struct Ground : public Model_Base
	{
		bool changed = false;
	};
	enum class GROUND_TYPE : int
	{
		Air,
		Sand,
		SandBroken,
		Snow,
		SnowBroken,
		WarpHole,
		SandWall,
		SnowWall,
	};
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
	std::unordered_map<GROUND_TYPE, MenyModels<KDL::DX12::Geometric_Board, Ground>> grounds;
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
	int sound_bgm;
	int sound_bgm_p;
	int sound_se_break;
	int sound_se_crack;
	int sound_se_warp;
	int se_decision;
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