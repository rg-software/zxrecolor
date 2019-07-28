#include "RcImage.h"
#include <D3DTYPES.H>
#include <map>

struct TNameRGB
{
	std::map<std::string, unsigned> RGB;
	TNameRGB()
	{
		RGB["black"] = RGB_MAKE(0, 0, 0);
		RGB["blue"] = RGB_MAKE(0, 0, 0xBF);
		RGB["red"] = RGB_MAKE(0xBF, 0, 0);
		RGB["magenta"] = RGB_MAKE(0xBF, 0, 0xBF);
		RGB["green"] = RGB_MAKE(0, 0xBF, 0);
		RGB["cyan"] = RGB_MAKE(0, 0xBF, 0xBF);
		RGB["yellow"] = RGB_MAKE(0xBF, 0xBF, 0);
		RGB["white"] = RGB_MAKE(0xBF, 0xBF, 0xBF);
		RGB["black+"] = RGB_MAKE(0, 0, 0);
		RGB["blue+"] = RGB_MAKE(0, 0, 0xFE);
		RGB["red+"] = RGB_MAKE(0xFE, 0, 0);
		RGB["magenta+"] = RGB_MAKE(0xFE, 0, 0xFE);
		RGB["green+"] = RGB_MAKE(0, 0xFE, 0);
		RGB["cyan+"] = RGB_MAKE(0, 0xFE, 0xFE);
		RGB["yellow+"] = RGB_MAKE(0xFE, 0xFE, 0);
		RGB["white+"] = RGB_MAKE(0xFE, 0xFE, 0xFE);
	}
};

struct MyRule
{
	enum RuleType { DUMMY_BEG_TYPE = 0, BLOCK, PIXEL, DUMMY_END_TYPE } Type;
	RcImage PcImage;
	RcImage ZxImage; // old is always small-screen (x1)
	RcImage ZxImages[8];
	bool MatchColor;
	int ColorX, ColorY;
	unsigned Color;
	int OffsetX, OffsetY;
	int Layer;

	MyRule() : MatchColor(false), OffsetY(0), OffsetX(0) {}
	MyRule(const std::string& line);

	unsigned short GetZxKey() const { return ZxImage.GetZxKey(); }
	
	bool operator<(const MyRule& rhs) const
	{
		return GetZxKey() < rhs.GetZxKey();
	}
};
