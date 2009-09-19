/*
Copyright (C) 2009  Simon Kerler

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

#ifndef _OVERLAYIMAGE_HPP_
#define _OVERLAYIMAGE_HPP_

#include "overlay/OverlayItem.hpp"
#include "Vector2.hpp"
#include "Texture.hpp"

namespace backlot
{
	class OverlayImage : public OverlayItem
	{
		public:
			OverlayImage();
			~OverlayImage();
			
			void setScale(Vector2F scale);
			Vector2F getScale();
			void setRotation(float rotation);
			float getRotation();
			/**
			 * Set the size of the texture in pixles.
			 */
			void setTextureSize(Vector2I size);
			Vector2I getTextureSize();
			/**
			 * Set the offset of the texture in pixles.
			 */
			void setTextureOffset(Vector2I offset);
			Vector2I getTextureOffset();

			/**
			 * Loads the texture which is used as image.
			 * @param imagepath The path of the image file.
			 * @param texturesize The size of the texture in pixles.
			 */
			bool loadImage(std::string imagepath, Vector2I texturesize);

			void render();
		private:
			Vector2F texturescale;
			float texturerotation;
			Vector2I texturesize;
			Vector2I textureoffset;
			Vector2I imagesize;
			TexturePointer texture;
	};
}

#endif
