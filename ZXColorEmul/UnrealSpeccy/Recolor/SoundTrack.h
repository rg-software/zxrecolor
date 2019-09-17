#pragma once

#include <string>
#include <windows.h>

class SoundTrack
{
public:
	SoundTrack(const std::string& soundName, bool isLooped, double volume);
	void Play() const;
	bool IsPlaying() const;
	void Stop() const;

private:
	std::string mSoundName;	// primarily for debug
	DWORD mSoundHandle;
	
	static bool mBassInitialized;
	static void initializeBass();
};

