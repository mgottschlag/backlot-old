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

#include "entity/EntityState.hpp"

#include <iostream>

namespace backlot
{
	SharedPointer<EntityState> EntityState::create(EntityTemplatePointer tpl)
	{
		return new EntityState(tpl);
	}
	EntityState::EntityState(EntityTemplatePointer tpl)
	{
		if (!tpl)
			return;
		this->tpl = tpl;
		// Create properties with default values.
		properties = tpl->getProperties();
	}
	EntityState::~EntityState()
	{
	}

	EntityTemplatePointer EntityState::getTemplate()
	{
		return tpl;
	}

	Property *EntityState::getProperty(std::string name)
	{
		// TODO: This is slow.
		for (unsigned int i = 0; i < properties.size(); i++)
		{
			if (properties[i].getName() == name)
				return &properties[i];
		}
		return 0;
	}

	BufferPointer EntityState::get()
	{
		// Create new buffer.
		BufferPointer buffer = new Buffer();
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
		buffer->setPosition(0);
		return buffer;
	}
}
