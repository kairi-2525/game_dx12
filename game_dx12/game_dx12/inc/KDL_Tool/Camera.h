#pragma once

#include "../KDL_Math.h"

namespace KDL
{
	namespace TOOL
	{
		class Camera
		{
		private:
			DirectX::XMMATRIX m_view, m_projection;
			DirectX::XMFLOAT3 m_position, m_target, m_up;
			float m_fov_angle, m_aspect_ratio;
			MATH::FLOAT2 m_visible_range;
			bool m_changed_view = true;
			bool m_changed_projection = true;
#ifdef _DEBUG
			bool m_initialized_view_parameters = false;
			bool m_initialized_projection_parameters = false;
#endif
		private:
#ifdef _DEBUG
			void CheckDebugView() const { assert(m_initialized_view_parameters && "Viewを個別のパラメーターで初期化していないためパラメーターを取得できません。"); }
			void CheckDebugProjection() const { assert(m_initialized_projection_parameters && "Projectionを個別のパラメーターで初期化していないためパラメーターを取得できません。"); }
			void SetDebugViewFlg(bool flg) { m_initialized_view_parameters = flg; }
			void SetDebugProjectionFlg(bool flg) { m_initialized_projection_parameters = flg; }
#else
			void CheckDebugView() const {}
			void CheckDebugProjection() const {}
			void SetDebugViewFlg(bool flg) {}
			void SetDebugProjectionFlg(bool flg) {}
#endif
		public:
			//個別のパラメーターで行列を初期化します。
			Camera(
				const DirectX::XMFLOAT3& pos,
				const DirectX::XMFLOAT3& target,
				const DirectX::XMFLOAT3& up,
				float fov_angle,
				float aspect_ratio,
				const MATH::FLOAT2& visible_range
			)
			{
				SetView(pos, target, up);
				SetProjection(fov_angle, aspect_ratio, visible_range);
				UpdateView();
				UpdateProjection();
				SetDebugViewFlg(true);
				SetDebugProjectionFlg(true);
			}
			//直接行列で初期化します。
			Camera(
				const DirectX::XMMATRIX& view,
				const DirectX::XMMATRIX& projection
			)
			{
				SetView(view);
				SetProjection(projection);
			}
			//view行列のみ更新します。
			void UpdateView()
			{
				m_view = DirectX::XMMatrixLookAtLH(
					DirectX::XMLoadFloat3(&m_position),
					DirectX::XMLoadFloat3(&m_target),
					DirectX::XMLoadFloat3(&m_up)
				);
				m_changed_view = false;
				SetDebugViewFlg(true);
			}
			//Projection行列のみ更新します。
			void UpdateProjection()
			{
				m_projection = DirectX::XMMatrixPerspectiveFovLH(
					m_fov_angle,
					m_aspect_ratio,
					m_visible_range.min,
					m_visible_range.max
				);
				m_changed_projection = false;
				SetDebugProjectionFlg(true);
			}
			//全ての行列を更新します。
			void Update() { UpdateView(); UpdateProjection(); }
			//World行列からWorldViewProjection行列を作成します。
			void CreateWorldViewProjection(DirectX::XMFLOAT4X4* out_matrix_p, const DirectX::XMMATRIX& world) const
			{
				assert(out_matrix_p && "out_matrix_pをnullptrにしないでください。");
				DirectX::XMStoreFloat4x4(out_matrix_p, world * m_view * m_projection);
			}
			//World行列からWorldViewProjection行列を作成します。(パラメーターが更新されていた場合行列も更新します)
			void CreateUpdateWorldViewProjection(DirectX::XMFLOAT4X4* out_matrix_p, const DirectX::XMMATRIX& world)
			{
				if (m_changed_view) UpdateView();
				if (m_changed_projection) UpdateProjection();
				CreateWorldViewProjection(out_matrix_p, world);
			}
		public: //Setter(行列)
			//各パラメーターからView行列を更新します。
			void SetView( const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
			{
				m_position = pos;
				m_target = target;
				m_up = up;
				UpdateView();
			}
			//View行列を直接更新します。この関数を使用した場合、各パラメーターの取得ができなくなります。
			void SetView(const DirectX::XMMATRIX& view) { m_view = view; SetDebugViewFlg(false); }
			//各パラメーターからProjection行列を更新します。
			void SetProjection( float fov_angle, float aspect_ratio, const MATH::FLOAT2& visible_range)
			{
				m_fov_angle = fov_angle;
				m_aspect_ratio = aspect_ratio;
				m_visible_range = visible_range;
				UpdateProjection();
			}
			//Projection行列を直接更新します。この関数を使用した場合、各パラメーターの取得ができなくなります。
			void SetProjection(const DirectX::XMMATRIX& projection) { m_projection = projection; SetDebugProjectionFlg(false); }
		public: //Setter(パラメーター)
			//個別にpositionを設定します。
			void SetPosition(const DirectX::XMFLOAT3& position) { m_changed_view = true; m_position = position; };
			//個別にtargetを設定します。
			void SetTarget(const DirectX::XMFLOAT3& target) { m_changed_view = true; m_target = target; };
			//個別にupを設定します。
			void SetUp(const DirectX::XMFLOAT3& up) { m_changed_view = true; m_up = up; };
			//個別にfov_angleを設定します。
			void SetFovAngle(float fov_angle) { m_changed_projection = true; m_fov_angle = fov_angle; }
			//個別にaspect_ratioを設定します。
			void SetAspectRatio(float aspect_ratio) { m_changed_projection = true; m_aspect_ratio = aspect_ratio; }
			//個別にVisibleRangeを設定します。
			void SetVisibleRange(const MATH::FLOAT2& visible_range) { m_changed_projection = true; m_visible_range = visible_range; }
		public: //Getter(行列)
			//view行列を取得できます。
			const DirectX::XMMATRIX& GetView() const { return m_view; }
			//view行列を取得できます。(パラメーターが更新されていた場合行列も更新します)
			const DirectX::XMMATRIX& GetUpdateView() { if (m_changed_view) UpdateView(); return m_view; }
			//projection行列を取得できます。
			const DirectX::XMMATRIX& GetProjection() const { return m_view; }
			//projection行列を取得できます。(パラメーターが更新されていた場合行列も更新します)
			const DirectX::XMMATRIX& GetUpdateProjection() { if (m_changed_projection) UpdateProjection(); return m_projection; }
		public: //Getter(パラメーター)
			//直接行列を初期化していない場合position(eye)を取得できます。
			const DirectX::XMFLOAT3& GetPosition() const { CheckDebugView(); return m_position; };
			//直接行列を初期化していない場合target(focus)を取得できます。
			const DirectX::XMFLOAT3& GetTarget() const { CheckDebugView(); return m_target; };
			//直接行列を初期化していない場合upを取得できます。
			const DirectX::XMFLOAT3& GetUp() const { CheckDebugView(); return m_up; };
			//直接行列を初期化していない場合fov_angleを取得できます。
			float GetFovAngle() const { CheckDebugProjection(); return m_fov_angle; }
			//直接行列を初期化していない場合aspect_ratioを取得できます。
			float GetAspectRatio() const { CheckDebugProjection(); return m_aspect_ratio; }
			//直接行列を初期化していない場合visible_rangeを取得できます。
			const MATH::FLOAT2& GetVisibleRange() const { CheckDebugProjection(); return m_visible_range; }
		};
	}
}