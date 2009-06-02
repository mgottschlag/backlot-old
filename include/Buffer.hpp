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
	 *
	 * Data is written/read to/from the buffer via read*() and write*(). All
	 * read/write operations start at the current read/write position which
	 * is 0 if the buffer has just been created. If multiple bytes are written
	 * or read, they are converted to/from network byte order if necessary.
	 */
	class Buffer : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Buffer();
			/**
			 * Constructor from preinitialized data.
			 * @param data Pointer to the new buffer content.
			 * @param size Length of the data to be used for the buffer.
			 * @param copy If set to true, a copy of the data is created, else
			 * the preallocated memory is used and deleted if the buffer is
			 * destroyed.
			 */
			Buffer(void *data, unsigned int size, bool copy = false);
			/**
			 * Copy constructor.
			 */
			Buffer(const Buffer &b);
			/**
			 * Destructor. Frees the memory connected to the buffer.
			 */
			~Buffer();

			/**
			 * Sets the size of the buffer. The read/write position is not
			 * changed if it is still valid.
			 */
			void setSize(unsigned int size);
			/**
			 * Returns the size of the buffer.
			 */
			unsigned int getSize();

			/**
			 * Sets the read/write position.
			 */
			void setPosition(unsigned int position);
			/**
			 * Returns the read/write position.
			 */
			unsigned int getPosition();

			/**
			 * Returns a pointer to the buffer data. The pointer is only valid
			 * as long as the buffer remains unchanged.
			 */
			void *getData();

			/**
			 * Writes one byte to the buffer.
			 */
			void write8(uint8_t value);
			/**
			 * Reads one byte from the buffer.
			 */
			uint8_t read8();
			/**
			 * Writes two bytes to the buffer.
			 */
			void write16(uint16_t value);
			/**
			 * Reads two bytes from the buffer.
			 */
			uint16_t read16();
			/**
			 * Writes four bytes to the buffer.
			 */
			void write32(uint32_t value);
			/**
			 * Reads four bytes from the buffer.
			 */
			uint32_t read32();
			/**
			 * Writes eight bytes to the buffer.
			 */
			void write64(uint64_t value);
			/**
			 * Reads eight bytes from the buffer.
			 */
			uint64_t read64();

			/**
			 * Writes a float to the buffer.
			 */
			void writeFloat(float value);
			/**
			 * Reads a float from the buffer.
			 */
			float readFloat();

			/**
			 * Writes a string to the buffer.
			 */
			void writeString(std::string value);
			/**
			 * Reads a string from the buffer.
			 */
			std::string readString();

			/**
			 * Creates a copy of buf.
			 */
			Buffer &operator=(const Buffer &buf);
			/**
			 * Appends one buffer to the other.
			 */
			Buffer &operator+=(const Buffer &buf);
		private:
			/**
			 * Buffer data.
			 */
			char *data;
			/**
			 * Buffer size.
			 */
			unsigned int size;
			/**
			 * Read/write position.
			 */
			unsigned int position;
	};

	typedef SharedPointer<Buffer> BufferPointer;
}

#endif
