#include "SoundEvents.h"

#include "../bass.h"
#include "../snd_bass.h"
#include "../sdk/ddk.h"
#include "../emul.h"
#include "../sound.h"
extern CONFIG conf;

SoundEvents::SoundEvents(unsigned beeperVolume, unsigned ayVolume, ActiveSoundsMap& soundsMap)
: mBeeperVolume(beeperVolume), mAyVolume(ayVolume), mActiveSounds(soundsMap)
{
}

void SoundEvents::Apply()
{
	for(auto e : mEvents)
	{
		// we must stop playing any soundtrack on the given channel
		// channel 0 means "stop all"

		if(e.Channel == 0)
		{
			for(auto& it : mActiveSounds)
				it.second.Sound->Stop();
			mActiveSounds.clear();
			break;
		}

		auto it = mActiveSounds.find(e.Channel);
		if(it != mActiveSounds.end())
		{
			it->second.Sound->Stop();
			mActiveSounds.erase(it);
		}
	}

	for(const auto& e : mEvents)
	{
		mActiveSounds.insert(std::make_pair(e.Channel, ActiveSoundsMapElement(e.Sound, e.MuteAY, e.MuteBeeper)));
		e.Sound->Play();
	}
}

void SoundEvents::Update()
{
	std::set<unsigned> inactiveChannels;

	for(const auto& e : mActiveSounds)
		if(!e.second.Sound->IsPlaying())
			inactiveChannels.insert(e.first);

	for(auto channel : inactiveChannels)
		mActiveSounds.erase(channel);

	conf.sound.beeper_vol = mBeeperVolume;
	conf.sound.ay_vol = mAyVolume;

	for(const auto& e : mActiveSounds)
	{
		if (e.second.MuteBeeper)
			conf.sound.beeper_vol = 0;
		if (e.second.MuteAY)
			conf.sound.ay_vol = 0;
	}

	apply_sound();
}
