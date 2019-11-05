#include "scene.h"
#include <assert.h>
#include <thread>

//-----------
// SceneBase
//-----------

void SceneBase::LoadAnotherThread(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	{
		std::lock_guard<std::mutex> lock(m_load_start_mutex);
		assert(!m_load_start && "[SceneBase]ロードに失敗しました。");
		m_load_start = true;
	}
	Load(p_scene_mgr, p_window, p_app);
	loaded();
}

void SceneBase::LoadStartAnotherThread(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	std::thread thread(&SceneBase::LoadAnotherThread, this, p_scene_mgr, p_window, p_app);
	thread.detach();
}

//--------------
// SceneManager
//--------------

SceneManager::SceneManager(SceneBase* p_scene, KDL::Window* p_window, KDL::DX12::App* p_app)
{
	assert(p_scene && "[SceneManager]初期化に失敗しました。");
	m_scene.reset(p_scene);
	m_scene->Load(this, p_window, p_app);
	m_scene->Initialize(this, p_window, p_app);
}

void SceneManager::Update(KDL::Window* p_window, KDL::DX12::App* p_app)
{
	assert(m_scene && "[SceneManager]更新に失敗しました。");
	if (m_scene)
	{
		m_scene->Update(this, p_window, p_app);
		m_scene->Draw(this, p_window, p_app);
		p_app->ScreenFlip();
		if (m_exit) return;
		SceneBase* next_scene = m_scene->GetNextScene();
		if (next_scene)
		{
			if (next_scene->IsLoaded())
			{
				m_scene->UnInitialize(this, p_window, p_app);
				std::unique_ptr<SceneBase> release_scene;
				release_scene = std::move(m_scene);
				release_scene->GetNextScene(&m_scene);
				if (!m_scene->IsLoadStart())
					m_scene->Load(this, p_window, p_app);
				m_scene->Initialize(this, p_window, p_app);
				p_app->WaitGPU();	//現在のフレームの描画処理を待機
				p_app->WaitGPU();	//念のため前のフレームの描画処理を待機(警告がでるので)
			}
			else if (!next_scene->IsLoadStart())
				next_scene->LoadStartAnotherThread(this, p_window, p_app);
		}
	}
}