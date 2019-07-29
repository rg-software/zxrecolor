#pragma once
#include <D3DTYPES.H>
#include <string>
#include <vector>

unsigned const TRANSPARENT_COLOR = RGB_MAKE(242, 10, 242);

class RcImage
{
public:
	void Load(const std::string& bmpName, bool convertZx = false, bool makeZxMask = false); // load 24-bit bmp
	void CopyWithShift(const RcImage& src_image, unsigned offset);
	bool IsFoundAt(uint8_t* curptr) const;
	void Blit(unsigned x, unsigned y, unsigned pitch, uint8_t* dst) const;
	unsigned short GetZxKey() const { return MAKEWORD(Data[Width / 8], Data[0]); } /* two first sprite lines */

	unsigned GetHeight() const { return Height; }
	unsigned GetWidth() const { return Width; }

private:
	std::vector<uint8_t> Data;
	std::vector<uint8_t> ZxMaskData;
	unsigned Height;
	unsigned Width;

	struct PlainBMP
	{
		unsigned Width, Height;
		std::vector<uint8_t> Data;
		explicit PlainBMP(const std::string& bmpName);
	};

	static std::vector<uint8_t> convertToZx(const PlainBMP& bmp, bool asMask);

	void copyData(const RcImage& src_image, std::vector<uint8_t>& data);
	void shiftData(std::vector<uint8_t>& data);
};
