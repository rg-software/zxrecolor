#include "RcRule.h"
#include <string>
#include <sstream>
#include <set>

unsigned RcRule::mRuleCount = 0;

std::map<std::string, unsigned> RcRule::mNameRGB = 
{
	{"black", RGB_MAKE(0, 0, 0)},
	{"blue",  RGB_MAKE(0, 0, 0xBF)},
	{"red", RGB_MAKE(0xBF, 0, 0)},
	{"magenta", RGB_MAKE(0xBF, 0, 0xBF)},
	{"green", RGB_MAKE(0, 0xBF, 0)},
	{"cyan", RGB_MAKE(0, 0xBF, 0xBF)},
	{"yellow", RGB_MAKE(0xBF, 0xBF, 0)},
	{"white", RGB_MAKE(0xBF, 0xBF, 0xBF)},
	{"black+", RGB_MAKE(0, 0, 0)},
	{"blue+", RGB_MAKE(0, 0, 0xFE)},
	{"red+", RGB_MAKE(0xFE, 0, 0)},
	{"magenta+", RGB_MAKE(0xFE, 0, 0xFE)},
	{"green+", RGB_MAKE(0, 0xFE, 0)},
	{"cyan+", RGB_MAKE(0, 0xFE, 0xFE)},
	{"yellow+", RGB_MAKE(0xFE, 0xFE, 0)},
	{"white+", RGB_MAKE(0xFE, 0xFE, 0xFE)}
};

RcRule::RcRule(const std::string& line) 
: mAppearsFlag(false), mDisappearsFlag(false), mMuteAyFlag(false), mMuteBeeperFlag(false), 
  mMatchColor(false), mProtectedFlag(false), mOffsetX(0), mOffsetY(0), mUseRuleXY(false), mRuleX(0), mRuleY(0)
{
	mID = ++mRuleCount;
	std::string type, orig_pic, new_pic;

	std::istringstream iss(line);
	iss >> mLayer >> type >> orig_pic >> new_pic;

	if(type.find('|') != std::string::npos)	// rule XY part present
	{
		mUseRuleXY = true;
		std::string rulexy_part = type.substr(type.find('|') + 1);
		type = type.substr(0, type.find('|'));

		mRuleX = atoi(rulexy_part.substr(0, rulexy_part.find(',')).c_str());
		rulexy_part = rulexy_part.substr(rulexy_part.find(',') + 1);
		mRuleY = atoi(rulexy_part.substr(0, rulexy_part.find(',')).c_str());
	}

	if(orig_pic.find('|') != std::string::npos) // color part is found (orig_pic|color_part)
	{
		std::string full_color_part = orig_pic.substr(orig_pic.find('|') + 1);
		orig_pic = orig_pic.substr(0, orig_pic.find('|'));

		mMatchColor = true; // color_part = "x,y,color_name" (repeated, |-delimited)

		std::stringstream ss(full_color_part);
		std::string color_part;
		while (std::getline(ss, color_part, '|'))
		{
			auto colorX = atoi(color_part.substr(0, color_part.find(',')).c_str());
			color_part = color_part.substr(color_part.find(',') + 1);
			auto colorY = atoi(color_part.substr(0, color_part.find(',')).c_str());
			color_part = color_part.substr(color_part.find(',') + 1);
			if (mNameRGB.find(color_part) == mNameRGB.end())
				throw std::runtime_error("Incorrect color name"); // should never happen
			auto color = mNameRGB[color_part];
			mXYColor.emplace_back(colorX, colorY, color);
		}
	}

	if(new_pic.find('|') != std::string::npos)
	{
		std::string xy_part = new_pic.substr(new_pic.find('|') + 1);
		new_pic = new_pic.substr(0, new_pic.find('|'));

		mOffsetX = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
		xy_part = xy_part.substr(xy_part.find(',') + 1);
		mOffsetY = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
	}

	if (type == "block")
		mType = BLOCK;
	else if (type == "pixel")
		mType = PIXEL;
	else if (type == "sound-block")
		mType = SOUND_BLOCK;
	else if (type == "sound-pixel")
		mType = SOUND_PIXEL;
	else 
		throw std::runtime_error("Unknown rule type " + type);

	ZxImage = std::make_shared<RcImage>(orig_pic, true, mType == PIXEL || mType == SOUND_PIXEL);

	for (unsigned i = 0; i < 8; ++i)
		ZxImages.push_back(std::make_shared<RcImage>(ZxImage, i));

	if (mType == SOUND_BLOCK || mType == SOUND_PIXEL)
		Sound = std::make_shared<SoundTrack>(new_pic);
	else
		RecoloredImage = std::make_shared<RcImage>(new_pic);

	mZxHeight = ZxImage->GetHeight();
	mZxWidth = ZxImage->GetWidth();

	std::set<std::string> flags;
	while(iss)
	{
		std::string flagstr;
		iss >> flagstr;
		flags.insert(flagstr);
	}
		
	mAppearsFlag = flags.find("appears") != flags.end();
	mDisappearsFlag = flags.find("disappears") != flags.end();
	mMuteAyFlag = flags.find("mute_ay") != flags.end();
	mMuteBeeperFlag = flags.find("mute_beeper") != flags.end();

	auto protectedIt = std::find_if(flags.begin(), flags.end(), [](const std::string& v) { return v.find("protected") != std::string::npos; });
	if(protectedIt != flags.end())
	{
		mProtectedFlag = true;
		mProtectedPixels = atoi(protectedIt->substr(protectedIt->find('|') + 1).c_str());
	}
}

void RcRule::AddToSoundEvents(bool appears, bool disappears, SoundEvents& events) const
{
	if(mAppearsFlag && appears)
		events.AddElement(mMuteAyFlag, mMuteBeeperFlag, mID, mLayer, Sound);

	if(mDisappearsFlag && disappears)
		events.AddElement(mMuteAyFlag, mMuteBeeperFlag, mID, mLayer, Sound);
}

void RcRule::AddToBlitList(unsigned x, unsigned y, BlitList& blitlist) const
{
	blitlist.AddElement(x + mOffsetX - 8 * ZxImage->GetKeyOffsetX(), y + mOffsetY - ZxImage->GetKeyOffsetY(),
	                    RecoloredImage, mLayer);
}

bool RcRule::IsValidPosition(unsigned x, unsigned y) const
{
	const int BORDER_LEFT = 32, BORDER_TOP = 24;
	return !mUseRuleXY || (x == BORDER_LEFT + mRuleX && y == BORDER_TOP + mRuleY);
}

bool RcRule::IsFoundColor(unsigned* dst, unsigned x, unsigned y) const
{
	if (!mMatchColor)
		return true;

	for(const auto& [mColorX, mColorY, mColor] : mXYColor)
		if(!ZxImage->IsFoundColor(dst + 2 * (x + mColorX + 640 * (y + mColorY)), mColor))
			return false;
	
	return true;
}

bool RcRule::IsApproximateMatch(const uint8_t* curptr, unsigned offset) const
{
	auto count = (mType == BLOCK || mType == SOUND_BLOCK ? ZxImage->MatchingPixelsCount(curptr) : ZxImages[offset]->MatchingPixelsCount(curptr));
	return count >= mProtectedPixels;
}

bool RcRule::IsProtected() const
{
	return mProtectedFlag;
}

unsigned short RcRule::GetZxKey() const
{
	return ZxImage->GetZxKey();
}

bool RcRule::operator<(const RcRule& rhs) const
{
	return GetZxKey() < rhs.GetZxKey();
}

bool RcRule::IsFoundAt(const uint8_t* curptr, unsigned offset) const
{
	// $mm NOTE: we list explict block types here, should eventually refactor it
	return mType == BLOCK || mType == SOUND_BLOCK ? ZxImage->IsFoundAt(curptr) : ZxImages[offset]->IsFoundAt(curptr);
}
