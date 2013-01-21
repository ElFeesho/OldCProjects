#include "globals.h"

void initGlobals()
{
	globals.gridSize = 0; //If grid size is 0, grid isn't drawn
	globals.toolMode = 1;
	globals.tline = NULL;
	globals.trect = NULL;
	globals.snap = 0;
	globals.start = 0;
	globals.alpha = 255;
	globals.visible = 1;
	globals.image = NULL;
}
