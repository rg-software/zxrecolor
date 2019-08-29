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

	void AddToSoundEvents(bool appears, bool disappears, SoundEvents& events) const
	{
		if(mAppearsFlag && appears)
			events.AddElement(mMuteAyFlag, mMuteBeeperFlag, mID, mLayer, Sound);

		if(mDisappearsFlag && disappears)
			events.AddElement(mMuteAyFlag, mMuteBeeperFlag, mID, mLayer, Sound);
	}

	void AddToBlitList(unsigned x, unsigned y, BlitList& blitlist) const
	{
		blitlist.AddElement(x + mOffsetX - 8 * ZxImage->GetKeyOffsetX(), y + mOffsetY - ZxImage->GetKeyOffsetY(), RecoloredImage, mLayer);
	}

	bool IsFoundColor(unsigned* dst, unsigned x, unsigned y) const;
	bool IsFoundAt(const uint8_t* curptr, unsigned offset) const;
	bool IsFoundAt(const uint8_t* curptr) const;

	unsigned short GetZxKey() const { return ZxImage->GetZxKey(); /*ZxImages[0]->GetZxKey();*/ }
	
	bool operator<(const RcRule& rhs) const
	{
		return GetZxKey() < rhs.GetZxKey();
	}

private:
	RuleType mType;
	int mID;
	int mZxHeight;
	int mZxWidth;
	bool mMatchColor;
	int mOffsetX, mOffsetY;
	std::vector<std::tuple<int, int, unsigned>> mXYColor;
	//int mColorX, mColorY;
	//unsigned mColor;
	int mLayer;	// or channel

	bool mAppearsFlag;
	bool mDisappearsFlag;
	bool mMuteAyFlag;
	bool mMuteBeeperFlag;

	std::vector<std::shared_ptr<RcImage>> ZxImages; // old is always small-screen (x1)
	std::shared_ptr<RcImage> ZxImage;		// maybe we should eventually keep ZxImages only
	std::shared_ptr<RcImage> RecoloredImage;
	std::shared_ptr<SoundTrack> Sound;

	static std::map<std::string, unsigned> mNameRGB;
	static unsigned mRuleCount;
};
