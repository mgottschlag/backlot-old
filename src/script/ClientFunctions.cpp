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
#include "Game.hpp"
#include "menu/Menu.hpp"
#include "menu/Dialog.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "entity/Entity.hpp"
#include "graphics/Graphics.hpp"

#include <guichan/widgets/checkbox.hpp>
#include <guichan/widgets/textfield.hpp>
#include <guichan/widgets/dropdown.hpp>

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
}
