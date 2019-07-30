#include "RcImage.h"
#include "BlitList.h"
#include <map>

class RcRule
{
public:
	enum RuleType { BLOCK, PIXEL };
	
	RuleType GetType() const { return Type; }
	int GetZxHeight() const { return ZxHeight; }
	int GetZxWidth() const { return ZxWidth; }

	RcRule() : MatchColor(false), OffsetY(0), OffsetX(0) {}
	RcRule(const std::string& line);


	void AddToBlitList(unsigned x, unsigned y, BlitList& blitlist) const
	{
		blitlist.AddElement(x + OffsetX, y + OffsetY, &RecoloredImage, Layer);
	}

	bool IsFoundColor(unsigned char *dst, unsigned pitch, unsigned x, unsigned y) const;
	bool IsFoundAt(uint8_t* curptr, unsigned offset) const;
	bool IsFoundAt(uint8_t* curptr) const;

	unsigned short GetZxKey() const { return ZxImages[0].GetZxKey(); }
	
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

	RcImage ZxImages[8]; // old is always small-screen (x1)
	RcImage ZxImage;
	RcImage RecoloredImage;

	static 	std::map<std::string, unsigned> mNameRGB;
};
