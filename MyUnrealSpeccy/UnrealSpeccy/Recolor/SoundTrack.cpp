#include "SoundTrack.h"

#include "../bass.h"
#include "../snd_bass.h"
#include "../sdk/ddk.h"
#include "../emul.h"
#include "../sound.h"
extern CONFIG conf;
//conf.sound.beeper_vol = 0;
///conf.sound.ay_vol = 0;
//apply_sound();

bool SoundTrack::BassInitialized = false;

void SoundTrack::initializeBass()
{
	if (BassInitialized)
		return;
	if(!BASS::Init(-1, conf.sound.fq, BASS_DEVICE_LATENCY, 0, 0))
		throw std::runtime_error("Cannot initialize BASS");
	BassInitialized = true;
}

SoundTrack::SoundTrack(const std::string& soundName)
{
	initializeBass();
	SoundName = soundName;
	SoundHandle = BASS::StreamCreateFile(false, (void*)soundName.c_str(), 0, 0, 0);
	if(!SoundHandle)
		throw std::runtime_error("Cannot load " + soundName);
	// $mm todo: deinitialize on exit
}

void SoundTrack::Play()
{
	if (BASS::ChannelIsActive(SoundHandle) == BASS_ACTIVE_PLAYING)	// $mm do we need this check here?
		return;

	BASS::ChannelPlay(SoundHandle, true);
}

void SoundTrack::Stop()
{
	BASS::ChannelStop(SoundHandle);
}
