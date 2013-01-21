#ifndef __FONT_HPP__
#define __FONT_HPP__

/*
	Bitmap Font lives here.

	Since we all love bitmap fonts, and I can't be bothered
	making an elegant solution to how to remove the overhead
	of blitting freetype fonts... Bitmap fonts for all!
*/

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

class Font
{
public:
	// Constructor
	// filename - Image file name
	// tmap - Text Map, how each letter is found in the image
	// all_upper - Whether all characters should be converted to uppercase
	Font(const char *filename, Uint32 c_width, Uint32 c_height, const char *tmap, bool all_upper);

	// Destructor
	~Font();

	// slant
	// amount - New slant amount
	void slant(Sint32 amount);

	// set_colour
	// red - Colour value between 0 and 1
	// green - ""
	// blue - ""
	// alpha - ""
	void set_colour(float red, float green, float blue);
	void set_colour(float red, float green, float blue, float alpha);

	// set_alpha
	// amount - The alpha value (0 to 1)
	void set_alpha(float amount);

	// draw
	// x - X position of where to draw text
	// y - Y position of where to draw text
	// text - The text to draw
	void draw(Sint64 x, Sint64 y, const char *text);
private:
	SDL_Surface *image;
	GLuint tex_id;
	Uint32 char_width;
	Uint32 char_height;
	const char *tex_map;
	bool upper;
	Sint32 slant_amount;
	Uint32 img_width;
	Uint32 img_height;
	float r, g, b, a;
};

#endif
