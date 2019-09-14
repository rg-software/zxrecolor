#pragma once
#include <string>
#include <vector>
#include "../dx/include/d3dtypes.h"

class RcImage
{
public:
	RcImage(const std::string& bmpName);									// load 24-bit bmp
	RcImage(const std::string& bmpName, bool convertZx, bool pixelAutoKey);	// load 24-bit bmp and convert to zx image
	RcImage(std::shared_ptr<RcImage> src_image, unsigned offset);			// copy from another image

	bool IsFoundAt(const uint8_t* curptr) const;
	unsigned MatchingPixelsCount(const uint8_t* curptr) const;
	bool IsFoundColor(unsigned* dst, unsigned color) const;
	bool IsValidPosition(unsigned x, unsigned y) const;

	void Blit(unsigned x, unsigned y, unsigned pitch, uint8_t* dst) const;
	unsigned short GetZxKey() const { return mKey; }
	unsigned GetKeyOffsetX() const { return mKeyOffsetX; }
	unsigned GetKeyOffsetY() const { return mKeyOffsetY; }

	unsigned GetHeight() const { return mHeight; }
	unsigned GetWidth() const { return mWidth; }

private:
	std::vector<uint8_t> mData;
	std::vector<uint8_t> mZxMaskData;
	unsigned mHeight;
	unsigned mWidth;
	unsigned mKey;
	unsigned mKeyOffsetX; // in bytes (8 pixels)
	unsigned mKeyOffsetY; // in lines
	
	std::string mBmpName;	// primarily for debug

	struct PlainBMP
	{
		unsigned Width, Height;
		std::vector<uint8_t> Data;
		explicit PlainBMP(const std::string& bmpName);
	};

	static unsigned const TRANSPARENT_COLOR = RGB_MAKE(242, 10, 242);			// $mm const
	static std::vector<uint8_t> convertToZx(const PlainBMP& bmp, bool asMask);

	void updateKeyData(bool pixelAutoKey);
	void copyData(std::shared_ptr<RcImage> src_image, const std::vector<uint8_t>& src_data, std::vector<uint8_t>& dst_data);
	void shiftData(std::vector<uint8_t>& data);
};
