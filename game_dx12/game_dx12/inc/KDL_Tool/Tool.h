#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <filesystem>
#include <string>

using UINT = unsigned int;

namespace KDL
{
	namespace TOOL
	{
		extern const unsigned short glyphRangesJapanese[];
		extern bool GetFileSize(const std::filesystem::path& path, size_t* out_size);
		extern size_t GetFileSize(const std::filesystem::path& path);
		extern std::string GetSizeToStr(size_t byte, int get_size = -1);

		class LOAD_THREAD
		{
		private:
			UINT progress = 0u;
			std::mutex progress_mutex;
			std::unique_ptr<std::thread> thread;
		protected:
			void AddProgress(UINT add_prg);
			void SetProgress(UINT set_prg);
			void Load(std::unique_ptr<std::thread> make_ptr);
		public:
			bool IsLoading();
			UINT GetProgress();
			LOAD_THREAD();
			virtual ~LOAD_THREAD();
			LOAD_THREAD(const LOAD_THREAD&) = delete;
			LOAD_THREAD& operator=(const LOAD_THREAD&) = delete;
		};
	}
}