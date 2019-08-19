#pragma once
#include <set>
#include "SoundTrack.h"
#include <map>

class SoundEvents
{
public:
	struct ActiveSoundsMapElement
	{
		std::shared_ptr<SoundTrack> Sound;
		bool MuteAY;
		bool MuteBeeper;

		ActiveSoundsMapElement(std::shared_ptr<SoundTrack> sound_, bool muteay_, bool mutebeeper_)
			: Sound(sound_), MuteAY(muteay_), MuteBeeper(mutebeeper_)
		{
		}
	};
	typedef std::map<unsigned, ActiveSoundsMapElement> ActiveSoundsMap;

	explicit SoundEvents(ActiveSoundsMap& soundsMap);

	void AddElement(bool mute_ay_, bool mute_beeper_, unsigned ruleId_, unsigned channel_, std::shared_ptr<SoundTrack> sound_)
	{
		mEvents.emplace(mute_ay_, mute_beeper_, ruleId_, channel_, sound_);
	}

	void Apply();
	void Update();

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
	unsigned mAyVolume, mBeeperVolume;
	ActiveSoundsMap& mActiveSounds;
};
