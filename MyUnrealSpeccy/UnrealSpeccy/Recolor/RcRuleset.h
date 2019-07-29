#pragma once

class RcRuleset
{
public:
	RcRuleset()
	{
		std::fill_n(Index, 256*256, 0);
		std::fill_n(IndexCount, 256*256, 0);
	}

	void Add(const RcRule& rule)
	{
		Rules.push_back(rule);
	}

	std::vector<RcRule>::const_iterator BeginByKey(uint16_t key) const
	{
		return Rules.begin() + Index[key];
	}

	std::vector<RcRule>::const_iterator EndByKey(uint16_t key) const
	{
		return Rules.begin() + Index[key] + IndexCount[key];
	}
	
	void BuildIndex()
	{
		std::sort(Rules.begin(), Rules.end()); // sort rules by key
		
		for(unsigned i = 0; i < Rules.size(); ++i)
		{
			unsigned short key = Rules[i].GetZxKey();
			if(IndexCount[key] == 0)
				Index[key] = i;		// start index of key-rules
			IndexCount[key]++;
		}
	}

private:
	uint16_t Index[256 * 256]{};			// start index of the rule specified by key in Rules[]
	uint16_t IndexCount[256 * 256]{};		// number of rules specified by key
	std::vector<RcRule> Rules;			// all rules of the same kind
};
