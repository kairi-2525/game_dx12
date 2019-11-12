#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <stdexcept>
#if __cpp_lib_filesystem
#include <filesystem>
#endif

// std::filesystem�Ńt�@�C�����������o�����̂ňӖ��Ȃ��Ȃ�����
#if __cpp_lib_filesystem
//!+ �t�@�C��������
// ������
// std::filesystem::path : dir_name �� �����������t�@�C���܂ł̃p�X
// �߂�l��
// std::vector<std::filesystem::path> �� dir_name�f�B���N�g�������Ɋ܂܂��S�t�@�C�����o��
// �uusing namespace std::string_literals;�v �Ƃ���Ɓu""s�v�œ��͂ł��Ċy
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

		assert(!"�t�@�C�������Ɏ��s" && failed_message);
	}

	return file_dirs;
}

#else
//!+ �t�@�C��������
// ������
// std::string : dir_name �� �����������t�@�C���܂ł̃p�X
// std::string : extension �� �{���������t�@�C���̊g���q
// �߂�l��
// std::vector<std::string> �� �����ɍ������S�Ẵt�@�C����
// �uusing namespace std::string_literals;�v �Ƃ���Ɓu""s�v�œ��͂ł��Ċy
static std::vector<std::wstring> GetAllFileName(
	const std::wstring& dir_name, const std::wstring& extension)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd; // defined at Windwos.h
	std::vector<std::wstring> file_names;

	// �g���q�̐ݒ�
	std::wstring search_name = dir_name + L"\\*." + extension;

	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	// ������Ȃ�
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