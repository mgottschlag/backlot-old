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

#include "PathFinder.hpp"

namespace backlot
{
	PathFinder::PathFinder(MapPointer map) : ReferenceCounted(), map(map)
	{
		status = EPFS_Inactive;
	}
	PathFinder::~PathFinder()
	{
	}

	bool PathFinder::initialize(Vector2F start, Vector2F end)
	{
		// Set path info
		this->start = start;
		this->end = end;
		status = EPFS_Waiting;
		// Add to job lists
		pathjobs.push(this);
		if (pathjobs.size() == 1)
			startSearch();
		return true;
	}

	PathFinderStatus PathFinder::getStatus()
	{
		return status;
	}
	const std::list<Vector2F> &PathFinder::getPath()
	{
		return path;
	}
	Vector2F PathFinder::getNextWaypoint(Vector2F currentposition)
	{
		// TODO: Erase waypoint if necessary
		return *path.begin();
	}

	void PathFinder::updateAll()
	{
		if (pathjobs.size() > 0)
		{
			// Continue first path finding job
			PathFinderPointer pf = pathjobs.front();
			if (pf->update())
			{
				pathjobs.pop();
				// Start next path job
				if (pathjobs.size() > 0)
					pathjobs.front()->startSearch();
			}
		}
	}
	bool PathFinder::update()
	{
		for (unsigned int i = 0; i < 1024; i++)
		{
			if (!opennodes.size())
			{
				// No open nodes left - we do not have any possible way.
				status = EPFS_Error;
				return true;
			}
			// Get the node with the best estimation and continue there
			OpenNode node = opennodes.top();
			opennodes.pop();
			Vector2I position(node.position % gridsize.x,
				node.position / gridsize.x);
			PathFinderCell &cell = grid[node.position];
			if (position == end)
			{
				// We reached our target
				collectPath();
				status = EPFS_Ready;
				return true;
			}
			// Check into which directions we can move
			unsigned char accessible = accessibility[node.position / 2];
			if (node.position % 2 == 0)
				accessible >>= 4;
			// Check surrounding tiles
			// TODO: Walk diagonally
			if (accessible & 0x8)
				addNode(position + Vector2I(1, 0), cell.cost + 1);
			if (accessible & 0x4)
				addNode(position + Vector2I(0, 1), cell.cost + 1);
			if (accessible & 0x2)
				addNode(position + Vector2I(-1, 0), cell.cost + 1);
			if (accessible & 0x1)
				addNode(position + Vector2I(0, -1), cell.cost + 1);
			// Mark cell as processed
			cell.state = lastgridid + 1;
		}
		return false;
	}

	void PathFinder::startSearch()
	{
		accessibility = map->getPathFindingInfo();
		resizeGrid();
		lastgridid += 2;
		if (!lastgridid)
		{
			lastgridid += 2;
			for (int i = 0; i < gridsize.x * gridsize.y; i++)
				grid[i].state = 0;
		}
		// Create initial search node
		int position = start.y * gridsize.x + start.x;
		PathFinderCell &startcell = grid[position];
		startcell.state = lastgridid;
		startcell.cost = 0;
		startcell.estimation = getEstimation(0, start, end);
		// Add node to the open list
		opennodes.push(OpenNode(startcell.estimation, position));
	}
	void PathFinder::resizeGrid()
	{
		// Check old size
		Vector2I mapsize = map->getSize();
		if (mapsize != gridsize)
		{
			// Recreate grid
			if (grid)
				delete[] grid;
			gridsize = mapsize;
			grid = new PathFinderCell[mapsize.x * mapsize.y];
			lastgridid = 0;
		}
	}

	void PathFinder::addNode(Vector2I position, float cost)
	{
		int gridindex = position.y * gridsize.x + position.x;
		PathFinderCell &cell = grid[gridindex];
		// Check whether we have already passed this cell
		if ((cell.state == lastgridid) || (cell.state == lastgridid + 1))
			return;
		// Mark cell as being processed
		cell.state = lastgridid;
		cell.cost = cost;
		cell.estimation = getEstimation(cost, position, end);
		// Add a node to the open list
		opennodes.push(OpenNode(cell.estimation, gridindex));
	}
	void PathFinder::collectPath()
	{
		// Travel back to the start taking the path with the least cost
		Vector2I position = end;
		while (position != start)
		{
			path.push_front(position);
			// Find the node with the shortest path
			position = getPreviousNode(position);
		}
	}
	Vector2I PathFinder::getPreviousNode(const Vector2I &position)
	{
		// Get the current grid tile
		int gridindex = position.y * gridsize.x + position.x;
		PathFinderCell &current = grid[gridindex];
		Vector2I bestposition = position;
		float bestcost = current.cost;
		// Find the node with the least costs until this point
		if (position.x > 0)
		{
			PathFinderCell &prev = grid[gridindex - 1];
			if (prev.state == lastgridid + 1 && prev.cost < bestcost)
			{
				bestcost = prev.cost;
				bestposition = position + Vector2I(-1, 0);
			}
		}
		if (position.y > 0)
		{
			PathFinderCell &prev = grid[gridindex - gridsize.x];
			if (prev.state == lastgridid + 1 && prev.cost < bestcost)
			{
				bestcost = prev.cost;
				bestposition = position + Vector2I(0, -1);
			}
		}
		if (position.x < gridsize.x - 1)
		{
			PathFinderCell &prev = grid[gridindex + 1];
			if (prev.state == lastgridid + 1 && prev.cost < bestcost)
			{
				bestcost = prev.cost;
				bestposition = position + Vector2I(1, 0);
			}
		}
		if (position.y < gridsize.y - 1)
		{
			PathFinderCell &prev = grid[gridindex + gridsize.x];
			if (prev.state == lastgridid + 1 && prev.cost < bestcost)
			{
				bestcost = prev.cost;
				bestposition = position + Vector2I(0, 1);
			}
		}
		return bestposition;
	}

	float PathFinder::getEstimation(float cost, const Vector2I &start,
		const Vector2I &end)
	{
		Vector2I way = end - start;
		return cost + way.x + way.y;
	}

	Vector2I PathFinder::gridsize;
	PathFinderCell *PathFinder::grid = 0;
	unsigned int PathFinder::lastgridid = 0;

	std::queue<SharedPointer<PathFinder> > PathFinder::pathjobs;
}
