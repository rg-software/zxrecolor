
/* (begin) ready screen  -- for debug */
/*void create_my_line(unsigned char *dst, unsigned char *src, unsigned *tab, bool isbw)
{
	unsigned ink = 0x00000000, paper = 0x00FFFFFF;

	for (unsigned x = 0; x < temp.scx * 4; x += 32)
	{
		unsigned char byte = *src++; // a bit sequence of 8 pixels
		unsigned *tt = tab + *src++; // attributes

		for (unsigned i = 0; i < 8; ++i)
			*(unsigned*)(dst + x + i * 4) = isbw ? (((byte << i) & 128) ? ink : paper) : tt[(byte << (i + 1)) & 0x100];
	}
}


void create_my_screen(unsigned char *dst, unsigned pitch, bool isbw)
{
	unsigned char *src = rbuf; unsigned delta = temp.scx / 4;
	for (unsigned y = 0; y < temp.scy; y++)
	{
		create_my_line(dst, src, t.sctab32[0], isbw);
		dst += pitch;
		src += delta;
	}
}*/
/* (end) ready screen  -- for debug */

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

void rend_dbl(unsigned char *dst, unsigned pitch);
void recolor_render_impl(unsigned char *dst, unsigned pitch, unsigned char* zx_screen, unsigned char* color_screen);

void recolor_render(unsigned char *src, unsigned char *dst, unsigned pitch, unsigned delta, const unsigned *tab)
{
	// zx screen is just a 2D array of bytes
	// each byte represents a black/white line of 8 pixels
	unsigned char zx_screen[240 * 320 / 8];
	
	//create_my_screen(dst, pitch, true); // renders bw or color screen (mostly for debug)
	create_zxscreen(src, zx_screen, delta, tab);

	rend_dbl(dst, pitch); // standard 2D renderer
	recolor_render_impl(dst, pitch, zx_screen, nullptr); // recoloring
}