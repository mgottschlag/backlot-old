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

#ifndef _REFERENCECOUNTED_HPP_
#define _REFERENCECOUNTED_HPP_

namespace backlot
{
	/**
	 * Base class for all classes which use reference counting. Reference
	 * counting makes sure that objects aren't deallocated as long as there are
	 * references to them (see: SharedPointer) by just counting the references.
	 * This is done via grab() (increments the reference count) and drop()
	 * (decrements the reference count). When the reference count reaches 0, the
	 * object is destroyed.
	 */
	class ReferenceCounted
	{
		public:
			ReferenceCounted()
			{
				refcount = 0;
			}
			virtual ~ReferenceCounted() {}

			/**
			 * Increments the reference count.
			 */
			void grab()
			{
				refcount++;
			}
			/**
			 * Decrements the reference count.
			 */
			void drop()
			{
				refcount--;
				if (refcount <= 0)
					delete this;
			}
		private:
			/**
			 * Number of references to the object.
			 */
			int refcount;
	};

	/**
	 * Pointer class for simple access to ReferenceCounted. Does the grab()
	 * and drop() itself. The pointer still has to be initialized before any
	 * use, null pointer access is not caught properly and has to be checked
	 * via casting the pointer to bool or isNull().
	 */
	template<class T> class SharedPointer
	{
		public:
			/**
			 * Constructor.
			 * @param target Pointer to a reference counted object
			 */
			SharedPointer(T *target = 0) : target(target)
			{
				if (target)
					target->grab();
			}
			/**
			 * Copy constructor.
			 */
			SharedPointer(const SharedPointer &ptr) : target(ptr.target)
			{
				if (target)
					target->grab();
			}
			/**
			 * Destructor.
			 */
			~SharedPointer()
			{
				if (target)
					target->drop();
			}

			/**
			 * Checks whether the pointer is invalid.
			 */
			bool isNull()
			{
				return target == 0;
			}

			SharedPointer &operator=(const SharedPointer &ptr)
			{
				if (target)
					target->drop();
				target = ptr.target;
				if (target)
					target->grab();
				return *this;
			}
			T *operator->()
			{
				return target;
			}
			bool operator==(const SharedPointer &ptr)
			{
				return &target == &ptr.target;
			}
			operator bool()
			{
				return target != 0;
			}
		private:
			T *target;
	};
}

#endif
