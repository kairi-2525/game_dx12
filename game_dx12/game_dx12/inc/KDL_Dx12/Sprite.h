#pragma once

#include "App.h"
#include <unordered_map>
#include <mutex>
#include <filesystem>
#include "Pipeline_State_Manager.h"

namespace KDL
{
	namespace DX12
	{
		using namespace MATH;

		class Sprite
		{
		protected:

			struct Vertex
			{
				FLOAT3 pos;
				FLOAT4 color;
			};

			struct VertexData
			{
				ComPtr<ID3D12Resource1> buffer;
				UINT8* buffer_data;
			};

		private:
			std::vector<size_t> m_instance_sizes;
			bool m_first_command = true;
		protected:
			size_t GetInstanceSize(UINT frame_num)
			{
				assert(m_instance_sizes.size() > frame_num);
				return m_instance_sizes[frame_num];
			}
			virtual HRESULT CompileShaders();
			virtual HRESULT CreateRootSignature(ComPtr<ID3D12Device>& device);
			virtual HRESULT CreateIndexBuffers(ComPtr<ID3D12Device>& device) = 0;
			virtual const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc() = 0;
			Sprite() = default;
			HRESULT Init(App* app, UINT vertex_count, UINT buffer_stock_size, UINT vertex_size = sizeof(Vertex));
			bool IsFirstCommand() { if (m_first_command) { m_first_command = false; return true; } return false; }
		protected:
			std::vector<VertexData> m_vertex_data;
			ComPtr<ID3D12Resource1> m_index_buffer;
			D3D12_INDEX_BUFFER_VIEW m_index_buffer_view;
			UINT m_index_count;

			ComPtr<ID3DBlob> m_vs_blob, m_ps_blob;
			ComPtr<ID3D12RootSignature> m_root_signature;
			Pipeline_State_Manager m_ps_mgr;

			static inline const UINT BUFFER_SAVE_SIZE = 100u;
			std::vector<UINT> m_render_num_log;
			UINT m_buffer_stock_size;
			UINT m_last_frame_num = 1u;
			UINT m_render_count = 0u;
			std::mutex m_count_mutex;
		public:
			virtual ~Sprite() = default;
			HRESULT AddVertexBuffer(ComPtr<ID3D12Device>& device, const UINT add_size, const UINT buffer_size, const UINT vertex_struct_size, const UINT frame_buffer_num);
		};

		class Sprite_Box : public Sprite
		{
		private:
			static inline const UINT VERTEX_COUNT = 4u;
			static inline const std::vector<D3D12_INPUT_ELEMENT_DESC> INPUT_ELEMENT_DESC =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
		protected:
			virtual HRESULT CreateIndexBuffers(ComPtr<ID3D12Device>& device) override;
			virtual const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc() override { return Sprite_Box::INPUT_ELEMENT_DESC; }
			Sprite_Box() = default;
			HRESULT Init(App* app, UINT stock_size = 100u, UINT vertex_size = sizeof(Vertex));
		public:
			Sprite_Box(App* app, UINT stock_size = 100u);
			virtual ~Sprite_Box() = default;
			virtual HRESULT AddCommand(ID3D12GraphicsCommandList* command_list, App* app,
				const FLOAT2& pos, const FLOAT2& size, const FLOAT2& center, const FLOAT2& scale, float radian_angle,
				const COLOR4F& left_top, const COLOR4F& right_top, const COLOR4F& left_bottom, const COLOR4F& right_bottom,
				int blend_mode = static_cast<int>(BLEND_STATE::LAST),
				bool wire_frame = false,
				bool z_test = false, bool z_write = false);
		};

		class Sprite_Image : public Sprite_Box
		{
		private:
			enum
			{
				TextureSrvDescriptorBase = 0,
				// サンプラーは別ヒープなので先頭を使用
				SamplerDescriptorBase = 0,
			};
			struct VertexTex
			{
				FLOAT3 pos;
				FLOAT4 color;
				FLOAT2 tex;
			};
		private:
			static inline const std::vector<D3D12_INPUT_ELEMENT_DESC> INPUT_ELEMENT_DESC =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexTex, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexTex, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexTex, tex), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
		private:
			App* const m_p_app;
			std::filesystem::path m_tex_file_pass;
			ComPtr<ID3D12Resource1> m_texture;
			D3D12_RESOURCE_DESC m_tex_desc;

			D3D12_GPU_DESCRIPTOR_HANDLE m_sampler;
			D3D12_GPU_DESCRIPTOR_HANDLE m_srv;
			ComPtr<ID3D12DescriptorHeap> m_heap_srv;
			ComPtr<ID3D12DescriptorHeap> m_heap_sampler;
		private:
			HRESULT CompileShaders() override;
			HRESULT CreateRootSignature(ComPtr<ID3D12Device>& device) override;
			const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc() override { return Sprite_Image::INPUT_ELEMENT_DESC; }
			HRESULT Init(App* app, const std::filesystem::path& image_path, UINT stock_size = 100u);
		public:
			~Sprite_Image();
			Sprite_Image(App* app, const std::filesystem::path& image_path, UINT stock_size = 100u);
			HRESULT AddCommand(ID3D12GraphicsCommandList* command_list, App* app,
				const FLOAT2& pos, const FLOAT2& size, const FLOAT2& center, const FLOAT2& scale, float radian_angle,
				const COLOR4F& left_top, const COLOR4F& right_top, const COLOR4F& left_bottom, const COLOR4F& right_bottom,
				int blend_mode = static_cast<int>(BLEND_STATE::LAST),
				bool wire_frame = false,
				bool z_test = false, bool z_write = true) override {
				return AddCommand(command_list, app, pos, size, center, { 0, 0 }, GetSize(), scale, radian_angle,
					left_top, right_top, left_bottom, right_bottom, blend_mode, wire_frame, z_test);
			}
			HRESULT AddCommand(ID3D12GraphicsCommandList* command_list, App* app,
				const FLOAT2& pos, const FLOAT2& size, const FLOAT2& center,
				const FLOAT2& tex_pos, const FLOAT2& tex_size, const FLOAT2& scale, float radian_angle,
				const COLOR4F& left_top, const COLOR4F& right_top, const COLOR4F& left_bottom, const COLOR4F& right_bottom,
				int blend_mode = static_cast<int>(BLEND_STATE::LAST),
				bool wire_frame = false,
				bool z_test = false, bool z_write = true);
			HRESULT AddTextCommands(ID3D12GraphicsCommandList* command_list, App* app,
				const std::string& text,
				const FLOAT2& pos, const FLOAT2& size, const FLOAT2& center,
				const FLOAT2& space,
				const FLOAT2& scale, float radian_angle,
				const COLOR4F& left_top, const COLOR4F& right_top, const COLOR4F& left_bottom, const COLOR4F& right_bottom,
				int blend_mode = static_cast<int>(BLEND_STATE::LAST),
				bool wire_frame = false,
				bool z_test = false, bool z_write = true);
			HRESULT AddTextCommands(ID3D12GraphicsCommandList* command_list, App* app,
				const std::string& text,
				const FLOAT2& pos, const FLOAT2& size, const FLOAT2& center,
				const FLOAT2& space,
				const FLOAT2& scale, float radian_angle,
				const COLOR4F& furst_left_top, const COLOR4F& furst_right_top, const COLOR4F& furst_left_bottom, const COLOR4F& furst_right_bottom,
				const COLOR4F& last_left_top, const COLOR4F& last_right_top, const COLOR4F& last_left_bottom, const COLOR4F& last_right_bottom,
				int blend_mode = static_cast<int>(BLEND_STATE::LAST),
				bool wire_frame = false,
				bool z_test = false, bool z_write = true);
			static size_t GetTextWidth(const std::string& text, float size_w, float scale_w, float space)
			{
				return static_cast<size_t>(text.size() * (size_w + space) * scale_w);
			}
			template <class T = FLOAT>
			Number2<T> GetSize() { return { static_cast<T>(m_tex_desc.Width), static_cast<T>(m_tex_desc.Height) }; }
		};
	}
}