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

#include "Script.hpp"
#include "Engine.hpp"
#include "entity/Property.hpp"
#include "Game.hpp"
#include "Preferences.hpp"
#include "Timer.hpp"
#ifdef CLIENT
#include "menu/Menu.hpp"
#include "menu/Dialog.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Graphics.hpp"
#include <guichan/widgets/checkbox.hpp>
#include <guichan/widgets/textfield.hpp>
#include <guichan/widgets/dropdown.hpp>
#endif
#ifdef SERVER
#include "Server.hpp"
#include "entity/EntityState.hpp"
#endif

#include <iostream>
extern "C"
{
	#include "lualib.h"
	#include "lauxlib.h"
}
#include <luabind/operator.hpp>

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

	void Script::addCoreFunctions()
	{
		luabind::module(state)
		[
			// ReferenceCounted class
			luabind::class_<ReferenceCounted, SharedPointer<ReferenceCounted> >("ReferenceCounted")
				.def(luabind::constructor<>()),
			// Vector2F
			luabind::class_<Vector2F>("Vector2F")
				.def(luabind::constructor<>())
				.def(luabind::constructor<float, float>())
				.def(luabind::constructor<const char *>())
				.def(luabind::constructor<const Vector2F &>())
				.def(luabind::constructor<const Vector2I &>())
				.def("set", &Vector2F::set)
				.def("getLengthSquared", &Vector2F::getLengthSquared)
				.def("getLength", &Vector2F::getLength)
				.def("getRotation", &Vector2F::getRotation)
				.def("rotate", &Vector2F::rotate)
				.def(luabind::self * float())
				.def(luabind::self + Vector2F())
				.def(luabind::self - Vector2F())
				.def(luabind::self + Vector2I())
				.def(luabind::self - Vector2I())
				.def(luabind::self == Vector2F())
				.def(luabind::self == Vector2I())
				.def_readwrite("x", &Vector2F::x)
				.def_readwrite("y", &Vector2F::y),
			// Vector2I
			luabind::class_<Vector2I>("Vector2I")
				.def(luabind::constructor<>())
				.def(luabind::constructor<float, float>())
				.def(luabind::constructor<const char *>())
				.def(luabind::constructor<const Vector2F &>())
				.def(luabind::constructor<const Vector2I &>())
				.def("set", &Vector2I::set)
				.def("getLengthSquared", &Vector2I::getLengthSquared)
				.def("getLength", &Vector2I::getLength)
				.def("getRotation", &Vector2I::getRotation)
				.def("rotate", &Vector2I::rotate)
				.def(luabind::self * float())
				.def(luabind::self + Vector2F())
				.def(luabind::self - Vector2F())
				.def(luabind::self + Vector2I())
				.def(luabind::self - Vector2I())
				.def(luabind::self == Vector2F())
				.def(luabind::self == Vector2I())
				.def_readwrite("x", &Vector2I::x)
				.def_readwrite("y", &Vector2I::y),
			// RectangleF
			luabind::class_<RectangleF>("RectangleF")
				.def(luabind::constructor<>())
				.def(luabind::constructor<const char *>())
				.def(luabind::constructor<float, float, float, float>())
				.def(luabind::constructor<const Vector2F &, const Vector2F &>())
				.def("contains", &RectangleF::contains)
				.def("overlapsWith", &RectangleF::overlapsWith)
				.def("insertPoint", &RectangleF::insertPoint)
				.def("insertRectangle", &RectangleF::insertRectangle)
				.def_readwrite("x", &RectangleF::x)
				.def_readwrite("y", &RectangleF::y)
				.def_readwrite("width", &RectangleF::width)
				.def_readwrite("height", &RectangleF::height),
			// Buffer
			luabind::class_<Buffer, ReferenceCounted, SharedPointer<Buffer> >("Buffer"),
			// Property class
			luabind::class_<Property>("Property")
				.def("getName", &Property::getName)
				.def("getType", &Property::getType)
				.def("getFlags", &Property::getFlags)
				.def("getSize", &Property::getSize)
				.def("setInt", &Property::setInt)
				.def("getInt", &Property::getInt)
				.def("setUnsignedInt", &Property::setUnsignedInt)
				.def("getUnsignedInt", &Property::getUnsignedInt)
				.def("setFloat", &Property::setFloat)
				.def("getFloat", &Property::getFloat)
				.def("setVector2F", &Property::setVector2F)
				.def("getVector2F", &Property::getVector2F)
				.def("setVector2I", &Property::setVector2I)
				.def("getVector2I", &Property::getVector2I)
				.def("setBool", &Property::setBool)
				.def("getBool", &Property::getBool)
				.def("bit", (bool (Property::*)(int) const)&Property::bit)
				.def("bit", (void (Property::*)(int, int))&Property::bit)
				.def("set", &Property::set)
				.def("write", &Property::write)
				.def("read", &Property::read)
				.def("getChangeTime", &Property::getChangeTime),
			// EntityTemplate
			luabind::class_<EntityTemplate, ReferenceCounted, SharedPointer<EntityTemplate> >("EntityTemplate")
				.scope
				[
					luabind::def("get", &EntityTemplate::get)
				],
			luabind::class_<EntityList, ReferenceCounted, SharedPointer<EntityList> >("EntityList")
				.def(luabind::constructor<>())
				.def("addEntity", &EntityList::addEntity)
				.def("removeEntity", &EntityList::removeEntity)
				.def("getSize", &EntityList::getSize)
				.def("getEntity", &EntityList::getEntity),
			// CollisionInfo
			luabind::class_<CollisionInfo>("CollisionInfo")
				.def(luabind::constructor<>())
				.def_readwrite("collision", &CollisionInfo::collision)
				.def_readwrite("entitycollision", &CollisionInfo::entitycollision)
				.def_readwrite("entity", &CollisionInfo::entity)
				.def_readwrite("point", &CollisionInfo::point),
			// Script
			luabind::class_<Script, ReferenceCounted, SharedPointer<Script> >("Script")
				.def("isFunction", &Script::isFunction)
				.def("callFunction", (void (Script::*)(std::string))&Script::callFunction)
				.def("callFunction", (void (Script::*)(std::string, int))&Script::callFunction<int>)
				.def("callFunction", (void (Script::*)(std::string, int, int))&Script::callFunction<int, int>)
				.def("callFunction", (void (Script::*)(std::string, Vector2F))&Script::callFunction<Vector2F>)
				.def("callFunction", (void (Script::*)(std::string, Vector2I))&Script::callFunction<Vector2I>)
				.def("callFunctionInt", (int (Script::*)(std::string))&Script::callFunction<int>),
			// Timer
			luabind::class_<Timer, ReferenceCounted, SharedPointer<Timer> >("Timer")
				.def(luabind::constructor<>())
				.def("setInterval", &Timer::setInterval)
				.def("setPeriodic", &Timer::setPeriodic)
				.def("setTime", &Timer::setTime)
				.def("getTime", &Timer::getTime)
				.def("start", &Timer::start)
				.def("stop", &Timer::stop)
				.def("reset", &Timer::reset)
				.def("setCallback", &Timer::setCallback),
			// Preferences class
			luabind::class_<Preferences>("Preferences")
				.scope
				[
					luabind::def("get", &Preferences::get)
				]
				.def("getResolution", &Preferences::getResolution)
				.def("setResolution", &Preferences::setResolution)
				.def("getXResolution", &Preferences::getXResolution)
				.def("setXResolution", &Preferences::setXResolution)
				.def("getYResolution", &Preferences::getYResolution)
				.def("setYResolution", &Preferences::setYResolution)
				.def("getFullscreen", &Preferences::getFullscreen)
				.def("setFullscreen", &Preferences::setFullscreen)
				.def("getColorDepth", &Preferences::getColorDepth)
				.def("setColorDepth", &Preferences::setColorDepth)
				.def("getStereo", &Preferences::getStereo)
				.def("setStereo", &Preferences::setStereo)
				.def("getFrequency", &Preferences::getFrequency)
				.def("setFrequency", &Preferences::setFrequency)
		];
	}
	#ifdef CLIENT
	void Script::addDialogFunctions()
	{
		luabind::module(state)
		[
			// Export guichan Widgets
			// Textfield
			luabind::class_<gcn::TextField>("TextField")
				.def("getText", &gcn::TextField::getText)
				.def("setText", &gcn::TextField::setText),
			// CheckBox
			luabind::class_<gcn::CheckBox>("CheckBox")
				.def("isSelected", &gcn::CheckBox::isSelected)
				.def("setSelected",  &gcn::CheckBox::setSelected),
			// Drop down menus
			luabind::class_<gcn::DropDown>("DropDown")
				.def("getSelected", &gcn::DropDown::getSelected)
				.def("setSelected", &gcn::DropDown::setSelected)
		];
	}
	void Script::addClientFunctions()
	{
		luabind::module(state)
		[
			// Entity class
			luabind::class_<Entity, ReferenceCounted, SharedPointer<Entity> >("Entity")
				.def("isLocal", &Entity::isLocal)
				.def("setPosition", &Entity::setPosition)
				.def("getPosition", &Entity::getPosition)
				.def("setSpeed", &Entity::setSpeed)
				.def("getSpeed", &Entity::getSpeed)
				.def("getProperty", &Entity::getProperty)
				.def("getScript", &Entity::getScript)
				.def("getRectangle", &Entity::getRectangle)
				.def("getID", &Entity::getID),
			// EntityImage
			luabind::class_<EntityImage, ReferenceCounted, SharedPointer<EntityImage> >("EntityImage")
				.def("load", &EntityImage::load)
				.def("setTexture", &EntityImage::setTexture)
				.def("setPosition", (void (EntityImage::*)(Vector2F))&EntityImage::setPosition)
				.def("setPosition", (void (EntityImage::*)(float, float))&EntityImage::setPosition)
				.def("getPosition", &EntityImage::getPosition)
				.def("setRotation", &EntityImage::setRotation)
				.def("getRotation", &EntityImage::getRotation)
				.def("setSize", &EntityImage::setSize)
				.def("getSize", &EntityImage::getSize)
				.def("setDepth", &EntityImage::setDepth)
				.def("getDepth", &EntityImage::getDepth)
				.def("setVisible", &EntityImage::setVisible)
				.def("isVisible", &EntityImage::isVisible)
				.def("rotate", (void (EntityImage::*)(float, Vector2F))&EntityImage::rotate)
				.def("rotate", (void (EntityImage::*)(float, float, float))&EntityImage::rotate),
			// Texture
			luabind::class_<Texture, ReferenceCounted, SharedPointer<Texture> >("Texture")
				.scope
				[
					luabind::def("get", (TexturePointer (*)(int))&Texture::get),
					luabind::def("get", (TexturePointer (*)(std::string, bool))&Texture::get)
				]
				.def("getID", &Texture::getID),
			// Animation
			luabind::class_<Animation, ReferenceCounted, SharedPointer<Animation> >("Animation")
				.def("start", &Animation::start)
				.def("stop", &Animation::stop)
				.def("isPlaying", &Animation::isPlaying),
			// Camera
			luabind::class_<Camera, ReferenceCounted, SharedPointer<Camera> >("Camera")
				.def("getPosition", &Camera::getPosition)
				.def("setPosition", &Camera::setPosition),
			// Graphics
			luabind::class_<Graphics>("Graphics")
				.scope
				[
					luabind::def("get", &Graphics::get)
				]
				.def("getCamera", &Graphics::getCamera),
			// Menu
			luabind::class_<Menu, ReferenceCounted, SharedPointer<Menu> >("Menu")
				.scope
				[
					luabind::def("get", &Menu::get),
					luabind::def("getActiveMenu", &Menu::getActiveMenu)
				]
				.def("setActive", &Menu::setActive)
				.def("isActive", &Menu::isActive)
				.def("getActiveMenu", &Menu::getActiveMenu),
			// Dialog
			luabind::class_<Dialog, ReferenceCounted, SharedPointer<Dialog> >("Dialog")
				.scope
				[
					luabind::def("get", &Dialog::get)
				]
				.def("setVisible", &Dialog::setVisible)
				.def("isVisible", &Dialog::isVisible),
			// Server class
			luabind::class_<Server>("ServerControl")
				.scope
				[
					luabind::def("get", &Server::get)
				]
				.def("init", &Server::init)
				.def("destroy", &Server::destroy),
			// Client class
			luabind::class_<Client>("Client")
				.scope
				[
					luabind::def("get", &Client::get)
				]
				.def("init", &Client::init)
				.def("destroy", &Client::destroy)
				.def("getMap", &Client::getMap),
			// Game class
			luabind::class_<Game>("Game")
				.scope
				[
					luabind::def("get", &Game::get)
				]
				.def("setInputTarget", &Game::setInputTarget)
				.def("getEntity", &Game::getEntity)
				.def("getCollision", &Game::getCollision)
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
	#endif
	#ifdef SERVER
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
	#endif
}
