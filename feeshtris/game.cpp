#include "game.hpp"
#include "menu.hpp"
#include "block.hpp"

using namespace SwordFeesh;

Game::Game()
{
	for(int i = 0; i<12;i++)
		for(int j = 0; j<18;j++)
			grid[i][j] = 0;

	next = new Piece(1);
}

Game::~Game()
{

}

bool Game::update(Uint8 *keys)
{
	static int type = 1;
	static int w = 0, h = 0;
	
	if(!w||!h)
	{
		Engine::get_inst()->get_viewport(&w, &h);
	}

	static ParticleFountain *pfountain[3] = { NULL };
	if(pfountain[0] == NULL)
	{
		pfountain[0] = new ParticleFountain("images/particle.png", 400, 1000, 150,150, 1.0f, 0.0f, 0.0f);
		pfountain[1] = new ParticleFountain("images/particle.png", 400, 1000, 490, 150, 0.0f, 0.0f, 1.0f);
		pfountain[2] = new ParticleFountain("images/particle.png", 400, 1000, 320, 330, 0.0f, 1.0f, 0.0);
	}

	glLoadIdentity();
   glBegin(GL_QUADS);
      glColor3f(0,0.8,1);
      glVertex2i(0,0);
      glVertex2i(w,0);
      glColor3f(0,0.5,1);
      glVertex2i(w,h);
      glVertex2i(0,h);
   glEnd();

	draw_grid();

	next->draw(50,120);

//	pfountain[0]->update();
//	pfountain[1]->update();
//	pfountain[2]->update();

	if(keys[SDLK_a])
	{
		pfountain[0]->X(pfountain[0]->X()-1.5);
	}
	if(keys[SDLK_d])
	{
		pfountain[0]->X(pfountain[0]->X()+1.5);
	}

	if(keys[SDLK_w])
	{
		pfountain[0]->Y(pfountain[0]->Y()-1.5);
	}
	if(keys[SDLK_s])
	{
		pfountain[0]->Y(pfountain[0]->Y()+1.5);
	}

	if(keys[SDLK_RIGHT])
	{
		keys[SDLK_RIGHT]=0;
		if(type<7)
		{
			type++;
		}
		next->set_type(type);
		
	}
	if(keys[SDLK_LEFT])
	{
		keys[SDLK_LEFT] = 0;
		if(type>1)
		{ 
			type--;
		}
		next->set_type(type);
	}
	if(keys[SDLK_SPACE])
	{
		keys[SDLK_SPACE]= 0;
		next->set_rotation((next->get_rotation()+1)%4);
	}

	if(keys[SDLK_ESCAPE])
	{
		keys[SDLK_ESCAPE] = 0;
		Engine::get_inst()->set_scene(new FadeTransition(new Game(*this), new Menu(),500));
	}

	return true;
}


int *Game::get_grid()
{
	return &grid[0][0];
}

void Game::draw_grid()
{
	for(int x = 0; x < 12; x++)
	{
		for(int y = 0; y < 18; y++)
		{
			Block::draw(200+x*32, 120+y*32, grid[x][y]);
		}
	}
}
