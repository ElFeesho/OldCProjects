#include "locator.hpp"

#include <SDL/SDL_image.h>

#include <math.h>

Locator::Locator() : x(400), y(300), show(false), alpha(0.0f)
{
	arrow = IMG_Load("arrow.png");
	if(!arrow)
		return;
	text = new Font("bitmapfont.png",16,23,"!.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ:;?'",true);
	
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex_id);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, arrow->w, arrow->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, arrow->pixels);

	SDL_FreeSurface(arrow);
}

Locator::~Locator()
{
	delete text;
}

void Locator::update()
{
	Uint8 *keys = SDL_GetKeyState(NULL);
	if(keys[SDLK_LSHIFT])
		show = true;
	else
		show = false;
	if(!show)
	{
		if(alpha <= 0.0f)
		{
			alpha = 0.0f;
			return;
		}
		alpha -= 0.1f;
	}
	else
	{
		if(alpha<1.0f)
			alpha+=0.1f;
	}
	
	// If we got this far, we deserve to get blitted!

	// TODO: Grab location from actual locator beacons not mouse!

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	
	double deltax, deltay;
	if(mx>x)
		deltax = mx-x;
	else
		deltax = x-mx;

	if(my>y)
		deltay = my-y;
	else
		deltay = y-my;	

	double ang = atan2(deltay, deltax);
	double angle = ang*(180/M_PI);
	
	if(my<y)
		angle*=-1;	

	if(mx<x)
		angle=180-angle;

	ang = angle/(180/M_PI);

	text->set_alpha(alpha);
	text->draw(x-(strlen("Feesh!")*16)/2,y-12,"Feesh!");
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex_id);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glLoadIdentity();
	glTranslatef((x+cos(ang)*(strlen("Feesh!")*16))+4, (y+sin(ang)*46)+4, 0);
	
	glRotatef(angle,0.0f,0.0f,1.0f);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE_MINUS_SRC_ALPHA);	
	glColor4f(0.8,0.8,0.8,0.5*alpha);
	glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex2i(-16,-16);
		glTexCoord2i(32,0);
		glVertex2i(16,-16);
		glTexCoord2i(32,32);
		glVertex2i(19,19);
		glTexCoord2i(0,32);
		glVertex2i(-16,16);
	glEnd();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	glLoadIdentity();
	glTranslatef(x+cos(ang)*(strlen("Feesh!")*16), y+sin(ang)*46, 0);
	glRotatef(angle,0.0f,0.0f,1.0f);

	glColor4f(1.0f,1.0f,1.0f,alpha);
	glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex2i(-16,-16);
		glTexCoord2i(32,0);
		glVertex2i(16,-16);
		glTexCoord2i(32,32);
		glVertex2i(16,16);
		glTexCoord2i(0,32);
		glVertex2i(-16,16);
	glEnd();
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}
