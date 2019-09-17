#include "RcImage.h"
#include "SoundTrack.h"
#include "Blitlist.h"
#include "SoundEvents.h"
#include <map>
#include <tuple>
#include <random>

class RcRule
{
public:
	enum RuleType { BLOCK, PIXEL, SOUND_BLOCK, SOUND_PIXEL };
	
	RcRule(const std::string& line);

	RuleType GetType() const { return mType; }
	int GetZxHeight() const { return mZxHeight; }
	int GetZxWidth() const { return mZxWidth; }
	void AddToSoundEvents(bool appears, bool disappears, SoundEvents& events) const;
	void AddToBlitList(unsigned x, unsigned y, BlitList& blitlist) const;
	bool IsFoundColor(unsigned* dst, unsigned x, unsigned y) const;
	bool IsFoundAt(const uint8_t* curptr, unsigned offset) const;
	bool IsValidPosition(unsigned x, unsigned y, unsigned offset) const;
	bool IsApproximateMatch(const uint8_t* curptr, unsigned offset) const;
	bool IsProtected() const;
	unsigned short GetZxKey() const;
	bool operator<(const RcRule& rhs) const;

private:
	RuleType mType;
	int mID;
	int mZxHeight;
	int mZxWidth;
	bool mMatchColor = false;
	int mOffsetX = 0, mOffsetY = 0;
	unsigned mRuleX = 0, mRuleY = 0;
	bool mUseRuleXY = false;
	std::vector<std::tuple<int, int, unsigned>> mXYColor;
	int mLayer;	// or channel

	bool mAppearsFlag = false;
	bool mDisappearsFlag = false;
	bool mMuteAyFlag = false;
	bool mMuteBeeperFlag = false;
	bool mProtectedFlag = false;
	bool mLoopFlag = false;
	//bool mSeqFlag = false;
	//bool mRandFlag = false;
	unsigned mProtectedPixels;

	std::vector<std::shared_ptr<RcImage>> ZxImages; // old is always small-screen (x1)
	std::shared_ptr<RcImage> ZxImage;		// maybe we should eventually keep ZxImages only
	std::shared_ptr<RcImage> RecoloredImage;
	std::vector<std::shared_ptr<SoundTrack>> Sounds;

	static std::map<std::string, unsigned> mNameRGB;
	static unsigned mRuleCount;
	static std::random_device mRandomDevice;
	static std::mt19937 mRandom;

	static bool hasSeparator(const std::string& str);
	static bool isFlagFound(const std::set<std::string>& flags, const std::string& flag);
	static std::string leftPart(const std::string& str);
	static std::string rightPart(const std::string& str);
	static std::string leftCommaPart(const std::string& str);
	static std::string rightCommaPart(const std::string& str);
	static RuleType parseRuleType(const std::string& type);
	static decltype(mXYColor) parseColorPart(const std::string& full_color_part);
};
