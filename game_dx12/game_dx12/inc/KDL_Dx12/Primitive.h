#pragma once

#include "App.h"
#include <mutex>
#include <filesystem>
#include "Pipeline_State_Manager.h"

namespace KDL
{
	namespace DX12
	{
		class Geometric_Base
		{
		protected:
			struct Vertex
			{
				FLOAT3 position;
				FLOAT3 normal;
				FLOAT2 texture;
			};
			struct BufferData
			{
				ComPtr<ID3D12Resource1> data;
				UINT8* view;
			};
			enum class DESCRIPTOR_BASE : UINT
			{
				TEXTURE_SRV = 0,
				CONSTANT_BUFFER = 1,
				// サンプラーは別ヒープなので先頭を使用
				SAMPLER = 0
			};
			struct ShaderParameterBase
			{
				DirectX::XMFLOAT4X4 world_view_projection;
				DirectX::XMFLOAT4X4 world;
				DirectX::XMFLOAT4  material_color;
				DirectX::XMFLOAT4 light_direction;
			};
			struct ShaderParameters : public ShaderParameterBase
			{
				float plov[(((sizeof(ShaderParameterBase) + 255) & ~255) - sizeof(ShaderParameterBase)) / sizeof(float)];
			};
		protected:
			static inline const std::vector<D3D12_INPUT_ELEMENT_DESC> INPUT_ELEMENT_DESC =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, texture), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
		protected:
			std::filesystem::path m_tex_file_pass;
			ComPtr<ID3D12Resource1> m_texture;
			D3D12_RESOURCE_DESC m_tex_desc;

			ComPtr<ID3D12RootSignature> m_root_signature;
			ComPtr<ID3DBlob> m_vs_blob, m_ps_blob;
			Pipeline_State_Manager m_ps_mgr;

			std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_srv;
			D3D12_GPU_DESCRIPTOR_HANDLE m_sampler;
			std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_cbv;
			std::vector<ComPtr<ID3D12DescriptorHeap>> m_heaps_srv_cbv;
			ComPtr<ID3D12DescriptorHeap> m_heap_sampler;
			std::vector<BufferData> m_constant_buffers;
			ComPtr<ID3D12Resource1> m_index_buffer;
			D3D12_INDEX_BUFFER_VIEW m_index_buffer_view;
			ComPtr<ID3D12Resource1> m_vertex_buffer;
			D3D12_VERTEX_BUFFER_VIEW m_vertex_buffer_view;
			UINT m_srv_cbv_descriptor_size;

			UINT m_last_frame_num = 1u;
			UINT m_render_count = 0u;
			std::mutex m_count_mutex;

			UINT m_buffer_stock_size;
		private:
			bool first_command = true;
		protected:
			Geometric_Base() = default;
			virtual ~Geometric_Base() = default;
			bool IsFirstCommand() { if (first_command) { first_command = false; return true; } return false; }
			static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc() { return INPUT_ELEMENT_DESC; };
		};

		class Geometric_Board : public Geometric_Base
		{
		private:
		HRESULT Init(App* app, const std::filesystem::path& path, UINT buffer_stock_size);
		public:
			Geometric_Board(App* app, const std::filesystem::path& path = App::DUMMY_TEXTURE_NAME, UINT buffer_stock_size = 100u)
			{
				Init(app, path, buffer_stock_size);
			}
			HRESULT AddCommand(
				ID3D12GraphicsCommandList* command_list,
				App& app,
				const DirectX::XMFLOAT4X4& wvp,
				const DirectX::XMFLOAT4X4& world,
				const DirectX::XMFLOAT4& light_direction,
				const DirectX::XMFLOAT4& material_color,
				int blend_mode = static_cast<int>(BLEND_STATE::ALPHA),
				bool wire_frame = false,
				bool z_test = true
			);
		};

		class Geometric_Board_S : public Geometric_Base
		{
		private:
			struct ShaderParameterTexBase : public ShaderParameterBase
			{
				FLOAT2 tex_scale;
				FLOAT2 tex_pos;
			};
			struct ShaderParametersTex : public ShaderParameterTexBase
			{
				float plov[(((sizeof(ShaderParameterTexBase) + 255) & ~255) - sizeof(ShaderParameterTexBase)) / sizeof(float)];
			};
		private:
		HRESULT Init(App* app, const std::filesystem::path& path, UINT buffer_stock_size);
		public:
			Geometric_Board_S(App* app, const std::filesystem::path& path = App::DUMMY_TEXTURE_NAME, UINT buffer_stock_size = 100u)
			{
				Init(app, path, buffer_stock_size);
			}
			HRESULT AddCommand(
				ID3D12GraphicsCommandList* command_list,
				App& app,
				const DirectX::XMFLOAT4X4& wvp,
				const DirectX::XMFLOAT4X4& world,
				const DirectX::XMFLOAT4& light_direction,
				const DirectX::XMFLOAT4& material_color,
				const FLOAT2& tex_pos,
				const FLOAT2& tex_scale,
				int blend_mode = static_cast<int>(BLEND_STATE::ALPHA),
				bool wire_frame = false,
				bool z_test = true
			);
		};
	}
}