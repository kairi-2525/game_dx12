#pragma once

#include "scene.h"
#include <KDL_Math.h>
#include <KDL_Tool\Camera.h>

struct Model_Base
{
	KDL::FLOAT3 position;
	KDL::FLOAT3 scale;
	KDL::FLOAT3 rotate;
	template<class _T>
	HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir, _T* p_model,
		KDL::COLOR4F color = { 1.f, 1.f, 1.f, 1.f })
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
		HRESULT hr = p_model->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, { light_dir.x, light_dir.y, light_dir.z, 0.f }, color.DCast<DirectX::XMFLOAT4, float>());
		return hr;
	}
};
template<class _T>
struct Model
	: public Model_Base
{
	std::unique_ptr<_T> model;
	HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir,
		KDL::COLOR4F color = { 1.f, 1.f, 1.f, 1.f })
	{
		assert(model && "model nullptr");
		return Model_Base::Draw<_T>(p_app, p_camera, light_dir, model.get(), color);
	}
};
template<class _T, class _M = Model_Base>
struct MenyModels
{
	std::shared_ptr<_T> model;
	std::vector<_M> data;
	HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir,
		KDL::COLOR4F color = { 1.f, 1.f, 1.f, 1.f })
	{
		HRESULT hr = S_OK;
		for (auto& it : data)
		{
			hr = it.Model_Base::Draw<_T>(p_app, p_camera, light_dir, model.get(), color);
			if (FAILED(hr)) return hr;
		}
		return hr;
	}
	template<class _TM>
	HRESULT Draw(KDL::DX12::App* p_app, KDL::TOOL::Camera* p_camera, const KDL::FLOAT3& light_dir, _TM* p_model,
		KDL::COLOR4F color = { 1.f, 1.f, 1.f, 1.f })
	{
		HRESULT hr = S_OK;
		for (auto& it : data)
		{
			hr = it.Model_Base::Draw<_TM>(p_app, p_camera, light_dir, p_model, color);
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