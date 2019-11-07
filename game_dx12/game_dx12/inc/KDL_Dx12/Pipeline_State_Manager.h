#pragma once

#include "App.h"
#include <unordered_map>

namespace KDL
{
	namespace DX12
	{
		enum class BLEND_STATE : int
		{
			DEFAULT,	//最後に設定されていた値を使う
			NONE,
			ALPHA,
			ADD,
			SUBTRACT,
			REPLEASE,
			MULTIPLY,
			LIGHTEN,
			DARKEN,
			SCREEN
		};

		struct Pipeline_Desc_State
		{
			int m_blend_mode;
			bool m_wire_frame;
			bool m_z_test;
			bool operator==(const Pipeline_Desc_State& state) const 
			{
				return
					m_blend_mode == state.m_blend_mode
					&&
					m_wire_frame == state.m_wire_frame
					;
			}
			bool operator!=(const Pipeline_Desc_State& state) const 
			{
				return
					m_blend_mode != state.m_blend_mode
					||
					m_wire_frame != state.m_wire_frame
					;
			}

			struct Hash {
				typedef std::size_t result_type;

				std::size_t operator()(const Pipeline_Desc_State& key) const
				{
					std::string bytes(reinterpret_cast<const char*>(&key), sizeof(Pipeline_Desc_State));
					return std::hash<std::string>()(bytes);
				}
			};
		};

		class Pipeline_State_Manager
		{
		private:
			std::unordered_map < Pipeline_Desc_State, ComPtr<ID3D12PipelineState>, Pipeline_Desc_State::Hash> m_pipelines;
			Pipeline_Desc_State m_desc_state;
			bool m_desc_changed;
			bool m_inited = false;
		public:
			D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc{};
		private:
			bool SetBlendMode(BLEND_STATE mode);
			bool SetWireFrameToggle(bool wire_frame);
			bool SetZTestToggle(bool z_test);
		public:
			ID3D12PipelineState* GetPipelineState(ID3D12Device* device, BLEND_STATE mode, bool wire_frame, bool z_test)
			{
				return GetPipelineState(device, static_cast<int>(mode), wire_frame, z_test);
			}
			ID3D12PipelineState* GetPipelineState(ID3D12Device* device, int mode, bool wire_frame, bool z_test);
			HRESULT SetPipelineState(ID3D12Device* device, BLEND_STATE mode, bool wire_frame, bool z_test)
			{
				return SetPipelineState(device, static_cast<int>(mode), wire_frame, z_test);
			}
			HRESULT SetPipelineState(ID3D12Device* device, int mode, bool wire_frame, bool z_test)
			{
				Pipeline_Desc_State state;
				state.m_blend_mode = mode;
				state.m_wire_frame = wire_frame;
				state.m_z_test = z_test;
				return SetPipelineState(device, state);
			}
			HRESULT SetPipelineState(ID3D12Device* device, const Pipeline_Desc_State& state);
		};
	}
}