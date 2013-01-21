#include <ft2build.h> 
#include FT_FREETYPE_H

#include <SDL/SDL.h>

FT_Library library;

FT_Face face;

void put_pixel(SDL_Surface *screen, unsigned char col, int x, int y)
{
	if(x>screen->w || x<0 || y<0 || y>screen->h) // ok- anal I know
		return;

	unsigned char *pixels = screen->pixels;
	pixels[(screen->pitch*y)+(x*screen->format->BytesPerPixel)] = col;
	pixels[(screen->pitch*y)+(x*screen->format->BytesPerPixel)+1] = col;
	pixels[(screen->pitch*y)+(x*screen->format->BytesPerPixel)+2] = col;
}

void renderGlyph(SDL_Surface *screen, char glyph)
{
	SDL_FillRect(screen, NULL, 0x00000000);
	int error;
   int glyph_index = FT_Get_Char_Index( face, glyph);
   error = FT_Load_Glyph( face, glyph_index, 0 ); // Load 'A' glyph with default parameters (0)
   if(error)
   {
      exit(0);
   }

   error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ); // Can haz bitmap?

   if(error)
   {
      exit(0); // no can haz bitmap :|
   }

	int i, j;
	for(i = 0; i<face->glyph->bitmap.rows;i++)
		for(j = 0;j<face->glyph->bitmap.width;j++)
			put_pixel(screen,face->glyph->bitmap.buffer[i*face->glyph->bitmap.pitch+j],j,i);
}

void renderString(SDL_Surface *screen, char *glyphs, int x, int y)
{
	int error;
	int c;
	int cx = x;
	int cy = y;
	for(c = 0;c<strlen(glyphs);c++)
	{
		if(glyphs[c] == '\n')
		{
			cy+=60;
			cx = x;
			continue;
		}
	   int glyph_index = FT_Get_Char_Index( face, glyphs[c]);
	   error = FT_Load_Glyph( face, glyph_index, 0 ); // Load 'A' glyph with default parameters (0)
	   if(error)
	   {
			continue;
	   }
	
	   error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ); // Can haz bitmap?
	
	   if(error)
	   {
			continue;
	   }
	
		int i, j;
		for(i = 0; i<face->glyph->bitmap.rows;i++)
			for(j = 0;j<face->glyph->bitmap.width;j++)
				put_pixel(screen,face->glyph->bitmap.buffer[i*face->glyph->bitmap.pitch+j],cx+j,cy+i+(32-face->glyph->bitmap.rows));
		cx += face->glyph->bitmap.width+5;
		}
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(640,480,32,SDL_SWSURFACE);

   int error = FT_Init_FreeType( &library );
   if ( error )
	  {
		printf("Failed initialising freetype\n");
      exit(0);
   }
   error = FT_New_Face( library, "Vera.ttf", 0, &face );
   if ( error == FT_Err_Unknown_File_Format )
   {
		printf("Failed loading arial.ttf :O unsupported :O\n");
      exit(0);
   } else if ( error )
   {
		printf("File not found probably\n");
      exit(0);
   }

   error = FT_Set_Pixel_Sizes( face, 0, 32 );
	if(error)
	{
		printf("Failed setting size\n");
		exit(0);
	}


	int quit = 0;
	char cchar = 0;
	for(;!quit;)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT)
			{
				quit = 1;
			}
			if(ev.type == SDL_KEYDOWN)
			{
				if(ev.key.keysym.sym == SDLK_LEFT)
				{
					if(cchar>0)
						cchar--;
					else
						cchar = 255;
				}
				if(ev.key.keysym.sym == SDLK_RIGHT)
				{
					if(cchar<255)
						cchar++;
					else
						cchar = 0;
				}
			}
		}

		renderGlyph(screen, cchar);
		char info[32] = { 0 };
		sprintf(info, "Char Num: %d",cchar);
		renderString(screen, info, 50,100);
		SDL_Flip(screen);
	}

	SDL_Quit();
	return 0;
}
