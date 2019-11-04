#pragma once

#define XAUDIO2_HELPER_FUNCTIONS

#include <unordered_map>
#include <xaudio2.h>
#include <DirectXTK/Audio/WAVFileReader.h>
#include <filesystem>

namespace KDL
{
	using AUDIO_FILTER_TYPE = XAUDIO2_FILTER_TYPE;

	class Audio
	{
	private:
		struct XAudio2SoundSourcePackage
		{
			IXAudio2SourceVoice* voice;
			bool exist = true;
			bool feed_stop = false;
			bool start = false;
			bool filter = false;
			float feed_time = -1.f;
			float feed_start_volume = 1.f;
			float feed_target_volume = 1.f;
			std::unique_ptr<float[]> balance;

			XAudio2SoundSourcePackage(IXAudio2SourceVoice* voice) : voice(voice) {}
		};
		struct XAudio2SoundPackage
		{
#ifdef USE_IMGUI
			std::filesystem::path file_name;
#endif
			std::unique_ptr<uint8_t[]> wave_file;
			DirectX::WAVData wave_data;
			std::unordered_map<int, XAudio2SoundSourcePackage> source_voices;
		};
	private:
		IXAudio2* x_audio2 = nullptr;
		IXAudio2MasteringVoice* mastering_voice = nullptr;
		XAUDIO2_VOICE_DETAILS mastering_details;
		std::unordered_map<int, XAudio2SoundPackage> files;
	private:
		HRESULT LoadWave(const std::filesystem::path& file_name);
	public:
		Audio() = default;
		~Audio();
		HRESULT Init();
		int Update(float elpased_time);
		int Load(const std::filesystem::path &file_name);
		int Delete(int handle);
		int CreatePlayHandle(int handle, float start_time, bool loop, bool wav_loop, float loop_start_time, float loop_time, int loop_count, bool use_filter, bool db);
		int Stop(int handle, int p_handle, float feed_out_time);
		int Pause(int handle, int p_handle, float feed_out_time);
		int Play(int handle, int p_handle, float feed_in_time, float volume, bool db);
		bool IsPlay(int handle, int p_handle);
		bool IsExistPlayHandle(int handle, int p_handle);
		bool IsEnd(int handle, int p_handle);
		float GetVolume(int handle, int p_handle, bool db);
		int SetVolume(int handle, int p_handle, float volume, bool db);
		int SetVolume2(int handle, int p_handle, float left_vol, float right_vol, bool db);
		int Feed(int handle, int p_handle, float target_volume, float time, bool db);
		int SetPitch(int handle, int p_handle, float pitch);
		float GetPitch(int handle, int p_handle);
		int SetFilter(int handle, int p_handle, AUDIO_FILTER_TYPE type, int frequency, float one_over_q, bool off);
		float GetFrequencyRatio(int handle, int p_handle);
	};
}