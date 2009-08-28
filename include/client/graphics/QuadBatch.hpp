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
	 * Batch of rendering geometry. Maps directly to an OpenGL VBO.
	 */
	class QuadBatch : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			QuadBatch();
			/**
			 * Constructor. Automatically fills the quad batch.
			 */
			QuadBatch(unsigned int quadcount, float *vertexdata);
			/**
			 * Destructor.
			 */
			~QuadBatch();

			/**
			 * Refills the quad batch. vertexdata holds a pointer to five floats
			 * per vertex, with 4 subsequent vertices forming a quad. First, 3
			 * floats per vertex contain the position, then 2 floats per vertex
			 * contain the texture coords (no interleaving used!).
			 */
			void set(unsigned int quadcount, float *vertexdata);
			/**
			 * Returns the number of quads in the batch.
			 */
			unsigned int getSize();

			/**
			 * Renders the batch with the current transformation/material
			 * settings.
			 */
			void render();
		private:
			/**
			 * Vertex buffer object.
			 */
			unsigned int vbo;
			/**
			 * Vertex data. Used for drawing if VBOs are not available.
			 */
			float *vertexdata;
			/**
			 * Number of quads in the batch.
			 */
			unsigned int quadcount;
	};

	typedef SharedPointer<QuadBatch> QuadBatchPointer;
}

#endif
