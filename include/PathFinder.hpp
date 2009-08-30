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

#ifndef _PATHFINDER_HPP_
#define _PATHFINDER_HPP_

#include "Map.hpp"

namespace backlot
{
	/**
	 * Status of a path finding request.
	 */
	enum PathFinderStatus
	{
		/**
		 * No current request available.
		 */
		EPFS_Inactive,
		/**
		 * The request is being processed, there is no result available yet.
		 */
		EPFS_Waiting,
		/**
		 * A path has been computed and the target has not yet been reached.
		 */
		EPFS_Ready,
		/**
		 * The target already has been reached.
		 */
		EPFS_Done,
		/**
		 * The path finder was unable to process the request, no possible way
		 * has been found.
		 */
		EPFS_Error
	};

	/**
	 * Class for A* path finding. Note that this class does not grant instant
	 * results, usually a path search will take more than one frame to finish.
	 * The current status then will be EPFS_Waiting until the path is available.
	 */
	class PathFinder : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 * @param map Map info to be used for the path.
			 */
			PathFinder(MapPointer map);
			/**
			 * Destructor.
			 */
			~PathFinder();

			/**
			 * Starts a new path search.
			 * @param start Start point on the map.
			 * @param end End point on the map.
			 */
			bool initialize(Vector2F start, Vector2F end);

			/**
			 * Returns the current status of the path finder.
			 */
			PathFinderStatus getStatus();
			/**
			 * Returns all remaining way points.
			 */
			const std::list<Vector2F> &getPath();
			/**
			 * Returns the next way point from the current position. Way points
			 * which have been reached are discarded and do not appear in the
			 * waypoint list returned by getPath() either.
			 */
			Vector2F getNextWaypoint(Vector2F currentposition);

			/**
			 * Updates all path finders.
			 */
			static void updateAll();
			/**
			 * Updates this path finder. The work of searching for a path is
			 * usually done over multiple frames depending on the CPU usage and
			 * the complexity of the request.
			 */
			void update();

		private:
			MapPointer map;

			static std::vector<PathFinder> pathfinders;
	};
}

#endif
