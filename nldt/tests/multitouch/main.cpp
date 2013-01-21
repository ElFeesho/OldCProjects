#include "QuickCam.h"
#include <iostream>
#include <string>
#include <list>
using namespace std;
#include "SDL.h"

int main(int argc, char **argv)
{
  /* Create camera */
  QuickCam cam;
  
  /* Setup SDL */
  if(SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    cout << "Unabled to initalize SDL: " << SDL_GetError() << endl;
    return 1;
  }

  /* Setup screen */
  SDL_Surface *screen = SDL_SetVideoMode(cam.get_width(), cam.get_height(), 32, SDL_DOUBLEBUF);
  if(!screen)
  {
    cout << "Unable to initialize screen: " << SDL_GetError() << endl;
    return 1;
  }

  /* Set title */
  SDL_WM_SetCaption("Multitouch Display", "Multitouch Display");

  /* Display camera */
  int x, y, yofs, ofs, ticks;
  uint8_t *p;
  bool running = true;
  SDL_Event event;
  while(running)
  {
    /* Poll SDL events */
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_KEYUP:
        case SDL_QUIT:
          running = false;
          break;
      }
    }

//	for(i = height;i>0;i--)
//		fwrite(data+i*(width*3), 1, width*3, bmp);
    /* Draw picture */
    p = (uint8_t *) cam.grab_image();
    yofs = 0;
    for(y = 0; y < cam.get_height(); y++)
    {
      for(x = 0, ofs = yofs; x < cam.get_width(); x++, ofs++)
      {
        ((unsigned int*)screen->pixels)[ofs] = (*p); p++;
        ((unsigned int*)screen->pixels)[ofs] += (*p) << 8; p++;
        ((unsigned int*)screen->pixels)[ofs] += (*p) << 16; p++;
        //0xFF * (x / 160.0) * (y / 120.0);
      }
      yofs += screen->pitch / 4;
    }
    ticks++;

    /* Flip buffers */
    SDL_Flip(screen);
  }

  /* Close SDL */
  SDL_Quit();
	return 0;
}

