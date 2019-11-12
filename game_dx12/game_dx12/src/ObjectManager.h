#pragma once

#include <deque>
#include <array>
#include <memory>
#include <variant>
#include <optional>
#include <atomic>
#include <fstream>

#include "MultipleObject.h"
#include "SingleObject.h"
#include "NodeData.h"
#include "Enemy.h"

struct Object
{
private:
	template<class _Ty>
	using SharedPtr = std::shared_ptr<_Ty>;
	template<class _Ty, size_t Size>
	using Array = std::array<_Ty, Size>;
	template<class _Ty>
	using Deque = std::deque<_Ty>;
	template<class _Ty>
	using Optional = std::optional<_Ty>;
	template<class... _Ty>
	using Variant = std::variant<_Ty...>;

public:
	Object() = default;
	~Object() noexcept = default;

	Object(const Object& _rt)
	{
		for (size_t i = 0; i < single_objects.size(); i++)
			single_objects[i] = _rt.single_objects[i];

		for (size_t i = 0; i < multiple_objects.size(); i++)
			multiple_objects[i] = _rt.multiple_objects[i];

		select_s_obj = _rt.select_s_obj;
		select_m_obj = _rt.select_m_obj;
	}
	auto& operator=(const Object& _rt)
	{
		for (size_t i = 0; i < single_objects.size(); i++)
			single_objects[i] = _rt.single_objects[i];

		for (size_t i = 0; i < multiple_objects.size(); i++)
			multiple_objects[i] = _rt.multiple_objects[i];

		select_s_obj = _rt.select_s_obj;
		select_m_obj = _rt.select_m_obj;

		return (*this);
	}

	Object(Object&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			single_objects.swap(_rt.single_objects);
			multiple_objects.swap(_rt.multiple_objects);
			select_s_obj.swap(_rt.select_s_obj);
			select_m_obj.swap(_rt.select_m_obj);
		}
	}
	auto& operator=(Object&& _rt) noexcept
	{
		using std::move;

		if (this != &_rt)
		{
			single_objects.swap(_rt.single_objects);
			multiple_objects.swap(_rt.multiple_objects);
			select_s_obj.swap(_rt.select_s_obj);
			select_m_obj.swap(_rt.select_m_obj);
		}

		return (*this);
	}

public:
	friend std::ofstream& operator<<(std::ofstream& ofs, const Object& data);
	friend std::ifstream& operator>>(std::ifstream& ifs, Object& data);

public:
	// �I�u�W�F�N�g�̃Q�b�^�[
	template<class _Ty>
	[[nodiscard]] const auto& GetObjects() const
	{
		using std::is_same;

		static_assert(std::is_base_of<Obj3D, _Ty>(), "Not Base Class");

		constexpr bool IsSingle
		{ (is_same<_Ty, Player>()) || (is_same<_Ty, Start>()) || (is_same<_Ty, Goal>()) };

		if constexpr (IsSingle)
			return (std::get<SharedPtr<_Ty>>(single_objects.at(_Ty::IndexNumber)));
		else
			return (std::get<Deque<_Ty>>(multiple_objects.at(_Ty::IndexNumber)));
	}

	// �I�u�W�F�N�g�̃Q�b�^�[�i�I�u�W�F�N�g��ύX�\�j
	template<class _Ty>
	[[nodiscard]] auto& GetChangeObjects()
	{
		using std::is_same;

		static_assert(std::is_base_of<Obj3D, _Ty>(), "Not Base Class");

		constexpr bool IsSingle
		{ (is_same<_Ty, Player>()) || (is_same<_Ty, Start>()) || (is_same<_Ty, Goal>()) };

		if constexpr (IsSingle)
			return (std::get<SharedPtr<_Ty>>(single_objects.at(_Ty::IndexNumber)));
		else
			return (std::get<Deque<_Ty>>(multiple_objects.at(_Ty::IndexNumber)));
	}

public:
	// �P��I�u�W�F�N�g�̊֐��Ăяo��
	template<class _Ty>
	void SingleObjVisit(const _Ty& func)
	{
		for (auto& s_objects : single_objects)
		{
			std::visit(func, s_objects);
		}
	}

	// �����I�u�W�F�N�g�̊֐��Ăяo��
	template<class _Ty>
	void MultiObjVisit(const _Ty& func)
	{
		for (auto& m_objects : multiple_objects)
		{
			std::visit(func, m_objects);
		}
	}

	// �P��I�u�W�F�N�g�̊֐��Ăяo��(const��)
	template<class _Ty>
	void SingleObjVisit(const _Ty& func) const
	{
		for (auto& s_objects : single_objects)
		{
			std::visit(func, s_objects);
		}
	}

	// �����I�u�W�F�N�g�̊֐��Ăяo��(const��)
	template<class _Ty>
	void MultiObjVisit(const _Ty& func) const
	{
		for (auto& m_objects : multiple_objects)
		{
			std::visit(func, m_objects);
		}
	}

	// �I�u�W�F�N�g�̒ǉ�
	template<class _Ty, class... Func>
	_Ty& AddObjects(Func&... func)
	{
		using std::is_same;
		using Tyep = typename std::remove_reference<_Ty>::type;

		static_assert(std::is_base_of<Obj3D, Tyep>(), "Not Base Class");

		constexpr bool IsSingle
		{ (is_same<Tyep, Player>()) || (is_same<Tyep, Start>()) || (is_same<Tyep, Goal>()) };

		if constexpr (IsSingle)
		{
			auto& object{ std::get<SharedPtr<Tyep>>(single_objects.at(Tyep::IndexNumber)) };

			if constexpr (is_same<Tyep, Player>())
			{
				if (!object) object = std::make_shared<Player>(
					GetObjects<Plane>(), GetObjects<WarpHole>(),
					GetObjects<Start>(), GetObjects<Goal>(),
					GetObjects<Door>(), GetObjects<Key>());
			}
			else
			{
				if (!object) object = std::make_shared<Tyep>(func...);
			}

			return *object;
		}
		else
		{
			auto& objects{ std::get<Deque<Tyep>>(multiple_objects.at(Tyep::IndexNumber)) };

			return objects.emplace_back(func...);
		}
	}

	// variant�I�u�W�F�N�g�\�z�i�����_���ł������������ǁA�W�F�l���b�N�����_�Ƀe���v���[�g�p�����[�^�[���g����̂�c++20�ȍ~�Ƃ���...�j
	template<class _Ty>
	auto& BuildVariant()
	{
		using std::is_same;

		static_assert(std::is_base_of<Obj3D, _Ty>(), "Not Base Class");

		constexpr bool IsSingle
		{ (is_same<_Ty, Player>()) || (is_same<_Ty, Start>()) || (is_same<_Ty, Goal>()) };

		// variant�I�u�W�F�N�g���\�z
		if constexpr (IsSingle)
		{
			return single_objects.at(_Ty::IndexNumber).emplace<SharedPtr<_Ty>>(std::move(SharedPtr<_Ty>{}));
		}
		else
		{
			return multiple_objects.at(_Ty::IndexNumber).emplace<Deque<_Ty>>(std::move(Deque<_Ty>{}));
		}
	}

	template<class _Ty>
	bool GetSelectObjectPtr() const
	{
		using std::is_same;

		static_assert(std::is_base_of<Obj3D, _Ty>(), "Not Base Class");

		constexpr bool IsSingle
		{ (is_same<_Ty, Player>()) || (is_same<_Ty, Start>()) || (is_same<_Ty, Goal>()) };

		if constexpr (IsSingle)
		{
			if (!select_s_obj)	return false;

			return std::holds_alternative<_Ty*>(*select_s_obj);
		}
		else
		{
			if (!select_m_obj)	return false;

			return std::holds_alternative<_Ty*>(*select_m_obj);
		}
	}

	// �O�I�u�W�F�N�g�폜
	void AllObjectClear()
	{
		SingleObjVisit([](auto& data) { data = nullptr; });

		MultiObjVisit([](auto& data) { data.clear(); });
	}

public:
	Array<Variant<SharedPtr<Player>, SharedPtr<Start>, SharedPtr<Goal>>, 3u> single_objects;
	Array<Variant<Deque<Enemy>, Deque<Plane>, Deque<Wall>, Deque<WarpHole>, Deque<Key>, Deque<Door>>, 6u> multiple_objects;
	Optional<Variant<Player*, Start*, Goal*>> select_s_obj;
	Optional<Variant<Enemy*, Plane*, Wall*, WarpHole*, Key*, Door*>> select_m_obj;
};

struct Node
{
	using VF3 = DirectX::XMFLOAT3;

	// �p�X���̐ݒ�
	void PathSetting(Object& objects);
	// �p�X�̔����inullptr�̏ꍇ�͑܏��H��ԁj
	[[nodiscard]] NodeData* PathFindingDijkstra(
		const VF3& base_pos, const VF3& target_pos, int64_t* processing_time = nullptr);
	// �p�X�̔���
	[[nodiscard]] NodeData& PathFindingAstar(
		const VF3& base_pos, const VF3& target_pos, int64_t* processing_time = nullptr);
	// �m�[�h�̐ڑ�
	[[nodiscard]] bool ConnectNode(NodeData* n1, NodeData* n2)
	{
		auto& connect_node{ n1->coonect_node };

		for (auto& node : connect_node)
		{
			//�o�^�ς݂łȂ���
			if (node.next_node == n2)	return false;
		}

		constexpr uint16_t Cost{ 1u };  // �������ɃR�X�g�͂P

		//�ڑ�
		n1->coonect_node.emplace_back(n2, Cost);
		n2->coonect_node.emplace_back(n1, Cost);

		return true;
	}

	std::deque<NodeData> data;
};

class ObjectManager final
{
private:
	using VF4 = DirectX::XMFLOAT4;
	using VF3 = DirectX::XMFLOAT3;
	using VF2 = DirectX::XMFLOAT2;
	using Path = std::filesystem::path;

public:
	ObjectManager();
	~ObjectManager() noexcept = default;
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager(ObjectManager&&) noexcept = delete;
	auto& operator=(const ObjectManager&) = delete;
	auto& operator=(ObjectManager&&) noexcept = delete;

public:
	// �I�u�W�F�N�g���̃Q�b�^�[
	constexpr size_t GetObjectKindsNum() const noexcept
	{
		return (objects.single_objects.size() + objects.multiple_objects.size() + 3u);
	}

	template<class _Ty>
	auto& GetObjectData() const noexcept { return objects.GetObjects<_Ty>(); }

	auto& GetObjectData() noexcept { return objects; }

public:
	void Update(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Draw(KDL::Window* p_window, KDL::DX12::App* p_app);
	void Load(std::atomic<size_t>* load_count, KDL::Window* p_window, KDL::DX12::App* p_app);
	void InitSelectObject()
	{
		select_enm = nullptr;
		select_waypoint = nullptr;
	}
	// �����o��
	void Output(std::ofstream& ofs);
	// �ǂݍ���
	void Input(std::ifstream& ifs);

private:
	// �ҏW���[�h�X�V
	void EditModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app);

	// �G�ҏW���[�h�X�V
	void EnmEditModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app);

	// �ʏ탂�[�h�X�V
	void NormalModeUpdate(KDL::Window* p_window, KDL::DX12::App* p_app);

	// �ݒu��̍��W�Ɏw�肵���I�u�W�F�N�g�����邩���m�F����i�����_���ł������������ǁA�W�F�l���b�N�����_�Ƀe���v���[�g�p�����[�^�[���g����̂�c++20�ȍ~�Ƃ���...�j
	template<class _Ty>
	bool IsSamePosObject() const
	{
		using std::is_same;

		static_assert(std::is_base_of<Obj3D, _Ty>(), "Not Base Class");

		constexpr bool IsSingle
		{ (is_same<_Ty, Player>()) || (is_same<_Ty, Start>()) || (is_same<_Ty, Goal>()) };

		const auto& get_obj{ objects.GetObjects<_Ty>() };

		if constexpr (IsSingle)
		{
			if (!get_obj)	return false;

			return (get_obj->pos == masu_pos);
		}
		else
		{
			if (get_obj.empty())	return false;

			return std::any_of(get_obj.cbegin(), get_obj.cend(),
				[&](const auto& dt) { return (dt.pos == masu_pos); });
		}
	}

	// �������ɏd�Ȃ��Ă��āA�G�Ǝ��@�ƌ��ɏd�Ȃ��Ă��Ȃ�
	bool AnyOfNotExceptPlaneObjSamePos()
	{
		return (IsSamePosObject<Plane>() &&
			!(IsSamePosObject<Player>() || IsSamePosObject<Enemy>() || IsSamePosObject<Key>()));
	}

	// �S�ẴI�u�W�F�N�g�ɂP�ł��d�Ȃ��ĂȂ����ǂ���
	bool AnyOfNotAllObjSamePos()
	{
		return !(IsSamePosObject<Player>() || IsSamePosObject<Start>() ||
			IsSamePosObject<Goal>() || IsSamePosObject<Enemy>() ||
			IsSamePosObject<Plane>() || IsSamePosObject<Wall>() ||
			IsSamePosObject<WarpHole>() || IsSamePosObject<Door>() ||
			IsSamePosObject<Key>());
	}

	// �I�u�W�F�N�g�̈ړ�
	void MoveObject(KDL::Window* p_window, KDL::DX12::App* p_app);

private:
	Object objects;
	Node node;

	bool now_move_object, edit_mode_first;
	Enemy* select_enm;
	WayPoint* select_waypoint;
	VF3 masu_pos;
};