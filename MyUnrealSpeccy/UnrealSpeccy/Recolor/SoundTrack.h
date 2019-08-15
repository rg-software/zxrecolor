#pragma once

#include <string>
#include <windows.h>

class SoundTrack
{
public:
	SoundTrack(const std::string& soundName);
	void Play();
	void Stop();

private:
	std::string SoundName;	// primarily for debug
	DWORD SoundHandle;
	
	static bool BassInitialized;
	static void initializeBass();
};

