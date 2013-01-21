#ifndef __PIECE_HPP__
#define __PIECE_HPP__

class Game;

class Piece
{
public:
	Piece(int type);
	~Piece();
	void draw(int x, int y);
	void set_type(int ntype);
	void set_rotation(int r);

	bool test_collision(Game *game);
	bool test_cell_bit(int x, int y);

	int get_rotation();
	char *get_config();
private:
	char *piece; // Mapping data
	int offset[2];
	int type;
	int rotation;
};

#endif
