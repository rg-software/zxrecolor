#include "RcImage.h"

void RcImage::LoadColored(const std::string& bmpName, bool convertZx, bool makeZxMask)
{
	PlainBMP bmp(bmpName);
	Width = bmp.Width;
	Height = bmp.Height;
	Data = PlainBMP(bmpName).Data;

	if(convertZx)
	{
		Data = convertToZx(bmp, makeZxMask);
		ZxMaskData = convertToZx(bmp, true);	// it will be the only mask in the future!
	}
}

void RcImage::CopyWithShift(const RcImage& src_image, unsigned offset)
{
	copyData(src_image, Data);
	copyData(src_image, ZxMaskData);

	for (unsigned i = 0; i < offset; ++i)
	{
		shiftData(Data);
		shiftData(ZxMaskData);
	}
}

void RcImage::copyData(const RcImage& src_image, std::vector<uint8_t>& data)
{
	Width = src_image.Width + 8;
	Height = src_image.Height;
	unsigned size = Width * Height / 8;
	data.resize(size);
	std::fill(data.begin(), data.end(), 0);

	for (unsigned x = 0; x < src_image.Width / 8; ++x)
		for (unsigned y = 0; y < src_image.Height; ++y)
			data[x + (Width / 8) * y] = src_image.Data[x + (src_image.Width / 8) * y];
}

bool RcImage::IsFoundAt(unsigned char* curptr)
{
	unsigned WidthBytes = Width / 8;

	for (unsigned y = 0; y < Height; ++y)
	{
		unsigned char* x_buff = curptr + (320 / 8) * y;
		unsigned char* x_databuff = &Data[0] + WidthBytes * y;
		unsigned char* x_maskbuff = &ZxMaskData[0] + WidthBytes * y;

		for (unsigned x = 0; x < WidthBytes; ++x)
		{
			if ((*x_buff++ & *x_maskbuff++) != *x_databuff++)
				return false;
		}
	}

	return true;
}

void RcImage::Blit(unsigned x, unsigned y, unsigned pitch, unsigned char* dst)
{
	unsigned wx = Width, wy = Height;

	unsigned char* dst_buff = dst + x * 3 + pitch * y; // for some reason x*3
	unsigned* UData = (unsigned*)&Data[0];
	unsigned uc = 1;
	bool is_tr_color;
	unsigned c = 0;
	unsigned color;

	for(unsigned yc = y; yc < y + wy; ++yc)
	{
		for(unsigned xc = x; xc < x + wx * 4; ++xc)
		{
			if(--uc == 0)
			{
				is_tr_color = *UData == TRANSPARENT_COLOR; // every four bytes check for the transparency
				UData++;
				uc = 4;
			}

			color = Data[c++];

			if(!is_tr_color)
				*(dst_buff + xc) = color; //Data[c++];
		}
		dst_buff += pitch;
	}
}

void RcImage::shiftData(std::vector<uint8_t>& data)
{
	for(unsigned y = 0; y < Height; ++y)
	{
		char lobit = data[0 + (Width / 8) * y] & 1;
		data[0 + (Width / 8) * y] >>= 1;

		for(unsigned x = 1; x < (Width / 8); ++x)
		{
			char old_lobit = lobit;
			lobit = data[x + (Width / 8) * y] & 1;

			data[x + (Width / 8) * y] >>= 1;
			if(old_lobit)
				data[x + (Width / 8) * y] |= 128; // 1000 0000
		}
	}
}

RcImage::PlainBMP::PlainBMP(const std::string& bmpName)
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

	Data.resize(Width * Height * 4);
	unsigned char* pData = &Data[0];
	for (unsigned i = 0; i < Width * Height; ++i)
	{
		*pData++ = *rgbData++; // r, g, b
		*pData++ = *rgbData++;
		*pData++ = *rgbData++;
		*pData++ = 0; // alpha
	}

	delete[] origRgbData;
	DeleteObject(hObj);
}

std::vector<uint8_t> RcImage::convertToZx(const PlainBMP& bmp, bool asMask)
{
	// zx image should be: black ink, white paper

	std::vector<unsigned char> NewData(bmp.Width * bmp.Height / 8);
	auto NewDataPtr = NewData.begin();
	auto UData = reinterpret_cast<const unsigned*>(&bmp.Data[0]);
	unsigned char nextbyte = 0;
	for(unsigned c = 1; c <= bmp.Width * bmp.Height; ++c)
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

	return NewData;
}
