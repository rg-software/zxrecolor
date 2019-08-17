#include "RcImage.h"
#include "SoundTrack.h"
#include "BlitList.h"
#include <map>

class RcRule
{
public:
	enum RuleType { BLOCK, PIXEL, SOUND };
	
	RcRule(const std::string& line);

	RuleType GetType() const { return Type; }
	int GetZxHeight() const { return ZxHeight; }
	int GetZxWidth() const { return ZxWidth; }

	void AddToBlitList(unsigned x, unsigned y, BlitList& blitlist) const
	{
		blitlist.AddElement(x + OffsetX - 8 * ZxImage->GetKeyOffsetX(), y + OffsetY - ZxImage->GetKeyOffsetY(), RecoloredImage, Layer);
	}

	bool IsFoundColor(unsigned char *dst, unsigned x, unsigned y) const;
	bool IsFoundAt(const uint8_t* curptr, unsigned offset) const;
	bool IsFoundAt(const uint8_t* curptr) const;

	unsigned short GetZxKey() const { return ZxImage->GetZxKey(); /*ZxImages[0]->GetZxKey();*/ }
	
	bool operator<(const RcRule& rhs) const
	{
		return GetZxKey() < rhs.GetZxKey();
	}

private:
	RuleType Type;
	int ZxHeight;
	int ZxWidth;
	bool MatchColor;
	int OffsetX, OffsetY;
	int ColorX, ColorY;
	unsigned Color;
	int Layer;

	std::vector<std::shared_ptr<RcImage>> ZxImages; // old is always small-screen (x1)
	std::shared_ptr<RcImage> ZxImage;		// maybe we should eventually keep ZxImages only
	std::shared_ptr<RcImage> RecoloredImage;
	std::shared_ptr<SoundTrack> Sound;

	static std::map<std::string, unsigned> mNameRGB;
};
