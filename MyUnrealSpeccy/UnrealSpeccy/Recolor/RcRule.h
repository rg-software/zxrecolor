#include "RcImage.h"
#include <map>

class RcRule
{
public:
	enum RuleType { BLOCK, PIXEL } Type;
	RcImage RecoloredImage;
	bool MatchColor;
	int ColorX, ColorY;
	unsigned Color;
	int OffsetX, OffsetY;
	int Layer;

	int ZxHeight;
	int ZxWidth;

	RcRule() : MatchColor(false), OffsetY(0), OffsetX(0) {}
	RcRule(const std::string& line);


	bool IsFoundColor(unsigned char *dst, unsigned pitch, unsigned x, unsigned y) const;
	bool IsFoundAt(uint8_t* curptr, unsigned offset) const;

	unsigned short GetZxKey() const { return ZxImages[0].GetZxKey(); }
	
	bool operator<(const RcRule& rhs) const
	{
		return GetZxKey() < rhs.GetZxKey();
	}

private:
	RcImage ZxImages[8]; // old is always small-screen (x1)

	static 	std::map<std::string, unsigned> mNameRGB;
};
