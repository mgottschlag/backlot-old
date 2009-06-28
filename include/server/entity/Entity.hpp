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

#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

#include "ReferenceCounted.hpp"
#include "Buffer.hpp"
#include "Script.hpp"
#include "entity/EntityTemplate.hpp"

#include <vector>

namespace backlot
{
	class Entity : public ReferenceCounted
	{
		public:
			Entity();
			~Entity();

			bool create(EntityTemplatePointer tpl, BufferPointer state = 0);
			EntityTemplatePointer getTemplate();

			void getState(BufferPointer buffer);

			void saveState();
			void getUpdate(int time, BufferPointer buffer);
			void applyUpdate(BufferPointer buffer);
			bool hasChanged(int time);

			void setOwner(int owner);
			int getOwner();

			void setPosition(const Vector2F &position);
			Vector2F getPosition();
			void setSpeed(const Vector2F &speed, bool ignoreobstacles);
			Vector2F getSpeed();

			bool isVisible(Entity *from);

			void update();

			ScriptPointer getScript();

			void onChange(Property *property);
		private:
			EntityTemplatePointer tpl;

			ScriptPointer script;
			std::vector<Property> properties;
			Property *positionproperty;
			Vector2F speed;

			bool changed;

			int owner;
	};

	typedef SharedPointer<Entity> EntityPointer;
}

#endif
