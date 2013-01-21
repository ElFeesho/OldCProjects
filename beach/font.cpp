#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "font.hpp"

Font::Font(const char *filename, Uint32 c_width, Uint32 c_height, const char *tmap, bool all_upper) : char_width(c_width), char_height(c_height), tex_map(tmap), upper(all_upper), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{
	image = IMG_Load(filename);

	if(!image) // Report errors? I laugh at you
		return;

	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex_id);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex_id);

	img_width = image->w;
	img_height = image->h;

	SDL_FreeSurface(image);
}

Font::~Font()
{
}

void Font::set_colour(float red, float green, float blue)
{
	r = red;
	g = green;
	b = blue;
}

void Font::set_colour(float red, float green, float blue, float alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}


void Font::set_alpha(float amount)
{
	a = amount;
}

void Font::draw(Sint64 x, Sint64 y, const char *text)
{
	if(!image)
		return;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex_id);

	glLoadIdentity();

	/*
		MAGIC HAPPENS HERE!
	*/
	Uint32 frame, frame_x, frame_y;
	Sint64 xpos = x;
	Sint64 ypos = y;

	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	for(Uint32 i = 0; i<strlen(text);i++)
	{
		if(text[i] == '\n')
		{
			xpos = x;
			ypos += char_height+4;
			continue;
		}

		/* Chaosteil sprinkles in the magic */
		char *offset;
		if(upper)
		{
			offset = index((char*)tex_map, toupper(text[i]));
			if(!offset)
			{
				xpos+=char_width+2;
				continue;
			}
			frame = (Uint32)(index(tex_map,toupper(text[i]))-tex_map);
		}
		else
		{
			offset = index((char*)tex_map,text[i]);;
			if(!offset)
			{
				xpos+=char_width+2;
				continue;
			}
			frame = (Uint32)(index(tex_map,text[i])-tex_map);
		}

		frame_x = (frame%(img_width/char_width))*char_width;
		frame_y = (frame/(img_width/char_width))*char_height;

     	Uint32 tx_coords[8] =
		{
			frame_x,
			frame_y ,
			(frame_x+char_width),
			frame_y,
			(frame_x+char_width),
			(frame_y+char_height),
			frame_x,
			(frame_y+char_height)
		};

		glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.8,0.8,0.8,0.5*a);
		glBegin(GL_QUADS);
			glTexCoord2i(tx_coords[0],tx_coords[1]);
			glVertex2i(xpos+3, ypos+3);
			glTexCoord2i(tx_coords[2],tx_coords[3]);
			glVertex2i(xpos+char_width+3, ypos+3);
			glTexCoord2i(tx_coords[4],tx_coords[5]);
			glVertex2i(xpos+char_width+3, ypos+char_height+3);
			glTexCoord2i(tx_coords[6],tx_coords[7]);
			glVertex2i(xpos+3, ypos+char_height+3);
		glEnd();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(r,g,b,a);
		glBegin(GL_QUADS);
			glTexCoord2i(tx_coords[0],tx_coords[1]);
			glVertex2i(xpos, ypos);
			glTexCoord2i(tx_coords[2],tx_coords[3]);
			glVertex2i(xpos+char_width, ypos);
			glTexCoord2i(tx_coords[4],tx_coords[5]);
			glVertex2i(xpos+char_width, ypos+char_height);
			glTexCoord2i(tx_coords[6],tx_coords[7]);
			glVertex2i(xpos, ypos+char_height);
		glEnd();

		xpos+=char_width+2;
	}
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void Font::slant(Sint32 amount)
{
	slant_amount = amount;
}
