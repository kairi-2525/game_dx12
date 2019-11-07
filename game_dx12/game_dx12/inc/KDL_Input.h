#pragma once
#include "KDL_Math.h"
#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>
#include <Xinput.h>
#include <vector>

namespace KDL
{
	class Window;
	using namespace MATH;

	enum MOUSE_INPUTS
	{
		leftButton,
		middleButton,
		rightButton,
		xButton1,
		xButton2,
	};

	enum GAMEPAD_INPUT
	{
		PAD_UP = 0b0000000000000001,
		PAD_DOWN = 0b0000000000000010,
		PAD_LEFT = 0b0000000000000100,
		PAD_RIGHT = 0b0000000000001000,
		PAD_START = 0b0000000000010000,
		PAD_BACK = 0b0000000000100000,
		PAD_L_STICK = 0b0000000001000000,
		PAD_R_STICK = 0b0000000010000000,
		PAD_LB = 0b0000000100000000,
		PAD_RB = 0b0000001000000000,
		PAD_LT = 0b0000010000000000,
		PAD_RT = 0b0000100000000000,
		PAD_A = 0b0001000000000000,
		PAD_B = 0b0010000000000000,
		PAD_X = 0b0100000000000000,
		PAD_Y = 0b1000000000000000,
	};

	using KEY_INPUTS = DirectX::Keyboard::Keys;
	using MOUSE_MODE = DirectX::Mouse::Mode;

	class Input
	{
		static const int STICK_L = 0;
		static const int STICK_R = 1;
	private:
		struct X_Pad
		{
			static const int STICK_MAX = 32767;
			static const int STICK_MIN = -32768;

			static const int STICK_DEAD_MAX = 120 + 7000;
			static const int STICK_DEAD_MIN = 120 - 7000;

			bool connect = false;
			DirectX::XMFLOAT2 state_stick[2] = { { 0, 0 }, { 0, 0 } };
			DirectX::XMFLOAT2 log_stick[2];
			XINPUT_STATE state = {};
			XINPUT_STATE log;
		};
	private:
		std::unique_ptr<DirectX::Keyboard> key;
		std::unique_ptr<DirectX::Mouse> mouse;
		DirectX::Keyboard::State key_log;
		DirectX::Keyboard::State key_state = {};
		DirectX::Mouse::State mouse_log;
		DirectX::Mouse::State mouse_state = {};
		X_Pad x_pads[XUSER_MAX_COUNT];
		size_t active_pads;
		
	private:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		void XPadUpdate();
	public:
		Input(HWND hwnd)
		{
			key = std::make_unique<DirectX::Keyboard>();
			mouse = std::make_unique<DirectX::Mouse>();
			XPadUpdate();
		}
		void Update(HWND hwnd);

		const DirectX::Keyboard::State& GetKey() const { return key_state; }
		const DirectX::Keyboard::State& GetKeyLog() const { return key_log; }
		const DirectX::Mouse::State& GetMouse() const { return mouse_state; }
		const DirectX::Mouse::State& GetMouseLog() const { return mouse_log; }
		const XINPUT_STATE* GetXPad(size_t num) const { if (num < XUSER_MAX_COUNT) if (x_pads[num].connect) return &x_pads[num].state; return nullptr; }
		const XINPUT_STATE* GetXPadLog(size_t num) const { if (num < XUSER_MAX_COUNT) if (x_pads[num].connect) return &x_pads[num].state; return nullptr; }
		void SetMouseMode(MOUSE_MODE mode) { mouse->SetMode(mode); mouse_state.positionMode = mode; }
		//void SetWindow(HWND hwnd) { mouse->SetWindow(hwnd); }
		//const DIJOYSTATE* GetDPad();

		bool IsHitKey(const KEY_INPUTS key) const
		{
			return GetKey().IsKeyDown(key);
		}
		bool IsTrgKey(const KEY_INPUTS key) const
		{
			return GetKey().IsKeyDown(key) && !GetKeyLog().IsKeyDown(key);
		}
		bool IsHitAnyKey() const
		{
			return !GetKey().IsKeyDown(DirectX::Keyboard::Keys::None);
		}
		bool IsHitMouse(const MOUSE_INPUTS button) const
		{
			switch (button)
			{
				case MOUSE_INPUTS::leftButton:
					return GetMouse().leftButton; break;
				case MOUSE_INPUTS::rightButton:
					return GetMouse().rightButton; break;
				case MOUSE_INPUTS::middleButton:
					return GetMouse().middleButton; break;
				case MOUSE_INPUTS::xButton1:
					return GetMouse().xButton1; break;
				case MOUSE_INPUTS::xButton2:
					return GetMouse().xButton2; break;
			}
			return false;
		}
		bool IsTrgMouse(const MOUSE_INPUTS button) const
		{
			switch (button)
			{
				case MOUSE_INPUTS::leftButton:
					return GetMouse().leftButton && !GetMouseLog().leftButton; break;
				case MOUSE_INPUTS::rightButton:
					return GetMouse().rightButton && !GetMouseLog().rightButton; break;
				case MOUSE_INPUTS::middleButton:
					return GetMouse().middleButton && !GetMouseLog().middleButton; break;
				case MOUSE_INPUTS::xButton1:
					return GetMouse().xButton1 && !GetMouseLog().xButton1; break;
				case MOUSE_INPUTS::xButton2:
					return GetMouse().xButton2 && !GetMouseLog().xButton2; break;
			}
			return false;
		}
		void GetMousePos(int* out_x, int* out_y) const
		{
			assert(out_x && "out_x‚ªnullptr");
			assert(out_y && "out_y‚ªnullptr");
			*out_x = GetMouse().x;
			*out_y = GetMouse().y;
		}
		INT2 GetMousePos() const
		{
			return { GetMouse().x, GetMouse().y };
		}
		void GetMousePos(INT2* out_pos) const
		{
			assert(out_pos && "out_pos‚ªnullptr");
			out_pos->x = GetMouse().x;
			out_pos->y = GetMouse().y;
		}
		void GetMouseLogPos(int* out_x, int* out_y) const
		{
			assert(out_x && "out_x‚ªnullptr");
			assert(out_y && "out_y‚ªnullptr");
			*out_x = GetMouseLog().x;
			*out_y = GetMouseLog().y;
		}
		INT2 GetMouseLogPos() const
		{
			return { GetMouseLog().x, GetMouseLog().y };
		}
		void GetMouseLogPos(INT2* out_pos) const
		{
			assert(out_pos && "out_pos‚ªnullptr");
			out_pos->x = GetMouseLog().x;
			out_pos->y = GetMouseLog().y;
		}
		bool IsHitPad(const UINT pad_num, const GAMEPAD_INPUT button) const
		{
			const XINPUT_STATE* state = GetXPad(pad_num);
			if (state)
			{
				switch (button)
				{
					case GAMEPAD_INPUT::PAD_LT:
						return state->Gamepad.bLeftTrigger > 0;
					case GAMEPAD_INPUT::PAD_RT:
						return state->Gamepad.bRightTrigger > 0;
					default:
						return (state->Gamepad.wButtons & button) != 0;
				}
			}
			return false;
		}
		bool IsTrgPad(const UINT pad_num, const GAMEPAD_INPUT button) const
		{
			const XINPUT_STATE* state = GetXPad(pad_num);
			const XINPUT_STATE* log_state = GetXPadLog(pad_num);
			if (state && log_state)
			{
				switch (button)
				{
					case GAMEPAD_INPUT::PAD_LT:
						return state->Gamepad.bLeftTrigger > 0 && log_state->Gamepad.bLeftTrigger == 0;
					case GAMEPAD_INPUT::PAD_RT:
						return state->Gamepad.bRightTrigger > 0 && log_state->Gamepad.bRightTrigger == 0;
					default:
						return (state->Gamepad.wButtons & button) != 0 && (log_state->Gamepad.wButtons & button) == 0;
				}
			}
			return false;
		}
		int GetMouseValue() const
		{
			return GetMouseLog().scrollWheelValue - GetMouse().scrollWheelValue;
		}
		int GetMouseValueNow() const
		{
			return GetMouse().scrollWheelValue;
		}
		int GetMouseValueLog() const
		{
			return GetMouseLog().scrollWheelValue;
		}
		MOUSE_MODE GetMouseMode() const
		{
			return GetMouse().positionMode;
		}
	};
}