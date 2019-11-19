#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <list>
#include <chrono>
#include <deque>
#include <memory>

#include "KDL_Math.h"
#include "KDL_Input.h"
#include "KDL_Audio.h"

namespace KDL
{
	using namespace MATH;

	class Window
	{
	public:
		static inline const DWORD STYLE_DEFAULT = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE;
		static inline const DWORD STYLE_FULLSCREEN = WS_VISIBLE | WS_POPUP;
	private:
		static inline std::list<std::string> title_list;

		class RefreshRate
		{
		private:
			static inline const double UPDATE_RATE = 0.1;
		private:
			std::chrono::system_clock::time_point time_point;
			std::deque<double> elpased_time_log;
			double m_elapsed_time_nano = 0.0;
			double m_elapsed_time_sec = 0.0;
			double m_average_time = 0.0;
			double m_refresh_rate = 0.0;
			double m_update_counter = 0.0;
		public:
			RefreshRate() : time_point(std::chrono::system_clock::now()) {}
			void Update();
			template<class T = double>
			T GetElpasedTime() const { return static_cast<T>(m_elapsed_time_sec); }
			template<class T = double>
			T GetRefreshRate() const { return static_cast<T>(m_refresh_rate); }
		} m_fps_mgr;
	private:
		HWND m_hwnd;
		std::string m_title;
		DWORD m_style;
		RECT m_rect;
		bool m_draw_flg = false;
		bool m_old_draw_flg = false;
		std::unique_ptr<Input> m_input;
		std::unique_ptr<Audio> m_audio;
	public:
		static bool IsFullScreen(HWND hwnd);
	public:
		Window(const std::string& title, DWORD style);
		Window(const std::string& title = "no title", const UINT2& size = { 100u, 100u }, DWORD style = STYLE_DEFAULT);
		~Window();
		bool Update();
		void DrawFPS();
		void ResetTitle() const;
		void SetTitle(const std::string& title);
		HWND GetHWND() { return m_hwnd; }
		bool IsFullScreen() const { return IsFullScreen(m_hwnd); }
		template<class T = float>
		T GetElapsedTime() const { return m_fps_mgr.GetElpasedTime<T>(); }
		Input* GetInput() { return m_input.get(); }
		Audio* GetAudio() { return m_audio.get(); }
	};
}