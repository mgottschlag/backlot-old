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

#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "ReferenceCounted.hpp"
#include "Vector2.hpp"

#include <string>
#include <map>

namespace backlot
{
	enum TextureFilter
	{
		ETF_Nearest,
		ETF_Linear,
		ETF_LinearMipMap
	};
	/**
	 * OpenGL texture. Textures can be loaded from image files, one texture can
	 * be used in different places as get() returns an already loaded instance
	 * if possible.
	 */
	class Texture : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 * @param filter Texture filter for the newly created texture.
			 */
			Texture(TextureFilter filter = ETF_Nearest);
			/**
			 * Destructor.
			 */
			~Texture();

			/**
			 * Loads a texture from a file or returns an already loaded one.
			 * @param path Path to the texture relative to the game directory.
			 * @param singleuse If set to true, the texture is not registered
			 * and will not be reused by later calls to get using the same path.
			 */
			static SharedPointer<Texture> get(std::string path, bool singleuse = false);
			/**
			 * Returns an already loaded texture.
			 * @param id ID of the wanted texture.
			 */
			static SharedPointer<Texture> get(int id);
			/**
			 * Loads a file into this texture and registers it if wanted.
			 * @param path File to be loaded, relative to the game directory.
			 * @param registertexture If set to true, the texture is registered
			 * for later reuse.
			 */
			bool load(std::string path, bool registertexture = true);

			/**
			 * Activates a different texture filter method.
			 */
			void setFilter(TextureFilter filter);
			/**
			 * Returns the current texture filter method.
			 */
			TextureFilter getFilter();

			/**
			 * Returns the size of the texture in pixels.
			 */
			const Vector2I &getSize();

			/**
			 * Returns the ID of the texture.
			 */
			int getID();
			/**
			 * Calls glBindTexture() for this texture.
			 */
			void bind();
		private:
			/**
			 * OpenGL texture handle.
			 */
			unsigned int texture;
			/**
			 * Current texture filter.
			 */
			TextureFilter filter;
			/**
			 * Size in pixels.
			 */
			Vector2I size;
			/**
			 * Path to the loaded file if the texture was registered.
			 */
			std::string path;
			/**
			 * ID of the texture if it was registered.
			 */
			int id;

			/**
			 * Registered textures accessible by ID.
			 */
			static std::map<int, Texture*> textures;
			/**
			 * Registered textures accessible by path.
			 */
			static std::map<std::string, Texture*> texturepaths;
			/**
			 * Last texture ID, is incremented every time a texture is
			 * registered.
			 */
			static unsigned int lasttextureid;
	};

	typedef SharedPointer<Texture> TexturePointer;
}

#endif
