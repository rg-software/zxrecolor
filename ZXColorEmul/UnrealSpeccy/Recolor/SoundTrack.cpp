#include "SoundTrack.h"

#include "../bass.h"
#include "../snd_bass.h"
#include "../sdk/ddk.h"
#include "../emul.h"
#include "../sound.h"
extern CONFIG conf;

bool SoundTrack::mBassInitialized = false;

void SoundTrack::initializeBass()
{
	if (mBassInitialized)
		return;
	if(!BASS::Init(-1, conf.sound.fq, BASS_DEVICE_LATENCY, 0, 0))
		throw std::runtime_error("Cannot initialize BASS");
	mBassInitialized = true;
}

SoundTrack::SoundTrack(const std::string& soundName, bool isLooped, double volume)
{
	initializeBass();
	mSoundName = soundName;
	mSoundHandle = BASS::StreamCreateFile(false, (void*)soundName.c_str(), 0, 0, isLooped ? BASS_SAMPLE_LOOP : 0);
	BASS::ChannelSetAttribute(mSoundHandle, BASS_ATTRIB_VOL, volume);

	if(!mSoundHandle)
		throw std::runtime_error("Cannot load " + soundName);
	// $mm todo: deinitialize on exit
}

void SoundTrack::Play() const
{
	BASS::ChannelPlay(mSoundHandle, true);
}

bool SoundTrack::IsPlaying() const
{
	return BASS::ChannelIsActive(mSoundHandle) == BASS_ACTIVE_PLAYING;
}

void SoundTrack::Stop() const
{
	BASS::ChannelStop(mSoundHandle);
}
