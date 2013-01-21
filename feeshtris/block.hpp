#ifndef __BLOCK_HPP__
#define __BLOCK_HPP__

#include <swordfeesh.hpp>

class Block
{
public:
	Block();
	Block(int x, int y, int num);
	~Block();
	void set_pos(int x, int y);
	void draw();
	static void draw(int x, int y, int type);
private:
	int x;
	int y;
	int type;
	static SwordFeesh::Image *block_img;
};

#endif
