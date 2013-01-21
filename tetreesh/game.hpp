#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <swordfeesh.hpp>
#include "piece.hpp"

class Game : public SwordFeesh::Scene
{
public:
	Game();
	~Game();
	bool update(Uint8 *keys);
private:
	SwordFeesh::Image *bg;
	Piece *next;
	int grid[12][18];
};

#endif
