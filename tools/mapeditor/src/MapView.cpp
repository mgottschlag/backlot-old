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

#include "MapView.hpp"
#include "TileSet.hpp"
#include "Map.hpp"
#include "Tile.hpp"

#include <QMouseEvent>
#include <iostream>

MapView::MapView(QWidget *parent) : QGLWidget(parent)
{
	grid = true;
	minimap = true;
	camerax = 0.0;
	cameray = 0.0;
	mousex = 0;
	mousey = 0;
	moving = false;
	currenttile = 0;
	action = EUA_None;
	painting = false;
	setMouseTracking(true);
}

void MapView::setTile(Tile *tile)
{
	currenttile = tile;
}
void MapView::setUserAction(UserAction action)
{
	this->action = action;
}

void MapView::showGrid(bool grid)
{
	if (this->grid == grid)
		return;
	this->grid = grid;
	repaint();
}
void MapView::showMiniMap(bool minimap)
{
	if (this->minimap == minimap)
		return;
	this->minimap = minimap;
	repaint();
}

void MapView::initializeGL()
{
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glMatrixMode(GL_TEXTURE);
	glScalef(1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	TileSet::loadTextures();
}
void MapView::paintGL()
{
	// Clear screen
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!Map::get().isLoaded())
		return;
	// Setup camera
	int width = Map::get().getWidth();
	int height = Map::get().getHeight();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (float)this->width() / 32, (float)this->height() / 32, 0, -128, 127);
	if (camerax < -5)
		camerax = -5;
	if (cameray < -5)
		cameray = -5;
	if (camerax > (float)width + 5 - (float)this->width() / 32)
		camerax = (float)width + 5 - (float)this->width() / 32;
	if (cameray > (float)height + 5 - (float)this->height() / 32)
		cameray = (float)height + 5 - (float)this->height() / 32;
	glTranslatef(-camerax, -cameray, 0);
	glMatrixMode(GL_MODELVIEW);
	// Draw map
	Map::get().render();
	// Draw grid
	if (grid)
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		for (int x = 0; x <= width; x++)
		{
			glVertex3f(x, 0, 100);
			glVertex3f(x, height, 100);
		}
		for (int y = 0; y <= height; y++)
		{
			glVertex3f(0, y, 100);
			glVertex3f(width, y, 100);
		}
		glEnd();
	}
	// Draw cursor
	if (action == EUA_DrawTile)
	{
		int x = (float)mousex / 32 + camerax;
		int y = (float)mousey / 32 + cameray;
		if (x >= 0 && x < width
			&& y >= 0 && y < height)
		{
			if (currenttile)
			{
				glClear(GL_DEPTH_BUFFER_BIT);
				currenttile->render(x, y);
				currenttile->renderShadows(x, y);
			}
		}
	}
	// Draw mini map
	if (minimap)
	{
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, this->width(), this->height(), 0, -128, 127);
		if (width > height)
			glScalef(1.0, (float)height / width, 1.0);
		else
			glScalef((float)width / height, 1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		// Background
		glColor4f(0.1, 0.1, 0.1, 1.0);
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(0, 150);
			glVertex2f(150, 150);
			glVertex2f(150, 0);
		glEnd();
		// Map
		glColor4f(0.8, 0.8, 0.8, 1.0);
		float borderx = 5.0 / (width + 10) * 150;
		float bordery = 5.0 / (height + 10) * 150;
		glBegin(GL_LINE_LOOP);
			glVertex2f(borderx, bordery);
			glVertex2f(borderx, 150 - bordery);
			glVertex2f(150 - borderx, 150 - bordery);
			glVertex2f(150 - borderx, bordery);
		glEnd();
		// Camera
		glColor4f(1.0, 1.0, 1.0, 1.0);
		float x = (camerax + 5) / (width + 10) * 150;
		float y = (cameray + 5) / (height + 10) * 150;
		float camwidth = (float)this->width() / 32 / (width + 10) * 150;
		float camheight = (float)this->height() / 32 / (height + 10) * 150;
		glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x, y + camheight);
			glVertex2f(x + camwidth, y + camheight);
			glVertex2f(x + camwidth, y);
		glEnd();
	}
}
void MapView::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}
void MapView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		// Start moving
		moving = true;
		mousex = event->x();
		mousey = event->y();
	}
	else if (event->button() == Qt::LeftButton)
	{
		// Start moving
		painting = true;
		mousex = event->x();
		mousey = event->y();
		performAction();
		repaint();
	}
}
void MapView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		// Stop moving
		moving = false;
		mousex = event->x();
		mousey = event->y();
	}
	else if (event->button() == Qt::LeftButton)
	{
		// Start moving
		painting = false;
		mousex = event->x();
		mousey = event->y();
	}
}
void MapView::mouseMoveEvent(QMouseEvent *event)
{
	if (moving)
	{
		// Move camera
		int diffx = event->x() - mousex;
		camerax -= (float)diffx / 32;
		int diffy = event->y() - mousey;
		cameray -= (float)diffy / 32;
	}
	mousex = event->x();
	mousey = event->y();
	if (painting)
		performAction();
	repaint();
}

void MapView::performAction()
{
	// Get map position
	if (!Map::get().isLoaded())
		return;
	int width = Map::get().getWidth();
	int height = Map::get().getHeight();
	int x = (float)mousex / 32 + camerax;
	int y = (float)mousey / 32 + cameray;
	if (x >= 0 && x < width
		&& y >= 0 && y < height)
	{
		// Perform user action
		switch (action)
		{
			case EUA_None:
				break;
			case EUA_DrawTile:
				if (currenttile)
					Map::get().setTile(x, y, currenttile);
				break;
			case EUA_EraseTile:
				if (currenttile)
					Map::get().setTile(x, y, 0);
				break;
		};
	}
}
