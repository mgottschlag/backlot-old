/*
Copyright (C) 2009  Mathias Gottschlag, Simon Kerler

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

#include "Engine.hpp"
#include "Game.hpp"
#include "Server.hpp"
#include "entity/EntityState.hpp"

extern "C"
{
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace backlot
{
	template<class T> T *get_pointer(const SharedPointer<T> &p)
	{
		return p.get();
	}

	template<class A> SharedPointer<A const> *get_const_holder(SharedPointer<A>*)
	{
		return 0;
	}
	
	void Script::addServerFunctions()
	{
		luabind::module(state)
		[
			// Entity
			luabind::class_<Entity, ReferenceCounted, SharedPointer<Entity> >("Entity")
				.def("setPosition", &Entity::setPosition)
				.def("getPosition", &Entity::getPosition)
				.def("setSpeed", &Entity::setSpeed)
				.def("getSpeed", &Entity::getSpeed)
				.def("getProperty", &Entity::getProperty)
				.def("getScript", &Entity::getScript)
				.def("getRectangle", &Entity::getRectangle)
				.def("getID", &Entity::getID),
			// EntityState
			luabind::class_<EntityState, ReferenceCounted, SharedPointer<EntityState> >("EntityState")
				.scope
				[
					luabind::def("create", &EntityState::create)
				]
				.def("getProperty", &EntityState::getProperty)
				.def("get", &EntityState::get),
			// Server
			luabind::class_<Server>("Server")
				.scope
				[
					luabind::def("get", &Server::get)
				]
				.def("getMap", &Server::getMap),
			// Game
			luabind::class_<Game>("Game")
				.scope
				[
					luabind::def("get", &Game::get)
				]
				.def("update", &Game::update)
				.def("addEntity", &Game::addEntity)
				.def("addEntity", &Game::addEntityWithState)
				.def("removeEntity", &Game::removeEntity)
				.def("getEntity", &Game::getEntity)
				.def("getCollision", &Game::getCollision)
				.def("registerForDeletion", &Game::registerForDeletion)
				.def("getEntities", (EntityListPointer (Game::*)(std::string))&Game::getEntities)
				.def("getEntities", (EntityListPointer (Game::*)(RectangleF, std::string))&Game::getEntities)
				.def("getEntities", (EntityListPointer (Game::*)(RectangleF))&Game::getEntities),
			// Engine
			luabind::class_<Engine>("Engine")
				.scope
				[
					luabind::def("get", &Engine::get)
				]
				.def("stop", &Engine::stop)
		];
	}
}
