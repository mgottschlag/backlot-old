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

#ifndef _PROPERTY_HPP_
#define _PROPERTY_HPP_

#include "Vector2.hpp"
#include "Buffer.hpp"

#include <string>

namespace backlot
{
	class Entity;

	/**
	 * Format of the property data.
	 */
	enum PropertyType
	{
		/**
		 * Integer. The size is variable and can be changed via setSize().
		 */
		EPT_Integer,
		/**
		 * Float. Always 32 bits large.
		 */
		EPT_Float,
		/**
		 * Float vector.
		 */
		EPT_Vector2F,
		/**
		 * Integer vector. The size is variable and can be changed via setSize().
		 */
		EPT_Vector2I
	};

	/**
	 * Flags controlling how a property is synchronized via network.
	 */
	enum PropertyFlags
	{
		/**
		 * No special changes to the behaviour, the server always sends the
		 * current value to all clients but the owner, the client stays passive.
		 */
		EPF_None = 0x0,
		/**
		 * Unused. Might be used for client side prediction of arbitrary
		 * components later on.
		 * @todo: Can this be removed?
		 */
		EPF_Predict = 0x1,
		/**
		 * If set, the owner of the entity also receives updates. This is
		 * probably the most common and intuitive setting as the entity is
		 * completely controlled by the server.
		 */
		EPF_OwnerUpdates = 0x2,
		/**
		 * The property is unlocked and can be changed by the owner. Other
		 * clients may not change it. This is mostly used for input handling.
		 * Do not use this in cases where it can be used for cheating!
		 */
		EPF_Unlocked = 0x4
	};

	/**
	 * Class for a network-synchronized entity variable. A property has a
	 * defined type (and, eventually, a restricted size for better network
	 * bandwidth efficiency), it is named (for simple access in scripts) and
	 * synchronization can be controlled via several flags.
	 * set*() and get*() expect proper flags and type to be set before any call
	 * to them, so do most other functions for property access. set*() flag the
	 * property as changed and raise a change event if callbacks are enabled.
	 */
	class Property
	{
		public:
			/**
			 * Constructor.
			 */
			Property();
			/**
			 * Constructor.
			 * @param name Name of the property.
			 * @param type Type of the property.
			 * @param flags Property flags defining how the property is
			 * synchronized.
			 */
			Property(std::string name, PropertyType type,
				PropertyFlags flags = EPF_None);
			/**
			 * Copy constructor.
			 */
			Property(const Property &property);
			/**
			 * Destructor.
			 */
			~Property();

			/**
			 * Sets the property name.
			 */
			void setName(std::string name);
			/**
			 * Returns the property name.
			 */
			std::string getName();
			/**
			 * Sets the property type.
			 */
			void setType(PropertyType type);
			/**
			 * Returns the property type.
			 */
			PropertyType getType();
			/**
			 * Sets the property flags.
			 */
			void setFlags(PropertyFlags flags);
			/**
			 * Returns the property flags.
			 */
			PropertyFlags getFlags();

			/**
			 * Sets the number of bits transmitted for integers and integer
			 * vectors (for the latter 2*size bits are used).
			 */
			void setSize(unsigned int size);
			/**
			 * Returns the number of bits transmitted for integer values.
			 */
			unsigned int getSize();

			/**
			 * Sets the entity which owns this property and gets change
			 * callbacks.
			 */
			void setEntity(Entity *entity);
			/**
			 * Returns the owner of this property.
			 */
			Entity *getEntity();
			/**
			 * Sets whether the entity wants to receive change callbacks.
			 */
			void setCallbacks(bool callbacks);
			/**
			 * Returns whether the entity wants to receive change callbacks.
			 */
			bool getCallbacks();

			/**
			 * Sets the value of an integer property.
			 */
			void setInt(int data);
			/**
			 * Returns the value of an integer property.
			 */
			int getInt();
			/**
			 * Sets the value of an unsigned integer property.
			 */
			void setUnsignedInt(unsigned int data);
			/**
			 * Returns the value of an unsigned integer property.
			 */
			unsigned int getUnsignedInt();
			/**
			 * Sets the value of a float property.
			 */
			void setFloat(float data);
			/**
			 * Returns the value of a float property.
			 */
			float getFloat();
			/**
			 * Sets the value of a float vector property.
			 */
			void setVector2F(const Vector2F &data);
			/**
			 * Returns the value of a float vector property.
			 */
			Vector2F getVector2F();
			/**
			 * Sets the value of an integer vector property.
			 */
			void setVector2I(const Vector2I &data);
			/**
			 * Returns the value of an integer vector property.
			 */
			Vector2I getVector2I();
			/**
			 * Sets the value of a boolean property.
			 */
			void setBool(bool data);
			/**
			 * Returns the value of a boolean property.
			 */
			bool getBool();

			/**
			 * Sets a single bit of an integer property.
			 */
			bool bit(int index);
			/**
			 * Returns a single bit of an integer property.
			 */
			void bit(int index, int value);

			/**
			 * Reads the value of the property from a string. The type has to
			 * be defined before calling this.
			 */
			void set(std::string s);

			/**
			 * Writes the property to a buffer.
			 */
			void write(const BufferPointer &buffer);
			/**
			 * Reads the property from a buffer, flags it as changed and
			 * possibly causes a change callback.
			 */
			void read(const BufferPointer &buffer);

			/**
			 * Returns the time of the last change.
			 */
			int getChangeTime();

			Property &operator=(const Property &property);
			bool operator==(const Property &property);
			bool operator!=(const Property &property);
		private:
			void onChange();

			std::string name;
			PropertyType type;
			PropertyFlags flags;
			unsigned int size;
			char data[8];
			Entity *entity;
			bool callbacks;
			int changetime;
	};
}

#endif
