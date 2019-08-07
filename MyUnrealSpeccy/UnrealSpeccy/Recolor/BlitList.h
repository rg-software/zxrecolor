#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include "RcImage.h"

class BlitList
{
public:
	struct BlitListElement
	{
		unsigned x, y;
		std::shared_ptr<RcImage> image;
		int layer;
		BlitListElement(unsigned x_, unsigned y_, std::shared_ptr<RcImage> image_, int layer_) 
			: x(x_), y(y_), image(std::move(image_)), layer(layer_) {}

		bool operator<(const BlitListElement& rhs) const { return layer < rhs.layer; }
	};

	void AddElement(unsigned x, unsigned y, std::shared_ptr<RcImage> image, int layer)
	{
		blitlist.emplace_back(x * 2, y * 2, image, layer);
	}

	void SortAndBlit(unsigned pitch, unsigned char* dst)
	{
		std::sort(blitlist.begin(), blitlist.end());

		for (auto p : blitlist)
			p.image->Blit(p.x, p.y, pitch, dst);
	}

private:
	std::vector<BlitListElement> blitlist;
};
