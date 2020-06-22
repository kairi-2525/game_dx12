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
			void CheckDebugView() const { assert(m_initialized_view_parameters && "View���ʂ̃p�����[�^�[�ŏ��������Ă��Ȃ����߃p�����[�^�[���擾�ł��܂���B"); }
			void CheckDebugProjection() const { assert(m_initialized_projection_parameters && "Projection���ʂ̃p�����[�^�[�ŏ��������Ă��Ȃ����߃p�����[�^�[���擾�ł��܂���B"); }
			void SetDebugViewFlg(bool flg) { m_initialized_view_parameters = flg; }
			void SetDebugProjectionFlg(bool flg) { m_initialized_projection_parameters = flg; }
#else
			void CheckDebugView() const {}
			void CheckDebugProjection() const {}
			void SetDebugViewFlg(bool flg) {}
			void SetDebugProjectionFlg(bool flg) {}
#endif
		public:
			//�ʂ̃p�����[�^�[�ōs������������܂��B
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
			//���ڍs��ŏ��������܂��B
			Camera(
				const DirectX::XMMATRIX& view,
				const DirectX::XMMATRIX& projection
			)
			{
				SetView(view);
				SetProjection(projection);
			}
			//view�s��̂ݍX�V���܂��B
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
			//Projection�s��̂ݍX�V���܂��B
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
			//�S�Ă̍s����X�V���܂��B
			void Update() { UpdateView(); UpdateProjection(); }
			//World�s�񂩂�WorldViewProjection�s����쐬���܂��B
			void CreateWorldViewProjection(DirectX::XMFLOAT4X4* out_matrix_p, const DirectX::XMMATRIX& world) const
			{
				assert(out_matrix_p && "out_matrix_p��nullptr�ɂ��Ȃ��ł��������B");
				DirectX::XMStoreFloat4x4(out_matrix_p, world * m_view * m_projection);
			}
			//World�s�񂩂�WorldViewProjection�s����쐬���܂��B(�p�����[�^�[���X�V����Ă����ꍇ�s����X�V���܂�)
			void CreateUpdateWorldViewProjection(DirectX::XMFLOAT4X4* out_matrix_p, const DirectX::XMMATRIX& world)
			{
				if (m_changed_view) UpdateView();
				if (m_changed_projection) UpdateProjection();
				CreateWorldViewProjection(out_matrix_p, world);
			}
		public: //Setter(�s��)
			//�e�p�����[�^�[����View�s����X�V���܂��B
			void SetView( const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
			{
				m_position = pos;
				m_target = target;
				m_up = up;
				UpdateView();
			}
			//View�s��𒼐ڍX�V���܂��B���̊֐����g�p�����ꍇ�A�e�p�����[�^�[�̎擾���ł��Ȃ��Ȃ�܂��B
			void SetView(const DirectX::XMMATRIX& view) { m_view = view; SetDebugViewFlg(false); }
			//�e�p�����[�^�[����Projection�s����X�V���܂��B
			void SetProjection( float fov_angle, float aspect_ratio, const MATH::FLOAT2& visible_range)
			{
				m_fov_angle = fov_angle;
				m_aspect_ratio = aspect_ratio;
				m_visible_range = visible_range;
				UpdateProjection();
			}
			//Projection�s��𒼐ڍX�V���܂��B���̊֐����g�p�����ꍇ�A�e�p�����[�^�[�̎擾���ł��Ȃ��Ȃ�܂��B
			void SetProjection(const DirectX::XMMATRIX& projection) { m_projection = projection; SetDebugProjectionFlg(false); }
		public: //Setter(�p�����[�^�[)
			//�ʂ�position��ݒ肵�܂��B
			void SetPosition(const DirectX::XMFLOAT3& position) { m_changed_view = true; m_position = position; };
			//�ʂ�target��ݒ肵�܂��B
			void SetTarget(const DirectX::XMFLOAT3& target) { m_changed_view = true; m_target = target; };
			//�ʂ�up��ݒ肵�܂��B
			void SetUp(const DirectX::XMFLOAT3& up) { m_changed_view = true; m_up = up; };
			//�ʂ�fov_angle��ݒ肵�܂��B
			void SetFovAngle(float fov_angle) { m_changed_projection = true; m_fov_angle = fov_angle; }
			//�ʂ�aspect_ratio��ݒ肵�܂��B
			void SetAspectRatio(float aspect_ratio) { m_changed_projection = true; m_aspect_ratio = aspect_ratio; }
			//�ʂ�VisibleRange��ݒ肵�܂��B
			void SetVisibleRange(const MATH::FLOAT2& visible_range) { m_changed_projection = true; m_visible_range = visible_range; }
		public: //Getter(�s��)
			//view�s����擾�ł��܂��B
			const DirectX::XMMATRIX& GetView() const { return m_view; }
			//view�s����擾�ł��܂��B(�p�����[�^�[���X�V����Ă����ꍇ�s����X�V���܂�)
			const DirectX::XMMATRIX& GetUpdateView() { if (m_changed_view) UpdateView(); return m_view; }
			//projection�s����擾�ł��܂��B
			const DirectX::XMMATRIX& GetProjection() const { return m_view; }
			//projection�s����擾�ł��܂��B(�p�����[�^�[���X�V����Ă����ꍇ�s����X�V���܂�)
			const DirectX::XMMATRIX& GetUpdateProjection() { if (m_changed_projection) UpdateProjection(); return m_projection; }
		public: //Getter(�p�����[�^�[)
			//���ڍs������������Ă��Ȃ��ꍇposition(eye)���擾�ł��܂��B
			const DirectX::XMFLOAT3& GetPosition() const { CheckDebugView(); return m_position; };
			//���ڍs������������Ă��Ȃ��ꍇtarget(focus)���擾�ł��܂��B
			const DirectX::XMFLOAT3& GetTarget() const { CheckDebugView(); return m_target; };
			//���ڍs������������Ă��Ȃ��ꍇup���擾�ł��܂��B
			const DirectX::XMFLOAT3& GetUp() const { CheckDebugView(); return m_up; };
			//���ڍs������������Ă��Ȃ��ꍇfov_angle���擾�ł��܂��B
			float GetFovAngle() const { CheckDebugProjection(); return m_fov_angle; }
			//���ڍs������������Ă��Ȃ��ꍇaspect_ratio���擾�ł��܂��B
			float GetAspectRatio() const { CheckDebugProjection(); return m_aspect_ratio; }
			//���ڍs������������Ă��Ȃ��ꍇvisible_range���擾�ł��܂��B
			const MATH::FLOAT2& GetVisibleRange() const { CheckDebugProjection(); return m_visible_range; }
		};
	}
}