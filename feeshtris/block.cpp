#include <swordfeesh.hpp>
#include "block.hpp"

using namespace SwordFeesh;

Block::Block()
{

}

Block::Block(int x, int y, int num):x(x), y(y), type(num)
{
	block_img = new Image("images/block.png");
}

Block::~Block()
{

}

void Block::set_pos(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Block::draw()
{
	switch(type)
	{
		case 1: // --__
			block_img->set_tint(1.0,0,0);
			block_img->draw(x, y);
			break;
		case 2: // _|_
			block_img->set_tint(0.0,1.0,0);
			block_img->draw(x, y);
			break;
		case 3: // __--
			block_img->set_tint(0.0,0,1.0);
			block_img->draw(x, y);
			break;
		case 4: // __--
			block_img->set_tint(1.0,1.0,0);
			block_img->draw(x, y);
			break;
		case 5: // |__
			block_img->set_tint(0.0,1.0,1.0);
			block_img->draw(x, y);
			break;
		case 6: // __|
			block_img->set_tint(1.0,0,1.0);
			block_img->draw(x, y);
			break;
		case 7: // ||
			block_img->set_tint(1.0,0.5,0);
			block_img->draw(x, y);
			break;
	}
}

void Block::draw(int x, int y, int type)
{
	if(!block_img)
		block_img = new Image("images/block.png");
	switch(type)
	{
		case 1: // --__
			block_img->set_tint(1.0,0,0);
			block_img->draw(x, y);
			break;
		case 2: // _|_
			block_img->set_tint(0.0,1.0,0);
			block_img->draw(x, y);
			break;
		case 3: // __--
			block_img->set_tint(0.0,0,1.0);
			block_img->draw(x, y);
			break;
		case 4: // __--
			block_img->set_tint(1.0,1.0,0);
			block_img->draw(x, y);
			break;
		case 5: // |__
			block_img->set_tint(0.0,1.0,1.0);
			block_img->draw(x, y);
			break;
		case 6: // __|
			block_img->set_tint(1.0,0,1.0);
			block_img->draw(x, y);
			break;
		case 7: // ||
			block_img->set_tint(1.0,0.5,0);
			block_img->draw(x, y);
			break;
	}
}

Image *Block::block_img = NULL;

