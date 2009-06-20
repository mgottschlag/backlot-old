/*
Copyright (C) 2009  Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Script.hpp"
#include "Engine.hpp"
#ifdef CLIENT
#include "Dialog.hpp"
#endif
#ifdef SERVER
#include "Game.hpp"
#endif

#include <iostream>
extern "C"
{
	#include "lualib.h"
	#include "lauxlib.h"
}
#include <luabind/luabind.hpp>

namespace backlot
{
	template<class T> T *get_pointer(const SharedPointer<T> &p)
	{
		return p.get();
	}

	template<class A> SharedPointer<A> *get_const_holder(SharedPointer<A>*)
	{
		return 0;
	}

	Script::Script() : ReferenceCounted()
	{
		state = lua_open();
		luaopen_base(state);
		luaopen_string(state);
		luaopen_math(state);
		luabind::open(state);
	}
	Script::~Script()
	{
		lua_close(state);
	}

	bool Script::runString(std::string data)
	{
		int error = luaL_loadbuffer(state, data.c_str(), data.size(), "runString");
		if (!error)
		{
			error = lua_pcall(state, 0, LUA_MULTRET, 0);
		}
		if (error)
		{
			std::cerr << "Error while executing string:" << std::endl;
			std::cerr <<  lua_tostring(state, -1) << std::endl;
			std::cerr << "Code: \"" << data << "\"" << std::endl;
			return false;
		}
		return true;
	}

	bool Script::isFunction(std::string name)
	{
		lua_getglobal(state, name.c_str());
		bool exists = lua_isfunction(state, -1);
		lua_pop(state, 1);
		return exists;
	}

	void Script::callFunction(std::string name)
	{
		lua_getglobal(state, name.c_str());
		lua_call(state, 0, 0);
	}

	void Script::push(int value)
	{
		lua_pushinteger(state, value);
	}
	void Script::push(float value)
	{
		lua_pushnumber(state, value);
	}
	void Script::push(double value)
	{
		lua_pushnumber(state, value);
	}
	void Script::push(std::string value)
	{
		lua_pushstring(state, value.c_str());
	}

	void Script::pop(int &value)
	{
		value = (int)lua_tointeger(state, -1);
		lua_pop(state, 1);
	}
	void Script::pop(float &value)
	{
		value = (float)lua_tonumber(state, -1);
		lua_pop(state, 1);
	}
	void Script::pop(double &value)
	{
		value = (double)lua_tonumber(state, -1);
		lua_pop(state, 1);
	}
	void Script::pop(std::string &value)
	{
		// TODO: Pop string.
	}

	static int core_exit(lua_State *state)
	{
		Engine::get().stop();
		return 0;
	}
	void Script::addCoreFunctions()
	{
		// Create namespace
		lua_newtable(state);
		int space = lua_gettop(state);
		// Push functions
		lua_pushliteral(state, "exit");
		lua_pushcfunction(state, core_exit);
		lua_settable(state, space);
		// Done
		lua_setglobal(state, "core");
	}
	#ifdef CLIENT
	static int menu_show_dialog(lua_State *state)
	{
		const char *name = luaL_checkstring(state, 1);
		if (!name)
		{
			std::cerr << "Wrong use of \"show_dialog(name)\"" << std::endl;
			return 0;
		}
		std::cout << "Showing dialog " << name << std::endl;
		DialogPointer dialog = Dialog::get(name);
		if (dialog)
			dialog->setVisible(true);
		return 0;
	}
	static int menu_hide_dialog(lua_State *state)
	{
		const char *name = luaL_checkstring(state, 1);
		if (!name)
		{
			std::cerr << "Wrong use of \"hide_dialog(name)\"" << std::endl;
			return 0;
		}
		std::cout << "Hiding dialog " << name << std::endl;
		DialogPointer dialog = Dialog::get(name);
		if (dialog)
			dialog->setVisible(false);
		return 0;
	}
	void Script::addMenuFunctions()
	{
		// Create namespace
		lua_newtable(state);
		int space = lua_gettop(state);
		// Push functions
		lua_pushliteral(state, "show_dialog");
		lua_pushcfunction(state, menu_show_dialog);
		lua_settable(state, space);
		lua_pushliteral(state, "hide_dialog");
		lua_pushcfunction(state, menu_hide_dialog);
		lua_settable(state, space);
		// Done
		lua_setglobal(state, "menu");
	}
	#endif
	#ifdef SERVER
	void Script::addServerFunctions()
	{
		luabind::module(state)
		[
			// Game class
			luabind::class_<Game>("Game")
				.scope
				[
					luabind::def("get", &Game::get)
				]
				.def("update", &Game::update)
				.def("addEntity", &Game::addEntity)
				.def("removeEntity", &Game::removeEntity)
		];
		luabind::module(state)
		[
			// Entitiy class
			luabind::class_<Entity, EntityPointer>("Entity")
		];
	}
	#endif
}
