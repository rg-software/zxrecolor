#pragma once
#include <set>
#include "SoundTrack.h"
#include <map>

class SoundEvents
{
public:
	void AddElement(bool mute_ay_, bool mute_beeper_, unsigned ruleId_, unsigned channel_, std::shared_ptr<SoundTrack> sound_)
	{
		mEvents.emplace(mute_ay_, mute_beeper_, ruleId_, channel_, sound_);
	}

	void Apply()
	{
		for(auto e : mEvents)
		{
			// we must stop playing any soundtrack on the given channel
			// channel 0 means "stop all"
			
			if(e.Channel == 0)
			{
				for (auto& it : mActiveSounds)
					it.second->Stop();
				mActiveSounds.clear();
				break;
			}
			
			auto it = mActiveSounds.find(e.Channel);
			if (it != mActiveSounds.end())
			{
				it->second->Stop();
				mActiveSounds.erase(it);
			}
		}

		for (auto e : mEvents)
		{
			// $mm: MuteAY, MuteBeeper: to be implemented

			e.Sound->Play();
		}
	}

private:
	struct SoundEventsElement
	{
		bool MuteAY;
		bool MuteBeeper;
		unsigned RuleID;
		unsigned Channel;
		std::shared_ptr<SoundTrack> Sound;

		SoundEventsElement(bool mute_ay_, bool mute_beeper_, unsigned ruleId_, unsigned channel_, std::shared_ptr<SoundTrack> sound_)
			: MuteAY(mute_ay_), MuteBeeper(mute_beeper_), RuleID(ruleId_), Channel(channel_), Sound(sound_)
		{
		}
		
		bool operator<(const SoundEventsElement& rhs) const { return RuleID < rhs.RuleID; }	// this is to ensure we have do duplicates
	};

	std::set<SoundEventsElement> mEvents;

	static std::map<unsigned, std::shared_ptr<SoundTrack>> mActiveSounds;
};
