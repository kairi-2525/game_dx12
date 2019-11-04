#pragma once

#include "App.h"

namespace KDL
{
	namespace DX12
	{
#ifdef KDL_USE_IMGUI
		namespace IMGUI_HELPER
		{
			extern void Init(ID3D12DescriptorHeap* heap, IDXGISwapChain4* sc, ID3D12Device* device, HWND hwnd, UINT buffer_count);
		}
#endif
	}
}