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

#include "entity/Property.hpp"
#include "entity/Entity.hpp"
#include "Game.hpp"

#include <cstring>
#include <iostream>

namespace backlot
{
	Property::Property()
	{
		type = EPT_Integer;
		flags = EPF_None;
		size = 32;
		memset(data, 0, 8);
		entity = 0;
		callbacks = true;
		changetime = 0;
	}
	Property::Property(std::string name, PropertyType type,
		PropertyFlags flags) : name(name), type(type), flags(flags)
	{
		size = 32;
		memset(data, 0, 8);
		entity = 0;
		callbacks = true;
		changetime = 0;
	}
	Property::Property(const Property &property)
	{
		name = property.name;
		type = property.type;
		flags = property.flags;
		size = property.size;
		memcpy(data, property.data, 8);
		entity = 0;
		callbacks = true;
		changetime = 0;
		stringdata = property.stringdata;
	}
	Property::~Property()
	{
	}

	void Property::setName(std::string name)
	{
		this->name = name;
	}
	std::string Property::getName() const
	{
		return name;
	}
	void Property::setType(PropertyType type)
	{
		this->type = type;
	}
	PropertyType Property::getType() const
	{
		return type;
	}
	void Property::setFlags(PropertyFlags flags)
	{
		this->flags = flags;
	}
	PropertyFlags Property::getFlags() const
	{
		return flags;
	}

	void Property::setSize(unsigned int size)
	{
		this->size = size;
	}
	unsigned int Property::getSize() const
	{
		return size;
	}

	void Property::setEntity(Entity *entity)
	{
		this->entity = entity;
	}
	Entity *Property::getEntity() const
	{
		return entity;
	}
	void Property::setCallbacks(bool callbacks)
	{
		this->callbacks = callbacks;
	}
	bool Property::getCallbacks() const
	{
		return callbacks;
	}

	void Property::setInt(int data)
	{
		if (type == EPT_Integer)
		{
			*((int*)this->data) = data;
			onChange();
		}
		else
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
	}
	int Property::getInt() const
	{
		if (type == EPT_Integer)
		{
			return *((int*)this->data);
		}
		else
		{
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
			return 0;
		}
	}
	void Property::setUnsignedInt(unsigned int data)
	{
		if (type == EPT_Integer)
		{
			*((unsigned int*)this->data) = data;
			onChange();
		}
		else
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
	}
	unsigned int Property::getUnsignedInt() const
	{
		if (type == EPT_Integer)
		{
			return *((unsigned int*)this->data);
		}
		else
		{
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
			return 0;
		}
	}
	void Property::setFloat(float data)
	{
		if (type == EPT_Float)
		{
			*((float*)this->data) = data;
			onChange();
		}
		else
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
	}
	float Property::getFloat() const
	{
		if (type == EPT_Float)
		{
			return *((float*)this->data);
		}
		else
		{
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
			return 0;
		}
	}
	void Property::setVector2F(const Vector2F &data)
	{
		if (type == EPT_Vector2F)
		{
			((float*)this->data)[0] = data.x;
			((float*)this->data)[1] = data.y;
			onChange();
		}
		else
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
	}
	Vector2F Property::getVector2F() const
	{
		if (type == EPT_Vector2F)
		{
			return Vector2F(((float*)this->data)[0], ((float*)this->data)[1]);
		}
		else
		{
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
			return Vector2F();
		}
	}
	void Property::setVector2I(const Vector2I &data)
	{
		if (type == EPT_Vector2I)
		{
			((int*)this->data)[0] = data.x;
			((int*)this->data)[1] = data.y;
			onChange();
		}
		else
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
	}
	Vector2I Property::getVector2I() const
	{
		if (type == EPT_Vector2I)
		{
			return Vector2I(((int*)this->data)[0], ((int*)this->data)[1]);
		}
		else
		{
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
			return Vector2I();
		}
	}
	void Property::setBool(bool data)
	{
		bit(0, data);
	}
	bool Property::getBool() const
	{
		return bit(0);
	}
	void Property::setString(std::string data)
	{
		if (type == EPT_String)
		{
			stringdata = data;
			onChange();
		}
		else
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
	}
	std::string Property::getString()
	{
		if (type == EPT_String)
		{
			return stringdata;
		}
		else
		{
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
			return "";
		}
	}

	bool Property::bit(int index) const
	{
		if (type == EPT_Integer)
		{
			return (*((unsigned int*)this->data) >> index) & 1;
		}
		else
		{
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
			return 0;
		}
	}
	void Property::bit(int index, int value)
	{
		if (type == EPT_Integer)
		{
			if (value)
				*((unsigned int*)this->data) |= 1 << index;
			else
				*((unsigned int*)this->data) &= ~(1 << index);
			onChange();
		}
		else
			std::cerr << "Warning: Wrong property type (" << name << ")." << std::endl;
	}

	void Property::set(std::string s)
	{
		switch (type)
		{
			case EPT_Integer:
				setInt(atoi(s.c_str()));
				break;
			case EPT_Float:
				setFloat(atof(s.c_str()));
				break;
			case EPT_Vector2F:
				setVector2F(Vector2F(s));
				break;
			case EPT_Vector2I:
				setVector2I(Vector2I(s));
				break;
			case EPT_String:
				setString(s);
				break;
		}
		onChange();
	}

	void Property::write(const BufferPointer &buffer) const
	{
		switch (type)
		{
			case EPT_Integer:
				// TODO: Unsigned integer
				buffer->writeInt(getInt(), size);
				break;
			case EPT_Float:
				buffer->writeFloat(getFloat());
				break;
			case EPT_Vector2F:
			{
				Vector2F vector = getVector2F();
				buffer->writeFloat(vector.x);
				buffer->writeFloat(vector.y);
				break;
			}
			case EPT_Vector2I:
			{
				Vector2I vector = getVector2I();
				buffer->writeInt(vector.x, size);
				buffer->writeInt(vector.y, size);
				break;
			}
			case EPT_String:
				buffer->writeString(stringdata);
				break;
		}
	}
	void Property::read(const BufferPointer &buffer)
	{
		switch (type)
		{
			case EPT_Integer:
				setInt(buffer->readInt(size));
				break;
			case EPT_Float:
				setFloat(buffer->readFloat());
				break;
			case EPT_Vector2F:
			{
				float x = buffer->readFloat();
				float y = buffer->readFloat();
				setVector2F(Vector2F(x, y));
				break;
			}
			case EPT_Vector2I:
			{
				int x = buffer->readInt(size);
				int y = buffer->readInt(size);
				setVector2I(Vector2I(x, y));
				break;
			}
			case EPT_String:
				stringdata = buffer->readString();
				break;
		}
	}

	int Property::getChangeTime() const
	{
		return changetime;
	}

	Property &Property::operator=(const Property &property)
	{
		if (name == "")
		{
			name = property.name;
			type = property.type;
			flags = property.flags;
			size = property.size;
			memcpy(data, property.data, 8);
		}
		else if (type == property.type)
			memcpy(data, property.data, 8);
		stringdata = property.stringdata;
		onChange();
		return *this;
	}
	bool Property::operator==(const Property &property)
	{
		if (type != property.type)
			return false;
		if (type == EPT_Vector2F)
			return ((float*)data)[0] == ((float*)property.data)[0]
				&& ((float*)data)[1] == ((float*)property.data)[1];
		else if (type == EPT_Vector2I)
			return ((int*)data)[0] == ((int*)property.data)[0]
				&& ((int*)data)[1] == ((int*)property.data)[1];
		else if (type == EPT_String)
			return property.stringdata == stringdata;
		else
			return !memcmp(data, property.data, 4);
	}
	bool Property::operator!=(const Property &property)
	{
		return !(*this == property);
	}

	void Property::onChange()
	{
		if (!callbacks)
			return;
		if (!entity)
			return;
		// Update time of the last change
		changetime = Game::get().getTime();
		// Entity callback
		entity->onChange(this);
	}
}
