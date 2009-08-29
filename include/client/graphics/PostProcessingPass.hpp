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

#ifndef _POSTPROCESSINGPASS_HPP_
#define _POSTPROCESSINGPASS_HPP_

#include "ReferenceCounted.hpp"
#include "graphics/Texture.hpp"
#include "graphics/RenderTarget.hpp"

#include <string>
#include <vector>

class TiXmlElement;

namespace backlot
{
	class PostProcessingPass : public ReferenceCounted
	{
		public:
			PostProcessingPass();
			~PostProcessingPass();

			bool load(TiXmlElement *xml);

			static void init();
			static void beginFrame();
			static void endFrame();
			static void drawResult();
		private:
			static TexturePointer current;
			static TexturePointer next;
			static RenderTargetPointer rtt;
	};

	typedef SharedPointer<PostProcessingPass> PostProcessingPassPointer;
}

#endif
