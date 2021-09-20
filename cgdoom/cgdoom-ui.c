#include "cgdoom-ui.h"
#include "i_system.h"
#include "z_zone.h"
#include <stdio.h>
#include <stdlib.h>

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

static inline void SetPixel(int x, int y, int color)
{
	if ((unsigned)x < 384 && (unsigned)y < 216)
		VRAM[WIDTH * y + x] = color;
}

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
		int b = font_glyphs[offset + (bit>>5)] & (0x80000000 >> (bit&31));
		if (b)
			SetPixel(*x+dx, y+dy, fg);
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

static int Font_ReverseStringLength(char const *text, int width)
{
	int i = 0;
	for(; text[i]; i++) {
		if(text[i] < 32)
			continue;

		int cw = font_width[text[i]-32] + 1;
		if(cw > width)
			break;

		width -= cw;
	}
	return i;
}

void UI_Print(int x, int y, int fg, int align, char const *text, int length)
{
	if(align == UI_CENTER)
		x -= (Font_StringLength(text) + 1) / 2;
	if(align == UI_RIGHT)
		x -= Font_StringLength(text);

	for(int i = 0; text[i]; i++) {
		if (length >= 0 && i >= length)
			break;
		Font_RenderChar(&x, y, text[i], fg);
	}
}

void UI_Vprintf(int x, int y, int fg, int align, char const *fmt, va_list args)
{
	char str[256];
	vsnprintf(str, 256, fmt, args);
	UI_Print(x, y, fg, align, str, -1);
}

void UI_Printf(int x, int y, int fg, int align, char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	UI_Vprintf(x, y, fg, align, fmt, args);
	va_end(args);
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
			SetPixel(x+dx, y+dy, 0x0000);
		}
	}
}

void UI_ProgressBar(int y0, int current, int total)
{
	const int w=192, h=5;
	const int x0=(WIDTH-w)/2;
	const int pos = x0 + (w * current) / total;

	for(int x = x0; x < x0+w; x++)
	{
		SetPixel(x, y0, 0x0000);
		SetPixel(x, y0+h-1, 0x0000);

		if(x > pos && x != x0+w-1) continue;
		for(int y = y0+1; y < y0+h-1; y++)
			SetPixel(x, y, 0x0000);
	}
	Bdisp_PutDisp_DD();
}

/* Layout functions. */

#define LAYOUT_LEFT 40
#define LAYOUT_RIGHT (WIDTH-40)

#define FOCUS_NOTHING 0
#define FOCUS_CHECKBOX 1
#define FOCUS_INTEGER 2

void Layout_Init(Layout *l)
{
	l->focus = -1;
	l->active = 0;
	Layout_StartFrame(l);
}

void Layout_StartFrame(Layout *l)
{
	l->focus_count = 0;
	l->y = 10;
}

void Layout_EndFrame(Layout *l)
{
	if(l->focus < 0)
		return;
	int y = l->focus_y[l->focus];

	if(l->active)
	{
		/* Draw cursor */
		int x = LAYOUT_RIGHT - Font_StringLength(l->text + l->cursor);
		for(int dy = 0; dy < 12; dy++)
			SetPixel(x, y+dy, 0x0000);
	}
	else
	{
		UI_AreaReverse(0, y, WIDTH-1, y+13);
	}
}

static void Layout_AddFocus(Layout *l, int type, void *data)
{
	if(l->focus < 0)
		l->focus = 0;
	if(l->focus_count >= 16)
		return;

	l->focus_y[l->focus_count] = l->y - 2;
	l->focus_type[l->focus_count] = type;
	l->focus_data[l->focus_count] = data;

	l->focus_count++;
}

static void Layout_StartInput(Layout *l)
{
	if (l->active)
		return;

	l->active = 1;
	memset(l->text, 0, sizeof l->text);
	l->cursor = 0;
}

static void Layout_EndInput(Layout *l)
{
	l->active = 0;
}

void Layout_CenteredText(Layout *l, const char *text)
{
	UI_Print(WIDTH / 2, l->y, 0x0000, UI_CENTER, text, -1);
	l->y += 14;
}

void Layout_Text(Layout *l, const char *label, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Layout_AddFocus(l, FOCUS_NOTHING, NULL);
	UI_Print(LAYOUT_LEFT, l->y, 0x0000, UI_LEFT, label, -1);
	UI_Vprintf(LAYOUT_RIGHT, l->y, 0x0000, UI_RIGHT, fmt, args);
	l->y += 14;
	va_end(args);
}

void Layout_Checkbox(Layout *l, const char *label, int *checked)
{
	Layout_AddFocus(l, FOCUS_CHECKBOX, checked);
	UI_Print(LAYOUT_LEFT, l->y, 0x0000, UI_LEFT, label, -1);
	UI_Checkbox(LAYOUT_RIGHT-7, l->y+1, *checked);
	l->y += 14;
}

void Layout_Integer(Layout *l, const char *label, int *value)
{
	Layout_AddFocus(l, FOCUS_INTEGER, value);
	UI_Print(LAYOUT_LEFT, l->y, 0x0000, UI_LEFT, label, -1);
	if(l->active && l->focus == l->focus_count - 1)
		UI_Print(LAYOUT_RIGHT, l->y, 0x0000, UI_RIGHT, l->text, -1);
	else
		UI_Printf(LAYOUT_RIGHT, l->y, 0x0000, UI_RIGHT, "%d", *value);
	l->y += 14;
}

void Layout_Spacing(Layout *l, int spacing)
{
	l->y += spacing;
}

int Layout_Event(Layout *l, int key)
{
	int type    = (l->focus >= 0) ? l->focus_type[l->focus] : -1;
	void *data  = (l->focus >= 0) ? l->focus_data[l->focus] : NULL;

	if(key == KEY_CTRL_UP && !l->active)
	{
		l->focus--;
		if(l->focus < 0)
			l->focus += l->focus_count;
		return 1;
	}
	if(key == KEY_CTRL_DOWN && !l->active)
	{
		l->focus++;
		if(l->focus >= l->focus_count)
			l->focus -= l->focus_count;
		return 1;
	}
	if(key == KEY_CTRL_EXE && type == FOCUS_CHECKBOX && data)
	{
		*(int *)data ^= 1;
		return 1;
	}
	if(key == KEY_CTRL_EXE && type == FOCUS_INTEGER && data)
	{
		if (!l->active)
			Layout_StartInput(l);
		else
		{
			Layout_EndInput(l);
			*(int *)data = atoi(l->text);
		}
		return 1;
	}
	if(key == KEY_CTRL_LEFT && type == FOCUS_INTEGER && data && !l->active)
	{
		*(int *)data -= 1;
		return 1;
	}
	if(key == KEY_CTRL_RIGHT && type == FOCUS_INTEGER && data && !l->active)
	{
		*(int *)data += 1;
		return 1;
	}

	/* Input controls */
	if(l->active && key == KEY_CTRL_LEFT && l->cursor > 0)
	{
		l->cursor--;
		return 1;
	}
	if(l->active && key == KEY_CTRL_RIGHT && l->text[l->cursor])
	{
		l->cursor++;
		return 1;
	}
	if(l->active && key >= '0' && key <= '9')
	{
		if (strlen(l->text) < sizeof l->text - 1)
		{
			memmove(l->text + l->cursor + 1, l->text + l->cursor,
				sizeof l->text - l->cursor);
			l->text[l->cursor++] = key;
		}
		return 1;
	}
	if(l->active && key == KEY_CTRL_DEL && l->cursor > 0)
	{
		memmove(l->text + l->cursor - 1, l->text + l->cursor,
			strlen(l->text + l->cursor) + 1);
		l->cursor--;
	}

	return 0;
}

int UI_Main(CGD_WADFileInfo *wads, int wad_count, int *dev_info, int *use_mmap,
	int *startmap, int *startepisode, int *trustunaligned,
	int *autostart, int *enabledemos, int *enable2MBline)
{
	Layout l;
	Layout_Init(&l);

	while(1)
	{
		Bdisp_AllClr_VRAM();
		Layout_StartFrame(&l);
		Layout_CenteredText(&l, "CGDoom for fx-CG 50 and Graph 90+E");
		Layout_Spacing(&l, 10);

		for(int i = 0; i < wad_count; i++)
		{
			char const *action = "Play other WAD";
			if(!strcmp(wads[i].name, "doom.wad"))
				action = "Play Doom Shareware";
			else if(!strcmp(wads[i].name, "doomu.wad"))
				action = "Play Ultimate Doom";
			else if(!strcmp(wads[i].name, "doomu1.wad"))
				action = "Play Ultimate Doom E1";
			else if(!strcmp(wads[i].name, "doomu2.wad"))
				action = "Play Ultimate Doom E2";
			else if(!strcmp(wads[i].name, "doomu3.wad"))
				action = "Play Ultimate Doom E3";
			else if(!strcmp(wads[i].name, "doomu4.wad"))
				action = "Play Ultimate Doom E4";
			else if(!strcmp(wads[i].name, "doom2.wad"))
				action = "Play Doom II";

			Layout_Text(&l, action, "%s (%d.%dM)",
				wads[i].name, wads[i].size / 1000000,
				(wads[i].size % 1000000) / 100000);
		}

		Layout_Spacing(&l, 10);
		Layout_Integer(&l, "Start at episode:", startepisode);
		Layout_Integer(&l, "Start at map:", startmap);
		Layout_Text(&l, "Title screen:",
			*autostart ? "Skip" :
			!*enabledemos ? "No demos" : "Full");

		Layout_Spacing(&l, 10);
		Layout_Checkbox(&l, "Developer info:", dev_info);
		Layout_Checkbox(&l, "Map file to memory:", use_mmap);
		Layout_Checkbox(&l, "Trust unaligned lumps:", trustunaligned);
		if(enable2MBline)
			Layout_Checkbox(&l, "Use experimental RAM:", enable2MBline);
		Layout_EndFrame(&l);
		Bdisp_PutDisp_DD();

		int key;
		GetKey(&key);

		if(Layout_Event(&l, key)) {
			/* Autostart when changing starting episode/map */
			int startparam = (l.focus == wad_count || l.focus == wad_count+1);
			int changekey = (key == KEY_CTRL_EXE || key == KEY_CTRL_LEFT ||
                key == KEY_CTRL_RIGHT);
			if(startparam && changekey)
				*autostart = 1;
		}
		else if(key == KEY_CTRL_EXE && l.focus == wad_count + 2) {
			if (!*autostart && *enabledemos)
				*autostart = 1;
			else if (!*autostart)
				*enabledemos = 1;
			else
				*autostart = 0, *enabledemos = 0;
		}
		else if(key == KEY_CTRL_EXE && l.focus < wad_count) {
			return l.focus;
		}

		if(*startmap < 1)
			*startmap = 1;
		if(*startepisode < 1)
			*startepisode = 1;
	}

	return -1;
}

static int isspace_(int c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

void UI_Error(const char *fmt, va_list args)
{
	char message[256], *str=message;
	vsnprintf(message, 256, fmt, args);

	int y = 12;
	Bdisp_AllClr_VRAM();
	UI_Print(192, y, 0x0000, UI_CENTER, "Error!", -1);
	y += 14 + 12;

	while(*str) {
		int len = Font_ReverseStringLength(str, LAYOUT_RIGHT - LAYOUT_LEFT);

		/* Try to stop at previous word boundary */
		int wordb = len;
		while(wordb > 0 && str[wordb] && !isspace_(str[wordb])) wordb--;
		if(wordb > 0) len = wordb;

		UI_Print(LAYOUT_LEFT, y, 0x0000, UI_LEFT, str, len);
		y += 14;

		/* Skip spacing */
		str += len;
		while(*str && isspace_(*str)) str++;
	}

	/* Additional info for heap errors */
	if(!strncmp(fmt, "Z_", 2)) {
		y += 12;

		UI_Print(LAYOUT_LEFT, y, 0x0000, UI_LEFT, "Free memory in heap:", -1);
		y += 14;

		for (int i = 0;; i++) {
			int size;
			byte *ptr = I_ZoneBase(&size, i);
			if (!ptr) break;

			UI_Printf(LAYOUT_LEFT, y, 0x0000, UI_LEFT,
				"Zone #%d: %d/%d kB (largest block %d B)",
				i, Z_FreeMemory(i) >> 10, size >> 10, Z_LargestFreeBlock(i));
			y += 14;
		}
	}

	Bdisp_FrameAndColor(3, 16);
	Bdisp_FrameAndColor(1, 0);
	Bdisp_PutDisp_DD();

	int key;
	while(PRGM_GetKey());
	GetKey(&key);
}

void UI_DelayedWrites(CGD_DelayedFileWrite const *dfw, int count,
	int current_file, int current_file_done)
{
	int total = 0;
	int current = 0;

	for(int i = 0; i < count; i++) {
		if(dfw[i].data == NULL)
			continue;

		total += dfw[i].size;
		if(i < current_file)
			current += dfw[i].size;
		else if(i == current_file)
			current += current_file_done;
	}

	Bdisp_AllClr_VRAM();

	UI_Printf(WIDTH / 2, 10, 0x0000, UI_CENTER,
		"Game saves");
	if(current_file_done == -1)
		UI_Printf(WIDTH / 2, HEIGHT / 2 - 12, 0x0000, UI_CENTER,
			"Creating %s... (%d kB)", dfw[current_file].filename,
			dfw[current_file].size >> 10);
	else
		UI_Printf(WIDTH / 2, HEIGHT / 2 - 12, 0x0000, UI_CENTER,
			"Saving %s... (%d/%d kB)", dfw[current_file].filename,
			current_file_done >> 10, dfw[current_file].size >> 10);
	UI_ProgressBar(HEIGHT / 2 + 7, current, total);

	Bdisp_FrameAndColor(3, 16);
	Bdisp_FrameAndColor(1, 0);
	Bdisp_PutDisp_DD();
}
