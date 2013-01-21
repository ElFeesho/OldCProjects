#include "piece.hpp"
#include "block.hpp"

Piece::Piece(int type) : type(type)
{
	piece = new int[8];

	set_type(type);
}

Piece::~Piece()
{
	
}

void Piece::shuffle()
{
	for(int i = 0; i<8;i+=2)
	{
		piece[i] ^= piece[i+1];
		piece[i+1] ^= piece[i];
		piece[i] ^= piece[i+1];
	}
}

int *Piece::get_config()
{
	return piece;
}

void Piece::set_type(int ntype)
{
	type = ntype;
	switch(type)
	{
		case 1: // --__ (originally starts upright...)
			piece[0] = 1;
			piece[1] = 0;
			piece[2] = 1;
			piece[3] = 1;
			piece[4] = 0;
			piece[5] = 1;
			piece[6] = 0;
			piece[7] = 2;
			break;
		case 2: // __-- (originally starts upright...)
			piece[0] = 0;
			piece[1] = 0;
			piece[2] = 0;
			piece[3] = 1;
			piece[4] = 1;
			piece[5] = 1;
			piece[6] = 1;
			piece[7] = 2;
			break;
		case 3: // __| (originally starts upright...)
			piece[0] = 0;
			piece[1] = 0;
			piece[2] = 0;
			piece[3] = 1;
			piece[4] = 0;
			piece[5] = 2;
			piece[6] = 1;
			piece[7] = 2;
			break;
		case 4: // --| (originally starts upright...)
			piece[0] = 1;
			piece[1] = 0;
			piece[2] = 1;
			piece[3] = 1;
			piece[4] = 0;
			piece[5] = 2;
			piece[6] = 1;
			piece[7] = 2;
			break;
		case 5: // ||
			piece[0] = 0;
			piece[1] = 0;
			piece[2] = 0;
			piece[3] = 1;
			piece[4] = 1;
			piece[5] = 0;
			piece[6] = 1;
			piece[7] = 1;
			break;
		case 6: // _|_
			piece[0] = 0;
			piece[1] = 1;
			piece[2] = 1;
			piece[3] = 1;
			piece[4] = 1;
			piece[5] = 0;
			piece[6] = 2;
			piece[7] = 1;
			break;
		case 7: // ----
			piece[0] = 0;
			piece[1] = 0;
			piece[2] = 0;
			piece[3] = 1;
			piece[4] = 0;
			piece[5] = 2;
			piece[6] = 0;
			piece[7] = 3;
			break;
	}
}

void Piece::draw(int x, int y)
{
	for(int i = 0; i<8;i+=2)
	{
		Block::draw(x+(piece[i])*32, y+(piece[i+1])*32, type);
	}
}

