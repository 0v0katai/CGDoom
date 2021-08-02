#include "cgdoom-ui.h"

/* gint's font in a PrizmSDK-friendly (though ugly) form */
static uint8_t const font_width[96] = {
	5, 2, 6, 7, 8, 6, 8, 3, 4, 4, 8, 8, 3, 7, 2, 5, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 2, 3, 5, 7, 5, 6, 8, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 8, 7, 8,
	7, 8, 7, 7, 8, 7, 8, 8, 8, 8, 6, 4, 5, 4, 7, 7, 4, 7, 7, 7, 8, 8, 7, 7,
	7, 4, 6, 7, 4, 8, 7, 8, 7, 7, 8, 7, 8, 7, 8, 8, 7, 7, 6, 4, 2, 4, 7, 6,
};
static uint32_t const font_glyphs[263] = {
	0x0, 0x0, 0xfff3c000, 0xcf3ce200, 0x0, 0x0, 0x6cdbfb66, 0xcdbfb66c, 0x0,
	0x187edbd8, 0x7e1bdb7e, 0x18000000, 0x30cc631, 0x8cc30000, 0x0,
	0x78cccc68, 0x70dbcece, 0x7b000000, 0xfde00000, 0x0, 0x36ccccc6,
	0x30000000, 0xc6333336, 0xc0000000, 0x18db7e, 0x3c7edb18, 0x0,
	0x181818, 0xff181818, 0x0, 0x7ef, 0x0, 0xf, 0xe0000000, 0x0, 0x3c000,
	0x18cc6731, 0x98c00000, 0x7d8f1e3c, 0x78f1e37c, 0x0, 0x1871e2c1,
	0x83060c7e, 0x0, 0x3ccc1830, 0xc30c307e, 0x0, 0x7d8c1833, 0xc0c1e37c,
	0x0, 0xc38f36c, 0xd9bf860c, 0x0, 0xfd8307e0, 0x60c1e37c, 0x0,
	0x7d8f07ec, 0x78f1e37c, 0x0, 0xff8c1860, 0xc3060c18, 0x0, 0x7d8f1e37,
	0xd8f1e37c, 0x0, 0x7d8f1e3c, 0x6fc1837c, 0x0, 0xf0f0000, 0x1b03b78,
	0x0, 0xcccc30, 0xc3000000, 0x7f0, 0x1fc00000, 0x0, 0x6186199,
	0x98000000, 0x7b30c630, 0xc00c3000, 0x0, 0x7cc6037b, 0xdbdbdbde,
	0x7c000000, 0x1070e366, 0xcdbfe3c6, 0x0, 0xfd8f1e3f, 0xd8f1e3fc, 0x0,
	0x3ccf060c, 0x1830333c, 0x0, 0xf99b1e3c, 0x78f1e6f8, 0x0, 0xff83060f,
	0x983060fe, 0x0, 0xff83060f, 0x983060c0, 0x0, 0x3ccf060c, 0xf8f1b33e,
	0x0, 0xc78f1e3f, 0xf8f1e3c6, 0x0, 0xfcc30c30, 0xc30cfc00, 0x0,
	0x1e0c1830, 0x60c1e37c, 0x0, 0xc79b678e, 0x1e3666c6, 0x0, 0xc183060c,
	0x183060fe, 0x0, 0xc3c3e7ff, 0xdbc3c3c3, 0xc3000000, 0xc78f9fbd,
	0xf9f1e3c6, 0x0, 0x3c66c3c3, 0xc3c3c366, 0x3c000000, 0xf99b1e3c,
	0xdf3060c0, 0x0, 0x3c66c3c3, 0xc3c3db6e, 0x37000000, 0xfd8f1e3c,
	0x7fb363c6, 0x0, 0x7d8f0607, 0xc0c1e37c, 0x0, 0xff181818, 0x18181818,
	0x18000000, 0xc78f1e3c, 0x78f1e37c, 0x0, 0xc3c3c366, 0x66663c3c,
	0x18000000, 0xc3c3c3db, 0xdb7e7e66, 0x66000000, 0xc3c3663c, 0x183c66c3,
	0xc3000000, 0xc3c36666, 0x3c181818, 0x18000000, 0xfc30c631, 0x8c30fc00,
	0x0, 0xfccccccc, 0xf0000000, 0xc618c718, 0xc3180000, 0xf3333333,
	0xf0000000, 0x38db1800, 0x0, 0x0, 0x0, 0xfe, 0x0, 0xe6310000, 0x0,
	0xf330, 0x6ff1e776, 0x0, 0xc183773c, 0x78f1f3dc, 0x0, 0xf33c,
	0x1830333c, 0x0, 0x3033b67, 0xc3c3c367, 0x3b000000, 0x3c66, 0xc3ffc066,
	0x3c000000, 0x3ccd830f, 0x8c183060, 0x0, 0x1de7c, 0x78f1e776,
	0xdf00000, 0xc183773c, 0x78f1e3c6, 0x0, 0x60e66666, 0xf0000000,
	0xc01c30c, 0x30c3cde0, 0x0, 0xc1831e6d, 0x9e3666c6, 0x0, 0xe6666666,
	0xf0000000, 0xf6db, 0xdbdbdbdb, 0xdb000000, 0x3773c, 0x78f1e3c6, 0x0,
	0x3c66, 0xc3c3c366, 0x3c000000, 0x3773c, 0x78f1f3dd, 0x83000000,
	0x1de7c, 0x78f1e776, 0xc180000, 0xdef3, 0xe0c0c0c0, 0xc0000000,
	0x1f63c, 0xf81e37c, 0x0, 0x3030fc30, 0x30303033, 0x1e000000, 0x31e3c,
	0x78f1e776, 0x0, 0xc3c3, 0x66663c3c, 0x18000000, 0xc3c3, 0xdbdb7e66,
	0x66000000, 0x31e36, 0xc71b63c6, 0x0, 0x31e3c, 0x6d9b1c18, 0x61800000,
	0xfc318, 0xc630fc00, 0x0, 0x3666c666, 0x30000000, 0xffffc000,
	0xc6663666, 0xc0000000, 0x3bd, 0xc0000000, 0x0, 0xfe186186, 0x1861fc00,
	0x0,
};

static void Font_RenderChar(int *x, int y, int c, int fg)
{
	if(c < 32 || c >= 128)
		return;
	int g = c - 32;

	/* Find offset in long array (we drop the glyph index for brevity) */
	int offset = 0;
	for(int glyph = 0; glyph < g; glyph++)
	{
		offset += (font_width[glyph] * 11 + 31) >> 5;
	}

	/* Render pixels */
	int bit = 0;
	for(int dy = 0; dy < 11; dy++)
	for(int dx = 0; dx < font_width[g]; dx++)
	{
		int b = font_glyphs[offset + (bit >> 5)] & (0x80000000 >> (bit & 31));
		if (b)
			Bdisp_SetPoint_VRAM(*x+dx, y+dy, fg);
		bit++;
	}

	*x += font_width[g] + 1;
}

static int Font_StringLength(char const *text)
{
	int w = 0;
	for(int i = 0; text[i]; i++)
	{
		if(text[i] < 32) /* text[i] >= 128 can't happen */
			continue;
		w += font_width[text[i]-32] + 1;
	}
	return w - 1;
}

enum {
	UI_LEFT = 0,
	UI_CENTER = 1,
	UI_RIGHT = 2,
};

void UI_Print(int x, int y, char const *text, int fg, int align)
{
	if(align == UI_CENTER)
		x -= (Font_StringLength(text) + 1) / 2;
	if(align == UI_RIGHT)
		x -= Font_StringLength(text);

	for(int i = 0; text[i]; i++)
		Font_RenderChar(&x, y, text[i], fg);
}

void UI_AreaReverse(int x1, int y1, int x2, int y2)
{
	uint16_t *VRAM = (uint16_t *)GetVRAMAddress() + WIDTH * y1;

	for(int y = y1; y <= y2; y++)
	{
		for(int x = x1; x <= x2; x++)
			VRAM[x] = ~VRAM[x];
		VRAM += WIDTH;
	}
}

void UI_Checkbox(int x, int y, int checked)
{
	for(int dy = 0; dy <= 7; dy++)
	for(int dx = 0; dx <= 7; dx++)
	{
		if((dx == 0) || (dx == 7) || (dy == 0) || (dy == 7) ||
			(checked && dx >= 2 && dx <= 5 && dy >= 2 && dy <= 5))
		{
			Bdisp_SetPoint_VRAM(x+dx, y+dy, 0x0000);
		}
	}
}

int UI_Main(char const wad_files[][32], int wad_file_count, int *dev_info)
{
	int key=0, cursor=0;

	while(key != KEY_CTRL_EXIT)
	{
		int l=60, r=WIDTH-l, y=34, cursor_max=0, play_max=0;

		Bdisp_AllClr_VRAM();
		UI_Print(WIDTH / 2, 12, "CGDOOM for fx-CG 50 and Graph 90+E",
			0x0000, UI_CENTER);

		for(int i = 0; i < wad_file_count; i++)
		{
			char const *action = "Play other WAD";
			if(!strcmp(wad_files[i], "doom.wad"))
				action = "Play DOOM";
			else if(!strcmp(wad_files[i], "doomu.wad"))
				action = "Play Ultimate DOOM";

			char name[34] = "";
			strcat(name, "(");
			strcat(name, wad_files[i]);
			strcat(name, ")");

			UI_Print(l, y, action, 0x0000, UI_LEFT);
			UI_Print(r, y, name, 0x0000, UI_RIGHT);
			y += 14;
		}
		play_max = cursor_max = wad_file_count;

		UI_Print(l, y, "Developer info:", 0x0000, UI_LEFT);
		UI_Checkbox(r-7, y+1, *dev_info);
		y += 14;
		cursor_max++;

		UI_AreaReverse(0, 32+14*cursor, WIDTH-1, 32+14*cursor+13);
		Bdisp_PutDisp_DD();

		GetKey(&key);
		if(key == KEY_CTRL_UP && cursor > 0)
			cursor--;
		if(key == KEY_CTRL_DOWN && cursor < cursor_max - 1)
			cursor++;
		if(key == KEY_CTRL_EXE && cursor < play_max)
			return cursor;
		if(key == KEY_CTRL_EXE && cursor == cursor_max - 1)
			*dev_info ^= 1;
	}

	return -1;
}

void UI_FileMappingProgressBar(int size_mapped, int size_total)
{
	const int w=192, h=5;
	const int x0=(WIDTH-w)/2, y0=HEIGHT-20;
	const int pos = x0 + (w * size_mapped) / size_total;

	for(int x = x0; x < x0+w; x++)
	{
		Bdisp_SetPoint_VRAM(x, y0, COLOR_BLACK);
		Bdisp_SetPoint_VRAM(x, y0+h-1, COLOR_BLACK);

		if(x > pos && x != x0+w-1) continue;
		for(int y = y0+1; y < y0+h-1; y++)
			Bdisp_SetPoint_VRAM(x, y, COLOR_BLACK);
	}
	Bdisp_PutDisp_DD();
}
