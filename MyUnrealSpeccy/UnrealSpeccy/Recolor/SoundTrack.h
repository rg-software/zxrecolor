#pragma once

#include <string>
#include <windows.h>

class SoundTrack
{
public:
	SoundTrack(const std::string& soundName);
	void Play() const;
	bool IsPlaying() const;
	void Stop() const;

private:
	std::string SoundName;	// primarily for debug
	DWORD SoundHandle;
	
	static bool BassInitialized;
	static void initializeBass();
};

