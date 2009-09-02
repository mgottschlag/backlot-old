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

#include "Game.hpp"
#include "Preferences.hpp"
#include "ReferenceCounted.hpp"
#include "entity/Property.hpp"
#include "Vector2.hpp"
#include "Rectangle.hpp"
#include "Timer.hpp"
#include "PathFinder.hpp"

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
			// PathFinder
			luabind::class_<PathFinder, ReferenceCounted, SharedPointer<PathFinder> >("PathFinder")
				.def(luabind::constructor<MapPointer>())
				.def("initialize", &PathFinder::initialize)
				.def("getNextWaypoint", &PathFinder::getNextWaypoint)
				.def("getStatus", &PathFinder::getStatus)
				.enum_("Status")
				[
					luabind::value("Inactive", EPFS_Inactive),
					luabind::value("Waiting", EPFS_Waiting),
					luabind::value("Ready", EPFS_Ready),
					luabind::value("Done", EPFS_Done),
					luabind::value("Error", EPFS_Error)
				],
			// Map
			luabind::class_<Map, ReferenceCounted, SharedPointer<Map> >("Map")
				.def("getSize", &Map::getSize)
				.def("getHeight", &Map::getHeight)
				.def("getMaximumHeight", &Map::getMaximumHeight)
				.def("getMinimumHeight", &Map::getMinimumHeight),
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
}
