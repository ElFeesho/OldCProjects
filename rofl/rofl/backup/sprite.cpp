#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "sprite.hpp"

using namespace std;



Sprite::Sprite(const char *filename) : sheet(NULL), w(0), h(0), ltime(0), frames(0), cframe(0), const_time(true), delays(NULL)
{
	fstream sprite_file;
	sprite_file.open(filename, fstream::in);
	if(!sprite_file.good())
	{
		cerr << "Failed openning: " << filename << " for sprite information." << endl;
	}

	int found = 0;

	while(!sprite_file.eof())
	{
		char *buffer = new char[256];
		sprite_file.getline(buffer,256);
		string line(buffer);

		if(line.find("sprite")==0)
		{
			size_t eq_pos = line.find("=");
			if(eq_pos == string::npos)
			{
				cerr << "Sprite file error: Malformed 'sprite' line" << endl;
			}
			else
			{
				string image_file = line.substr(eq_pos+2, line.length()-eq_pos);
				sheet = new Image(image_file.c_str());
				if(!sheet)
				{
					cerr << "FAILED LOADING SPRITE IMAGE!: " << image_file << endl;
				}
				found &= 1;
			}
		}

		if(line.find("frames")==0)
		{
			size_t eq_pos = line.find("=");
			if(eq_pos == string::npos)
			{
				cerr << "Sprite file error: Malformed 'frames' line" << endl;
			}
			else
			{
				string frame_count = line.substr(eq_pos+1, line.length()-eq_pos);
				stringstream conv;
				conv << frame_count;
				conv >> frames;
				found &= 2;
			}
		}

		if(line.find("frame_w")==0)
		{
			size_t eq_pos = line.find("=");
			if(eq_pos == string::npos)
			{
				cerr << "Sprite file error: Malformed 'frame_w' line" << endl;
			}
			else
			{
				string frame_w = line.substr(eq_pos+1, line.length()-eq_pos);
				stringstream conv;
				conv << frame_w;
				conv >> w;
				found &= 4;
			}
		}

		if(line.find("frame_h")==0)
		{
			size_t eq_pos = line.find("=");
			if(eq_pos == string::npos)
			{
				cerr << "Sprite file error: Malformed 'frame_h' line" << endl;
			}
			else
			{
				string frame_h = line.substr(eq_pos+1, line.length()-eq_pos);
				stringstream conv;
				conv << frame_h;
				conv >> h;
				found &= 8;
			}
		}

		if(line.find("delay")==0)
		{
			size_t eq_pos = line.find("=");
			if(eq_pos == string::npos)
			{
				cerr << "Sprite file error: Malformed 'delay' line" << endl;
			}
			else
			{
				string frame_delay = line.substr(eq_pos+1, line.length()-eq_pos);
				stringstream conv;
				conv << frame_delay;
				int del = 0;
				conv >> del;
				delays = new Uint32[1];
				delays[0] = del;
				found &= 16;
				const_time = true;
			}
		}
		delete buffer;
	}
}

Sprite::~Sprite()
{
	if(delays)
		delete delays;

	delete sheet;
}

void Sprite::draw(int x, int y)
{
	sheet->draw(x,y,(cframe%(sheet->w()/w))*w,(cframe/(sheet->w()/w))*h,w,h);

	if(Engine::get_ticks()>ltime+delays[0])
	{
		cframe++;
		if(cframe >= frames)
			cframe = 0;

		ltime = Engine::get_ticks();
	}
}


Uint32 Sprite::W()
{
	return w;
}

Uint32 Sprite::H()
{
	return h;
}

Image *Sprite::get_sheet()
{
	return sheet;
}
