#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

#define LEFT 1
#define RIGHT 2
#define UP 4
#define DOWN 8

class car
{
	public:
		car(double x, double y, double rot, SDL_Surface *screen);
		~car();
		void rotate(double am);
		void draw();
		void update();
		void setX(double nx);
		void setY(double ny);
		void setAc(double ac);
		void setKeys(char nkeys);
		char getKeys();
		double getX();
		double getY();
		double getRot();
		double getRam();
		double getAccel();
	private:
		char keys;
		double rot;
		double ram;
		double x,y;
		double accel;
		double speed;
		SDL_Surface *straight[32];
		SDL_Surface *turningl[32];
		SDL_Surface *turningr[32];
		SDL_Surface *screen;
};

car::car(double x, double y, double rot, SDL_Surface *screen) : keys(0), x(x), y(y), rot(rot), screen(screen), ram(0.0), accel(0.0),speed(0.0)
{
	int i;
	char fname[64];
	for(i=0;i<32;i++)
	{
		sprintf(fname,"images/s%04d.png",i);
		straight[i] = IMG_Load(fname);
	}
	for(i=0;i<32;i++)
	{
		sprintf(fname,"images/tl%04d.png",i);
		turningl[i] = IMG_Load(fname);
	}
	for(i=0;i<32;i++)
	{
		sprintf(fname,"images/tr%04d.png",i);
		turningr[i] = IMG_Load(fname);
	}
}

car::~car()
{
	for(int i = 0;i<32;i++)
		delete (straight[i]), delete(turningr[i]), delete(turningl[i]);
}

void car::setKeys(char nkeys)
{
	keys = nkeys;
}

char car::getKeys()
{
	return keys;
}
void car::rotate(double am)
{
	ram=am;
}

void car::draw()
{
	int img = (int)round(rot/11.4);
	if(img == 32) img = 0;
	SDL_Rect pos;
	pos.x = (int)round(x);
	pos.y = (int)round(y);
	pos.w = 128;
	pos.h = 128;
	if(!ram)
		SDL_BlitSurface(straight[img],NULL,screen,&pos);
	else if(ram>0)
		SDL_BlitSurface(turningr[img],NULL,screen,&pos);
	else
		SDL_BlitSurface(turningl[img],NULL,screen,&pos);
}

void car::update()
{
	if((keys&LEFT)==LEFT)
		rot+=-3*(speed/7);	

	if((keys&RIGHT)==RIGHT)
		rot+=3*(speed/7);	

	if(rot>=360.0) rot-=360.0;
	if(rot<=0.0) rot+=360.0;

	if((keys&UP)==UP)
	{
		if(accel<1.2)
			accel+=0.2;
		if(speed<14)
			speed+=accel;
	}
	else
	{
		if(accel>0)
		{
			accel-=0.05;
		}
		if(speed>0.0)
			speed-=0.6;	
	}
	
	if((keys&DOWN)==DOWN)
	{
		if(accel>-1.2)
			accel-=0.2;
		if(speed>-7)
			speed+=accel;
	}
	else
	{
		if(accel<0)
		{
			accel+=0.05;
		}
		if(speed<0.0)
			speed+=0.6;	
	}
	if(speed>-0.5 && speed<0.5) speed = 0.0;
	y += sin((rot+90)*(3.142/180))*speed;
	x += cos((rot+90)*(3.142/180))*speed;
	
}

void car::setX(double nx)
{
	x = nx;
}

void car::setY(double ny)
{
	y = ny;
}

void car::setAc(double ac)
{
	accel = ac;
}

double car::getX()
{
	return x;
}

double car::getY()
{
	return y;
}

double car::getRot()
{
	return rot;
}

double car::getRam()
{
	return ram;
}

double car::getAccel()
{
	return accel;
}

enum MAP_KEY
{
	VK_UP         , // 0
	VK_UP_LEFT    , // 1
	VK_LEFT       , // 2
	VK_DOWN_LEFT  , // 3
	VK_DOWN       , // 4
	VK_DOWN_RIGHT , // 5
	VK_RIGHT      , // 6
	VK_UP_RIGHT   , // 7
	VK_START      , // 8
	VK_SELECT     , // 9
	VK_FL         , // 10
	VK_FR         , // 11
	VK_FA         , // 12
	VK_FB         , // 13
	VK_FX         , // 14
	VK_FY         , // 15
	VK_VOL_UP     , // 16
	VK_VOL_DOWN   , // 17
	VK_TAT          // 18
};

/* The screen surface, joystick device */
SDL_Surface *screen = NULL;
SDL_Joystick *joy = NULL;

void drawImg(SDL_Surface *img, SDL_Surface *dst,int x, int y, int w, int h,int x2, int y2)
{
	SDL_Rect iPos, dPos;
	iPos.x=x;
	iPos.y=y;
	iPos.w=w;
	iPos.h=h;
	dPos.x =x2;
	dPos.y = y2;
	dPos.w = w;
	dPos.h = h;
	SDL_BlitSurface(img,&iPos,screen,&dPos);
}

void drawTiled(SDL_Surface *img, SDL_Surface *dst, int x, int y, int w, int h, int dw, int dh)
{
	//draw in rows
	int i;
	for(i = 0; i<round(dw/w)+((dw%w)?(0):(1));i++)
	{
		int k;
		for(k = 0; k<round(dh/h)+((dh%h)?(0):(1));k++)
			drawImg(img,dst,x,y,w,h,i*w,k*h);
	}
}

void Terminate(void)
{
	SDL_Quit();
#ifdef GP2X
	chdir("/usr/gp2x");
	execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
}

int main (int argc, char *argv[])
{
	int done;
	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
		fprintf (stderr, "Couldn't initialize SDL: %s\n", SDL_GetError ());
		exit (1);
	}
	atexit (Terminate);

	SDL_ShowCursor(SDL_DISABLE);

#ifdef GP2X
	screen = SDL_SetVideoMode (320,240, 16, SDL_SWSURFACE);
#else
	screen = SDL_SetVideoMode (640,480, 16, SDL_SWSURFACE);
#endif
	if (screen == NULL) {
		fprintf (stderr, "Couldn't set 320x240x16 video mode: %s\n", SDL_GetError ());
		exit (2);
	}
	SDL_Surface *bg = IMG_Load("images/tset.png");
	if (SDL_NumJoysticks() > 0) {
		joy = SDL_JoystickOpen(0);
		if(!joy) {
			fprintf (stderr, "Couldn't open joystick 0: %s\n", SDL_GetError ());
		}
	}
	done = 0;
	car Car(200,100,0.0,screen);
	long int lframe = SDL_GetTicks();
	while (!done)
	{
		SDL_Event event;

		/* Check for events */
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					/* if press Ctrl + C, terminate program */
					if ((event.key.keysym.sym == SDLK_ESCAPE))
						done = 1;
					if(event.key.keysym.sym == SDLK_LEFT)
						Car.setKeys(Car.getKeys()|LEFT);
					if(event.key.keysym.sym == SDLK_RIGHT)
						Car.setKeys(Car.getKeys()|RIGHT);
					if(event.key.keysym.sym == SDLK_UP)
						Car.setKeys(Car.getKeys()|UP);
					if(event.key.keysym.sym == SDLK_DOWN)
						Car.setKeys(Car.getKeys()|DOWN);
					break;
				case SDL_KEYUP:
					if(event.key.keysym.sym == SDLK_LEFT)
						Car.setKeys(Car.getKeys()^LEFT);
					if(event.key.keysym.sym == SDLK_RIGHT)
						Car.setKeys(Car.getKeys()^RIGHT);
					if(event.key.keysym.sym == SDLK_UP)
						Car.setKeys(Car.getKeys()^UP);
					if(event.key.keysym.sym == SDLK_DOWN)
						Car.setKeys(Car.getKeys()^DOWN);
					break;
				case SDL_JOYBUTTONDOWN:
					/* if press Start button, terminate program */
					if(event.jbutton.button == VK_LEFT)
						Car.rotate(-3);
					if(event.jbutton.button == VK_RIGHT)
						Car.rotate(3);
					if(event.jbutton.button == VK_UP)
						Car.setAc(1.2);
					if(event.jbutton.button == VK_DOWN)
						Car.setAc(-1.2);
					if ( event.jbutton.button == VK_START )
						done = 1;
					break;
				case SDL_JOYBUTTONUP:
					if(event.jbutton.button == VK_LEFT && Car.getRam()<0)
						Car.rotate(0);
					if(event.jbutton.button == VK_RIGHT && Car.getRam()>0)
						Car.rotate(0);
					if(event.jbutton.button == VK_UP && Car.getAccel()>0)
						Car.setAc(0.0);
					if(event.jbutton.button == VK_DOWN && Car.getAccel()<0)
						Car.setAc(0.0);
					break;
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
		drawTiled(bg,screen,0,0,128,96,640,480);
		Car.update();
		Car.draw();
		SDL_Flip(screen);
	}
	return 0;
}
