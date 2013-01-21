#ifndef __LPADDLE_HPP__
#define __LPADDLE_HPP__

#include <lua.hpp>

#include "paddle.hpp"

class lpaddle : public Paddle
{
public:
	lpaddle(void *eng, const char *id, int player, double x, double y);
	~lpaddle();
	bool update(int keys);
private:
	struct lua_State *l;
	void *engine;
};	

#endif
