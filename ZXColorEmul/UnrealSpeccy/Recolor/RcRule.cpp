#include "RcRule.h"
#include <string>
#include <sstream>
#include <set>

unsigned RcRule::mRuleCount = 0;
std::random_device RcRule::mRandomDevice;
std::mt19937 RcRule::mRandom(mRandomDevice());

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

RcRule::RuleType RcRule::parseRuleType(const std::string& type)
{
	if (type == "block")
		return BLOCK;
	if (type == "pixel")
		return PIXEL;
	if (type == "sound-block")
		return SOUND_BLOCK;
	if (type == "sound-pixel")
		return SOUND_PIXEL;
	throw std::runtime_error("Unknown rule type " + type);
}

bool RcRule::hasSeparator(const std::string& str) {	return str.find('|') != std::string::npos; }
std::string RcRule::leftPart(const std::string& str) { return str.substr(0, str.find('|')); }
std::string RcRule::rightPart(const std::string& str) { return str.substr(str.find('|') + 1); }
std::string RcRule::leftCommaPart(const std::string& str) { return str.substr(0, str.find(',')); }
std::string RcRule::rightCommaPart(const std::string& str) { return str.substr(str.find(',') + 1); }

decltype(RcRule::mXYColor) RcRule::parseColorPart(const std::string& full_color_part)
{
	decltype(mXYColor) result;

	std::stringstream ss(full_color_part);
	std::string color_part;
	while (std::getline(ss, color_part, '|'))
	{
		auto colorX = std::stoi(leftCommaPart(color_part));
		color_part = rightCommaPart(color_part);
		auto colorY = std::stoi(leftCommaPart(color_part));
		color_part = rightCommaPart(color_part);
		if (mNameRGB.find(color_part) == mNameRGB.end())
			throw std::runtime_error("Incorrect color name"); // should never happen
		auto color = mNameRGB[color_part];
		result.emplace_back(colorX, colorY, color);
	}

	return result;
}

bool RcRule::isFlagFound(const std::set<std::string>& flags, const std::string& flag)
{
	return flags.find(flag) != flags.end();
}

RcRule::RcRule(const std::string& line) 
{
	mID = ++mRuleCount;
	std::string type, orig_pic, new_pic;

	std::istringstream iss(line);
	iss >> mLayer >> type >> orig_pic >> new_pic;

	if (hasSeparator(type))	// rule XY part present
	{
		mUseRuleXY = true;
		std::string rulexy_part = rightPart(type);
		type = leftPart(type);
		mRuleX = std::stoi(leftCommaPart(rulexy_part));
		mRuleY = std::stoi(rightCommaPart(rulexy_part));
	}

	mType = parseRuleType(type);

	if(hasSeparator(orig_pic)) // color part is found (orig_pic|color_part)
	{
		mMatchColor = true; // color_part = "x,y,color_name" (repeated, |-delimited)
		mXYColor = parseColorPart(rightPart(orig_pic));
		orig_pic = leftPart(orig_pic);
	}

	ZxImage = std::make_shared<RcImage>(orig_pic, true, mType == PIXEL || mType == SOUND_PIXEL);

	for (unsigned i = 0; i < 8; ++i)
		ZxImages.push_back(std::make_shared<RcImage>(ZxImage, i));

	mZxHeight = ZxImage->GetHeight();
	mZxWidth = ZxImage->GetWidth();

	if((mType == BLOCK || mType == PIXEL) && hasSeparator(new_pic))	// displacement in case of an image, samples in case of sounds
	{
		std::string xy_part = rightPart(new_pic);
		std::tie(mOffsetX, mOffsetY) = std::make_tuple(std::stoi(leftCommaPart(xy_part)), std::stoi(rightCommaPart(xy_part)));
		new_pic = leftPart(new_pic);
	}

	std::vector<std::string> soundNames;
	while(hasSeparator(new_pic))	// this works for sounds ONLY, we have removed separators for image rules in the code above
	{
		soundNames.push_back(leftPart(new_pic));
		new_pic = rightPart(new_pic);
	}
	soundNames.push_back(new_pic);

	std::set<std::string> flags;
	while(iss)
	{
		std::string flagstr;
		iss >> flagstr;
		flags.insert(flagstr);
	}
		
	mAppearsFlag = isFlagFound(flags, "appears");
	mDisappearsFlag = isFlagFound(flags, "disappears");
	mMuteAyFlag = isFlagFound(flags, "mute_ay");
	mMuteBeeperFlag = isFlagFound(flags, "mute_beeper");

	mLoopFlag = isFlagFound(flags, "loop");
	//mSeqFlag = isFlagFound(flags, "seq");
	//mRandFlag = isFlagFound(flags, "rand");

	auto protectedIt = std::find_if(flags.begin(), flags.end(), [](const std::string& v) { return v.find("protected") != std::string::npos; });
	if(protectedIt != flags.end())
	{
		mProtectedFlag = true;
		mProtectedPixels = std::stoi(rightPart(*protectedIt));
	}

	if (mType == SOUND_BLOCK || mType == SOUND_PIXEL)
	{
		for(const auto& sound : soundNames)
			Sounds.push_back(std::make_shared<SoundTrack>(sound, mLoopFlag));
	}
	else
		RecoloredImage = std::make_shared<RcImage>(new_pic);

}

void RcRule::AddToSoundEvents(bool appears, bool disappears, SoundEvents& events) const
{
	std::uniform_int_distribution<> dist(0, Sounds.size() - 1);

	if(mAppearsFlag && appears)
		events.AddElement(mMuteAyFlag, mMuteBeeperFlag, mID, mLayer, Sounds[dist(mRandom)]);

	if(mDisappearsFlag && disappears)
		events.AddElement(mMuteAyFlag, mMuteBeeperFlag, mID, mLayer, Sounds[dist(mRandom)]);
}

void RcRule::AddToBlitList(unsigned x, unsigned y, BlitList& blitlist) const
{
	blitlist.AddElement(x + mOffsetX - 8 * ZxImage->GetKeyOffsetX(), y + mOffsetY - ZxImage->GetKeyOffsetY(),
	                    RecoloredImage, mLayer);
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

bool RcRule::IsValidPosition(unsigned x, unsigned y, unsigned offset) const
{
	if (!mUseRuleXY)
		return true;
	
	const int BORDER_LEFT = 32, BORDER_TOP = 24;	// $mm CONST!
	return mType == BLOCK || mType == SOUND_BLOCK ? ZxImage->IsValidPosition(x - BORDER_LEFT - mRuleX, y - BORDER_TOP - mRuleY) :
													ZxImages[offset]->IsValidPosition(x - BORDER_LEFT - mRuleX, y - BORDER_TOP - mRuleY);
}
