#include <string>
#include <optional>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>

#include "ObjectManager.h"
#include "scene.h"

class SceneGame final
	: public SceneBase
{
private:
	using VF4 = DirectX::XMFLOAT4;
	using VF3 = DirectX::XMFLOAT3;
	using VF2 = DirectX::XMFLOAT2;
	using Path = std::filesystem::path;

public:
	SceneGame()
		: camera_angle(Math::ToRadian(-80.f), 0.f, 0.f), load_count(0u), load_end(false), camera_dis(50.f)
	{}
	~SceneGame() noexcept = default;
	SceneGame(const SceneGame&) = delete;
	SceneGame(SceneGame&&) noexcept = delete;
	auto& operator=(const SceneGame&) = delete;
	auto& operator=(SceneGame&&) noexcept = delete;

private:
	struct FileDataFlg
	{
	public:
		inline bool operator=(const bool all_set)
		{
			return (is_save = is_save_as = is_open = is_delete = is_close = is_rename = is_copy = all_set);
		}

	public:
		bool is_save = false, is_save_as = false, is_open = false, is_delete = false, is_close = false,
			is_rename = false, is_copy = false;

	public:
		void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
		inline bool IsNotAllFileOperate() noexcept
		{
			return !(is_save || is_save_as || is_open || is_delete || is_close || is_rename || is_copy);
		}
	};
	struct EditerFlg
	{
	public:
		bool operator=(const bool all_set)
		{
			return (is_save = now_move_object = edit_mode_first = all_set);
		}

	public:
		bool is_save = false, now_move_object = false, edit_mode_first = false;
	};

public:
	static constexpr DirectX::XMFLOAT4 LightDir{ 0.f, -1.f, 0.f, 1.f };
	static constexpr std::string_view LabelName{ "GameEdit" };
	static inline const Path FileDir{ "Data\\File" };
	static inline const Path FileExtension{ ".bin" };
	static inline const Path PlanePathFileDir{ FileDir / "PathData" };
	static inline const Path SaveFileDir{ FileDir / "Stage" };
	static inline const Path TempFileDir{ FileDir / "Temp" += FileExtension };
	static inline const Path CameraFileDir{ FileDir / "Camera" += FileExtension };

public:
	static const VF3& GetMasuPos() noexcept { return masu_pos; }
	static bool GetEditMode() noexcept { return edit_mode; }
	static bool GetEnmEditMode() noexcept { return enm_edit_mode; }

public:
	void Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);

private:
	void NormalModeUpdate(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);  // 通常モード更新
	void EditModeUpdate(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);    // 編集モード更新
	void ModeChange(bool* save_f1, SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);  // モード変更処理
	VF3 MouseToLocalPos(const VF2& mouse_pos, KDL::DX12::App* p_app);  // マウス座標からローカル座標への変換

	// スクリーン座標をワールド座標に変換
	[[nodiscard]] DirectX::XMFLOAT3 TransformScreenToWorld(
		const VF2& screen_size,
		const VF2& screen_pos,	//スクリーン座標の位置
		float fz,								// 射影空間でのZ値（0～1）
		const DirectX::XMMATRIX& view			//ビュー行列
	) const
	{
		using namespace DirectX;



		// 各行列の逆行列を算出
		XMMATRIX inv_view, inv_prj, inv_viewport;
		inv_view = XMMatrixInverse(nullptr, view);
		inv_prj = XMMatrixInverse(nullptr, camera->GetProjection());
		{
			XMMATRIX viewport{ XMMatrixIdentity() };
			{
				XMFLOAT4X4 vp;
				XMStoreFloat4x4(&vp, viewport);
				vp._11 = screen_size.x / 2.0f; vp._22 = -screen_size.y / 2.0f;
				vp._41 = screen_size.x / 2.0f; vp._42 = screen_size.y / 2.0f;
				viewport = XMLoadFloat4x4(&vp);
			}
			inv_viewport = XMMatrixInverse(nullptr, viewport);
		}

		// 逆変換
		XMMATRIX tmp = inv_viewport * inv_prj * inv_view;
		XMVECTOR pout;
		XMFLOAT3 s = { screen_pos.x, screen_pos.y, fz }, result;
		pout = XMVector3TransformCoord(XMLoadFloat3(&s), tmp);
		XMStoreFloat3(&result, pout);
		return result;
	}

	// マウス座標から飛ばしたレイと板の交点を求める（係数が「NaN」・「正または負の無限大」になった場合はfloatの最大値が返る）
	[[nodiscard]] VF3 TransformMouseToPlanePos(const VF2& screen_size, const VF2& mouse_pos,
		const VF3& plane_point1, const VF3& plane_point2, const VF3& plane_point3)
	{
		namespace DX = DirectX;
		using DX::XMVectorSet;

		const auto view{ camera->GetView() };

		// スクリーン座標からワールド座標に変換
		const VF3 near_z{ TransformScreenToWorld(screen_size, mouse_pos, 0.f, view) };
		const VF3 far_z{ TransformScreenToWorld(screen_size, mouse_pos, 1.f, view) };

		const std::array<DX::XMVECTOR, 3u> Points  // プレーンの３点
		{ { XMVectorSet(plane_point1.x, plane_point1.y, plane_point1.z, 0.f),
			XMVectorSet(plane_point2.x, plane_point2.y, plane_point2.z, 0.f),
			XMVectorSet(plane_point3.x, plane_point3.y, plane_point3.z, 0.f)  } };

		// プレーンを定義
		const auto plane{ DX::XMPlaneFromPoints(Points[0], Points[1], Points[2]) };

		// プレーンと線との判定
		auto local_point{ DX::XMPlaneIntersectLine(plane, XMVectorSet(near_z.x, near_z.y, near_z.z, 0.f), XMVectorSet(far_z.x, far_z.y, far_z.z, 0.f)) };

		if (DX::XMPlaneIsInfinite(local_point) || DX::XMPlaneIsNaN(local_point))
		{
			constexpr float MaxFloat{ (std::numeric_limits<float>::max)() };

			return { MaxFloat, MaxFloat, MaxFloat };
		}

		// 型を変換して返す
		return ToXMFLOAT3(local_point);
	}

		// 基本座標の周囲を回る座標を求める
	[[nodiscard]] VF3 CreateRotationPos(const VF3& rotation_ang, const float dis,
		const VF3& base_pos = VF3{ 0.f, 0.f, 0.f }) const
	{
		using DirectX::XMMatrixRotationRollPitchYaw;
		using DirectX::XMVector3Transform;
		using DirectX::XMVector3Normalize;

		constexpr VF3 InitVector{ 0.f, 0.f, 1.f };

		static const auto InitVec{ ToXMVECTOR(InitVector) };  // 初期ベクトル

		// 回転行列でベクトルを求め
		auto rotation_vec
		{ XMVector3Transform(InitVec,
			XMMatrixRotationRollPitchYaw(rotation_ang.x, rotation_ang.y, rotation_ang.z)) };

		// 正規化し
		auto result_position{ ToXMFLOAT3(XMVector3Normalize(rotation_vec)) };

		// 基準点からの距離分スカラー倍する
		result_position *= dis;

		// 基準点を加算する
		result_position += base_pos;

		return result_position;
	}

	// ファイル関係-----------------------------------------------------------------------------------------

	// ファイル更新
	void FileOperateUpdate();

	// 保存
	void SaveFile(const VF2& screen, const float PrintTimeMax);

	// 開ける
	void OpenFile(const VF2& screen, const float PrintTimeMax);

	// 書き出し
	bool Output(const Path& output_path, const bool temp_file = false);

	// 読み込み
	bool Input(const Path& input_path, const bool temp_file = false);

	//------------------------------------------------------------------------------------------------------

private:
	VF3 camera_angle;
	VF2 mouse_pos;
	float camera_dis;
	std::optional<ObjectManager> object_manager;

	//std::unique_ptr<KDL::DX12::Geometric_Board> grit_board;
	//std::unique_ptr<KDL::DX12::Geometric_Board> grit_handle;

	std::mutex load_mutex;
	std::atomic<bool> load_end;
	std::atomic<size_t> load_count;
	std::unique_ptr<KDL::TOOL::Camera> camera;

	Path open_file_path;
	FileDataFlg file_flg;

private:
	static inline VF3 masu_pos{ 0.f, 0.f, 0.f };
	static inline bool edit_mode{ false };
	static inline bool enm_edit_mode{ false };
public:
	static inline bool is_save;
	static inline bool back_world_mode{ false };
};

class SceneLoad
	: public SceneBase
{
	std::unique_ptr<KDL::DX12::Geometric_Board> board;
	std::unique_ptr<KDL::TOOL::Camera> camera;
	float angle;
public:
	void Load(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Initialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Update(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
	void UnInitialize(SceneManager* p_scene_mgr, KDL::Window* p_window, KDL::DX12::App* p_app);
};
