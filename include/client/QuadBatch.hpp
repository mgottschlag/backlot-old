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

#ifndef _QUADBATCH_HPP_
#define _QUADBATCH_HPP_

#include "ReferenceCounted.hpp"
#include "Texture.hpp"
#include "Quad.hpp"

#include <vector>

namespace backlot
{
	/**
	 * Batch of rendering geometry. Maps directly to an OpenGL VBO/IBO.
	 */
	class QuadBatch : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			QuadBatch();
			/**
			 * Destructor.
			 */
			~QuadBatch();

			/**
			 * Adds a quad to the batch.
			 */
			void addQuad(const Quad &quad);
			/**
			 * Removes a quad from the batch. This is quite slow.
			 */
			void removeQuad(int index);
			/**
			 * Returns a reference to the batch. Can be used to manipulate or
			 * save the data.
			 */
			std::vector<Quad> &getQuads();

			/**
			 * Returns the number of quads in the batch.
			 */
			unsigned int getSize();

			/**
			 * Sets whether the batch needs to be uploaded to the GPU again.
			 * The uploading itself is not done automatically but rather has to
			 * be done by calling rebuild() when you are finished with
			 * manipulating the batch.
			 */
			void setDirty(bool dirty);
			/**
			 * Returns whether the batch needs to be uploaded to the GPU again.
			 */
			bool isDirty();
			/**
			 * Updates the batch on the GPU.
			 */
			void rebuild();

			/**
			 * Renders the batch with the current transformation/material
			 * settings.
			 */
			void render();
		private:
			/**
			 * Dynamic array holding all quads in the batch.
			 */
			std::vector<Quad> quads;
			/**
			 * Set to true if the batch needs to be uploaded.
			 */
			bool dirty;
			/**
			 * Vertex buffer object.
			 */
			unsigned int vbo;
			/**
			 * Vertex data. Used for drawing if VBOs are not available.
			 */
			float *vertexdata;
	};

	typedef SharedPointer<QuadBatch> QuadBatchPointer;
}

#endif
