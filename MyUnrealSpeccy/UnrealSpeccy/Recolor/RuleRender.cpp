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
#include "BlitList.h"


std::map<RcRule::RuleType, RcRuleset> AllRules;

void LoadRules()
{
	_chdir("game");
	std::ifstream is("settings.txt");
	std::string line;
	while(std::getline(is, line))
	{
		if(line.empty() || line[0] == ';' || line[0] == '\r' || line[0] == '\n') 
			continue;

		RcRule rule(line);
		AllRules[rule.GetType()].Add(rule);
	}

	for(auto& e : AllRules)
		e.second.BuildIndex();
}

// find a list of found zx-images (only perfect matches within 8*8 blocks are found)
void RunBlockRules(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, BlitList& blitlist)
{
	RcRuleset& Rules = AllRules[RcRule::BLOCK];

	for(unsigned scry = 0; scry < 240 - 8; scry += 8) // every 8 pixels
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned short curKey = MAKEWORD(*(curptr + 320 / 8), *curptr);
			unsigned short keys[] = { 0,  curKey == 0 ? 0xFFFF : curKey }; // we don't have sprites with 0xFFFF key, so it will be skipped
			for(auto key : keys)
			{
				auto endIt = Rules.EndByKey(key);
				for (auto p = Rules.BeginByKey(key); p != endIt; ++p)
					if (scry + p->GetZxHeight() < 240 && p->IsFoundColor(dst, pitch, scrx * 8, scry) && p->IsFoundAt(curptr))
					{
						p->AddToBlitList(scrx * 8, scry, blitlist);
					}
			}
			
			++curptr;
		}
	}
}

void RunPixelRules(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, BlitList& blitlist)
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
				unsigned short curKey = HIBYTE((curptr_v << offset)) * 256 + HIBYTE((curptr_next_v << offset));
				unsigned short keys[] = { 0, curKey == 0 ? 0xFFFF : curKey };

				for(auto key : keys)	// always handle 0 key
				{
					auto endIt = Rules.EndByKey(key);
					for (auto p = Rules.BeginByKey(key); p != endIt; ++p)
					{
						// fix this checking code (prepare the image?)
						if (scry + p->GetZxHeight() < 240 && p->IsFoundColor(dst, pitch, scrx * 8 + offset, scry) && p->IsFoundAt(curptr, offset))
						{
							p->AddToBlitList(scrx * 8 + offset, scry, blitlist);
						}
					}
				}
			}

			++curptr;
		}
	}
}

void RunSoundRules()
{
	RcRuleset& Rules = AllRules[RcRule::SOUND];

}

void recolor_render_impl(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, unsigned char* color_screen)
{
	static bool firstRun = true;
	
	if(firstRun)
	{
		LoadRules(); 
		firstRun = false;
	}

	BlitList blitlist;

	RunBlockRules(dst, pitch, zx_screen, blitlist);
	RunPixelRules(dst, pitch, zx_screen, blitlist);
	RunSoundRules();

	blitlist.SortAndBlit(pitch, dst);
}
