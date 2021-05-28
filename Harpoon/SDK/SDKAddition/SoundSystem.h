
#pragma once
#include "../../Memory.h"
#include <vector>
#include <chrono>
namespace SoundSystem {

	void voice_record_start(const char* uncompressed_file, const char* decompressed_file, const char* mic_input_file);

	struct wavheader {
		char chunk_id[4];
		int chunk_size;
		char format[4];
		char subchunk1_id[4];
		int subchunk1_size;
		short int audio_format;
		short int num_channels;
		int sample_rate;
		int byte_rate;
		short int block_align;
		short int bits_per_sample;
		char subchunk2_id[4];
		int subchunk2_size;
	};

	class SoundPlayer {
	public:
		struct SoundFile {
			std::string FilePath;
			std::string Name;
			float duration;
		};
		SoundPlayer();
		~SoundPlayer();

		void Play(int Index);
		void Play(std::string Name, bool bForcePlay = false);

		void Update();

	private:
		wavheader GetWavHeaderData(std::vector<char>& wavfile) {
			// http://soundfile.sapp.org/doc/WaveFormat/
			wavheader* datastart = reinterpret_cast<wavheader*>(wavfile.data());
			return *datastart;
		}
		float GetWavLength(std::vector<char>& wavfile) {
			wavheader hdr = GetWavHeaderData(wavfile);
			int numSamples = hdr.subchunk2_size /
				(hdr.num_channels * (hdr.bits_per_sample / 8));
			int durationSeconds = numSamples / hdr.sample_rate;
			if (durationSeconds < 1.f)
				return 1.f;
			return durationSeconds;
		}

		std::vector<SoundFile> SoundFiles;
		//float lastPlayTime = 0.0f;
		float lastAudioFileDuration = 0.0f;
		std::chrono::high_resolution_clock::time_point lastPlayTime = std::chrono::high_resolution_clock::now();
		bool InPlay = false;
	};

	std::unique_ptr<SoundSystem::SoundPlayer> soundPlayer;
}