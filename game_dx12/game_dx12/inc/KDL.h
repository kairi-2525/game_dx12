#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <stdexcept>

#include "KDL_Math.h"
#include "KDL_Window.h"

#define KDL_USE_IMGUI

#ifdef KDL_USE_DX12
#include "KDL_Dx12/App.h"
#include "KDL_Dx12/Sprite.h"
#include "KDL_Dx12/Mesh.h"
#include "KDL_Dx12/Primitive.h"
#endif

#ifdef KDL_USE_IMGUI
#include <ImGUI/imgui.h>
#include <ImGUI/examples/imgui_impl_dx12.h>
#include <ImGUI/examples/imgui_impl_win32.h>
#endif

#include <KDL_Tool/Tool.h>

#define INLINE_TEXT(...) #__VA_ARGS__

namespace KDL
{
	extern void RuntimeErrorStop(const std::runtime_error& exception);
	extern bool HrToStr(HRESULT hr);
}

#define HR_CHECK_ERROR_RETURN(hr, msg) assert(HrToStr(hr) && (msg)); if (FAILED(hr)) return hr