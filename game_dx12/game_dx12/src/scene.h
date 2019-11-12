#pragma once
#define KDL_USE_DX12
#include <KDL.h>
#include <KDL_Tool/Camera.h>
#include <memory>
#include <mutex>

#include "KDL_Dx12/App.h"
#include "KDL_Dx12/Mesh.h"

class SceneManager;

class SceneBase
{
private:
	std::unique_ptr<SceneBase> m_next_scene;
	std::mutex m_load_start_mutex;
	std::mutex m_loaded_mutex;
	bool m_load_start = false;
	bool m_loaded = false;
private:
	void loaded()
	{
		std::lock_guard<std::mutex> lock(m_loaded_mutex);
		m_loaded = true;
	}
	void LoadAnotherThread(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
protected:
	SceneBase(SceneBase* p_scene = nullptr)
	{
		if (p_scene) m_next_scene.reset(p_scene);
	}
public:
	bool IsLoaded()
	{
		std::lock_guard<std::mutex> lock(m_loaded_mutex);
		return m_loaded;
	}
	bool IsLoadStart()
	{
		std::lock_guard<std::mutex> lock(m_load_start_mutex);
		return m_load_start;
	}
	//次のシーンをセットします。
	template <class T>
	void SetNextScene()
	{
		if (m_next_scene) return;
		m_next_scene = std::make_unique<T>();
	}
	//次のシーンをセットし、即座に切り替えます。
	template <class T>
	void ChangeScene()
	{
		if (m_next_scene) return;
		SetNextScene<T>();
		m_next_scene->loaded();
	}
	void GetNextScene(std::unique_ptr<SceneBase>* unique_p_scene)
	{
		if (unique_p_scene)
		{
			*unique_p_scene = std::move(m_next_scene);
		}
	}
	SceneBase* GetNextScene() { return m_next_scene.get(); }
	void LoadStartAnotherThread(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	virtual ~SceneBase() = default;
public:
	//読み込み
	virtual void Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app) {};
	//読み込み後の初期化
	virtual void Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app) = 0;
	//更新
	virtual void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app) = 0;
	//描画
	virtual void Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app) = 0;
	//終了処理
	virtual void UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app)
	{
		std::lock_guard<std::mutex> lock(m_loaded_mutex);
		m_loaded = false;
	};
};

class SceneManager
{
	std::unique_ptr<SceneBase> m_scene;
	bool m_exit = false;
public:
	SceneManager(SceneBase* p_scene, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Exit() { m_exit = false; }
	bool IsExit() { return m_exit; }
};

class SceneMain : public SceneBase
{
	std::unique_ptr<KDL::DX12::Mesh_FBX> mesh;
	std::unique_ptr<KDL::TOOL::Camera> camera;
	float angle;
public:
	void Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
};
