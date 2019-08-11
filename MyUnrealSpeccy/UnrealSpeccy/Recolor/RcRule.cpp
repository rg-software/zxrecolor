#include "RcRule.h"
#include <string>
#include <sstream>

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
{
	MatchColor = false;
	OffsetX = OffsetY = 0;
	
	std::string type, orig_pic, new_pic;

	std::istringstream iss(line);
	iss >> Layer >> type >> orig_pic >> new_pic;

	if(orig_pic.find('|') != std::string::npos) // color part is found (orig_pic|color_part)
	{
		std::string color_part = orig_pic.substr(orig_pic.find('|') + 1);
		orig_pic = orig_pic.substr(0, orig_pic.find('|'));

		MatchColor = true; // color_part = "x,y,color_name"
		ColorX = atoi(color_part.substr(0, color_part.find(',')).c_str());
		color_part = color_part.substr(color_part.find(',') + 1);
		ColorY = atoi(color_part.substr(0, color_part.find(',')).c_str());
		color_part = color_part.substr(color_part.find(',') + 1);
		if(mNameRGB.find(color_part) == mNameRGB.end())
			throw std::runtime_error("Incorrect color name"); // should never happen
		Color = mNameRGB[color_part];
	}

	if(new_pic.find('|') != std::string::npos)
	{
		std::string xy_part = new_pic.substr(new_pic.find('|') + 1);
		new_pic = new_pic.substr(0, new_pic.find('|'));

		OffsetX = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
		xy_part = xy_part.substr(xy_part.find(',') + 1);
		OffsetY = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
	}


	if (type == "block")
		Type = BLOCK;
	else if (type == "pixel")
		Type = PIXEL;
	else
		throw std::runtime_error("Unknown rule type " + type);

	ZxImage = std::make_shared<RcImage>(orig_pic, true, Type == PIXEL);

	for (unsigned i = 0; i < 8; ++i)
		ZxImages.push_back(std::make_shared<RcImage>(ZxImage, i));

	RecoloredImage = std::make_unique<RcImage>(new_pic);

	ZxHeight = ZxImage->GetHeight();
	ZxWidth = ZxImage->GetWidth();
}

bool RcRule::IsFoundColor(unsigned char *dst, unsigned pitch, unsigned x, unsigned y) const
{
	// $mm double-check with new "auto-key finding" option
	if (!MatchColor)
		return true;

	unsigned* dst_buff = (unsigned*)dst + (x * 2 + ColorX) + 640 * (y * 2 + ColorY);	// $mm CONST
	return *dst_buff == Color;
}

bool RcRule::IsFoundAt(const uint8_t* curptr) const
{
	return ZxImage->IsFoundAt(curptr);
}

bool RcRule::IsFoundAt(const uint8_t* curptr, unsigned offset) const
{
	return ZxImages[offset]->IsFoundAt(curptr);
}
