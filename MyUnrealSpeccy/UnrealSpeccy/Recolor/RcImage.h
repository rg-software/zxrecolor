#pragma once
#include <D3DTYPES.H>
#include <string>
#include <cassert>

unsigned const TRANSPARENT_COLOR = RGB_MAKE(242, 10, 242);

class RcImage
{
public:
	unsigned Width, Height;
	unsigned char* Data;

	void LoadColored(const std::string& bmpName, bool convertZx = false, bool makeZxMask = false); // load 24-bit bmp
	unsigned char* loadColoredImpl(const std::string& bmpName, bool convertZx, bool makeZxMask);

	void CopyAndShift(const RcImage& image, unsigned offset)
	{
		Width = image.Width + 8;
		Height = image.Height;
		unsigned size = Width * Height / 8;
		Data = new unsigned char[size];
		std::fill(Data, Data + size, 0);

		for(unsigned x = 0; x < image.Width/8; ++x)
			for(unsigned y = 0; y < image.Height; ++y)
				Data[x + (Width/8)*y] = image.Data[x + (image.Width/8)*y];

		for(unsigned i = 0; i < offset; ++i)
			shift();
		//LogImage();
	}


	bool IsFoundAt(unsigned char* curptr, RcImage& mask)
	{
		unsigned WidthBytes = Width / 8;

		for (unsigned y = 0; y < Height; ++y)
		{
			unsigned char* x_buff = curptr + (320 / 8)*y;
			unsigned char* x_databuff = Data + WidthBytes * y;
			unsigned char* x_maskbuff = mask.Data + WidthBytes * y;

			for (unsigned x = 0; x < WidthBytes; ++x)
			{
				if ((*x_buff++ & *x_maskbuff++) != *x_databuff++)
					return false;
			}
		}

		return true;

	}

	void Blit(unsigned x, unsigned y, unsigned pitch, unsigned char *dst)
	{
		unsigned wx = Width, wy = Height;

		unsigned char* dst_buff = dst + x * 3 + pitch * y; // for some reason x*3
		unsigned *UData = (unsigned*)Data;
		unsigned uc = 1;
		bool is_tr_color;
		unsigned c = 0;
		unsigned color;

		for (unsigned yc = y; yc < y + wy; ++yc)
		{
			for (unsigned xc = x; xc < x + wx * 4; ++xc)
			{
				if (--uc == 0)
				{
					is_tr_color = *UData == TRANSPARENT_COLOR;  // every four bytes check for the transparency
					UData++;
					uc = 4;
				}

				color = Data[c++];

				if (!is_tr_color)
					*(dst_buff + xc) = color; //Data[c++];
			}
			dst_buff += pitch;
		}
	}

	unsigned short GetZxKey() const { return Data[0] * 256 + Data[Width / 8]; /* two first sprite lines */ }


private:
	void convertToZx(bool asMask);
	void shift() const;
};
