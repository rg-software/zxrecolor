#include "RcImage.h"
#include <map>

class RcRule
{
public:
	enum RuleType { DUMMY_BEG_TYPE = 0, BLOCK, PIXEL, DUMMY_END_TYPE } Type;
	RcImage RecoloredImage;
	RcImage ZxImage; // old is always small-screen (x1)
	RcImage ZxImages[8];
	bool MatchColor;
	int ColorX, ColorY;
	unsigned Color;
	int OffsetX, OffsetY;
	int Layer;


	RcRule() : MatchColor(false), OffsetY(0), OffsetX(0) {}
	RcRule(const std::string& line);

	unsigned short GetZxKey() const { return ZxImage.GetZxKey(); }
	
	bool operator<(const RcRule& rhs) const
	{
		return GetZxKey() < rhs.GetZxKey();
	}

private:
	static 	std::map<std::string, unsigned> mNameRGB;
};
