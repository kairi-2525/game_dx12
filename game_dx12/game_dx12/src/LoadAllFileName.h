#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <stdexcept>
#if __cpp_lib_filesystem
#include <filesystem>
#endif

// std::filesystemでファイル名走査が出来たので意味なくなったｗ
#if __cpp_lib_filesystem
//!+ ファイル名走査
// 引数↓
// std::filesystem::path : dir_name → 走査したいファイルまでのパス
// 戻り値↓
// std::vector<std::filesystem::path> → dir_nameディレクトリ直下に含まれる全ファイルを出力
// 「using namespace std::string_literals;」 とすると「""s」で入力できて楽
static std::vector<std::filesystem::path> GetAllFileName(
	const std::filesystem::path& dir_name)
{
	namespace fs = std::filesystem;

	std::vector<fs::path> file_dirs;

	try
	{
		const auto& dir_itr{ fs::directory_iterator(dir_name) };

		for (const auto& di : dir_itr)
		{
			file_dirs.emplace_back(di.path());
		}
	}
	catch (const fs::filesystem_error& err)
	{
		const char* failed_message{ err.what() };

		assert(!"ファイル走査に失敗" && failed_message);
	}

	return file_dirs;
}

#else
//!+ ファイル名走査
// 引数↓
// std::string : dir_name → 走査したいファイルまでのパス
// std::string : extension → 捜査したいファイルの拡張子
// 戻り値↓
// std::vector<std::string> → 条件に合った全てのファイル名
// 「using namespace std::string_literals;」 とすると「""s」で入力できて楽
static std::vector<std::wstring> GetAllFileName(
	const std::wstring& dir_name, const std::wstring& extension)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd; // defined at Windwos.h
	std::vector<std::wstring> file_names;

	// 拡張子の設定
	std::wstring search_name = dir_name + L"\\*." + extension;

	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	// 見つからない
	if (hFind == INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);

		file_names.clear();

		return file_names;
	}

	do
	{
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
		}
		else
			file_names.emplace_back(win32fd.cFileName);

	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);

	return file_names;
}
#endif