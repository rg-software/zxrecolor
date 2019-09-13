#include "RcImage.h"
#include "SoundTrack.h"
#include "Blitlist.h"
#include "SoundEvents.h"
#include <map>
#include <tuple>

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
	bool IsValidPosition(unsigned x, unsigned y) const;
	bool IsApproximateMatch(const uint8_t* curptr, unsigned offset) const;
	bool IsProtected() const;
	unsigned short GetZxKey() const;
	bool operator<(const RcRule& rhs) const;

private:
	RuleType mType;
	int mID;
	int mZxHeight;
	int mZxWidth;
	bool mMatchColor;
	int mOffsetX, mOffsetY;
	unsigned mRuleX, mRuleY;
	bool mUseRuleXY;
	std::vector<std::tuple<int, int, unsigned>> mXYColor;
	int mLayer;	// or channel

	bool mAppearsFlag;
	bool mDisappearsFlag;
	bool mMuteAyFlag;
	bool mMuteBeeperFlag;
	bool mProtectedFlag;
	unsigned mProtectedPixels;

	std::vector<std::shared_ptr<RcImage>> ZxImages; // old is always small-screen (x1)
	std::shared_ptr<RcImage> ZxImage;		// maybe we should eventually keep ZxImages only
	std::shared_ptr<RcImage> RecoloredImage;
	std::shared_ptr<SoundTrack> Sound;

	static std::map<std::string, unsigned> mNameRGB;
	static unsigned mRuleCount;
};
