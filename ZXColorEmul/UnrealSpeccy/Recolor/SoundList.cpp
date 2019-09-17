#include "SoundList.h"

std::random_device SoundList::mRandomDevice;
std::mt19937 SoundList::mRandom(mRandomDevice());

void SoundList::Add(std::shared_ptr<SoundTrack> sound)
{
	mSounds.push_back(sound);
}

std::shared_ptr<SoundTrack> SoundList::GetSound(bool isSeq)
{
	if (isSeq)
	{
		mCurrentSample = (mCurrentSample + 1) % mSounds.size();
		return mSounds[mCurrentSample];
	}

	std::uniform_int_distribution<> dist(0, mSounds.size() - 1);
	return mSounds[dist(mRandom)];
}
