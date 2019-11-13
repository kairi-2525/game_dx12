#pragma once

#include "../KDL.h"

#ifdef _DEBUG
#define KDL_DEBUG_DX12
#else
#define KDL_DEBUG_DX12
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "d3dx12.h"
#include <dxgi1_6.h>

#include <vector>
#include <deque>
#include <unordered_map>
#include <filesystem>

#include "../KDL_Math.h"

#include <mutex>

namespace KDL
{
	namespace DX12
	{
		template <class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		class App
		{
		private:
			struct Worker
			{
				ComPtr<ID3D12GraphicsCommandList> command_list;
				ComPtr<ID3D12CommandAllocator> command_allocator;
			};
			struct TextureResource
			{
				ComPtr<ID3D12Resource1> resource;
				UINT count = 0;
			};
			class ResourceManager
			{
			public:
				std::unordered_map<std::wstring, TextureResource> m_textures;
			public:
				bool GetTexture(const std::filesystem::path& path, ID3D12Resource1** resource);
				bool DelTexture(const std::filesystem::path& path);
			};
		private:
#ifdef KDL_USE_IMGUI
			static inline const UINT GUI_COMMAND_LIST_NUM = 2;
#endif
			static inline const UINT GPU_WAIT_TIMEOUT = (10 * 1000);  // 10s
			static inline const UINT ALLOCATOR_COUNTER_MAX = 3;	//Windowsの場合、サブミットされたコマンドの実行は3フレーム遅延されます。
		public:
			static inline const std::wstring DUMMY_TEXTURE_NAME = L"Dummy";
			static inline const UINT FrameBufferCount = 2;
			static inline const COLOR4F DEFAULT_BACK_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };
		private:
			HWND m_hwnd;
			bool m_tearing_support;
			UINT m_allocator_counter = 0;
#ifdef KDL_DEBUG_DX12
			ComPtr<ID3D12DebugDevice> m_debug_device;
#endif
			ResourceManager m_resource_manager;
			ComPtr<ID3D12Device> m_device;
			ComPtr<ID3D12CommandQueue> m_command_queue;
			std::vector<ComPtr<ID3D12GraphicsCommandList>> m_command_lists;
			std::vector<std::vector<Worker>> m_worker_threads;
			std::deque<std::mutex> m_command_allocators_mutex;
			std::vector<ComPtr<ID3D12CommandAllocator>> m_command_allocators;
			ComPtr<IDXGISwapChain4> m_swapchain;
			std::vector<ComPtr<ID3D12Resource1>> m_render_targets;
			ComPtr<ID3D12Resource1> m_depth_buffer;

			std::vector<ComPtr<ID3D12Fence1>> m_frame_fences;
			std::vector<UINT64> m_frame_fence_values;
			HANDLE m_fence_wait_event;
			std::mutex m_frame_index_mutex;
			UINT m_frame_index = 0;

			ComPtr<ID3D12DescriptorHeap> m_heap_rtv;
			UINT m_rtv_descriptor_size = 0;
			ComPtr<ID3D12DescriptorHeap> m_heap_dsv;
			UINT m_dsv_descriptor_size = 0;

			ComPtr<ID3D12DescriptorHeap> m_heap_srv;
			UINT m_srv_descriptor_size = 0;

			CD3DX12_VIEWPORT m_viewport;
			CD3DX12_RECT m_scissor_rect;
		public:
			static HRESULT CreateBuffer(ComPtr<ID3D12Device>& device, ComPtr<ID3D12Resource1>& buffer, UINT64 buffer_size, const void* initial_data);
			static HRESULT CompileShaderFromFile(const LPCWSTR shader_name, const std::string& shader_resouce, const std::wstring& profile, ComPtr<ID3DBlob>& shader_blob, ComPtr<ID3DBlob>& error_blob, const std::wstring& entry_point_name = L"main");
		private:
			bool CheckTearingSupport();
			void WaitPreviousFrame();
			HRESULT MakeTexture(const std::filesystem::path& path);
		public:
			const UINT m_worker_thread_size;
		public:
			App(HWND hwnd, UINT worker_thread_size);
			~App();
			void WaitGPU(bool index_lock = true);
			ID3D12GraphicsCommandList* GetCmd() { return m_command_lists[m_frame_index].Get(); }
			HRESULT ScreenFlip();
			HRESULT ClearBackBuffer(const KDL::COLOR4F& clear_color);
			ComPtr<ID3D12Device>& GetDevice() { return m_device; }
			ID3D12GraphicsCommandList* GetCommandList(size_t num = 0u) { assert(m_worker_threads[m_frame_index].size() > num && "このnumに対応するコマンドリストがありませんｌ。");  return m_worker_threads[m_frame_index][num].command_list.Get(); }
			UINT GetFrameIndex() const { return m_frame_index; }
			const CD3DX12_VIEWPORT& GetViewport() const { return m_viewport; };
			const CD3DX12_RECT& GetScissorRect() const { return m_scissor_rect; }
			HRESULT LoadTexture(const std::filesystem::path& path, ID3D12Resource1** resource);
			bool DeleteTexture(const std::filesystem::path& path);
		};
	}
}