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
#include "RcRule.h"
#include "RcRuleset.h"

std::map<RcRule::RuleType, RcRuleset> AllRules;

void setup_custom()
{
	_chdir("game");
	std::ifstream is("settings.txt");
	std::string line;
	while(std::getline(is, line))
	{
		if(line[0] == ';') continue;

		RcRule rule(line);
		AllRules[rule.Type].Add(rule);
	}

	for(auto& e : AllRules)
		e.second.BuildIndex();
}

struct blist_list_element
{
	unsigned x, y;
	const RcImage* image;
	int layer;
	blist_list_element(unsigned x_, unsigned y_, const RcImage* image_, int layer_) : x(x_), y(y_), image(image_), layer(layer_) {}

	bool operator<(const blist_list_element& rhs) const { return layer < rhs.layer; }
};

struct create_blit_list
{
	std::vector<blist_list_element>& blitlist;
	create_blit_list(std::vector<blist_list_element>& blitlist_) : blitlist(blitlist_) {}
	
	void operator()(unsigned x, unsigned y, const RcImage* image, int layer) 
	{ 
		blitlist.emplace_back(x*2, y*2, image, layer);
	}
};

void blit_all(unsigned pitch, unsigned char* dst, const std::vector<blist_list_element>& blitlist)
{
	for(auto p : blitlist)
		p.image->Blit(p.x, p.y, pitch, dst);
}

// find a list of found zx-images (only perfect matches within 8*8 blocks are found)
template<typename Op> void foreach_zxblock(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, Op op)
{
	RcRuleset& Rules = AllRules[RcRule::BLOCK];

	for(unsigned scry = 0; scry < 240 - 8; scry += 8) // every 8 pixels
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned short key = (*curptr)*256 + *(curptr + 320/8);
			
			for (auto p = Rules.BeginByKey(key); p != Rules.EndByKey(key); ++p)
				if (scry + p->ZxHeight < 240 &&
					p->IsFoundColor(dst, pitch, scrx * 8, scry) && p->IsFoundAt(curptr, 0))
				{
					//p->AddToBlitList(scrx * 8, scry, blitlist)
					op(scrx * 8 + p->OffsetX, scry + p->OffsetY, &p->RecoloredImage, p->Layer);
				}
			
			++curptr;
		}
	}
}

template<typename Op> void foreach_zxpixel(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, Op op)
{
	RcRuleset& Rules = AllRules[RcRule::PIXEL];

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

				for (auto p = Rules.BeginByKey(key); p != Rules.EndByKey(key); ++p)
				{
					// fix this checking code (prepare the image?)
					if (scry + p->ZxHeight < 240 &&
						p->IsFoundColor(dst, pitch, scrx * 8 + offset, scry) &&
						p->IsFoundAt(curptr, offset))
						op(scrx * 8 + offset + p->OffsetX, scry + p->OffsetY, &p->RecoloredImage, p->Layer);
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
