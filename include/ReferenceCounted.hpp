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

namespace backlot
{
	class ReferenceCounted
	{
		public:
			ReferenceCounted()
			{
				refcount = 0;
			}
			virtual ~ReferenceCounted() {}

			void grab()
			{
				refcount++;
			}
			void drop()
			{
				refcount--;
				if (refcount <= 0)
					delete this;
			}
		private:
			int refcount;
	};

	template<class T> class SharedPointer
	{
		public:
			SharedPointer(T *target = 0) : target(target)
			{
				if (target)
					target->grab();
			}
			SharedPointer(const SharedPointer &ptr) : target(ptr.target)
			{
				if (target)
					target->grab();
			}
			~SharedPointer()
			{
				if (target)
					target->drop();
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
		private:
			T *target;
	};
}
