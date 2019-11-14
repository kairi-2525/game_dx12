#pragma once

#include <DirectXMath.h>

#include "ImGuiSeting.h"

//-------------------------------------------------------------------------------------------------------------
// ImVec4ån
//-------------------------------------------------------------------------------------------------------------

static inline void operator+= (ImVec4& v1, const ImVec4& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
}

static inline void operator-= (ImVec4& v1, const ImVec4& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
}

static inline void operator/= (ImVec4& v1, const ImVec4& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	v1.w /= v2.w;
}

static inline void operator*= (ImVec4& v1, const ImVec4& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;
}

[[nodiscard]] static inline  auto operator+ (const ImVec4& v1, const ImVec4& v2)
{
	return ImVec4{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}

[[nodiscard]] static inline  auto operator- (const ImVec4& v1, const ImVec4& v2)
{
	return ImVec4{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

[[nodiscard]] static inline  auto operator* (const ImVec4& v1, const ImVec4& v2)
{
	return ImVec4{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w };
}

[[nodiscard]] static inline  auto operator/ (const ImVec4& v1, const ImVec4& v2)
{
	return ImVec4{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w };
}

static inline void operator+= (ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
}

static inline void operator-= (ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
}

static inline void operator/= (ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	v1.w /= v2.w;
}

static inline void operator*= (ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;
}

[[nodiscard]] static inline  auto operator+ (const ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	return ImVec4{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}

[[nodiscard]] static inline  auto operator- (const ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	return ImVec4{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

[[nodiscard]] static inline  auto operator* (const ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	return ImVec4{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w };
}

[[nodiscard]] static inline  auto operator/ (const ImVec4& v1, const DirectX::XMFLOAT4& v2)
{
	return ImVec4{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w };
}

static inline void operator+= (DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
}

static inline void operator-= (DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
}

static inline void operator/= (DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	v1.w /= v2.w;
}

static inline void operator*= (DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;
}

[[nodiscard]] static inline  auto operator+ (const DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	return DirectX::XMFLOAT4{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}

[[nodiscard]] static inline  auto operator- (const DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	return DirectX::XMFLOAT4{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

[[nodiscard]] static inline  auto operator* (const DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	return DirectX::XMFLOAT4{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w };
}

[[nodiscard]] static inline  auto operator/ (const DirectX::XMFLOAT4& v1, const ImVec4& v2)
{
	return DirectX::XMFLOAT4{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w };
}

static inline void operator+= (ImVec4& v1, const float num)
{
	v1.x += num;
	v1.y += num;
	v1.z += num;
	v1.w += num;
}

static inline void operator-= (ImVec4& v1, const float num)
{
	v1.x -= num;
	v1.y -= num;
	v1.z -= num;
	v1.w -= num;
}

static inline void operator/= (ImVec4& v1, const float num)
{
	v1.x /= num;
	v1.y /= num;
	v1.z /= num;
	v1.w /= num;
}

static inline void operator*= (ImVec4& v1, const float num)
{
	v1.x *= num;
	v1.y *= num;
	v1.z *= num;
	v1.w *= num;
}

[[nodiscard]] static inline  auto operator+ (const ImVec4& v1, const float num)
{
	return ImVec4{ v1.x + num, v1.y + num, v1.z + num, v1.w + num };
}

[[nodiscard]] static inline  auto operator- (const ImVec4& v1, const float num)
{
	return ImVec4{ v1.x - num, v1.y - num, v1.z - num, v1.w - num };
}

[[nodiscard]] static inline  auto operator* (const ImVec4& v1, const float num)
{
	return ImVec4{ v1.x * num, v1.y * num, v1.z * num, v1.w * num };
}

[[nodiscard]] static inline  auto operator/ (const ImVec4& v1, const float num)
{
	return ImVec4{ v1.x / num, v1.y / num, v1.z / num, v1.w / num };
}

//-------------------------------------------------------------------------------------------------------------
// ImVec2ån
//-------------------------------------------------------------------------------------------------------------

static inline void operator+= (ImVec2& v1, const ImVec2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
}

static inline void operator-= (ImVec2& v1, const ImVec2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
}

static inline void operator/= (ImVec2& v1, const ImVec2& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
}

static inline void operator*= (ImVec2& v1, const ImVec2& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
}

[[nodiscard]] static inline  auto operator+ (const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2{ v1.x + v2.x, v1.y + v2.y };
}

[[nodiscard]] static inline  auto operator- (const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2{ v1.x - v2.x, v1.y - v2.y };
}

[[nodiscard]] static inline  auto operator* (const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2{ v1.x * v2.x, v1.y * v2.y };
}

[[nodiscard]] static inline  auto operator/ (const ImVec2& v1, const ImVec2& v2)
{
	return ImVec2{ v1.x / v2.x, v1.y / v2.y };
}

static inline void operator+= (ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
}

static inline void operator-= (ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
}

static inline void operator/= (ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
}

static inline void operator*= (ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
}

[[nodiscard]] static inline  auto operator+ (const ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	return ImVec2{ v1.x + v2.x, v1.y + v2.y };
}

[[nodiscard]] static inline  auto operator- (const ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	return ImVec2{ v1.x - v2.x, v1.y - v2.y };
}

[[nodiscard]] static inline  auto operator* (const ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	return ImVec2{ v1.x * v2.x, v1.y * v2.y };
}

[[nodiscard]] static inline  auto operator/ (const ImVec2& v1, const DirectX::XMFLOAT2& v2)
{
	return ImVec2{ v1.x / v2.x, v1.y / v2.y };
}

static inline void operator+= (DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
}

static inline void operator-= (DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
}

static inline void operator/= (DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
}

static inline void operator*= (DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
}

[[nodiscard]] static inline  auto operator+ (const DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	return DirectX::XMFLOAT2{ v1.x + v2.x, v1.y + v2.y };
}

[[nodiscard]] static inline  auto operator- (const DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	return DirectX::XMFLOAT2{ v1.x - v2.x, v1.y - v2.y };
}

[[nodiscard]] static inline  auto operator* (const DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	return DirectX::XMFLOAT2{ v1.x * v2.x, v1.y * v2.y };
}

[[nodiscard]] static inline  auto operator/ (const DirectX::XMFLOAT2& v1, const ImVec2& v2)
{
	return DirectX::XMFLOAT2{ v1.x / v2.x, v1.y / v2.y };
}

static inline void operator+= (ImVec2& v1, const float num)
{
	v1.x += num;
	v1.y += num;
}

static inline void operator-= (ImVec2& v1, const float num)
{
	v1.x -= num;
	v1.y -= num;
}

static inline void operator/= (ImVec2& v1, const float num)
{
	v1.x /= num;
	v1.y /= num;
}

static inline void operator*= (ImVec2& v1, const float num)
{
	v1.x *= num;
	v1.y *= num;
}

[[nodiscard]] static inline  auto operator+ (const ImVec2& v1, const float num)
{
	return ImVec2{ v1.x + num, v1.y + num };
}

[[nodiscard]] static inline  auto operator- (const ImVec2& v1, const float num)
{
	return ImVec2{ v1.x - num, v1.y - num };
}

[[nodiscard]] static inline  auto operator* (const ImVec2& v1, const float num)
{
	return ImVec2{ v1.x * num, v1.y * num };
}

[[nodiscard]] static inline  auto operator/ (const ImVec2& v1, const float num)
{
	return ImVec2{ v1.x / num, v1.y / num };
}

//-------------------------------------------------------------------------------------------------------------
// óòï÷ä÷êîån
//-------------------------------------------------------------------------------------------------------------

static inline auto ToXMFLOAT(const ImVec4& vec)
{
	return DirectX::XMFLOAT4{ vec.x, vec.y, vec.z, vec.w };
}

static inline auto ToXMFLOAT(const ImVec2& vec)
{
	return DirectX::XMFLOAT2{ vec.x, vec.y };
}

static inline auto ToImVec(const DirectX::XMFLOAT4& vec)
{
	return ImVec4{ vec.x, vec.y, vec.z, vec.w };
}

static inline auto ToImVec(const DirectX::XMFLOAT2& vec)
{
	return ImVec2{ vec.x, vec.y };
}

struct [[maybe_unused]] ImguiHeler
{
	using VF2 = DirectX::XMFLOAT2;
	using VF3 = DirectX::XMFLOAT3;
	using VF4 = DirectX::XMFLOAT4;
	using VI2 = DirectX::XMINT2;
	using VI3 = DirectX::XMINT3;
	using VI4 = DirectX::XMINT4;

	// SliderFloat -------------------------------------------------------------------------------------------

#if true
	static inline void SliderFloat2(const char* label, float* v1, float* v2, float v_min, float v_max,
		const char* format = "%.3f", float power = 1.0f)
	{
		std::array<float*, 2u> data{ v1, v2 };

		ImGui::SliderFloat2(label, data.front(), v_min, v_max, format, power);
	}

	static inline void SliderFloat(const char* label, VF2* vf2, float v_min, float v_max,
		const char* format = "%.3f", float power = 1.0f)
	{
		std::array<float*, 2u> data{ &vf2->x, &vf2->y };

		ImGui::SliderFloat2(label, data.front(), v_min, v_max, format, power);
	}

	static inline void SliderFloat3(const char* label, float* v1, float* v2, float* v3, float v_min,
		float v_max, const char* format = "%.3f", float power = 1.0f)
	{
		std::array<float*, 3u> data{ v1, v2, v3 };

		ImGui::SliderFloat3(label, data.front(), v_min, v_max, format, power);
	}

	static inline void SliderFloat(const char* label, VF3* vf3, float v_min,
		float v_max, const char* format = "%.3f", float power = 1.0f)
	{
		std::array<float*, 3u> data{ &vf3->x, &vf3->y, &vf3->z };

		ImGui::SliderFloat3(label, data.front(), v_min, v_max, format, power);
	}

	static inline void SliderFloat4(const char* label, float* v1, float* v2, float* v3, float* v4,
		float v_min, float v_max, const char* format = "%.3f", float power = 1.0f)
	{
		std::array<float*, 4u> data{ v1, v2, v3, v4 };

		ImGui::SliderFloat4(label, data.front(), v_min, v_max, format, power);
	}

	static inline void SliderFloat(const char* label, VF4* vf4,
		float v_min, float v_max, const char* format = "%.3f", float power = 1.0f)
	{
		std::array<float*, 4u> data{ &vf4->x, &vf4->y, &vf4->z, &vf4->w };

		ImGui::SliderFloat4(label, data.front(), v_min, v_max, format, power);
	}
#endif

	// SliderInt --------------------------------------------------------------------------------------------

#if true
	static inline void SliderInt2(const char* label, int* v1, int* v2, int v_min, int v_max,
		const char* format = "%d")
	{
		std::array<int*, 2u> data{ v1, v2 };

		ImGui::SliderInt2(label, data.front(), v_min, v_max, format);
	}

	static inline void SliderInt3(const char* label, int* v1, int* v2, int* v3, int v_min, int v_max,
		const char* format = "%d")
	{
		std::array<int*, 3u> data{ v1, v2, v3 };

		ImGui::SliderInt3(label, data.front(), v_min, v_max, format);
	}

	static inline void SliderInt4(const char* label, int* v1, int* v2, int* v3, int* v4, int v_min,
		int v_max, const char* format = "%d")
	{
		std::array<int*, 4u> data{ v1, v2, v3, v4 };

		ImGui::SliderInt4(label, data.front(), v_min, v_max, format);
	}

	static inline void SliderInt2(const char* label, VI2* vi2, int v_min, int v_max,
		const char* format = "%d")
	{
		std::array<int*, 2u> data{ &vi2->x, &vi2->y };

		ImGui::SliderInt2(label, data.front(), v_min, v_max, format);
	}

	static inline void SliderInt3(const char* label, VI3* vi3, int v_min, int v_max,
		const char* format = "%d")
	{
		std::array<int*, 3u> data{ &vi3->x, &vi3->y, &vi3->z };

		ImGui::SliderInt3(label, data.front(), v_min, v_max, format);
	}

	static inline void SliderInt4(const char* label, VI4* vi4, int v_min,
		int v_max, const char* format = "%d")
	{
		std::array<int*, 4u> data{ &vi4->x, &vi4->y, &vi4->z, &vi4->w };

		ImGui::SliderInt4(label, data.front(), v_min, v_max, format);
	}
#endif

	// InputFloat --------------------------------------------------------------------------------------------

#if true
	static inline void InputFloat2(const char* label, float* v1, float* v2, const char* format = "%.3f",
		ImGuiInputTextFlags flags = 0)
	{
		std::array<float*, 2u> data{ v1, v2 };

		ImGui::InputFloat2(label, data.front(), format, flags);
	}

	static inline void InputFloat(const char* label, VF2* vf2, const char* format = "%.3f",
		ImGuiInputTextFlags flags = 0)
	{
		std::array<float*, 2u> data{ &vf2->x, &vf2->y };

		ImGui::InputFloat2(label, data.front(), format, flags);
	}

	static inline void InputFloat3(const char* label, float* v1, float* v2, float* v3,
		const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		std::array<float*, 3u> data{ v1, v2, v3 };

		ImGui::InputFloat3(label, data.front(), format, flags);
	}

	static inline void InputFloat(const char* label, VF3* vf3,
		const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		std::array<float*, 3u> data{ &vf3->x, &vf3->y, &vf3->z };

		ImGui::InputFloat3(label, data.front(), format, flags);
	}

	static inline void InputFloat4(const char* label, float* v1, float* v2, float* v3, float* v4,
		const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		std::array<float*, 4u> data{ v1, v2, v3, v4 };

		ImGui::InputFloat4(label, data.front(), format, flags);
	}

	static inline void InputFloat(const char* label, VF4* vf4,
		const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		std::array<float*, 4u> data{ &vf4->x, &vf4->y, &vf4->z, &vf4->w };

		ImGui::InputFloat4(label, data.front(), format, flags);
	}
#endif

	// InputInt --------------------------------------------------------------------------------------------

#if true
	static inline void InputInt2(const char* label, int* v1, int* v2, ImGuiInputTextFlags flags = 0)
	{
		std::array<int*, 2u> data{ v1, v2 };

		ImGui::InputInt2(label, data.front(), flags);
	}

	static inline void InputInt3(const char* label, int* v1, int* v2, int* v3, ImGuiInputTextFlags flags = 0)
	{
		std::array<int*, 3u> data{ v1, v2, v3 };

		ImGui::InputInt3(label, data.front(), flags);
	}

	static inline void InputInt4(const char* label, int* v1, int* v2, int* v3, int* v4, ImGuiInputTextFlags flags = 0)
	{
		std::array<int*, 4u> data{ v1, v2, v3, v4 };

		ImGui::InputInt4(label, data.front(), flags);
	}

	static inline void InputInt2(const char* label, VI2* vi2, ImGuiInputTextFlags flags = 0)
	{
		std::array<int*, 2u> data{ &vi2->x, &vi2->y };

		ImGui::InputInt2(label, data.front(), flags);
	}

	static inline void InputInt3(const char* label, VI3* vi3, ImGuiInputTextFlags flags = 0)
	{
		std::array<int*, 3u> data{ &vi3->x, &vi3->y, &vi3->z };

		ImGui::InputInt3(label, data.front(), flags);
	}

	static inline void InputInt4(const char* label, VI4* vi4, ImGuiInputTextFlags flags = 0)
	{
		std::array<int*, 4u> data{ &vi4->x, &vi4->y, &vi4->z, &vi4->w };

		ImGui::InputInt4(label, data.front(), flags);
	}
#endif

	// ColorEdit --------------------------------------------------------------------------------------------

#if true
	static inline void ColorEdit4(const char* label, float* r, float* g, float* b, float* a,
		ImGuiColorEditFlags flags = 0)
	{
		std::array<float*, 4u> data{ r, g, b, a };

		ImGui::ColorEdit4(label, data.front(), flags);
	}

	static inline void ColorEdit3(const char* label, float* r, float* g, float* b,
		ImGuiColorEditFlags flags = 0)
	{
		std::array<float*, 3u> data{ r, g, b };

		ImGui::ColorEdit3(label, data.front(), flags);
	}
#endif

	// Text-------------------------------------------------------------------------------------------------

#if true
	static inline void Text(
		const std::string& label, const DirectX::XMFLOAT2& contents, const std::string& format = "%.3f",
		const DirectX::XMFLOAT4& color = { 1.f, 1.f, 1.f, 1.f })
	{
		using namespace std::string_literals;

		std::string print_string{ label + "(XY) : "s };

		for (size_t i = 0, length = 2u; i < length; i++)
		{
			print_string += format;

			if (i != length - 1u)
				print_string += ", ";
		}

		ImGui::TextColored({ color.x, color.y, color.z, color.w }, print_string.data(),
			contents.x, contents.y);
	}

	static inline void Text(
		const std::string& label, const DirectX::XMFLOAT3& contents, const std::string& format = "%.3f",
		const DirectX::XMFLOAT4& color = { 1.f, 1.f, 1.f, 1.f })
	{
		using namespace std::string_literals;

		std::string print_string{ label + "(XYZ) : "s };

		for (size_t i = 0, length = 3u; i < length; i++)
		{
			print_string += format;

			if (i != length - 1u)
				print_string += ", ";
		}

		ImGui::TextColored({ color.x, color.y, color.z, color.w }, print_string.data(),
			contents.x, contents.y, contents.z);
	}

	static inline void Text(
		const std::string& label, const DirectX::XMFLOAT4& contents, const std::string& format = "%.3f",
		const DirectX::XMFLOAT4& color = { 1.f, 1.f, 1.f, 1.f })
	{
		using namespace std::string_literals;

		std::string print_string{ label + "(XYZ) : "s };

		for (size_t i = 0, length = 4u; i < length; i++)
		{
			print_string += format;

			if (i != length - 1u)
				print_string += ", ";
		}

		ImGui::TextColored({ color.x, color.y, color.z, color.w }, print_string.data(),
			contents.x, contents.y, contents.z, contents.w);
	}
#endif
};