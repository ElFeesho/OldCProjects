#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>

#ifndef SDLWINDOW
#include <GL/glx.h> 
#include <GL/gl.h> 
#include <GL/glu.h> 
#include <X11/keysym.h> 
#include <X11/Xlib.h> 
#include <X11/Xutil.h> 
#endif

#include "scene.hpp"

using std::vector;

namespace SwordFeesh
{
	class Engine
	{
	public:
		Engine(int w, int h);
		~Engine();
		bool update();

		void set_scene(Scene *scene);

		static Engine *get_inst();
		static Uint32 get_ticks();
		void get_viewport(int *w, int *h);
		void shutdown();
		void clear_screen(int r = 0, int g = 0, int b = 0);

	private:
	#ifndef SDLWINDOW
		Display *dpy; 
		Window glwin;
	#endif
		static Engine *inst;
		static Uint32 ticks;
		int w;
		int h;
		Scene *nscene; // New scene
		Scene *scene; // The actual scene

		bool quit; // Shutdown sets this to true
	};
}

#endif
