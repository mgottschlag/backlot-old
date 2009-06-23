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

#include "entity/Entity.hpp"

#include <iostream>

namespace backlot
{
	Entity::Entity()
	{
	}
	Entity::~Entity()
	{
	}

	bool Entity::create(EntityTemplatePointer tpl, BufferPointer state)
	{
		// Get a copy of the properties and their default values
		properties = tpl->getProperties();
		// Apply state
		applyUpdate(state);
		// Attach properties to this entity
		for (unsigned int i = 0; i < properties.size(); i++)
			properties[i].setEntity(this);
		// Create the script
		script = new Script();
		script->addCoreFunctions();
		script->addServerFunctions();
		// TODO: Other functions
		// Export script variables
		script->setVariable("this", this);
		for (unsigned int i = 0; i < properties.size(); i++)
			script->setVariable(properties[i].getName(), &properties[i]);
		// Run the script code
		if (!script->runString(tpl->getScript()))
		{
			return false;
		}
		this->tpl = tpl;
		changed = false;
		// Call on_loaded()
		if (script->isFunction("on_loaded"))
		{
			script->callFunction("on_loaded");
		}
		return true;
	}
	EntityTemplatePointer Entity::getTemplate()
	{
		return tpl;
	}

	void Entity::getState(BufferPointer buffer)
	{
		const std::vector<Property> &tplproperties = tpl->getProperties();
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			if (properties[i] != tplproperties[i])
			{
				// Bit set: Property changed.
				buffer->writeUnsignedInt(0, 1);
				// Write the property to the stream.
				properties[i].write(buffer);
			}
			else
			{
				// Bit not set: Property remained unchanged.
				buffer->writeUnsignedInt(0, 1);
			}
		}
	}

	void Entity::getUpdate(BufferPointer state, BufferPointer buffer)
	{
	}
	void Entity::applyUpdate(BufferPointer buffer)
	{
	}
	bool Entity::hasChanged(BufferPointer state)
	{
		return changed;
	}

	bool Entity::isVisible(Entity *from)
	{
		return false;
	}

	void Entity::update()
	{
	}

	void Entity::onChange(Property *property)
	{
		changed = true;
		// Callback
		if (script->isFunction("on_changed"))
		{
			script->callFunction("on_changed", property);
		}
	}
}
