/*
Copyright (C) 2008  Mathias Gottschlag, Lukas Kropatschek

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

#include "Buffer.hpp"
#include "Engine.hpp"

#include <cstdlib>
#include <cstring>

namespace backlot
{
	Buffer::Buffer()
	{
		data = 0;
		size = 0;
		position = 0;
	}
	Buffer::Buffer(void *data, unsigned int size, bool copy)
	{
		if (copy)
		{
			this->data = (char*)malloc(size);
			memcpy(this->data, data, size);
			this->size = size;
		}
		else
		{
			this->data = (char*)data;
			this->size = size;
		}
		position = 0;
	}
	Buffer::Buffer(const Buffer &b)
	{
		data = (char*)malloc(b.size);
		memcpy(data, b.data, b.size);
		size = b.size;
		position = 0;
	}
	Buffer::~Buffer()
	{
		if (data)
			free(data);
	}

	void Buffer::setSize(unsigned int size)
	{
		data = (char*)realloc(data, size);
		this->size = size;
		if (position > size)
			position = size;
	}
	unsigned int Buffer::getSize()
	{
		return size;
	}

	void *Buffer::getData()
	{
		return data;
	}

	void Buffer::write8(uint8_t value)
	{
		if (position == size)
		{
			data = (char*)realloc(data, size + 1);
			*(uint8_t*)(data + position) = value;
			size++;
			position++;
		}
		else
		{
			*(uint8_t*)(data + position) = value;
			position++;
		}
	}
	uint8_t Buffer::read8()
	{
		if (position == size)
		{
			return 0;
		}
		uint8_t value = *(uint8_t*)(data + position);
		position++;
		return value;
	}
	void Buffer::write16(uint16_t value)
	{
		if (position + 2 > size)
		{
			data = (char*)realloc(data, position + 2);
			*(uint16_t*)(data + position) = htons(value);
			position += 2;
			size += 2;
		}
		else
		{
			*(uint16_t*)(data + position) = htons(value);
			position += 2;
		}
	}
	uint16_t Buffer::read16()
	{
		if (position + 2 > size)
		{
			return 0;
		}
		uint16_t value = *(uint16_t*)(data + position);
		position += 2;
		return ntohs(value);
	}
	void Buffer::write32(uint32_t value)
	{
		if (position + 4 > size)
		{
			data = (char*)realloc(data, position + 4);
			*(uint32_t*)(data + position) = htonl(value);
			position += 4;
			size += 4;
		}
		else
		{
			*(uint32_t*)(data + position) = htonl(value);
			position += 4;
		}
	}
	uint32_t Buffer::read32()
	{
		if (position + 4 > size)
		{
			return 0;
		}
		uint32_t value = *(uint32_t*)(data + position);
		position += 4;
		return ntohl(value);
	}
	void Buffer::write64(uint64_t value)
	{
		// TODO: Byte ordering
		if (position + 8 > size)
		{
			data = (char*)realloc(data, position + 8);
			*(uint64_t*)(data + position) = value;
			position += 8;
			size += 8;
		}
		else
		{
			*(uint64_t*)(data + position) = value;
			position += 8;
		}
	}
	uint64_t Buffer::read64()
	{
		// TODO: Byte ordering
		if (position + 8 > size)
		{
			return 0;
		}
		uint64_t value = *(uint64_t*)(data + position);
		position += 8;
		return value;
	}

	void Buffer::writeFloat(float value)
	{
		// TODO: Byte ordering
		if (position + 4 > size)
		{
			data = (char*)realloc(data, position + 4);
			*(float*)(data + position) = value;
			position += 4;
			size += 4;
		}
		else
		{
			*(float*)(data + position) = value;
			position += 4;
		}
	}
	float Buffer::readFloat()
	{
		// TODO: Byte ordering
		if (position + 4 > size)
		{
			return 0;
		}
		float value = *(float*)(data + position);
		position += 4;
		return value;
	}

	void Buffer::writeString(std::string value)
	{
		if (position + value.size() + 1 > size)
		{
			data = (char*)realloc(data, position + value.size() + 1);
			strcpy(data + position, value.c_str());
			position += value.size() + 1;
			size += value.size() + 1;
		}
		else
		{
			strcpy(data + position, value.c_str());
			position += value.size() + 1;
		}
	}
	std::string Buffer::readString()
	{
		if (position == size)
			return "";
		std::string value;
		while (data[position] != 0)
		{
			value += data[position];
			position++;
			if (position == size)
				break;
		}
		if (position < size)
			position++;
		return value;
	}

	Buffer &Buffer::operator=(const Buffer &b)
	{
		data = (char*)malloc(b.size);
		memcpy(data, b.data, b.size);
		size = b.size;
		position = 0;
		return *this;
	}
	Buffer &Buffer::operator+=(const Buffer &b)
	{
		data = (char*)realloc(data, size + b.size);
		memcpy(data + size, b.data, b.size);
		size += b.size;
		return *this;
	}
}
