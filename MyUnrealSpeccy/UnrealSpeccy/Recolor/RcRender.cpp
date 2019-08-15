#include <exception>
#include <cstdio>
#include <windows.h>

void create_zxline(unsigned char *dst, unsigned char *src, const unsigned *tab)
{
	for (unsigned x = 0; x < 320 / 8; ++x)
	{
		unsigned char byte = *src++;		// a bit sequence of 8 pixels
		const unsigned *tt = tab + *src++;	// attributes (ignore them here)

		*(dst + x) = byte;
	}
}

void create_zxscreen(unsigned char *src, unsigned char* dst, unsigned delta, const unsigned *tab)
{
	for (unsigned y = 0; y < 240; y++)
	{
		create_zxline(dst, src, tab);
		dst += 320 / 8;
		src += delta;
	}
}

void blackwhite_render_line(unsigned char *dst, unsigned char *src, const unsigned *tab)//, bool isbw)
{
	unsigned ink = 0x00000000, paper = 0x00FFFFFF;
	unsigned* uc_dst = (unsigned*)dst;

	for (unsigned x = 0; x < 320*4; x += 32)
	{
		unsigned char byte = *src++; // a bit sequence of 8 pixels
		const unsigned *tt = tab + *src++; // attributes

		for (unsigned i = 0; i < 8; ++i)
		{
			auto color = (((byte << i) & 128) ? ink : paper);
			*uc_dst = color;
			uc_dst++;
			*uc_dst = color;
			uc_dst++;
		}
	}
}

void blackwhite_render(unsigned char *src, unsigned char *dst, unsigned delta, unsigned pitch, const unsigned *tab)
{
	for (unsigned y = 0; y < 240; y++)
	{
		blackwhite_render_line(dst, src, tab);
		blackwhite_render_line(dst + pitch, src, tab);
		dst += pitch*2;
		src += delta;
	}
}


void rend_dbl(unsigned char *dst, unsigned pitch);
void recolor_render_impl(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, unsigned char* color_screen);
void __declspec(noreturn) errexit(const char *err, const char *str = 0);

enum class DisplayMode { NORMAL, BLACKWHITE, RECOLOR_OFF };
DisplayMode CurrentDisplayMode = DisplayMode::NORMAL;

void AdjustDisplayMode()
{
	if (!(GetKeyState(VK_CONTROL) >> 15))	// high bit is not set
		return;

	if (GetKeyState('1') & 0x8000)
		CurrentDisplayMode = DisplayMode::NORMAL;
	if (GetKeyState('2') & 0x8000)
		CurrentDisplayMode = DisplayMode::RECOLOR_OFF;
	if (GetKeyState('3') & 0x8000)
		CurrentDisplayMode = DisplayMode::BLACKWHITE;
}

void recolor_render(unsigned char *src, unsigned char *dst, unsigned pitch, unsigned delta, const unsigned *tab)
{
	// zx screen is just a 2D array of bytes
	// each byte represents a black/white line of 8 pixels
	unsigned char zx_screen[240 * 320 / 8];

	AdjustDisplayMode();

	if(CurrentDisplayMode == DisplayMode::BLACKWHITE)
	{
		blackwhite_render(src, dst, delta, pitch, tab);
		return;
	}

	create_zxscreen(src, zx_screen, delta, tab);
	rend_dbl(dst, pitch); // standard 2D renderer
	
	try
	{
		if (CurrentDisplayMode == DisplayMode::NORMAL)
			recolor_render_impl(dst, pitch, zx_screen, nullptr); // recoloring
	}
	catch(const std::exception& e)
	{
		errexit(e.what());
	}
}