#pragma once

#include "App.h"
#include <mutex>
#include <filesystem>
#include "Pipeline_State_Manager.h"

namespace KDL
{
	namespace DX12
	{
		using namespace MATH;

		class Mesh_Base
		{
		protected:
			struct Vertex
			{
				FLOAT3 position;
				FLOAT3 normal;
				FLOAT2 texture;
			};
			struct Material
			{
				DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
				ComPtr<ID3D12Resource1> texture;
				D3D12_RESOURCE_DESC tex_desc;
				std::filesystem::path tex_file_pass;
				std::vector<CD3DX12_GPU_DESCRIPTOR_HANDLE> srv;
			};
			struct Subset
			{
				UINT index_start = 0;
				UINT index_count = 0;
				Material diffuse;
			};
			struct Mesh
			{
				std::vector<Subset> subsets;
				DirectX::XMFLOAT4X4 global_transform;
				ComPtr<ID3D12Resource1> index_buffer;
				D3D12_INDEX_BUFFER_VIEW index_buffer_view;
				ComPtr<ID3D12Resource1> vertex_buffer;
				D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
			};
			struct BufferData
			{
				ComPtr<ID3D12Resource1> buffer;
				UINT8* buffer_data;
			};
			enum class DESCRIPTOR_BASE : UINT
			{
				//TEXTURE_SRV = 0,
				CONSTANT_BUFFER = 0,
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
			static inline const DirectX::XMFLOAT4X4 CORDINATE_CONVERSION = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1 };
		private:
			bool m_first_command = true;
		protected:
			size_t m_file_size;
			size_t m_load_size;
			std::mutex load_size_mutex;
			std::filesystem::path m_model_file_pass;

			std::vector<Mesh> m_meshes;
			ComPtr<ID3D12RootSignature> m_root_signature;
			ComPtr<ID3DBlob> m_vs_blob, m_ps_blob;
			Pipeline_State_Manager m_ps_mgr;
			
			D3D12_GPU_DESCRIPTOR_HANDLE m_sampler;
			std::vector<ComPtr<ID3D12DescriptorHeap>> m_heaps_srv_cbv;
			ComPtr<ID3D12DescriptorHeap> m_heap_sampler;
			std::vector<BufferData> m_constant_buffers;
			UINT m_srv_cbv_descriptor_size;
			UINT m_total_material_counts;

			UINT m_last_frame_num = 1u;
			UINT m_render_count = 0u;
			std::mutex m_count_mutex;

			UINT m_buffer_stock_size;
		protected:
			Mesh_Base() = default;
			virtual HRESULT LoadModelAndCreateBuffers(App* app, UINT buffer_stock_size) = 0;
			virtual const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc() = 0;
			virtual HRESULT CompileShaders() = 0;
			HRESULT AddConstantBuffer(ComPtr<ID3D12Device>& device, const UINT add_size, const UINT buffer_size, const UINT frame_buffer_num);
			bool IsFirstCommand() { if (m_first_command) { m_first_command = false; return true; } return false; }
		public:
			~Mesh_Base() = default;
			Mesh_Base(App* app, const std::filesystem::path& path, UINT buffer_stock_size = 100u)
			{ Init(app, path, buffer_stock_size); }
			HRESULT Init(App* app, const std::filesystem::path& path, UINT buffer_stock_size);
		};

		class Mesh_FBX : public Mesh_Base
		{
		private:
			static inline const std::vector<D3D12_INPUT_ELEMENT_DESC> INPUT_ELEMENT_DESC =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, texture), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
		private:
			HRESULT LoadModelAndCreateBuffers(App* app, UINT buffer_stock_size) override;
			HRESULT CreateIndexBuffers(ComPtr<ID3D12Device>& device, Mesh* mesh, UINT64 buffer_size, const void* initial_data);
			HRESULT CreateVertexBuffers(ComPtr<ID3D12Device>& device, Mesh* mesh, UINT64 buffer_size, const void* initial_data);
			const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc() override { return INPUT_ELEMENT_DESC; };
			HRESULT CompileShaders() override;
		public:
			Mesh_FBX(App* app, const std::filesystem::path& path, UINT buffer_stock_size = 100u)
			{ Init(app, path, buffer_stock_size); }
			HRESULT AddCommand(
				ID3D12GraphicsCommandList* command_list,
				App* app,
				const DirectX::XMFLOAT4X4& wvp,
				const DirectX::XMFLOAT4X4& world,
				const DirectX::XMFLOAT4& light_direction,
				const DirectX::XMFLOAT4& material_color,
				int blend_mode = static_cast<int>(BLEND_STATE::LAST),
				bool wire_frame = false,
				bool z_test = true, bool z_write = true
			);
		};
	}
}