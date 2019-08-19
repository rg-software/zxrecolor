#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

#include "RcImage.h"
#include "RcRule.h"
#include "RcRuleVector.h"
#include "Blitlist.h"
#include "SoundEvents.h"
#include <iterator>

typedef std::set<std::shared_ptr<RcRule>> RuleSet;
std::map<RcRule::RuleType, RcRuleVector> AllRules;
SoundEvents::ActiveSoundsMap ActiveSounds;
RuleSet PrevMatchedRules;

void LoadRules()
{
	_chdir("game");
	std::ifstream is("settings.txt");
	std::string line;
	while(std::getline(is, line))
	{
		if(line.empty() || line[0] == ';' || line[0] == '\r' || line[0] == '\n') 
			continue;

		auto rule = std::make_shared<RcRule>(line);
		AllRules[rule->GetType()].Add(rule);
	}

	for(auto& e : AllRules)
		e.second.BuildIndex();
}

// find a list of found zx-images (only perfect matches within 8*8 blocks are found)
template<typename Op> void RunBlockRules(const RcRuleVector& Rules, unsigned char *dst, unsigned char* zx_screen, Op op)
{
	for(unsigned scry = 0; scry < 240 - 8; scry += 8) // every 8 pixels
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned curKey = MAKEWORD(*(curptr + 320 / 8), *curptr);
			unsigned keys[] = { 0,  curKey == 0 ? 0xFFFF : curKey }; // we don't have sprites with 0xFFFF key, so it will be skipped
			for(auto key : keys)
			{
				auto endIt = Rules.EndByKey(key);
				for (auto p = Rules.BeginByKey(key); p != endIt; ++p)
					op(scry, scrx * 8, dst, *p, curptr);
			}
			
			++curptr;
		}
	}
}

template<typename Op> void RunPixelRules(const RcRuleVector& Rules, unsigned char *dst, unsigned char* zx_screen, Op op)
{
	for(unsigned scry = 0; scry < 240 - 8; ++scry) // every pixel
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned short curptr_v = (*curptr)*256 + *(curptr+1);
			unsigned short curptr_next_v = (*(curptr + 320/8))*256 + *(curptr + 320/8 + 1);

			for(unsigned offset = 0; offset < 8; ++offset) // check all 8 possible offsets
			{
				unsigned curKey = HIBYTE((curptr_v << offset)) * 256 + HIBYTE((curptr_next_v << offset));
				unsigned keys[] = { 0, curKey == 0 ? 0xFFFF : curKey };

				for(auto key : keys)	// always handle 0 key
				{
					auto endIt = Rules.EndByKey(key);
					for (auto p = Rules.BeginByKey(key); p != endIt; ++p)
						op(scry, scrx * 8, offset, dst, *p, curptr);
				}
			}

			++curptr;
		}
	}
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
	SoundEvents soundevents(ActiveSounds);
	RuleSet MatchedRules;

	RunBlockRules(AllRules[RcRule::BLOCK], dst, zx_screen, [&blitlist](unsigned y, unsigned x, unsigned char *dst, std::shared_ptr<RcRule> rule, unsigned char* curptr)
			{
				if (y + rule->GetZxHeight() < 240 && rule->IsFoundColor(dst, x, y) && rule->IsFoundAt(curptr))
					rule->AddToBlitList(x, y, blitlist);
			});

	RunPixelRules(AllRules[RcRule::PIXEL], dst, zx_screen, [&blitlist](unsigned y, unsigned x, unsigned offset, unsigned char *dst, std::shared_ptr<RcRule> rule, unsigned char* curptr)
			{
				if (y + rule->GetZxHeight() < 240 && rule->IsFoundColor(dst, x + offset, y) && rule->IsFoundAt(curptr, offset))
					rule->AddToBlitList(x + offset, y, blitlist);
			});


	RunBlockRules(AllRules[RcRule::SOUND_BLOCK], dst, zx_screen, [&MatchedRules](unsigned y, unsigned x, unsigned char *dst, std::shared_ptr<RcRule> rule, unsigned char* curptr)
	{
		if (y + rule->GetZxHeight() < 240 && rule->IsFoundColor(dst, x, y) && rule->IsFoundAt(curptr))	// $mm to refactor
			MatchedRules.insert(rule);
	});
	
	RunPixelRules(AllRules[RcRule::SOUND_PIXEL], dst, zx_screen, [&MatchedRules](unsigned y, unsigned x, unsigned offset, unsigned char *dst, std::shared_ptr<RcRule> rule, unsigned char* curptr)
	{
		if (y + rule->GetZxHeight() < 240 && rule->IsFoundColor(dst, x + offset, y) && rule->IsFoundAt(curptr, offset))	// $mm to refactor
			MatchedRules.insert(rule);
	});

	blitlist.SortAndBlit(pitch, dst);

	RuleSet appeared, disappeared;
	
	std::set_difference(MatchedRules.begin(), MatchedRules.end(), PrevMatchedRules.begin(), PrevMatchedRules.end(), std::inserter(appeared, appeared.begin()));
	std::set_difference(PrevMatchedRules.begin(), PrevMatchedRules.end(), MatchedRules.begin(), MatchedRules.end(), std::inserter(disappeared, disappeared.begin()));
	
	for (const auto& rule : appeared)
		rule->AddToSoundEvents(true, false, soundevents);

	for (const auto& rule : disappeared)
		rule->AddToSoundEvents(false, true, soundevents);

	soundevents.Apply();
	soundevents.Update();

	PrevMatchedRules = MatchedRules;
}
