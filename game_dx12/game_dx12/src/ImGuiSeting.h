#pragma once

#define USE_IMGUI (true)

#if USE_IMGUI

#include"imgui.h"
// DirectX
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include <filesystem>
#include <array>
#include <DirectXMath.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace ImGuiSeting
{
	IMGUI_IMPL_API bool     ImGui_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* device_context);
	IMGUI_IMPL_API void     ImGui_ImplDX11_Shutdown();
	IMGUI_IMPL_API void     ImGui_ImplDX11_NewFrame();
	IMGUI_IMPL_API void     ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data);

	// Use if you want to reset your rendering device without losing ImGui state.
	IMGUI_IMPL_API void     ImGui_ImplDX11_InvalidateDeviceObjects();
	IMGUI_IMPL_API bool     ImGui_ImplDX11_CreateDeviceObjects();

	IMGUI_IMPL_API bool     ImGui_ImplWin32_Init(void* hwnd);
	IMGUI_IMPL_API void     ImGui_ImplWin32_Shutdown();
	IMGUI_IMPL_API void     ImGui_ImplWin32_NewFrame();
}

class [[maybe_unused]] ImguiTool
{
private:
	using VF4 = DirectX::XMFLOAT4;
	using VF3 = DirectX::XMFLOAT3;
	using VF2 = DirectX::XMFLOAT2;
	using VI4 = DirectX::XMINT4;
	using VI3 = DirectX::XMINT3;
	using VI2 = DirectX::XMINT2;

private:
	ImguiTool() = delete;
	~ImguiTool() = delete;
	ImguiTool(const ImguiTool&) = delete;
	ImguiTool(ImguiTool&&) noexcept = delete;
	auto& operator=(const ImguiTool&) = delete;
	auto& operator=(ImguiTool&&) noexcept = delete;

public:

#if true
	[[nodiscard]] static inline bool InitImgui(void* hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& font_name = nullptr)
	{
		bool rv{ true };

		assert(!is_init && "���ɏ���������Ă���");

		// Setup Dear ImGui context
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			auto& io = ImGui::GetIO(); (void)io;
		}

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		rv = ImGuiSeting::ImGui_ImplWin32_Init(hwnd);
		rv = ImGuiSeting::ImGui_ImplDX11_Init(pDevice, pDeviceContext);

		if (!rv)	return false;

		// ���{��t�H���g�ݒ�
		if (!font_name.empty())
		{
			auto& io{ ImGui::GetIO() };
			io.Fonts->AddFontFromFileTTF(font_name.data(), 20.f, nullptr,
				io.Fonts->GetGlyphRangesJapanese());
		}

		OutputDebugString(L"ImGui�̏���������\n");

		is_init = true;

		return true;
	}

	static inline void NewFrameImgui()
	{
		assert(is_init && "����������Ă��Ȃ�");

		ImGuiSeting::ImGui_ImplDX11_NewFrame();
		ImGuiSeting::ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	static inline void ShutdownImgui()
	{
		assert(is_init && "����������Ă��Ȃ�");

		ImGuiSeting::ImGui_ImplDX11_Shutdown();
		ImGuiSeting::ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		is_init = false;

		OutputDebugString(L"ImGui�̏I����������\n");
	}

	static inline void RenderImgui()
	{
		assert(is_init && "����������Ă��Ȃ�");

		ImGui::EndFrame();
		ImGui::Render();
		ImGuiSeting::ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
#endif

	// ���֌n -------------------------------------------------------------------------------------------

#if true
	// �\���������ꏊ�̉��ɐ錾����i�⏕�������}�E�X���u(�H)�v�Ɏ����Ă���Ɠ��e���\�������j
	static inline void ShowHelp(const char* help_coments)
	{
		assert(is_init && "����������Ă��Ȃ�");

		ImGui::SameLine(); ShowHelpMarker(help_coments);
	}

	// ���I�Ȍ��̕��͂�\������i�߂�l�͑I�����ꂽ�A�C�e�������Ԗڂ��������B�I������Ă��Ȃ��Ȃ�size_t�̍ő�l���Ԃ�j
	static inline size_t SelectVectorString(const char* const name,
		const std::vector<std::filesystem::path>& items, std::filesystem::path& select_item)
	{
		assert(is_init && "����������Ă��Ȃ�");

		if (!items.empty() && ImGui::BeginCombo(name, select_item.u8string().data()))
		{
			size_t num{ 0u };

			for (auto& itm : items)
			{
				bool is_selected{ select_item == itm };

				num++;

				if (ImGui::Selectable(itm.u8string().data(), is_selected))
				{
					select_item = itm;

					break;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();

					continue;
				}
			}
			ImGui::EndCombo();

			return (--num);
		}

		return (std::numeric_limits<size_t>::max)();
	}

	// �󔒂Ƌ�؂�
	static inline void Spacing(const size_t space_size, const bool is_separator = false)
	{
		assert(is_init && "����������Ă��Ȃ�");

		for (size_t i = 0; i < space_size; i++)
		{
			ImGui::Spacing();
		}

		if (is_separator)	ImGui::Separator();
	}

	static inline void Begin(const VF2& pos, const VF2& size, const char* window_name,
		const ImGuiWindowFlags flags = ImGuiBackendFlags_None, const float alpha = 1.f,
		const ImGuiCond_ cond_flag = ImGuiCond_Once)
	{
		assert(is_init && "����������Ă��Ȃ�");

		ImGui::SetNextWindowBgAlpha(alpha); // Transparent background
		ImGui::SetNextWindowSize({ size.x, size.y }, cond_flag);
		ImGui::SetNextWindowPos({ pos.x, pos.y }, cond_flag);
		ImGui::Begin(window_name, nullptr, flags);
	}

	// �ꎞ�I�ȃE�C���h�E��\������
	static inline void BeginShowTempWindow(const VF2& pos, const char* name,
		const bool is_input = true, const float alpha = 0.7f, const ImGuiCond_ cond = ImGuiCond_Always)
	{
		assert(is_init && "����������Ă��Ȃ�");

		if (is_input)
		{
			constexpr auto Type{ ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav  };

			ImGui::SetNextWindowBgAlpha(alpha);
			ImGui::SetNextWindowPos({ pos.x, pos.y }, cond);
			ImGui::Begin(name, nullptr, Type);
		}
		else
		{
			constexpr auto Type{ ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs };

			ImGui::SetNextWindowBgAlpha(alpha);
			ImGui::SetNextWindowPos({ pos.x, pos.y }, cond);
			ImGui::Begin(name, nullptr, Type);
		}
	}

	// �u�͂��v�u�������v�̗l�Ȏ���̃{�^����\������
	// �߂�l��YES�ANO�̏�
	static inline auto YesNoButton(const char* yes_lavel, const char* no_label,
		const ImVec2& yes_size = ImVec2(0, 0), const ImVec2& no_size = ImVec2(0, 0),
		float offset_from_start_x = 0.0f, float spacing = -1.0f)
	{
		bool yes, no;

		yes = ImGui::Button(yes_lavel, yes_size);

		ImGui::SameLine(offset_from_start_x, spacing);

		no = ImGui::Button(no_label, no_size);

		return (std::make_pair(yes, no));
	}

private:
	static inline void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(desc);
	}

public:
#endif

private:
	static inline bool is_init { false };
};

#endif // USE_IMGUI