#pragma once
#include <vector>
#include <memory>
#include <random>
#include "SoundTrack.h"

class SoundList
{
public:
	void Add(std::shared_ptr<SoundTrack> sound);
	std::shared_ptr<SoundTrack> GetSound(bool isSeq);

private:
    std::vector<std::shared_ptr<SoundTrack>> mSounds;
	int mCurrentSample = -1;
	static std::random_device mRandomDevice;
	static std::mt19937 mRandom;
};