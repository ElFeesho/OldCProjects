#ifndef __PIECE_HPP__
#define __PIECE_HPP__

class Piece
{
public:
	Piece(int type);
	~Piece();
	void draw(int x, int y);
	void shuffle();
	void set_type(int ntype);
	int *get_config();
private:
	int *piece; // Mapping data
	int type;
};

#endif
