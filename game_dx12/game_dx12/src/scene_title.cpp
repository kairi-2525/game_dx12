#include "TitleScene.h"
#include "GameScene.h"

void SceneTitle::Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	player.model = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, "./data/models/player.fbx", 100);

	title_parallel.model = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, "./data/models/Tittle/parallel.fbx", 100);
	title_labyrinth.model = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, "./data/models/Tittle/labyrinth.fbx", 100);

	for (auto& it : grounds_list)
	{
		switch (it.first)
		{
			case  GROUND_TYPE::WarpHole:
			{
				warp_hole.model = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, ("./data/models/Game/" + it.second + ".fbx"), 100);
				continue;
			}
			case GROUND_TYPE::SandWall:
			{
				sands.model = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, ("./data/models/Game/" + it.second + ".fbx"), 100);
				continue;
			}
			case GROUND_TYPE::SnowWall:
			{
				snows.model = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, ("./data/models/Game/" + it.second + ".fbx"), 100);
				continue;
			}
			case GROUND_TYPE::Air:
			{
				grounds[it.first];
				continue;
			}
			default:
			{
				grounds[it.first].model = std::make_unique<KDL::DX12::Geometric_Board>(p_app, ("./data/images/Game/" + it.second + ".png"), 100);
				break;
			}
		}	
	}

	sand_bg = std::make_unique<KDL::DX12::Sprite_Image>(p_app, "./data/images/Sand.png", 100);
	snow_bg = std::make_unique<KDL::DX12::Sprite_Image>(p_app, "./data/images/Snow.png", 100);
	warp_hole_off = std::make_unique<KDL::DX12::Mesh_FBX>(p_app, "./data/models/Game/Warp_off.fbx", 100);
}

void SceneTitle::Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	camera =
		std::make_unique<KDL::TOOL::Camera>(
			KDL::TOOL::Camera(
				{ 0.f, 1.f, -5.f },		//eye
				{ 0.f, 1.f, 0.f },			//focus
				{ 0.f, 1.f, 0.f },			//up
				DirectX::XMConvertToRadians(30.f),
				p_app->GetViewport().Width / p_app->GetViewport().Height,
				{ 0.1f, 1000.f }
			)
		);
	light_dir = KDL::FLOAT3{ 0.f, -1.f, 1.f };
	light_dir /= light_dir.Length();

	player.position = KDL::FLOAT3{ 0.f, 0.8f, 0.f };
	player.rotate = KDL::FLOAT3{ 0.f, DirectX::XMConvertToRadians(-90.f), 0.f };
	player.scale = KDL::FLOAT3{ 0.1f, 0.1f, 0.1f };

#if 0
	title_parallel.position = KDL::FLOAT3{ -1.f, 2.035f, 0.f };
	title_parallel.rotate = KDL::FLOAT3{ 0.f, 0.f, 0.f };
	title_parallel.scale = KDL::FLOAT3{ 0.024f, 0.024f, 0.024f };
	title_labyrinth.position = KDL::FLOAT3{ 1.f, 2.035f, 0.f };
	title_labyrinth.rotate = KDL::FLOAT3{ 0.f, 0.f, 0.f };
	title_labyrinth.scale = KDL::FLOAT3{ 0.024f, 0.024f, 0.024f };
#else
	title_parallel.position = KDL::FLOAT3{ -1.1f, 2.035f, 0.f };
	title_parallel.rotate = KDL::FLOAT3{ 0.f, 0.f, 0.f };
	title_parallel.scale = KDL::FLOAT3{ 0.033f, 0.033f, 0.033f };
	title_labyrinth.position = KDL::FLOAT3{ 0.9f, 1.614f, 0.f };
	title_labyrinth.rotate = KDL::FLOAT3{ 0.f, 0.f, 0.f };
	title_labyrinth.scale = KDL::FLOAT3{ 0.033f, 0.033f, 0.033f };
#endif

	snow = true;
	for (auto& it : grounds)
		it.second.data.clear();
	float set_pos = -static_cast<float>(BLOCK_NUM) / 2;
	for (UINT i = 0u; i < BLOCK_NUM; i++)
	{
		auto& it = grounds[rand() % 2 == 0 ? GROUND_TYPE::Snow : GROUND_TYPE::SnowBroken].data.emplace_back();
		it.position = KDL::FLOAT3{ set_pos, 0, 0};
		it.rotate = KDL::FLOAT3{ 0, 0, 0 };
		it.scale = KDL::FLOAT3{ 1.f, 1.f, 1.f };
		set_pos += BLOCK_SIZE;
	}
	warp_hole.data.clear();
	auto& it = warp_hole.data.emplace_back();
	it.position = KDL::FLOAT3{ set_pos, 0, 0 };
	it.rotate = KDL::FLOAT3{ 0, 0, 0 };
	it.scale = KDL::FLOAT3{ WARP_HOLE_SCALE, WARP_HOLE_SCALE, WARP_HOLE_SCALE };
}

void SceneTitle::Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	const auto* input = p_window->GetInput();
	const float elpased_time = p_window->GetElapsedTime();

	const KDL::COLOR4F clear_color = { 0.f, 0.f, 0.f, 1.f };
	p_app->ClearBackBuffer(clear_color);

#ifdef KDL_USE_IMGUI
	ImGui::Begin("gui");
	{
		ImGui::SliderFloat3("title_LB_p", &title_labyrinth.position.x, -4.f, +4.f);
		ImGui::SliderFloat("title_LB_s", &title_labyrinth.scale.x, 0, +1);
		title_labyrinth.scale.y = title_labyrinth.scale.z = title_labyrinth.scale.x;
		ImGui::SliderFloat3("title_PR_p", &title_parallel.position.x, -4.f, +4.f);
		ImGui::SliderFloat("title_PR_s", &title_parallel.scale.x, 0, +1);
		title_parallel.scale.y = title_parallel.scale.z = title_parallel.scale.x;
		ImGui::SliderFloat("WALL_HOLE_SCALE", &WARP_HOLE_SCALE, 0.f, 1.01f);
		for (auto& data : warp_hole.data)
		{
			data.scale = KDL::FLOAT3{ WARP_HOLE_SCALE, WARP_HOLE_SCALE, WARP_HOLE_SCALE };
		}
		for (const auto& it : grounds)
		{
			ImGui::SetNextTreeNodeOpen(true);
			if (ImGui::TreeNode(grounds_list.at(it.first).c_str()))
			{
				for (const auto& data : it.second.data)
				{
					ImGui::TextColored(data.position.x > 0 ? ImVec4(0.5f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%+2.2f, %+2.2f, %+2.2f (%s)", data.position.x, data.position.y, data.position.z, data.changed ? "TRUE" : "FALSE");
				}
				ImGui::TreePop();
			}
		}
		ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode(grounds_list.at(GROUND_TYPE::WarpHole).c_str()))
		{
			for (const auto& data : warp_hole.data)
			{
				ImGui::TextColored(data.position.x > 0 ? ImVec4(0.5f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%+2.2f, %+2.2f, %+2.2f (%s)", data.position.x, data.position.y, data.position.z, data.changed ? "TRUE" : "FALSE");
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
#endif
	if (input->IsTrgKey(KDL::KEY_INPUTS::Enter))
	{
		SetNextScene<SceneLoad>();	//別スレッドでシーン切り替え
	}
#ifdef _DEBUG
	if (input->IsTrgKey(KDL::KEY_INPUTS::Space))
	{
		Initialize(p_scene_mgr, p_window, p_app);
	}
#endif
	const float speed = 1.f;
	const float end_line = -5.f;
	const float check_line = 0.f;
	std::vector<GROUND_TYPE> deleted_type;
	for (auto& g : grounds)
	{
		auto& data = g.second.data;
		for (size_t i = 0; i < data.size();)
		{
			data[i].position.x -= speed * elpased_time;
			if (data[i].position.x < end_line)
			{
				data[i] = data.back();
				data.pop_back();
				deleted_type.emplace_back(g.first);
			}
			else i++;
		}
	}
	{	//ワープホールのフリップ
		auto& data = warp_hole.data;
		const bool snow_log = snow;
		for (size_t i = 0; i < data.size();)
		{
			data[i].position.x -= speed * elpased_time;
			if (!data[i].changed && data[i].position.x < check_line)
			{
				data[i].changed = true;
				snow = !snow;
			}
			if (data[i].position.x < end_line)
			{
				data[i] = data.back();
				data.pop_back();
				deleted_type.emplace_back( GROUND_TYPE::WarpHole);
			}
			else i++;
		}
		if (snow != snow_log)
		{
			FlipGrounds();
			auto& set_data = grounds.at(snow ? GROUND_TYPE::Snow : GROUND_TYPE::Sand).data;
			auto& set_break_data = grounds.at(snow ? GROUND_TYPE::SnowBroken : GROUND_TYPE::SandBroken).data;
			auto& clear_data = grounds.at(!snow ? GROUND_TYPE::Snow : GROUND_TYPE::Sand).data;
			auto& clear_break_data = grounds.at(!snow ? GROUND_TYPE::SnowBroken : GROUND_TYPE::SandBroken).data;
			for (auto& it : grounds.at(GROUND_TYPE::Air).data)
			{
				it.changed = false;
				set_data.emplace_back(it);
			}
			grounds.at(GROUND_TYPE::Air).data.clear();
			for (auto& it : clear_data)
			{
				it.changed = false;
				grounds.at(GROUND_TYPE::Air).data.emplace_back(it);
			}
			clear_data.clear();
			for (auto& it : clear_break_data)
			{
				it.changed = false;
				set_break_data.emplace_back(it);
			}
			clear_break_data.clear();
		}
	}
	{	//各ブロックのブレイク
		const float break_line = -0.5f;
		{
			auto& data = grounds[GROUND_TYPE::Sand].data;
			for (size_t i = 0; i < data.size();)
			{
				if (!data[i].changed && data[i].position.x < break_line)
				{
					data[i].changed = true;
					grounds[GROUND_TYPE::SandBroken].data.emplace_back(data[i]);
					data[i] = grounds[GROUND_TYPE::Sand].data.back();
					data.pop_back();
					BreakGrounds();
				}
				else i++;
			}
		}
		{
			auto& data = grounds[GROUND_TYPE::Snow].data;
			for (size_t i = 0; i < data.size();)
			{
				if (!data[i].changed && data[i].position.x < break_line)
				{
					data[i].changed = true;
					grounds[GROUND_TYPE::SnowBroken].data.emplace_back(data[i]);
					data[i] = data.back();
					data.pop_back();
					BreakGrounds();
				}
				else i++;
			}
		}
		{
			auto& data = grounds[GROUND_TYPE::SandBroken].data;
			for (size_t i = 0; i < data.size();)
			{
				if (!data[i].changed && data[i].position.x < break_line)
				{
					data[i] = data.back();
					data.pop_back();
					deleted_type.emplace_back(GROUND_TYPE::SandBroken);
					RemoveGrounds();
				}
				else i++;
			}
		}
		{
			auto& data = grounds[GROUND_TYPE::SnowBroken].data;
			for (size_t i = 0; i < data.size();)
			{
				if (!data[i].changed && data[i].position.x < break_line)
				{
					data[i] = data.back();
					data.pop_back();
					deleted_type.emplace_back(GROUND_TYPE::SnowBroken);
					RemoveGrounds();
				}
				else i++;
			}
		}
	}
	if (!deleted_type.empty())
	{	//消滅した数だけ追加する。
		float top_x = end_line;
		float top_warp_hole_x = end_line;
		for (const auto& g : grounds)
			for (const auto& data : g.second.data)
				top_x = max(top_x, data.position.x);
		bool fliped_warp_hole = false;
		for (const auto& data : warp_hole.data)
		{
			top_x = max(top_x, data.position.x);
			top_warp_hole_x = max(top_warp_hole_x, data.position.x);
			fliped_warp_hole = data.changed;
		}
		top_x += BLOCK_SIZE;
		for (const auto& type : deleted_type)
		{
			switch (type)
			{
				case GROUND_TYPE::WarpHole:
				{
					auto& it = warp_hole.data.emplace_back();
					it.position = KDL::FLOAT3{ top_x, 0, 0 };
					it.rotate = KDL::FLOAT3{ 0, 0, 0 };
					it.scale = KDL::FLOAT3{ WARP_HOLE_SCALE, WARP_HOLE_SCALE, WARP_HOLE_SCALE };
					top_x += BLOCK_SIZE;
					break;
				}
			}
		}
		for (const auto& type : deleted_type)
		{
			switch (type)
			{
				case GROUND_TYPE::WarpHole:
				{
					break;
				}
				default:
				{
					bool breaked = rand() % 2 == 0 ? true : false;
					auto& data = grounds.at(
						fliped_warp_hole ?
						snow ?
						breaked ?
						GROUND_TYPE::SnowBroken : GROUND_TYPE::Snow :
						breaked ? 
						GROUND_TYPE::SandBroken : GROUND_TYPE::Sand :
						snow ?
						breaked ?
						GROUND_TYPE::SnowBroken : GROUND_TYPE::Air :
						breaked ?
						GROUND_TYPE::SandBroken : GROUND_TYPE::Air
					).data;
					auto& it = data.emplace_back();
					it.position = KDL::FLOAT3{ top_x, 0, 0 };
					it.rotate = KDL::FLOAT3{ 0, 0, 0 };
					it.scale = KDL::FLOAT3{ 1.f, 1.f, 1.f };
					break;
				}
			}
			top_x += BLOCK_SIZE;
		}
	}
}

void SceneTitle::Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{

	{
		auto& bg = snow ? snow_bg : sand_bg;
		const auto vp{ p_app->GetViewport() };
		const KDL::FLOAT2 s_size{ vp.Width, vp.Height };
		const KDL::COLOR4F bg_color = { 1.f, 1.f, 1.f, 1.f };
		const float set_scale = 1.f;
		if (FAILED(bg->AddCommand(p_app->GetCommandList(), p_app,
			{ 0.f, 0.f }, s_size, { 0.f, 0.f }, { 0.f, 0.f }, bg->GetSize(), { set_scale, set_scale }, 0.f, bg_color, bg_color, bg_color, bg_color,
			0, false, false)))
		{
			p_scene_mgr->Exit();
			return;
		}
	}

	if (FAILED(player.Draw(p_app, camera.get(), light_dir)))
	{
		p_scene_mgr->Exit();
		return;
	}
	if (FAILED(title_parallel.Draw(p_app, camera.get(), { 0, 0, 1 })))
	{
		p_scene_mgr->Exit();
		return;
	}
	if (FAILED(title_labyrinth.Draw(p_app, camera.get(), { 0, 0, 1 })))
	{
		p_scene_mgr->Exit();
		return;
	}
	for (auto& it : grounds)
	{
		if (it.first == GROUND_TYPE::Air) continue;
		if (FAILED(it.second.Draw(p_app, camera.get(), light_dir)))
		{
			p_scene_mgr->Exit();
			return;
		}
	}
	bool fliped = false;
	for (const auto& it : warp_hole.data)
		fliped = it.changed ? true : fliped;
	if (fliped)
		warp_hole.Draw<KDL::DX12::Mesh_FBX>(p_app, camera.get(), light_dir, warp_hole_off.get());
	else
		warp_hole.Draw(p_app, camera.get(), light_dir);

	float x_zero = BLOCK_SIZE;
	for (const auto& it : grounds)
	{
		for (const auto& data : it.second.data)
			if (data.position.x > 0)
				x_zero = min(fabsf(x_zero), fabsf(data.position.x));
	}
	for (const auto& data : warp_hole.data)
		if (data.position.x > 0)
			x_zero = min(fabsf(x_zero), fabsf(data.position.x));
	auto& wall = snow ? snows : sands;
	{
		wall.data.clear();
		wall.data.resize(BLOCK_NUM);
		size_t count = 0u;
		float set_x = -static_cast<float>(BLOCK_NUM) / 2;
		set_x += x_zero;
		for (auto& it : wall.data)
		{
			it.position = KDL::FLOAT3{ set_x + (count * BLOCK_SIZE), BLOCK_SIZE / 2, BLOCK_SIZE };
			it.rotate = KDL::FLOAT3{ 0, 0, 0 };
			it.scale = KDL::FLOAT3{ WALL_SCALE, WALL_SCALE, WALL_SCALE };
			count++;
		}
	}
	wall.Draw(p_app, camera.get(), light_dir);
}

void SceneTitle::UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{

}

//サウンド用

//ヒビが入ったとき
void SceneTitle::BreakGrounds()
{

}
//消えたとき
void SceneTitle::RemoveGrounds()
{

}
//反転したとき
void SceneTitle::FlipGrounds()
{

}
