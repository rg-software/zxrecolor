#include "RcImage.h"

void RcImage::LoadColored(const std::string& bmpName, bool convertZx, bool makeZxMask)
{
	BITMAP gBitmap;
	HBITMAP hObj = (HBITMAP)::LoadImage(0, bmpName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	assert(hObj != NULL);
	GetObject(hObj, sizeof(BITMAP), (LPVOID)&gBitmap);

	Width = gBitmap.bmWidth;
	Height = gBitmap.bmHeight;
	unsigned char* rgbData = new unsigned char[Width * Height * 3]; // original format is 24bpp (no alpha)
	unsigned char* origRgbData = rgbData;

	GetBitmapBits(hObj, Width * Height * 3, (LPVOID)rgbData);

	Data = new unsigned char[Width * Height * 4];
	unsigned char* pData = Data;
	for(unsigned i = 0; i < Width * Height; ++i)
	{
		*pData++ = *rgbData++; // r, g, b
		*pData++ = *rgbData++;
		*pData++ = *rgbData++;
		*pData++ = 0; // alpha
	}

	delete[] origRgbData;
	DeleteObject(hObj);

	if(convertZx)
		convertToZx(makeZxMask);
}

void RcImage::shift() const
{
	for(unsigned y = 0; y < Height; ++y)
	{
		char lobit = Data[0 + (Width / 8) * y] & 1;
		Data[0 + (Width / 8) * y] >>= 1;

		for(unsigned x = 1; x < (Width / 8); ++x)
		{
			char old_lobit = lobit;
			lobit = Data[x + (Width / 8) * y] & 1;

			Data[x + (Width / 8) * y] >>= 1;
			if(old_lobit)
				Data[x + (Width / 8) * y] |= 128; // 1000 0000
		}
	}
}

void RcImage::convertToZx(bool asMask)
{
	// zx image should be: black ink, white paper

	unsigned char* NewData = new unsigned char[Width * Height / 8];
	unsigned char* NewDataPtr = NewData;
	unsigned* UData = (unsigned*)Data;
	unsigned char nextbyte = 0;
	for(unsigned c = 1; c <= Width * Height; ++c)
	{
		nextbyte <<= 1;

		bool placeOne = asMask ? *UData != TRANSPARENT_COLOR : *UData == 0;

		UData++;
		if(placeOne) //if(*UData++ == 0) // black, put 1
			nextbyte |= 1;

		if(c > 7 && c % 8 == 0) // starting with c == 8
		{
			*NewDataPtr++ = nextbyte;
			nextbyte = 0;
		}
	}

	delete[] Data;
	Data = NewData;
}
