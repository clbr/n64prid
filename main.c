#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

typedef uint64_t u64;
typedef unsigned int u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef signed int s32;
typedef int16_t s16;
typedef int8_t s8;

volatile u32 frames;

display_context_t lockVideo(int wait)
{
	display_context_t dc;

	if (wait)
		while (!(dc = display_lock()));
	else
		dc = display_lock();
	return dc;
}

void unlockVideo(display_context_t dc)
{
	if (dc)
		display_show(dc);
}

/* text functions */
void printText(display_context_t dc, char *msg, int x, int y)
{
	if (dc)
		graphics_draw_text(dc, x*8, y*8, msg);
}

/* vblank callback */
void vblCallback(void)
{
	frames++;
}

void delay(u32 cnt)
{
	u32 then = frames + cnt;
	while (then > frames);
}

/* initialize console hardware */
void init_n64(void)
{
	/* enable interrupts (on the CPU) */
	init_interrupts();

	/* Initialize peripherals */
	display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);

	register_VI_handler(vblCallback);
}

/* main code entry point */
int main(void)
{
	char buf[32];
	u32 prid, fpid;
	asm("mfc0 %0, $15" : "=r" (prid) : :);
	asm("cfc1 %0, $0" : "=r" (fpid) : :);

	sprintf(buf, "prid 0x%x fpid 0x%x", prid, fpid);

	display_context_t disp;

	init_n64();

	while (1)
	{
		u32 color;

		disp = lockVideo(1);
		color = graphics_make_color(0xCC, 0xCC, 0xCC, 0xFF);
		graphics_fill_screen(disp, color);

		color = graphics_make_color(0x00, 0x00, 0x00, 0xFF);
		graphics_set_color(color, 0);

		printText(disp, buf, 4, 4);

		unlockVideo(disp);

		delay(1);
	}

	return 0;
}
