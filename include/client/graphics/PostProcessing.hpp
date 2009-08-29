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

#ifndef _POSTPROCESSING_HPP_
#define _POSTPROCESSING_HPP_

#include "graphics/PostProcessingPass.hpp"

#include <string>

namespace backlot
{
	/**
	 * Full-screen post-processing effect (like blur, colouring etc.).
	 */
	class PostProcessing : public ReferenceCounted
	{
		public:
			PostProcessing();
			~PostProcessing();

			static SharedPointer<PostProcessing> get(std::string name);
			static void add(SharedPointer<PostProcessing> effect,
				std::string name);
			bool load(std::string name);

			void setName(std::string name);
			std::string getName();

			void setActive(bool active);
			bool isActive();

			void addPass(PostProcessingPassPointer pass);

			static void init();
			static void begin();
			static void end();

			void apply();
		private:
			std::string name;
			bool active;

			std::vector<PostProcessingPassPointer> passes;

			static std::vector<PostProcessing*> effects;
	};

	typedef SharedPointer<PostProcessing> PostProcessingPointer;
}

#endif
