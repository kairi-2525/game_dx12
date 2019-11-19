#include "main.h"
#include "scene.h"
#include "TitleScene.h"
#include "SceneSelect.h"

KDL::Window* p_main_window = nullptr;
KDL::DX12::App* p_app = nullptr;

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	{
#ifdef NDEBUG
		KDL::Window main_window("parallel labyrinth", KDL::Window::STYLE_FULLSCREEN);
		main_window.GetInput()->SetMouseVisible(false);
#else
		KDL::Window main_window("game", { 1888, 1062 });
#endif
		p_main_window = &main_window;
		{
			KDL::DX12::App dx12(main_window.GetHWND(), 1);
			p_app = &dx12;
#ifdef KDL_USE_IMGUI
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->AddFontFromFileTTF("./data/fonts/APJapanesefont.ttf", 20.0f, nullptr, KDL::TOOL::glyphRangesJapanese);
#endif
			SceneManager scene_mgr(new SceneTitle, &main_window, &dx12);

			while (main_window.Update())
			{
				scene_mgr.Update(&main_window, &dx12);
				if (scene_mgr.IsExit()) break;
			}
			p_app = nullptr;
		}
		p_main_window = nullptr;
	}
}