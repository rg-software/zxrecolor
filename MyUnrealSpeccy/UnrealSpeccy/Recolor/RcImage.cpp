#include "RcImage.h"
#include <cassert>

RcImage::RcImage(const std::string& bmpName) : RcImage(bmpName, false, false)
{
}

RcImage::RcImage(const std::string& bmpName, bool convertZx, bool pixelAutoKey)
{
	mBmpName = bmpName;

	PlainBMP bmp(bmpName);
	mWidth = bmp.Width;
	mHeight = bmp.Height;
	mData = PlainBMP(bmpName).Data;

	if(convertZx)
	{
		mData = convertToZx(bmp, false);
		mZxMaskData = convertToZx(bmp, true);	// it will be the only mask in the future!
		
		updateKeyData(pixelAutoKey);
		//KeyOffset = findKeyOffset();
	}
}

RcImage::RcImage(std::shared_ptr<RcImage> src_image, unsigned offset)
{
	mKey = src_image->mKey;
	mKeyOffsetX = src_image->mKeyOffsetX;
	mKeyOffsetY = src_image->mKeyOffsetY;
	mBmpName = src_image->mBmpName;

	mWidth = src_image->mWidth + 8;
	mHeight = src_image->mHeight;

	copyData(src_image, src_image->mData, mData);
	copyData(src_image, src_image->mZxMaskData, mZxMaskData);

	for (unsigned i = 0; i < offset; ++i)
	{
		shiftData(mData);
		shiftData(mZxMaskData);
	}
}

void RcImage::copyData(std::shared_ptr<RcImage> src_image, const std::vector<uint8_t>& src_data, std::vector<uint8_t>& dst_data)
{
	unsigned size = mWidth * mHeight / 8;
	dst_data.resize(size);
	std::fill(dst_data.begin(), dst_data.end(), 0);

	for (unsigned x = 0; x < src_image->mWidth / 8; ++x)
		for (unsigned y = 0; y < src_image->mHeight; ++y)
			dst_data[x + (mWidth / 8) * y] = src_data[x + (src_image->mWidth / 8) * y];
}

bool RcImage::IsFoundAt(const uint8_t* curptr) const
{
	unsigned WidthBytes = mWidth / 8;
	const uint8_t* realCurPtr = curptr -(320 / 8) * mKeyOffsetY - mKeyOffsetX;

	for (unsigned y = 0; y < mHeight; ++y)
	{
		const uint8_t* x_buff = realCurPtr + (320 / 8) * y;
		const uint8_t* x_databuff = &mData[0] + WidthBytes * y;
		const uint8_t* x_maskbuff = &mZxMaskData[0] + WidthBytes * y;

		for (unsigned x = 0; x < WidthBytes; ++x)
		{
			if ((*x_buff++ & *x_maskbuff++) != *x_databuff++)
				return false;
		}
	}

	return true;
}

bool RcImage::IsFoundColor(unsigned* dst, unsigned color) const	
{
	return color == *(dst - 2*(640 * mKeyOffsetY - mKeyOffsetX));
}

void RcImage::Blit(unsigned x, unsigned y, unsigned pitch, uint8_t* dst) const
{
	unsigned wx = mWidth, wy = mHeight;

	uint8_t* dst_buff = dst + x * 3 + pitch * y; // why x*3 ?.. 3 bytes per point?; pitch is 320*2*4 (640 pixels, unsigned elements)
	auto UData = reinterpret_cast<const unsigned*>(&mData[0]);
	unsigned uc = 1;
	bool is_tr_color = false;
	unsigned c = 0;

	// image data consists of "unsigned" (4-byte) values
	// the fourth byte is not used, and "transparency" is a special color (TRANSPARENT_COLOR)
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

			unsigned color = mData[c++];

			if(!is_tr_color)
				*(dst_buff + xc) = color; // write four bytes
		}
		dst_buff += pitch;
	}
}

void RcImage::shiftData(std::vector<uint8_t>& data)
{
	for(unsigned y = 0; y < mHeight; ++y)
	{
		char lobit = data[0 + (mWidth / 8) * y] & 1;
		data[0 + (mWidth / 8) * y] >>= 1;

		for(unsigned x = 1; x < (mWidth / 8); ++x)
		{
			char old_lobit = lobit;
			lobit = data[x + (mWidth / 8) * y] & 1;

			data[x + (mWidth / 8) * y] >>= 1;
			if(old_lobit)
				data[x + (mWidth / 8) * y] |= 128; // 1000 0000
		}
	}
}

RcImage::PlainBMP::PlainBMP(const std::string& bmpName)
{
	if (GetFileAttributes(bmpName.c_str()) == -1)
		throw std::runtime_error("File " + bmpName + " not found");

	BITMAP gBitmap;
	HBITMAP hObj = (HBITMAP)::LoadImage(0, bmpName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	assert(hObj != NULL);
	GetObject(hObj, sizeof(BITMAP), (LPVOID)&gBitmap);

	Width = gBitmap.bmWidth;
	Height = gBitmap.bmHeight;
	uint8_t* rgbData = new uint8_t[Width * Height * 3]; // original format is 24bpp (no alpha)
	uint8_t* origRgbData = rgbData;

	GetBitmapBits(hObj, Width * Height * 3, (LPVOID)rgbData);

	Data.resize(Width * Height * 4);
	uint8_t* pData = &Data[0];
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

	std::vector<uint8_t> NewData(bmp.Width * bmp.Height / 8);
	auto NewDataPtr = NewData.begin();
	auto UData = reinterpret_cast<const unsigned*>(&bmp.Data[0]);
	uint8_t nextbyte = 0;

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

void RcImage::updateKeyData(bool pixelAutoKey)
{
	mKey = mKeyOffsetX = mKeyOffsetY = 0;

	// find an appropriate key fragment with no masked pixels and non-zero/non-65535 value
	auto WidthBytes = mWidth / 8;
	
	for (unsigned i = 0; i < mZxMaskData.size() - WidthBytes; ++i)
	{
		if (!pixelAutoKey && (i / WidthBytes) % 8 != 0)
			continue;
		if (mZxMaskData[i] == 0xFF && mZxMaskData[i + WidthBytes] == 0xFF)
		{
			auto curKey = MAKEWORD(mData[i + WidthBytes], mData[i]);

			if (curKey != 0 && curKey != 0xFFFF)
			{
				mKey = curKey;
				mKeyOffsetX = i % WidthBytes;
				mKeyOffsetY = i / WidthBytes;

				//printf("%s: key %d found at (%d, %d)\n", mBmpName.c_str(), mKey, mKeyOffsetX, mKeyOffsetY);
				return;
			}
		}
	}

	printf("%s: zero key assigned\n", mBmpName.c_str());
}

