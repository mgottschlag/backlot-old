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
#include "Game.hpp"
#include "Engine.hpp"

#include <iostream>

namespace backlot
{
	Entity::Entity() : ReferenceCounted()
	{
		owner = 0;
		active = true;
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
		// Create images
		const std::vector<EntityImageInfo> &imageinfo = tpl->getImages();
		for (unsigned int i = 0; i < imageinfo.size(); i++)
		{
			// Image
			EntityImagePointer newimage = new EntityImage(this);
			newimage->setTexture(imageinfo[i].texture);
			newimage->setPosition(imageinfo[i].position);
			newimage->setSize(imageinfo[i].size);
			newimage->setDepth(imageinfo[i].depth);
			// TODO: Rotate with entity?
			images.push_back(newimage);
			// Animation
			if (!imageinfo[i].animation)
				continue;
			AnimationPointer animation = new Animation();
			animation->setFrameNumbers(imageinfo[i].animationsize.x,
				imageinfo[i].animationsize.y);
			animation->setPeriod(imageinfo[i].animationperiod);
			animation->setLoop(-1);
			if (imageinfo[i].animationrunning)
				animation->start();
			newimage->setAnimation(animation);
			animations.push_back(animation);
		}
		// Create the script
		script = new Script();
		script->addCoreFunctions();
		script->addClientFunctions();
		// TODO: Other functions
		// Export script variables
		script->setVariable("this", EntityPointer(this));
		for (unsigned int i = 0; i < properties.size(); i++)
			script->setVariable(properties[i].getName(), &properties[i]);
		int nextanimation = 0;
		for (unsigned int i = 0; i < imageinfo.size(); i++)
		{
			script->setVariable(imageinfo[i].name, images[i]);
			if (imageinfo[i].animation)
			{
				script->setVariable(imageinfo[i].animname, animations[nextanimation]);
				nextanimation++;
			}
		}
		// Run the script
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
	void Entity::destroyScript()
	{
		script = 0;
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
				properties[i].read(buffer);
		}
	}

	void Entity::getUpdate(int time, BufferPointer buffer)
	{
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			if (!isLocal() || ((properties[i].getFlags() & EPF_Unlocked) == 0))
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
	void Entity::applyUpdate(BufferPointer buffer, int timedifference)
	{
		// Update all properties.
		bool positionchanged = false;
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			int changed = buffer->readUnsignedInt(1);
			if (changed)
			{
				properties[i].read(buffer);
				if (&properties[i] == positionproperty)
					positionchanged = true;
			}
		}
		// Client side prediction
		if (!isLocal() || timedifference <= 0 || !positionchanged)
			return;
		int originaltime = Game::get().getTime() - timedifference;
		std::deque<SpeedInfo>::reverse_iterator speedinfo = recentspeeds.rbegin();
		while (speedinfo != recentspeeds.rend())
		{
			// Check speedinfo range
			int start = speedinfo->time;
			if (start < originaltime)
				start = originaltime;
			std::deque<SpeedInfo>::reverse_iterator next = speedinfo;
			next++;
			int end = 0;
			if (next != recentspeeds.rend())
			{
				end = next->time;
			}
			else
			{
				end = Game::get().getTime();
			}
			if (end >= start)
			{
				if (positionproperty && speedinfo->speed != Vector2F(0, 0))
				{
					Vector2F position = positionproperty->getVector2F();
					// Apply speed
					for (int i = start; i < end; i++)
					{
						position += speedinfo->speed / 50;
					}
					if (Client::get().getMap()->isAccessible(RectangleF(position.x - 0.35,
						position.y - 0.35, 0.7, 0.7)))
					{
						positionproperty->setVector2F(position);
					}
				}
			}
			speedinfo++;
		}
	}
	bool Entity::hasChanged(int time)
	{
		if (!isLocal())
			return false;
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			if ((properties[i].getFlags() & EPF_Unlocked) && properties[i].getChangeTime() > time)
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
	bool Entity::isLocal()
	{
		return getOwner() == Game::get().getClientID();
	}

	void Entity::setID(int id)
	{
		this->id = id;
	}
	int Entity::getID()
	{
		return id;
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
	void Entity::setSpeed(Vector2F speed, bool ignoreobstacles)
	{
		if (this->speed == speed)
			return;
		// Set speed
		this->speed = speed;
		// Insert speed into the recent speeds list
		int time = Game::get().getTime();
		std::deque<SpeedInfo>::iterator first = recentspeeds.begin();
		if (first != recentspeeds.end() && first->time == time)
		{
			first->speed = speed;
			return;
		}
		SpeedInfo newentry;
		newentry.time = time;
		newentry.speed = speed;
		recentspeeds.push_front(newentry);
	}
	Vector2F Entity::getSpeed()
	{
		return speed;
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

	void Entity::setActive(bool active)
	{
		this->active = active;
	}
	bool Entity::isActive()
	{
		return active;
	}

	void Entity::update()
	{
		// Move entity
		if (positionproperty && speed != Vector2F(0, 0))
		{
			Vector2F position = positionproperty->getVector2F();
			position += speed / 50;
			if (Client::get().getMap()->isAccessible(RectangleF(position.x - 0.35,
				position.y - 0.35, 0.7, 0.7)))
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

	void Entity::dropPredictionData(int time)
	{
		if (recentspeeds.size() == 0)
			return;
		// Delete all speed infos which are not used any more
		while (recentspeeds.rbegin()->time <= time)
		{
			// Also check for speed infos which started earlier but last longer
			std::deque<SpeedInfo>::reverse_iterator next = recentspeeds.rbegin();
			next++;
			if (next == recentspeeds.rend())
				break;
			if (next->time > time)
				break;
			recentspeeds.pop_back();
		}
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
