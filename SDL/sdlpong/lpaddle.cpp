#include "lpaddle.hpp"
#include "engine.hpp"
#include <iostream>

#include <lua.hpp>


lpaddle::lpaddle(void *eng, const char *id, int player, double x, double y) : Paddle(id, player, x, y), engine(eng)
{
	l = lua_open();
	std::cout << "Openned lua libs" << std::endl;

	lua_newtable(l);
	lua_setglobal(l,"ball");
	lua_getglobal(l,"ball");
	lua_pushnumber(l,0.0);
	lua_setfield(l,-2,"x");
	lua_pushnumber(l,0.0);
	lua_setfield(l,-2,"y");
	lua_pushnumber(l,20.0);
	lua_setfield(l,-2,"w");
	lua_pushnumber(l,20.0);
	lua_setfield(l,-2,"h");
	std::cout << "Made ball table" << std::endl;

	lua_newtable(l);
	lua_setglobal(l,"paddle");
	lua_getglobal(l,"paddle");
	lua_pushnumber(l,x);
	lua_setfield(l,-2,"x");
	lua_pushnumber(l,y);
	lua_setfield(l,-2,"y");
	lua_pushnumber(l,20);
	lua_setfield(l,-2,"w");
	lua_pushnumber(l,70);
	lua_setfield(l,-2,"h");

	std::cout << "Made paddle table" << std::endl;

	int file_stat = luaL_loadfile(l, "ai.lua");
	if(file_stat)
	{
		lua_close(l);
		return;
	}
	std::cout << "Running ai file" << std::endl;
	lua_pcall(l,0,0,0);
	std::cout << "Ran AI file" << std::endl;
	lua_getglobal(l,"paddle");
	lua_pushnumber(l,x);
	lua_setfield(l,-2,"x");
	lua_pushnumber(l,y);
	lua_setfield(l,-2,"y");

	lua_getglobal(l,"ball");
	lua_pushnumber(l,0.0);
	lua_setfield(l,-2,"x");
	lua_pushnumber(l,0.0);
	lua_setfield(l,-2,"y");
	lua_pushnumber(l,20.0);
	lua_setfield(l,-2,"w");
	lua_pushnumber(l,20.0);
	lua_setfield(l,-2,"h");

}

lpaddle::~lpaddle()
{
	lua_close(l);
}

bool lpaddle::update(int keys)
{
	Engine *g_eng = (Engine*)engine;
	Ent *ball = g_eng->get_ent_by_name("ball");
	if(ball == NULL)
	{
		return true;
	}
	lua_getglobal(l,"ball");
	lua_pushnumber(l,ball->get_x());
	lua_setfield(l,-2,"x");
	lua_pushnumber(l,ball->get_y());
	lua_setfield(l,-2,"y");
	lua_pushnumber(l,ball->get_w());
	lua_setfield(l,-2,"w");
	lua_pushnumber(l,ball->get_h());
	lua_setfield(l,-2,"h");
	
	lua_getglobal(l,"paddle");
	lua_pushnumber(l,70);
	lua_setfield(l,-2,"h");
		
	lua_getglobal(l,"do_update");
	lua_pcall(l,0,0,0);
	
	lua_getglobal(l,"paddle");
	lua_getfield(l,-1,"y");
	set_y(lua_tonumber(l,-1));
	return true;
}

