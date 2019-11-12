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

// コメントの読み込み用（戻り値：コメントがあるかどうか）
static inline bool InputComment(std::ifstream& ifs)
{
	std::string temp;
	const auto s_pos{ ifs.tellg() };

	ifs >> temp;

	// コメント文字を発見した場合は、改行まで無視する
	if (temp == "//")
	{
		ifs.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

		return true;
	}
	// 違う場合は、読み込み位置を戻す
	else
	{
		ifs.seekg(s_pos);

		return false;
	}
}

// コメントの書き込み用（コメント、タグ、タイプ、改行数）
static inline std::ofstream& OutputComment(std::ofstream& ofs, const std::string& comment,
	const std::string& tag = "", const std::string& type = "", const size_t endl_count = 1u)
{
	assert(!comment.empty() && "コメントを書いていない");

	// タグ無し、タイプ無し
	if (tag.empty() && type.empty())
	{
		ofs << "// " + comment;
	}
	// タグ有り、タイプ有り
	else if (!tag.empty() && !type.empty())
	{
		ofs << "// " + tag + "(" + type + ")" + " : " + comment;
	}
	// タグ有り、タイプなし
	else if (!tag.empty() && type.empty())
	{
		ofs << "// " + tag + " : " + comment;
	}
	// タグなし、タイプ有り
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
