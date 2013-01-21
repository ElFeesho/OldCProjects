#include <iostream>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include "engine.hpp"

using namespace SwordFeesh;

#ifndef SDLWINDOW
static Bool WaitForMapNotify(Display *d, XEvent *e, char *arg)
{ 
	return(e->type == MapNotify) && (e->xmap.window == (Window)arg); 
}
#endif

using namespace std;

Engine::Engine(int w, int h) : scene(NULL), nscene(NULL), w(w), h(h), quit(false)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

#ifdef SDLWINDOW
	SDL_SetVideoMode(w,h,32,SDL_OPENGL|SDL_HWSURFACE);
#else
	dpy = XOpenDisplay(NULL); 
	if (dpy == NULL)
	{
		cerr << "Failed to open X display - fatal error" << endl;
		exit(0);
		return;
	}

	XVisualInfo *vi; 
	Colormap cmap; 
	XSetWindowAttributes swa; 
	GLXContext cx; 
	XEvent event; 
	GLboolean needRedraw = GL_FALSE, recalcModelView = GL_TRUE;

	int dummy; 
	if(!glXQueryExtension(dpy, &dummy, &dummy))
	{ 
		cerr << "Couldn't open X display for GLX - fatal error" << endl;
		exit(1); 
	} 
	
	/* find an OpenGL-capable Color Index visual with depth buffer */ 
	int attributes[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
 
	vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributes); 
	if (vi == NULL) 
	{ 
		cerr << "Couldn't get GLX visual - fatal error" << endl;
		exit(1);
	} 

	/* create an OpenGL rendering context */ 
	cx = glXCreateContext(dpy, vi,  None, GL_TRUE); 
	if (cx == NULL)
	{ 
		cerr << "Failed to create rendering context" << endl;
		exit(1); 
	} 
 
	/* create an X colormap since probably not using default visual */ 
	cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),  
                                vi->visual, AllocNone); 
	swa.colormap = cmap; 
	swa.border_pixel = 0; 
	swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask; 
	glwin = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, w, h, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa); 
	XSetStandardProperties(dpy, glwin, "xogl", "xogl", None, NULL, 0, NULL); 
 
	glXMakeCurrent(dpy, glwin, cx); 
 
	XMapWindow(dpy, glwin); 
	XIfEvent(dpy,  &event,  WaitForMapNotify,  (char *)glwin);
#endif

	if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024)<0)
	{
		cerr << "Couldn't Initialise Audio" << endl;
	}

	inst = this;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable (GL_COLOR_MATERIAL);

	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

   // Calculate The Aspect Ratio Of The Window
	gluOrtho2D(0,w,h,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

Engine::~Engine()
{
#ifndef SDLWINDOW
	XCloseDisplay(dpy);
#endif
	SDL_Quit();

	inst = NULL;
}

bool Engine::update()
{
	if(nscene!=NULL)
	{
		if(scene)
		{
			delete scene;
		}
		scene = nscene;
		nscene = NULL;
	}

	if(!scene)
	{
		cerr << "Reached update cycle with no scene - fatal error" << endl;
		exit(-1);
	}

	ticks = SDL_GetTicks();

	Uint8 *keys = SDL_GetKeyState(NULL);

	clear_screen(255,255,255);
	scene->update(keys);

#ifdef SDLWINDOW
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT)
			return false;
	}

	SDL_GL_SwapBuffers();
#else
	KeySym key; 
	XEvent event;
	while (XPending(dpy)) 
	{ 
		XNextEvent(dpy, &event); 
		switch (event.type) { 
		case KeyPress: 
			XLookupString((XKeyEvent *)&event, NULL, 0, &key, NULL); 
			switch (key) { 
				case XK_Left:
					shutdown();
					break; 
				} 
			break; 
		}
	} 

	glXSwapBuffers(dpy, glwin);
#endif
	if(SDL_GetTicks()<ticks+(1000.0/60.0))
	{
		SDL_Delay(ticks+(1000.0/60.0)-SDL_GetTicks());
	}
	return !quit;
}

void Engine::set_scene(Scene *scene)
{
	if(scene == NULL)
		return;
	nscene = scene;
}

Engine *Engine::get_inst()
{
	return inst;
}

Uint32 Engine::get_ticks()
{
	return ticks;
}

void Engine::shutdown()
{
	quit = true;
}

void Engine::clear_screen(int r, int g, int b)
{
	glClearColor(r/255.0f,g/255.0f,b/255.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::get_viewport(int *w, int *h)
{
	*w = this->w;
	*h = this->h;
}

Engine *Engine::inst = NULL;
Uint32 Engine::ticks = 0;

