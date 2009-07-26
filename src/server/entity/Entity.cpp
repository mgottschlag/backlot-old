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
#include "Server.hpp"

#include <iostream>

namespace backlot
{
	Entity::Entity() : ReferenceCounted()
	{
		owner = 0;
		positionproperty = 0;
		id = 0;
	}
	Entity::~Entity()
	{
	}

	bool Entity::create(EntityTemplatePointer tpl, BufferPointer state)
	{
		// Get a copy of the properties and their default values
		properties = tpl->getProperties();
		// Apply state
		setState(state);
		// Attach properties to this entity
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			properties[i].setEntity(this);
			if (properties[i].getName() == "position")
				positionproperty = &properties[i];
		}
		// Create the script
		script = new Script();
		script->addCoreFunctions();
		script->addServerFunctions();
		// TODO: Other functions
		// Export script variables
		script->setVariable("this", EntityPointer(this));
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
				buffer->writeUnsignedInt(1, 1);
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
	void Entity::setState(BufferPointer buffer)
	{
		if (buffer.isNull())
			return;
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			int changed = buffer->readUnsignedInt(1);
			if (changed)
			{
				properties[i].read(buffer);
			}
		}
	}

	void Entity::getUpdate(int time, BufferPointer buffer, int client)
	{
		bool local = client == getOwner();
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			if (local && ((properties[i].getFlags() & EPF_OwnerUpdates) == 0))
			{
				// Potential update ignored because of property flags.
				buffer->writeUnsignedInt(0, 1);
				continue;
			}
			if (properties[i].getChangeTime() > time)
			{
				// Bit set: Property changed.
				buffer->writeUnsignedInt(1, 1);
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
	void Entity::applyUpdate(BufferPointer buffer)
	{
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			int changed = buffer->readUnsignedInt(1);
			if (changed && (properties[i].getFlags() & EPF_Unlocked))
			{
				properties[i].read(buffer);
			}
		}
	}
	bool Entity::hasChanged(int time)
	{
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			if (properties[i].getChangeTime() > time)
			{
				return true;
			}
		}
		return false;
	}

	void Entity::setOwner(int owner)
	{
		this->owner = owner;
	}
	int Entity::getOwner()
	{
		return owner;
	}

	void Entity::setID(int id)
	{
		this->id = id;
	}
	int Entity::getID()
	{
		return id;
	}

	bool Entity::isMovable()
	{
		return positionproperty;
	}
	void Entity::setPosition(const Vector2F &position)
	{
		if (positionproperty)
			positionproperty->setVector2F(position);
	}
	Vector2F Entity::getPosition()
	{
		if (positionproperty)
			return positionproperty->getVector2F();
		return Vector2F();
	}
	void Entity::setSpeed(const Vector2F &speed, bool ignoreobstacles)
	{
		this->speed = speed;
	}
	Vector2F Entity::getSpeed()
	{
		return speed;
	}
	RectangleF Entity::getRectangle()
	{
		return RectangleF(getPosition() - tpl->getOrigin(), tpl->getSize());
	}

	Property *Entity::getProperty(std::string name)
	{
		// TODO: This is slow.
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			if (properties[i].getName() == name)
				return &properties[i];
		}
		return 0;
	}

	bool Entity::isVisible(Entity *from)
	{
		return true;
	}

	void Entity::update()
	{
		// Move entity
		if (positionproperty && speed != Vector2F(0, 0))
		{
			Vector2F position = positionproperty->getVector2F();
			MapPointer map = Server::get().getMap();
			float currentheight = map->getHeight(position);
			position += speed / 50;
			RectangleF area(position.x - 0.35, position.y - 0.35, 0.7, 0.7);
			float maxheight = map->getMaximumHeight(area);
			if (maxheight <= currentheight + 0.5)
			{
				positionproperty->setVector2F(position);
			}
		}
		// Call frame callback
		if (script->isFunction("on_update"))
			script->callFunction("on_update");
	}

	ScriptPointer Entity::getScript()
	{
		return script;
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
