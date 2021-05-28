

#include "SoundSystem.h"


#include "../../Resource/Sounds/Sounds.h"
#include <fileapi.h>
#include <stdexcept>
#include <cstdint>
#include <fstream>
//#include <windows.h>
#include <iostream>
#include <filesystem>
#include "../../Other/base64.h"
#include "../../Interfaces.h"
#include "../OsirisSDK/Engine.h"
#include "../../Hacks/OTHER/Debug.h"
#include "../../COMPILERDEFINITIONS.h"
namespace SoundSystem {

	void voice_record_start(const char* uncompressed_file, const char* decompressed_file, const char* mic_input_file){
	static Voice_RecordStartFn voice_record_start_fn = memory->Voice_RecordStart;
	__asm {
		push mic_input_file
		mov edx, decompressed_file
		mov ecx, uncompressed_file
		call voice_record_start_fn
		add esp, 0x4
	}

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/258133-music-voice-chat.html


	/*
		wav
		mono
		22050Hz
		PCM Signed 16bit
	*/
	}
	SoundPlayer::SoundPlayer() {
		std::filesystem::remove_all(".\\Sounds");
		Debug::QuickPrint("Creating Sounds\\\n", false);
		CreateDirectoryA(".\\Sounds", NULL);

		for (auto Sound : SoundData::Sounds) {

			std::string binDat;

			for (std::string line : Sound.FileData) {
				binDat += line;
			}
			std::vector<char> melterbin = base64::decode(binDat);
			char OUT_FILE[256];
			snprintf(OUT_FILE, 256, ".\\Sounds\\%s.wav", Sound.Name.c_str());
			std::ofstream outfile(OUT_FILE, std::ios::out | std::ios::binary);
			outfile.write(&melterbin[0], melterbin.size());
			outfile.close();
			Debug::QuickPrint(("Creating " + std::string(OUT_FILE) + "\n").c_str(), false);
			SoundFile NewFile;
			NewFile.Name = Sound.Name;
			NewFile.FilePath = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\Sounds\\" + Sound.Name + ".wav";
			NewFile.duration = GetWavLength(melterbin) * 2.f;
			SoundFiles.push_back(NewFile);
		}

		SoundData::Sounds.clear();

	}
	SoundPlayer::~SoundPlayer() {
		std::filesystem::remove_all(".\\Sounds");
	}

	void SoundPlayer::Play(std::string Name, bool bForcePlay) {

#ifdef SILENCE
		return;
#endif
		//if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected())
		//	return;


		std::chrono::high_resolution_clock::time_point nowTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration elapsed = nowTime - lastPlayTime;

		if ((lastAudioFileDuration > std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()) && !bForcePlay) {
			Debug::QuickPrint(("SoundsPlayer::Play AlreadyPlaying"));
			return;
		}


		SoundFile PlayFile;
		bool found = false;
		for (SoundFile file : SoundFiles) {
			if (file.Name == Name) {
				found = true;
				PlayFile = file;
				break;
			}
		}

		if (!found) { Debug::QuickPrint(("SoundPlayer::Play No File Exists : " + Name).c_str()); return; }


		std::string Path = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\Sounds\\" + Name + ".wav";
		WIN32_FIND_DATAA data;
		auto file_handle = FindFirstFileA(Path.c_str(), &data);
		if (file_handle == INVALID_HANDLE_VALUE) {
			Debug::QuickPrint(("SoundsPlayer::Play Unable to Play " + Path + " No File Exists").c_str());
			return;
		}
		lastPlayTime = std::chrono::high_resolution_clock::now();
		lastAudioFileDuration = PlayFile.duration + 1.f;
		Debug::QuickPrint(("SoundsPlayer::Play Playing " +  std::string(".\\Sounds\\" + std::string(data.cFileName))).c_str());
		voice_record_start(Path.c_str(), 0, Path.c_str());
		InPlay = true;
		interfaces->engine->clientCmdUnrestricted("voice_loopback 1");
	}
	void SoundPlayer::Update() {
		std::chrono::high_resolution_clock::time_point nowTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration elapsed = nowTime - lastPlayTime;

		if ((lastAudioFileDuration < std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()) && InPlay) {
			InPlay = false;
			interfaces->engine->clientCmdUnrestricted("-voicerecord");
			interfaces->engine->clientCmdUnrestricted("voice_loopback 0");
		}
	}



}