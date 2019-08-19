#pragma once

class RcRuleVector
{
public:
	RcRuleVector()
	{
		std::fill_n(mIndex, 256*256, 0);
		std::fill_n(mIndexCount, 256*256, 0);
	}

	void Add(std::shared_ptr<RcRule> rule)
	{
		mRules.push_back(rule);
	}

	std::vector<std::shared_ptr<RcRule>>::const_iterator BeginByKey(uint16_t key) const
	{
		return mRules.begin() + mIndex[key];
	}

	std::vector<std::shared_ptr<RcRule>>::const_iterator EndByKey(uint16_t key) const
	{
		return mRules.begin() + mIndex[key] + mIndexCount[key];
	}
	
	void BuildIndex()
	{
		std::sort(mRules.begin(), mRules.end(), [](auto lhs, auto rhs) { return *lhs < *rhs; }); // sort rules by key
		
		for(unsigned i = 0; i < mRules.size(); ++i)
		{
			unsigned short key = mRules[i]->GetZxKey();
			if(mIndexCount[key] == 0)
				mIndex[key] = i;		// start index of key-rules
			mIndexCount[key]++;
		}

		for(unsigned i = 0; i < 256*256; ++i)	// for checking hash collisions
		{
			if (mIndexCount[i] > 1)
				printf("Sprites for key %d: %d\n", i, mIndexCount[i]);
		}
	}

private:
	uint16_t mIndex[256 * 256]{};			// start index of the rule specified by key in mRules[]
	uint16_t mIndexCount[256 * 256]{};		// number of rules specified by key
	std::vector<std::shared_ptr<RcRule>> mRules;			// all rules of the same kind
};
