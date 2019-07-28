#include "RcRule.h"
#include <string>
#include <sstream>

TNameRGB NameRGB;

MyRule::MyRule(const std::string& line)
{
	MatchColor = false;
	OffsetX = OffsetY = 0;
	std::istringstream iss(line);

	std::string type;
	std::string orig_pic, new_pic;
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
		if(NameRGB.RGB.find(color_part) == NameRGB.RGB.end())
			throw std::exception("Incorrect color name"); // should never happen
		Color = NameRGB.RGB[color_part];
	}

	if(new_pic.find('|') != std::string::npos)
	{
		std::string xy_part = new_pic.substr(new_pic.find('|') + 1);
		new_pic = new_pic.substr(0, new_pic.find('|'));

		OffsetX = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
		xy_part = xy_part.substr(xy_part.find(',') + 1);
		OffsetY = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
	}

	ZxImage.LoadColored(orig_pic, true, false);

	for(unsigned i = 0; i < 8; ++i)
		ZxImages[i].CopyWithShift(ZxImage, i);

	PcImage.LoadColored(new_pic);

	if(type == "block")
		Type = BLOCK;
	else if(type == "pixel")
		Type = PIXEL;
}
