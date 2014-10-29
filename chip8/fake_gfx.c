#include "gfx.h"

static int gfx_cls_called = 0;

static int gfx_draw_last_x = -1;
static int gfx_draw_last_y = -1;
static int gfx_draw_last_count = -1;

int fake_gfx_cls_called()
{
	return gfx_cls_called;
}

int fake_gfx_draw_last_x()
{
	return gfx_draw_last_x;
}

int fake_gfx_draw_last_y()
{
	return gfx_draw_last_y;
}

int fake_gfx_draw_last_count()
{
	return gfx_draw_last_count;
}

void fake_gfx_cls_reset()
{
	gfx_cls_called = 0;
}

void fake_gfx_draw_reset()
{
	gfx_draw_last_x = -1;
	gfx_draw_last_y = -1;
	gfx_draw_last_count = -1;
}

void gfx_init()
{

}

void gfx_flip()
{
	
}

int gfx_draw(int x, int y, unsigned char *mem, int count)
{
	gfx_draw_last_x = x;
	gfx_draw_last_y = y;
	gfx_draw_last_count = count;
}

void gfx_cls()
{
	gfx_cls_called = 1;
}