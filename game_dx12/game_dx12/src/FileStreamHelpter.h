#pragma once

#include <fstream>

#include "Vector2.h"
#include "Vector3.h"

#define	F_OUT(data) ' ' << (data)

template<typename _Ty = float>
static inline std::ofstream& operator<<(std::ofstream& ofs, const Vector2<_Ty>& data)
{
	ofs << F_OUT(data.x) << F_OUT(data.y);

	return ofs;
}

template<typename _Ty = float>
static inline std::ifstream& operator>>(std::ifstream& ifs, Vector2<_Ty>& data)
{
	ifs >> data.x >> data.y;

	return ifs;
}

template<typename _Ty = float>
static inline std::ofstream& operator<<(std::ofstream& ofs, const Vector3<_Ty>& data)
{
	ofs << F_OUT(data.x) << F_OUT(data.y) << F_OUT(data.z);

	return ofs;
}

template<typename _Ty = float>
static inline std::ifstream& operator>>(std::ifstream& ifs, Vector3<_Ty>& data)
{
	ifs >> data.x >> data.y >> data.z;

	return ifs;
}

static inline std::ofstream& operator<<(std::ofstream& ofs, const DirectX::XMFLOAT2& data)
{
	ofs << F_OUT(data.x) << F_OUT(data.y);

	return ofs;
}

static inline std::ifstream& operator>>(std::ifstream& ifs, DirectX::XMFLOAT2& data)
{
	ifs >> data.x >> data.y;

	return ifs;
}

static inline std::ofstream& operator<<(std::ofstream& ofs, const DirectX::XMFLOAT3& data)
{
	ofs << F_OUT(data.x) << F_OUT(data.y) << F_OUT(data.z);

	return ofs;
}

static inline std::ifstream& operator>>(std::ifstream& ifs, DirectX::XMFLOAT3& data)
{
	ifs >> data.x >> data.y>> data.z;

	return ifs;
}

static inline std::ofstream& operator<<(std::ofstream& ofs, const DirectX::XMFLOAT4& data)
{
	ofs << F_OUT(data.x) << F_OUT(data.y) << F_OUT(data.z) << F_OUT(data.w);

	return ofs;
}

static inline std::ifstream& operator>>(std::ifstream& ifs, DirectX::XMFLOAT4& data)
{
	ifs >> data.x >> data.y >> data.z >> data.w;

	return ifs;
}

// �R�����g�̓ǂݍ��ݗp�i�߂�l�F�R�����g�����邩�ǂ����j
static inline bool InputComment(std::ifstream& ifs)
{
	std::string temp;
	const auto s_pos{ ifs.tellg() };

	ifs >> temp;

	// �R�����g�����𔭌������ꍇ�́A���s�܂Ŗ�������
	if (temp == "//")
	{
		ifs.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

		return true;
	}
	// �Ⴄ�ꍇ�́A�ǂݍ��݈ʒu��߂�
	else
	{
		ifs.seekg(s_pos);

		return false;
	}
}

// �R�����g�̏������ݗp�i�R�����g�A�^�O�A�^�C�v�A���s���j
static inline std::ofstream& OutputComment(std::ofstream& ofs, const std::string& comment,
	const std::string& tag = "", const std::string& type = "", const size_t endl_count = 1u)
{
	assert(!comment.empty() && "�R�����g�������Ă��Ȃ�");

	// �^�O�����A�^�C�v����
	if (tag.empty() && type.empty())
	{
		ofs << "// " + comment;
	}
	// �^�O�L��A�^�C�v�L��
	else if (!tag.empty() && !type.empty())
	{
		ofs << "// " + tag + "(" + type + ")" + " : " + comment;
	}
	// �^�O�L��A�^�C�v�Ȃ�
	else if (!tag.empty() && type.empty())
	{
		ofs << "// " + tag + " : " + comment;
	}
	// �^�O�Ȃ��A�^�C�v�L��
	else if (tag.empty() && !type.empty())
	{
		ofs << "// " + comment + "(" + type + ")";
	}

	for (size_t i = 0; i < endl_count; i++)
	{
		ofs << std::endl;
	}

	return ofs;
}
