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

#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

#include "ReferenceCounted.hpp"

#include <stdint.h>
#include <string>

namespace backlot
{
	/**
	 * Dynamically sized buffer for network data etc.
	 */
	class Buffer : public ReferenceCounted
	{
		public:
			Buffer();
			Buffer(void *data, unsigned int size, bool copy = false);
			Buffer(const Buffer &b);
			~Buffer();

			void setSize(unsigned int size);
			unsigned int getSize();

			void setPosition(unsigned int position);
			unsigned int getPosition();

			void *getData();

			void write8(uint8_t value);
			uint8_t read8();
			void write16(uint16_t value);
			uint16_t read16();
			void write32(uint32_t value);
			uint32_t read32();
			void write64(uint64_t value);
			uint64_t read64();

			void writeFloat(float value);
			float readFloat();

			void writeString(std::string value);
			std::string readString();

			Buffer &operator=(const Buffer &buf);
			Buffer &operator+=(const Buffer &buf);
		private:
			char *data;
			unsigned int size;
			unsigned int position;
	};

	typedef SharedPointer<Buffer> BufferPointer;
}

#endif
