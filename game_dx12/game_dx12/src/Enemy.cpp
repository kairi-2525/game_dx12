#include "MultipleObject.h"
#include "Enemy.h"
#include "GameLib.h"
#include "GameScene.h"
#include "Math.h"
#include "SingleObject.h"

//------------------------------------------------------------------------------------------------------
// 矢印
//------------------------------------------------------------------------------------------------------

Arrow::Arrow(const VF3& pos)
{
	this->pos = pos;
	scale = { 0.005f, 0.005f, 0.005f };
}

void Arrow::Update()
{
	scale = { 0.005f, 0.005f, 0.005f };
}

void Arrow::Draw() const
{
	GMLIB->DrawModel(model_handle, pos, scale, angle, GameScene::LightDir, { YELLOW, 0.5f });
}

//------------------------------------------------------------------------------------------------------
// ウェイポイント
//------------------------------------------------------------------------------------------------------

WayPoint::WayPoint(const VF3& pos)
	: is_back(false)
{
	this->pos = pos;
	scale = { 0.005f, 0.005f, 0.005f };
}

void WayPoint::Update()
{
	scale = { 0.005f, 0.005f, 0.005f };
}

void WayPoint::Draw() const
{
	GMLIB->DrawModel(model_handle, pos, scale, angle, GameScene::LightDir, { GREEN, 0.5f });
}

//------------------------------------------------------------------------------------------------------
// 敵
//------------------------------------------------------------------------------------------------------

Enemy::Enemy()
	: is_select(false), path_result(std::nullopt), ai(std::nullopt)
{
	scale = { 0.005f, 0.005f, 0.005f };
	color = { PINK, 1.f };

	ai.emplace(&way_points);
}

void Enemy::Update()
{
	using GS = GameScene;

#if _DEBUG && false
	using Keys = KeyData::Keys;

	constexpr float Movement{ 1.f };
	const bool is_ctrl{ GMLIB->IsKeyPress(Keys::LeftControl) };

	if (!is_ctrl || !GS::GetEditMode())
	{
		if (GMLIB->isKeyDown({ Keys::Left, Keys::A }))
		{
			speed.x += Movement;
		}
		else if (GMLIB->isKeyDown({ Keys::Right, Keys::D }))
		{
			speed.x -= Movement;
		}
		else if (GMLIB->isKeyDown({ Keys::Up, Keys::W }))
		{
			speed.z -= Movement;
		}
		else if (GMLIB->isKeyDown({ Keys::Down, Keys::S }))
		{
			speed.z += Movement;
		}
	}

	pos += speed;

	speed.Clear();
#endif

	// 敵編集モードのみ
	if (GS::GetEnmEditMode())
	{
		is_select ? color = { PINK, 1.f } : color = { PINK, 0.5f };

		for (auto& wp : way_points)
			wp.Update();

		//for (auto& arrow : arrows)
		//	arrow.Update();

		auto EraseObject{ [](auto& objects)
		{ objects.erase(std::remove_if(objects.begin(), objects.end(), [](const auto& obj)
			{ return !(obj); }), objects.end()); } };

		EraseObject(way_points);
		//EraseObject(arrows);
	}
	// 敵編集モード以外
	else
	{
		color = { PINK, 1.f };

		// 実行モード時
		if (!GS::GetEditMode())
		{
			if (!ai) ai.emplace(&way_points);

			ai->Update(*this, node);
		}
		else
		{
			path_result = std::nullopt;
			if (ai)	ai = std::nullopt;
		}
	}

	// 編集モードのみ
	if ((GS::GetEditMode() && !GS::GetEnmEditMode()) && is_move_select)
	{
		pos = GS::GetMasuPos();  // 座標をマス座標にする
	}
}

void Enemy::Draw() const
{
	GMLIB->DrawModel(model_handle, pos, scale, angle, GameScene::LightDir, color);

	if (is_select)
	{
		if (!way_points.empty())
		{
			for (size_t i = 0, length = way_points.size() - 1u; i < length; i++)
			{
				way_points[i].Draw();
			}
		}

		//for (auto& ar : arrows) ar.Draw();
	}

#if false
	if (path_result)
	{
		// スタート地点からゴールまで
		auto dt = path_result->to_goal;

		while (true)
		{
			if (!dt)	break;

			GMLIB->DrawCube(dt->pos, Fill3(0.5f), Fill3(0.f), GameScene::LightDir, { YELLOW, 0.5f });

			if (dt->pos == way_points.front().pos)	break;

			dt = dt->to_goal;
		}
	}
#endif
}