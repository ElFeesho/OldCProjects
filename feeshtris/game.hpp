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
	int *get_grid();

	void draw_grid();
private:
	Piece *next;
	int grid[12][18];
};

#endif
