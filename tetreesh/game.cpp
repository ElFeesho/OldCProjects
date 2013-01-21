#include "game.hpp"
#include "block.hpp"

using namespace SwordFeesh;

Game::Game()
{
	bg = new Image("images/game.png");
	for(int i = 0; i<12;i++)
		for(int j = 0; j<18;j++)
			grid[i][j] = 1;

	next = new Piece(1);
}

Game::~Game()
{

}

bool Game::update(Uint8 *keys)
{
	bg->draw();
	next->draw(50,80);
	if(keys[SDLK_RIGHT])
	{
		next->set_type(7);
	}
	if(keys[SDLK_LEFT])
	{
		next->set_type(6);
	}
	if(keys[SDLK_SPACE])
	{
		keys[SDLK_SPACE]= 0;
		next->shuffle();
	}
	int type = 1;
	for(int i = 0; i<12;i++)
	{
		for(int j = 0; j<18;j++)
		{
			Block::draw(208+i*32,10+j*32,type++);
			if(type>7) type = 1;
		}
	}

	return true;
}

