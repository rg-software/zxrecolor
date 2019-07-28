#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include "RcImage.h"

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
}
NameRGB;

struct MyRule
{
	enum RuleType { DUMMY_BEG_TYPE = 0, BLOCK, PIXEL, DUMMY_END_TYPE } Type;
	RcImage PcImage;
	RcImage ZxImage, ZxMask; // old is always small-screen (x1)
	RcImage ZxImages[8], ZxMasks[8];
	bool MatchColor;
	int ColorX, ColorY;
	unsigned Color;
	int OffsetX, OffsetY;
	int Layer;

	MyRule() : MatchColor(false), OffsetY(0), OffsetX(0) {}
	MyRule(const std::string& line)
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

			MatchColor = true;	// color_part = "x,y,color_name"
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
		ZxMask.LoadColored(orig_pic, true, true);
		
		for(unsigned i = 0; i < 8; ++i)
		{
			ZxImages[i].CopyAndShift(ZxImage, i);
			ZxMasks[i].CopyAndShift(ZxMask, i);
		}

		PcImage.LoadColored(new_pic);

		if(type == "block")
			Type = BLOCK;
		else if(type == "pixel")
			Type = PIXEL;
	}

	unsigned short GetZxKey() const { return ZxImage.GetZxKey(); }
	
	bool operator<(const MyRule& rhs) const
	{
		return GetZxKey() < rhs.GetZxKey();
	}
};

const unsigned char MAXRULES = 250;

struct MyRules
{
	MyRule Rules[MAXRULES];					// all rules of the same kind
	unsigned RulesCount;					// the actual number of rules
	unsigned char Index[256*256];			// start index of the rule specified by key in Rules[]
	unsigned char IndexCount[256*256];		// number of rules specified by key

	MyRules()
	{
		RulesCount = 0;
		std::fill_n(Index, 256*256, 0);
		std::fill_n(IndexCount, 256*256, 0);
	}

	void Add(const MyRule& rule)
	{
		if(RulesCount == MAXRULES)
			throw std::exception("MAXRULES exceeded"); // should never happen
		Rules[RulesCount++] = rule;
	}

	void BuildIndex()
	{
		std::sort(Rules, Rules + RulesCount); // sort rules by key
		
		for(unsigned i = 0; i < RulesCount; ++i)
		{
			unsigned short key = Rules[i].GetZxKey();
			if(IndexCount[key] == 0)
				Index[key] = i;		// start index of key-rules
			IndexCount[key]++;
		}
	}

};

MyRules* AllRules = new MyRules[MyRule::DUMMY_END_TYPE];

void setup_custom()
{
	_chdir("game");
	std::ifstream is("settings.txt");
	std::string line;
	while(std::getline(is, line))
	{
		if(line[0] == ';') continue;

		MyRule rule(line);
		AllRules[rule.Type].Add(rule);
	}

	for(unsigned i = 0; i < MyRule::DUMMY_END_TYPE; ++i)
		AllRules[i].BuildIndex();
}

// checks whether a zx-image is found under curptr
bool image_found_at(unsigned char* curptr, RcImage& image, RcImage& mask)
{
	return image.IsFoundAt(curptr, mask);
}

struct blist_list_element
{
	unsigned x, y;
	RcImage* image;
	int layer;
	blist_list_element(unsigned x_, unsigned y_, RcImage* image_, int layer_) : x(x_), y(y_), image(image_), layer(layer_) {}

	bool operator<(const blist_list_element& rhs) const { return layer < rhs.layer; }
};

struct create_blit_list
{
	std::vector<blist_list_element>& blitlist;
	create_blit_list(std::vector<blist_list_element>& blitlist_) : blitlist(blitlist_) {}
	
	void operator()(unsigned x, unsigned y, RcImage* image, int layer) 
	{ 
		blitlist.push_back(blist_list_element(x*2, y*2, image, layer));
	}
};

void blit_all(unsigned pitch, unsigned char* dst, const std::vector<blist_list_element>& blitlist)
{
	for (std::vector<blist_list_element>::const_iterator p = blitlist.begin(); p != blitlist.end(); ++p)
		p->image->Blit(p->x, p->y, pitch, dst);
}

bool found_color(unsigned char *dst, unsigned pitch, unsigned x, unsigned y, MyRule& rule)
{
	if(!rule.MatchColor)
		return true;

	unsigned* dst_buff = (unsigned*)dst + (x*2 + rule.ColorX) + 640*(y*2 + rule.ColorY);
	return *dst_buff == rule.Color;
}

// find a list of found zx-images (only perfect matches within 8*8 blocks are found)
template<typename Op> void foreach_zxblock(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, Op op)
{
	MyRules& Rules = AllRules[MyRule::BLOCK];

	for(unsigned scry = 0; scry < 240 - 8; scry += 8) // every 8 pixels
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned short key = (*curptr)*256 + *(curptr + 320/8);
			unsigned char index = Rules.Index[key];			// find the corresponding rules (possibly matching sprites, 2 bytes)
			unsigned char count = Rules.IndexCount[key];

			for(unsigned char i = index; i < index + count; ++i) // check carefully
			{
				if(scry + Rules.Rules[i].ZxImage.Height < 240 && 
								found_color(dst, pitch, scrx*8, scry, Rules.Rules[i]) && 
								image_found_at(curptr, Rules.Rules[i].ZxImage, Rules.Rules[i].ZxMask))
					op(scrx*8 + Rules.Rules[i].OffsetX, scry + Rules.Rules[i].OffsetY, &Rules.Rules[i].PcImage, Rules.Rules[i].Layer);
			}

			++curptr;
		}
	}
}

template<typename Op> void foreach_zxpixel(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, Op op)
{
	MyRules& Rules = AllRules[MyRule::PIXEL];

	for(unsigned scry = 0; scry < 240 - 8; ++scry) // every pixel
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned short curptr_v = (*curptr)*256 + *(curptr+1);
			unsigned short curptr_next_v = (*(curptr + 320/8))*256 + *(curptr + 320/8 + 1);

			for(unsigned offset = 0; offset < 8; ++offset) // check all 8 possible offsets
			{
				//unsigned short key = ((curptr_v << offset) >> 8)*256 + ((curptr_next_v << offset) >> 8);
				unsigned short key = HIBYTE((curptr_v << offset)) * 256 + HIBYTE((curptr_next_v << offset));
				
				unsigned char index = Rules.Index[key];			// find the corresponding rules (possibly matching sprites, 2 bytes)
				unsigned char count = Rules.IndexCount[key];

				for(unsigned char i = index; i < index + count; ++i) // check carefully
				{
					// fix this checking code (prepare the image?)
					if(scry + Rules.Rules[i].ZxImage.Height < 240 && 
								found_color(dst, pitch, scrx*8 + offset, scry, Rules.Rules[i]) &&
								image_found_at(curptr, Rules.Rules[i].ZxImages[offset], Rules.Rules[i].ZxMasks[offset]))
						op(scrx*8 + offset + Rules.Rules[i].OffsetX, scry + Rules.Rules[i].OffsetY, &Rules.Rules[i].PcImage, Rules.Rules[i].Layer);
				}
			}

			++curptr;
		}
	}
}

void recolor_render_impl(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, unsigned char* color_screen)
{
	static bool first = true;
	if(first) { setup_custom(); first = false; }

	std::vector<blist_list_element> blitlist;

	foreach_zxblock(dst, pitch, zx_screen, create_blit_list(blitlist));
	foreach_zxpixel(dst, pitch, zx_screen, create_blit_list(blitlist));
	
	std::sort(blitlist.begin(), blitlist.end());
	blit_all(pitch, dst, blitlist);
}
