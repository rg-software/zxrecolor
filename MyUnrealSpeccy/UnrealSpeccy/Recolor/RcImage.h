#pragma once
#include <string>
#include <vector>
#include "../dx/include/d3dtypes.h"

class RcImage
{
public:
	RcImage(const std::string& bmpName);	// load 24-bit bmp
	RcImage(const std::string& bmpName, bool convertZx, bool pixelAutoKey);	// load 24-bit bmp
	RcImage(std::shared_ptr<RcImage> src_image, unsigned offset);	// copy from another image

	bool IsFoundAt(const uint8_t* curptr) const;
	void Blit(unsigned x, unsigned y, unsigned pitch, uint8_t* dst) const;
	unsigned short GetZxKey() const { return Key; }
	unsigned GetKeyOffsetX() const { return KeyOffsetX; }
	unsigned GetKeyOffsetY() const { return KeyOffsetY; }

	unsigned GetHeight() const { return Height; }
	unsigned GetWidth() const { return Width; }

private:
	std::vector<uint8_t> Data;
	std::vector<uint8_t> ZxMaskData;
	unsigned Height;
	unsigned Width;
	unsigned Key;
	unsigned KeyOffsetX; // in bytes (8 pixels)
	unsigned KeyOffsetY; // in lines
	
	std::string BmpName;	// primarily for debug

	struct PlainBMP
	{
		unsigned Width, Height;
		std::vector<uint8_t> Data;
		explicit PlainBMP(const std::string& bmpName);
	};

	static unsigned const TRANSPARENT_COLOR = RGB_MAKE(242, 10, 242);			// $mm const
	static std::vector<uint8_t> convertToZx(const PlainBMP& bmp, bool asMask);

	void updateKeyData(bool pixelAutoKey);
	void copyData(std::shared_ptr<RcImage> src_image, std::vector<uint8_t>& data);
	void shiftData(std::vector<uint8_t>& data);
};
