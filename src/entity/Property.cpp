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
#ifdef SERVER
#include "entity/Entity.hpp"
#endif

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
	}
	Property::Property(std::string name, PropertyType type,
		PropertyFlags flags) : name(name), type(type), flags(flags)
	{
		size = 32;
		memset(data, 0, 8);
		entity = 0;
		callbacks = true;
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
	}
	Property::~Property()
	{
	}

	void Property::setName(std::string name)
	{
		this->name = name;
	}
	std::string Property::getName()
	{
		return name;
	}
	void Property::setType(PropertyType type)
	{
		this->type = type;
	}
	PropertyType Property::getType()
	{
		return type;
	}
	void Property::setFlags(PropertyFlags flags)
	{
		this->flags = flags;
	}
	PropertyFlags Property::getFlags()
	{
		return flags;
	}

	void Property::setSize(unsigned int size)
	{
		this->size = size;
	}
	unsigned int Property::getSize()
	{
		return size;
	}

	void Property::setEntity(Entity *entity)
	{
		this->entity = entity;
	}
	Entity *Property::getEntity()
	{
		return entity;
	}
	void Property::setCallbacks(bool callbacks)
	{
		this->callbacks = callbacks;
	}
	bool Property::getCallbacks()
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
	int Property::getInt()
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
	unsigned int Property::getUnsignedInt()
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
	float Property::getFloat()
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
	Vector2F Property::getVector2F()
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
	Vector2I Property::getVector2I()
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
		}
		onChange();
	}

	void Property::write(const BufferPointer &buffer)
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
				setVector2F(Vector2F(buffer->readFloat(), buffer->readFloat()));
				break;
			case EPT_Vector2I:
				setVector2I(Vector2I(buffer->readInt(size),
					buffer->readInt(size)));
				break;
		}
		onChange();
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
		else
			return !memcmp(data, property.data, 4);
	}
	bool Property::operator!=(const Property &property)
	{
		return !(*this == property);
	}

	void Property::onChange()
	{
		#ifdef SERVER
		if (!callbacks)
			return;
		if (!entity)
			return;
		entity->onChange(this);
		#else
		// TODO
		#endif
	}
}
