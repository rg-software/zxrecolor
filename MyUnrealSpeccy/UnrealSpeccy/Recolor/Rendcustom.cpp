#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <assert.h>

/*
struct BinaryImage
{
	short Width, Height; 
	char *Data;

	void LoadImage(const char *name)
	{
		FILE *f = fopen(name, "rb");
		fread(&Width, sizeof(short), 1, f);
		fread(&Height, sizeof(short), 1, f);
		Data = new char[Width * Height];
		fread(Data, sizeof(char), Width * Height, f);
		fclose(f);
	}

	void ProcessZX()
	{
		char* NewData = new char[Width * Height / 8];
		char* newPtr = NewData;
		char* ptr = Data;

		for(unsigned i = 0; i < Width * Height; ++i)
		{

		}
	}
};

struct Rule
{
	enum RuleType { DUMMY_BEG_TYPE, BLOCK_COLOR, PIXEL_COLOR, DUMMY_END_TYPE } Type;
	BinaryImage OldImage, NewImage; // old is always small-screen (x1)
	int Layer;

	Rule() {}
	Rule(const std::string& line)
	{
		std::istringstream iss(line);

		std::string type;
		std::string orig_pic, new_pic;
		iss >> Layer >> type >> orig_pic >> new_pic;

		OldImage.LoadImage(orig_pic.c_str());
		NewImage.LoadImage(new_pic.c_str());

		if(type == "block_color")
			Type = BLOCK_COLOR;
		else if(type == "pixel_color")
			Type = PIXEL_COLOR;
	}
};

const int RULETYPES = 10;
const int LAYERS = 3;
const int SPRITES = 150;
Rule Rules[RULETYPES][LAYERS][SPRITES];		// type, layer, rule; RULETYPES types, LAYERS layers, SPRITES sprites in each
int RulesCount[RULETYPES][LAYERS];  // sprites count in type i, layer j


void old_setup_custom()
{
	for(unsigned i = 0; i < LAYERS; ++i) 
		for(unsigned j = 0; j < RULETYPES; ++j)
			RulesCount[j][i] = 0;
		
	_chdir("game");
	std::ifstream is("settings.txt");
	std::string line;
	while(std::getline(is, line))
	{
		if(line[0] == ';') continue;
		
		Rule rule(line);
		Rules[rule.Type][rule.Layer][RulesCount[rule.Type][rule.Layer]++] = rule;
	}
}

void blit_image(unsigned x, unsigned y, unsigned pitch, unsigned char *dst, const Rule& rule)
{
	x /= 2; // for some reason...
	unsigned wx = rule.NewImage.Width, wy = rule.NewImage.Height;

	unsigned char* dst_buff = dst + x + pitch*y;
	unsigned c = 0;

	for(unsigned yc = y; yc < y + wy; ++yc)
	{
		for(unsigned xc = x; xc < x + wx; ++xc)
			*(dst_buff + xc) = rule.NewImage.Data[c++];
		dst_buff += pitch;
	}
}

bool is_found_here(unsigned char* color_buf, Rule& rule)
{
	for(int y = 0; y < rule.OldImage.Height; ++y)
	{
		unsigned char* x_buff = color_buf + 640*y;
		unsigned char* x_databuff = (unsigned char*)rule.OldImage.Data + rule.OldImage.Width*y;
		for(int x = 0; x < rule.OldImage.Width; ++x)
		{
			if(*x_buff++ != *x_databuff++)
				return false;
		}
	}
	return true;
}

void process_block_color(unsigned char* color_screen, unsigned type, unsigned layer, unsigned pitch, unsigned char* dst)
{
	//const unsigned border_x_margin = 32;
	//const unsigned border_y_margin = 24;
	const unsigned border_x_margin = 0;
	const unsigned border_y_margin = 0;

	for(unsigned y = border_y_margin; y < 240 - border_y_margin; y += 8)
	{
		unsigned char* color_buf = color_screen	+ 640*y;

		for(unsigned x = border_x_margin; x < 320 - border_x_margin; x += 8)
		{
			for(int i = 0; i < RulesCount[type][layer]; ++i)
				if(is_found_here(color_buf, Rules[type][layer][i]))
					blit_image(x*2, y*2, pitch, dst, Rules[type][layer][i]);
			//++color_buf;
			color_buf += 8;
		}
	}
}

void process_pixel_color(unsigned char* color_screen, unsigned type, unsigned layer, unsigned pitch, unsigned char* dst)
{
	for(unsigned y = 0; y < 480/2; ++y)
	{
		unsigned char* color_buf = color_screen	+ 640*y;

		for(unsigned x = 0; x < 640/2; ++x)
		{
			for(int i = 0; i < RulesCount[type][layer]; ++i)
				if(is_found_here(color_buf, Rules[type][layer][i]))
					blit_image(x*2, y*2, pitch, dst, Rules[type][layer][i]);
			++color_buf;
		}
	}
}

void old_render_custom(unsigned char *dst, unsigned pitch, unsigned char* bw_screen, unsigned char* color_screen)
{
	static bool first = true;
	if(first) { old_setup_custom(); first = false; }

	for(unsigned type = Rule::DUMMY_BEG_TYPE + 1; type != Rule::DUMMY_END_TYPE; ++type) // process each type
	{
		for(int layer = 0; layer < LAYERS; ++layer)
		{
			if(RulesCount[type][layer] == 0)
				continue;

			// process current type and layer 

			switch(type)
			{
				case Rule::BLOCK_COLOR: process_block_color(color_screen, type, layer, pitch, dst); break;
				case Rule::PIXEL_COLOR: process_pixel_color(color_screen, type, layer, pitch, dst); break;
			}
		}
	}
}
*/
/*
void bin_prnt_byte(FILE *f, int x)
{
int n;
for(n=0; n<8; n++)
{
if((x & 0x80) !=0)
{
fprintf(f, "1");
}
else
{
fprintf(f, "0");
}
if (n==3)
{
//printf(" ");
}
x = x<<1;
}
}
*/
//blit_image(300, 200, pitch, dst, Key);
/*	FILE *f = fopen("!log.txt", "wb");
unsigned char* zx = zx_screen;
for(unsigned y = 0; y < 240; ++y)
{
for(unsigned x = 0; x < 320/8; ++x)
{
if(x<10){bin_prnt_byte(f, *zx); fprintf(f, " ");}
++zx;
}
fprintf(f, "\n");
}
fclose(f);

f = fopen("!key.txt", "wb");
zx = Key.Data;
for(unsigned y = 0; y < Key.Height; ++y)
{
for(unsigned x = 0; x < Key.Width/8; ++x)
{
if(x<10){bin_prnt_byte(f, *zx); fprintf(f, " ");}
++zx;
}
fprintf(f, "\n");
}
fclose(f);

*/

////////////////////////////////////////
#define TORGB(r, g, b) ((r) << 16) + ((g) << 8) + (b)
unsigned const TRANSPARENT_COLOR = TORGB(242, 10, 242); //(242 << 16) + (10 << 8) + 242; 

struct MyImage
{
	unsigned Width, Height; 
	unsigned char* Data;

	void LoadImage(const char *bmpName) // load 24-bit bmp
	{
		BITMAP gBitmap;
		HBITMAP hObj = (HBITMAP)::LoadImage(0, bmpName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		assert(hObj != NULL);
		GetObject(hObj, sizeof(BITMAP), (LPVOID)&gBitmap); 

		Width = gBitmap.bmWidth;
		Height = gBitmap.bmHeight;
		unsigned char* rgbData = new unsigned char[Width * Height * 3];  // original format is 24bpp (no alpha)
		unsigned char* origRgbData = rgbData;

		GetBitmapBits(hObj, Width * Height * 3, (LPVOID)rgbData);
		
		Data = new unsigned char[Width * Height * 4];
		unsigned char* pData = Data;
		for(unsigned i = 0; i < Width * Height; ++i)
		{
			*pData++ = *rgbData++;	// r, g, b
			*pData++ = *rgbData++;
			*pData++ = *rgbData++;
			*pData++ = 0;			// alpha
		}


		delete[] origRgbData;
		DeleteObject(hObj);
	}

	void ConvertToZx(bool asMask = false)
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
			if(placeOne)  //if(*UData++ == 0) // black, put 1
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

	void LogImage()
	{
		char binary[10];
		FILE *f = fopen("!log.txt", "at");
		for(unsigned y = 0; y < Height; ++y)
		{
			for(unsigned x = 0; x < (Width/8); ++x)
			{
				itoa(Data[x+ (Width/8)*y], binary, 2);
				for(unsigned i = 0; i < 8-strlen(binary); ++i) fprintf(f, "0");
				fprintf(f, "%s ", binary);
			}
			fprintf(f, "\n");
		}
		
		fprintf(f, "\n");
		fclose(f);
	}

	void CopyAndShift(const MyImage& image, unsigned offset)
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
			Shift();
		//LogImage();
	}

	void Shift()
	{
		for(unsigned y = 0; y < Height; ++y)
		{
			char lobit = Data[0 + (Width/8)*y] & 1;
			Data[0 + (Width/8)*y] >>= 1;

			for(unsigned x = 1; x < (Width/8); ++x)
			{
				char old_lobit = lobit;
				lobit = Data[x + (Width/8)*y] & 1;
				
				Data[x + (Width/8)*y] >>= 1;
				if(old_lobit)
					Data[x + (Width/8)*y] |= 128; // 1000 0000

			}
		}
	}
};

// display image in (x,y) location of dst
void blit_image(unsigned x, unsigned y, unsigned pitch, unsigned char *dst, MyImage& image)
{
	unsigned wx = image.Width, wy = image.Height;

	unsigned char* dst_buff = dst + x*3 + pitch*y; // for some reason x*3
	unsigned *UData = (unsigned*)image.Data;
	unsigned uc = 1;
	bool is_tr_color;
	unsigned c = 0;
	unsigned color;

	for(unsigned yc = y; yc < y + wy; ++yc)
	{
		for(unsigned xc = x; xc < x + wx*4; ++xc)
		{
			if(--uc == 0)
			{
				is_tr_color = *UData == TRANSPARENT_COLOR;  // every four bytes check for the transparency
				UData++;
				uc = 4;
			}

			color = image.Data[c++];

			if(!is_tr_color)
				*(dst_buff + xc) = color; //image.Data[c++];
		}
		dst_buff += pitch;
	}
}

struct TNameRGB
{
	std::map<std::string, unsigned> RGB;
	TNameRGB()
	{
		RGB["black"] = TORGB(0, 0, 0);
		RGB["blue"] = TORGB(0, 0, 0xBF);
		RGB["red"] = TORGB(0xBF, 0, 0);
		RGB["magenta"] = TORGB(0xBF, 0, 0xBF);
		RGB["green"] = TORGB(0, 0xBF, 0);
		RGB["cyan"] = TORGB(0, 0xBF, 0xBF);
		RGB["yellow"] = TORGB(0xBF, 0xBF, 0);
		RGB["white"] = TORGB(0xBF, 0xBF, 0xBF);
		RGB["black+"] = TORGB(0, 0, 0);
		RGB["blue+"] = TORGB(0, 0, 0xFE);
		RGB["red+"] = TORGB(0xFE, 0, 0);
		RGB["magenta+"] = TORGB(0xFE, 0, 0xFE);
		RGB["green+"] = TORGB(0, 0xFE, 0);
		RGB["cyan+"] = TORGB(0, 0xFE, 0xFE);
		RGB["yellow+"] = TORGB(0xFE, 0xFE, 0);
		RGB["white+"] = TORGB(0xFE, 0xFE, 0xFE);
	}
}
NameRGB;

struct MyRule
{
	enum RuleType { DUMMY_BEG_TYPE = 0, BLOCK, PIXEL, DUMMY_END_TYPE } Type;
	MyImage PcImage;
	MyImage ZxImage, ZxMask; // old is always small-screen (x1)
	MyImage ZxImages[8], ZxMasks[8];
	bool MatchColor;
	int ColorX, ColorY;
	unsigned Color;
	int OffsetX, OffsetY;
	int Layer;

	MyRule() : MatchColor(false), OffsetY(0), OffsetX(0) {}
	MyRule(const std::string& line)
	{
		MatchColor = false;
		OffsetX = OffsetY = 0;
		std::istringstream iss(line);

		std::string type;
		std::string orig_pic, new_pic;
		iss >> Layer >> type >> orig_pic >> new_pic;

		if(orig_pic.find('|') != std::string::npos) // color part is found (orig_pic|color_part)
		{
			std::string color_part = orig_pic.substr(orig_pic.find('|') + 1);
			orig_pic = orig_pic.substr(0, orig_pic.find('|'));

			MatchColor = true;	// color_part = "x,y,color_name"
			ColorX = atoi(color_part.substr(0, color_part.find(',')).c_str());
			color_part = color_part.substr(color_part.find(',') + 1);
			ColorY = atoi(color_part.substr(0, color_part.find(',')).c_str());
			color_part = color_part.substr(color_part.find(',') + 1);
			if(NameRGB.RGB.find(color_part) == NameRGB.RGB.end())
				throw std::exception("Incorrect color name"); // should never happen
			Color = NameRGB.RGB[color_part];
		}

		if(new_pic.find('|') != std::string::npos)
		{
			std::string xy_part = new_pic.substr(new_pic.find('|') + 1);
			new_pic = new_pic.substr(0, new_pic.find('|'));

			OffsetX = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
			xy_part = xy_part.substr(xy_part.find(',') + 1);
			OffsetY = atoi(xy_part.substr(0, xy_part.find(',')).c_str());
		}

		ZxImage.LoadImage(orig_pic.c_str());
		ZxMask.LoadImage(orig_pic.c_str());
		ZxImage.ConvertToZx();
		ZxMask.ConvertToZx(true);
		
		for(unsigned i = 0; i < 8; ++i)
		{
			ZxImages[i].CopyAndShift(ZxImage, i);
			ZxMasks[i].CopyAndShift(ZxMask, i);
		}

		PcImage.LoadImage(new_pic.c_str());

		if(type == "block")
			Type = BLOCK;
		else if(type == "pixel")
			Type = PIXEL;
	}

	unsigned short GetZxKey() const { return ZxImage.Data[0]*256 + ZxImage.Data[ZxImage.Width/8]; /* two first sprite lines */ }
	
	bool operator<(const MyRule& rhs) const
	{
		return GetZxKey() < rhs.GetZxKey();
	}
};

const unsigned char MAXRULES = 250;

struct MyRules
{
	MyRule Rules[MAXRULES];					// all rules of the same kind
	unsigned RulesCount;					// the actual number of rules
	unsigned char Index[256*256];			// start index of the rule specified by key in Rules[]
	unsigned char IndexCount[256*256];		// number of rules specified by key

	MyRules()
	{
		RulesCount = 0;
		std::fill_n(Index, 256*256, 0);
		std::fill_n(IndexCount, 256*256, 0);
	}

	void Add(const MyRule& rule)
	{
		if(RulesCount == MAXRULES)
			throw std::exception("MAXRULES exceeded"); // should never happen
		Rules[RulesCount++] = rule;
	}

	void BuildIndex()
	{
		std::sort(Rules, Rules + RulesCount); // sort rules by key
		
		for(unsigned i = 0; i < RulesCount; ++i)
		{
			unsigned short key = Rules[i].GetZxKey();
			if(IndexCount[key] == 0)
				Index[key] = i;		// start index of key-rules
			IndexCount[key]++;
		}
	}

};

MyRules* AllRules = new MyRules[MyRule::DUMMY_END_TYPE];

void setup_custom()
{
	_chdir("game");
	std::ifstream is("settings.txt");
	std::string line;
	while(std::getline(is, line))
	{
		if(line[0] == ';') continue;

		MyRule rule(line);
		AllRules[rule.Type].Add(rule);
	}

	for(unsigned i = 0; i < MyRule::DUMMY_END_TYPE; ++i)
		AllRules[i].BuildIndex();
}

// checks whether a zx-image is found under curptr
bool image_found_at(unsigned char* curptr, MyImage& image, MyImage& mask)
{
	unsigned WidthBytes = image.Width / 8;
	
	for(unsigned y = 0; y < image.Height; ++y)
	{
		unsigned char* x_buff = curptr + (320/8)*y;
		unsigned char* x_databuff = image.Data + WidthBytes*y;
		unsigned char* x_maskbuff = mask.Data + WidthBytes*y;

		for(unsigned x = 0; x < WidthBytes; ++x)
		{
			if((*x_buff++ & *x_maskbuff++) != *x_databuff++)
				return false;
		}
	}

	return true;
}

struct blist_list_element
{
	unsigned x, y;
	MyImage* image;
	int layer;
	blist_list_element(unsigned x_, unsigned y_, MyImage* image_, int layer_) : x(x_), y(y_), image(image_), layer(layer_) {}

	bool operator<(const blist_list_element& rhs) const { return layer < rhs.layer; }
};

struct create_blit_list
{
	std::vector<blist_list_element>& blitlist;
	create_blit_list(std::vector<blist_list_element>& blitlist_) : blitlist(blitlist_) {}
	
	void operator()(unsigned x, unsigned y, MyImage* image, int layer) 
	{ 
		blitlist.push_back(blist_list_element(x*2, y*2, image, layer));
	}
};

void blit_all(unsigned pitch, unsigned char* dst, const std::vector<blist_list_element>& blitlist)
{
	for(std::vector<blist_list_element>::const_iterator p = blitlist.begin(); p != blitlist.end(); ++p)
		blit_image(p->x, p->y, pitch, dst, *(p->image));
}

bool found_color(unsigned char *dst, unsigned pitch, unsigned x, unsigned y, MyRule& rule)
{
	if(!rule.MatchColor)
		return true;

	unsigned* dst_buff = (unsigned*)dst + (x*2 + rule.ColorX) + 640*(y*2 + rule.ColorY);
	return *dst_buff == rule.Color;
}

// find a list of found zx-images (only perfect matches within 8*8 blocks are found)
template<typename Op> void foreach_zxblock(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, Op op)
{
	MyRules& Rules = AllRules[MyRule::BLOCK];

	for(unsigned scry = 0; scry < 240 - 8; scry += 8) // every 8 pixels
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned short key = (*curptr)*256 + *(curptr + 320/8);
			unsigned char index = Rules.Index[key];			// find the corresponding rules (possibly matching sprites, 2 bytes)
			unsigned char count = Rules.IndexCount[key];

			for(unsigned char i = index; i < index + count; ++i) // check carefully
			{
				if(scry + Rules.Rules[i].ZxImage.Height < 240 && 
								found_color(dst, pitch, scrx*8, scry, Rules.Rules[i]) && 
								image_found_at(curptr, Rules.Rules[i].ZxImage, Rules.Rules[i].ZxMask))
					op(scrx*8 + Rules.Rules[i].OffsetX, scry + Rules.Rules[i].OffsetY, &Rules.Rules[i].PcImage, Rules.Rules[i].Layer);
			}

			++curptr;
		}
	}
}

template<typename Op> void foreach_zxpixel(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, Op op)
{
	MyRules& Rules = AllRules[MyRule::PIXEL];

	for(unsigned scry = 0; scry < 240 - 8; ++scry) // every pixel
	{
		unsigned char* curptr = zx_screen + (320/8) * scry;
		for(unsigned scrx = 0; scrx < 320/8; scrx++)  // every 8 pixels (1 byte)
		{
			unsigned short curptr_v = (*curptr)*256 + *(curptr+1);
			unsigned short curptr_next_v = (*(curptr + 320/8))*256 + *(curptr + 320/8 + 1);

			for(unsigned offset = 0; offset < 8; ++offset) // check all 8 possible offsets
			{
				//unsigned short key = ((curptr_v << offset) >> 8)*256 + ((curptr_next_v << offset) >> 8);
				unsigned short key = HIBYTE((curptr_v << offset)) * 256 + HIBYTE((curptr_next_v << offset));
				
				unsigned char index = Rules.Index[key];			// find the corresponding rules (possibly matching sprites, 2 bytes)
				unsigned char count = Rules.IndexCount[key];

				for(unsigned char i = index; i < index + count; ++i) // check carefully
				{
					// fix this checking code (prepare the image?)
					if(scry + Rules.Rules[i].ZxImage.Height < 240 && 
								found_color(dst, pitch, scrx*8 + offset, scry, Rules.Rules[i]) &&
								image_found_at(curptr, Rules.Rules[i].ZxImages[offset], Rules.Rules[i].ZxMasks[offset]))
						op(scrx*8 + offset + Rules.Rules[i].OffsetX, scry + Rules.Rules[i].OffsetY, &Rules.Rules[i].PcImage, Rules.Rules[i].Layer);
				}
			}

			++curptr;
		}
	}
}

void render_custom(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, unsigned char* color_screen)
{
	static bool first = true;
	if(first) { setup_custom(); first = false; }

	std::vector<blist_list_element> blitlist;

	foreach_zxblock(dst, pitch, zx_screen, create_blit_list(blitlist));
	foreach_zxpixel(dst, pitch, zx_screen, create_blit_list(blitlist));
	
	std::sort(blitlist.begin(), blitlist.end());
	blit_all(pitch, dst, blitlist);
}
