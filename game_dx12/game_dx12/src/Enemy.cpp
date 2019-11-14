#include "MultipleObject.h"
#include "Enemy.h"
#include "KDL.h"
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

void Arrow::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	scale = { 0.005f, 0.005f, 0.005f };
}

void Arrow::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	//GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, { YELLOW, 0.5f });
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

void WayPoint::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	scale = { 0.005f, 0.005f, 0.005f };
}

void WayPoint::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	//GMLIB->DrawModel(model_handle, pos, scale, angle, SceneGame::LightDir, { GREEN, 0.5f });
}

//------------------------------------------------------------------------------------------------------
// 敵
//------------------------------------------------------------------------------------------------------

Enemy::Enemy()
	: is_select(false), ai(std::nullopt)
{
	scale = Fill3(0.1f);
	color = { PINK, 1.f };
	before_pos = pos;

	ai.emplace(&way_points, &angle.y);
}

void Enemy::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

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

#if false
	// 敵編集モードのみ
	if (GS::GetEnmEditMode())
	{
		is_select ? color = { PINK, 1.f } : color = { PINK, 0.5f };

		for (auto& wp : way_points)
			wp.Update(p_window, p_app);

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
#else
	constexpr float AdjDistance{ 0.05f };

	if (!ai) ai.emplace(&way_points, &angle.y);

	ai->Update(*this, p_window->GetElapsedTime(), node);

	// 移動した
	if (Math::Distance(pos, before_pos) >= AdjDistance)
	{
		using Vec2sub::MakeVector2;

		constexpr float AdjRadY{ Math::PAI<float> / 2.f };

		float& ang{ angle.y };

		// 右
		if (pos.x < before_pos.x)
			ang = AdjRadY;
		// 左
		else if (pos.x > before_pos.x)
			ang = -AdjRadY * 1.f;
		// 上
		else if (pos.z < before_pos.z)
			ang = AdjRadY * 0.f;
		// 下
		else if (pos.z > before_pos.z)
			ang = AdjRadY * 2.f;

		// 履歴更新
		before_pos = pos;
	}
#endif
}

void Enemy::Draw(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	using GS = SceneGame;

	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(0.f, angle.y, 0.f);
		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
	}
	DirectX::XMFLOAT4X4 wvp, w;
	DirectX::XMStoreFloat4x4(&w, W);
	GS::camera->CreateUpdateWorldViewProjection(&wvp, W);
	model->AddCommand(p_app->GetCommandList(0), p_app, wvp, w, GS::LightDir, { WHITE, 1.f });

#if false
	if (is_select)
	{
		if (!way_points.empty())
		{
			for (size_t i = 0, length = way_points.size() - 1u; i < length; i++)
			{
				way_points[i].Draw(p_window, p_app);
			}
		}

		//for (auto& ar : arrows) ar.Draw();
	}

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

std::ofstream& operator<<(std::ofstream& ofs, const Enemy& data)
{
	ofs << data.pos << data.angle << std::endl << std::endl;

	OutputComment(ofs, "WeyPoints", "", "", 2u);
	ofs << F_OUT(data.way_points.size()) << std::endl;

	if (size_t size{ data.way_points.size() }; size > 0u)
	{
		for (auto& wp : data.way_points)
		{
			ofs << wp;
		}
	}

	ofs << std::endl;

	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, Enemy& data)
{
	size_t wp_size{ 0u };

	ifs >> data.pos >> data.angle;
	data.before_pos = data.pos;

	InputComment(ifs);
	ifs >> wp_size;

	if (wp_size != 0u)
	{
		data.way_points.resize(wp_size);

		for (auto& wp : data.way_points)
		{
			ifs >> wp;
		}
	}

	return ifs;
}