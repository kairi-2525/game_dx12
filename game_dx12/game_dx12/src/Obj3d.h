#pragma once

#include <memory>

#include "XMFLOAT_Hlper.h"
#include "FileStreamHelpter.h"
#include "KDL.h"
#include "KDL_Dx12/App.h"
#include "KDL_Dx12/Mesh.h"
#include "KDL_Dx12/Primitive.h"

#define IMGUI_RED 1.f, 0.3f, 0.3f, 1.f

//------------------------------------------------------------------------------------------------------
// ÉxÅ[ÉX
//------------------------------------------------------------------------------------------------------

struct Obj3D
{
protected:
	using VF4 = DirectX::XMFLOAT4;
	using VF3 = DirectX::XMFLOAT3;
	using VF2 = DirectX::XMFLOAT2;

public:
	Obj3D() noexcept
		: pos(0.f, 0.f, 0.f), scale(1.f, 1.f, 1.f), angle(0.f, 0.f, 0.f),
		color(1.f, 1.f, 1.f, 1.f), exist(true), speed(0.f, 0.f, 0.f), is_move_select(false)
	{}
	virtual ~Obj3D() = default;

	Obj3D(const Obj3D& _rt)
	{
		pos = (_rt.pos);
		scale = (_rt.scale);
		angle = (_rt.angle);
		color = (_rt.color);
		speed = (_rt.speed);
		exist = _rt.exist;
		is_move_select = _rt.is_move_select;
	}
	auto& operator=(const Obj3D& _rt)
	{
		pos = (_rt.pos);
		scale = (_rt.scale);
		angle = (_rt.angle);
		color = (_rt.color);
		speed = (_rt.speed);
		exist = _rt.exist;
		is_move_select = _rt.is_move_select;

		return (*this);
	}

	Obj3D(Obj3D&& _rt) noexcept
		: pos(std::move(_rt.pos)), scale(std::move(_rt.scale)), angle(std::move(_rt.angle)),
		color(std::move(_rt.color)), exist(_rt.exist), speed(std::move(_rt.speed)), is_move_select(_rt.is_move_select)
	{}
	auto& operator=(Obj3D&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			pos = move(_rt.pos);
			scale = move(_rt.scale);
			angle = move(_rt.angle);
			color = move(_rt.color);
			speed = move(_rt.speed);
			exist = _rt.exist;
			is_move_select = _rt.is_move_select;
		}

		return (*this);
	}

public:
	operator bool() const noexcept { return exist; }

public:
	virtual void Update(KDL::Window* p_window, KDL::DX12::App* p_app) = 0;
	virtual void Draw(KDL::Window* p_window, KDL::DX12::App* p_app) = 0;

	void EndExist() noexcept { exist = false; }

public:
	VF3 pos;
	bool is_move_select;

protected:
	bool exist;
	VF3 scale;
	Vector3<float> speed;
	VF3 angle;
	VF4 color;
};
