#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "piece.hpp"
#include "block.hpp"
#include "game.hpp"

// Each one of the indices indexes a rotation for a block
static int indices[] = { 0,0,0,0, 1,2,1,2, 3,4,3,4, 5,6,5,6, 7,8,9,10, 11,12,13,14, 15,16,17,18 };

// This int array is a bitmap of the pieces
static int pieces[] = { 0xCC, 0x8C4, 0x6C, 0x4444, 0xF0, 0x264, 0xC6, 0xE4, 0x4C4, 0x4E0, 0x464, 0x8E, 0xC88, 0xE2, 0x226, 0x2E, 0x88C, 0xE8, 0x622};

Piece::Piece(int type) : type(type), rotation(0)
{
	piece = new char[16];
	memset(piece, 0, 16);
	set_type(type);
}

Piece::~Piece()
{
	delete[] piece;
}

char *Piece::get_config()
{
	return piece;
}

void Piece::set_rotation(int r)
{
	rotation = r;
	set_type(type);
}

int Piece::get_rotation()
{
	return rotation;
}

void Piece::set_type(int ntype)
{
	type = ntype;

	memset(piece, 0, 16);

	for(int cy = 0; cy < 4; cy++)
	{
		for(int cx = 0; cx < 4; cx++)
		{
			piece[cy * 4 + cx] = test_cell_bit(cx, cy);
		}
	}
}

bool Piece::test_collision(Game *game)
{
	
}

bool Piece::test_cell_bit(int x, int y)
{
	int index = indices[type * 4 + rotation];
	return (pieces[index] & (1 << (y * 4 + x))) > 0;
}

void Piece::draw(int x, int y)
{
	for(int i = 0; i<16; i++)
	{
		if(piece[i] == 1)
		{
			Block::draw(x+(i%4)*32, y+(i/4)*32, type);
		}
	}
}

