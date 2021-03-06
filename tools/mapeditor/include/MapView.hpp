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

#ifndef _MAPVIEW_HPP_
#define _MAPVIEW_HPP_

#include <QGLWidget>

class Tile;

enum UserAction
{
	EUA_None,
	EUA_Draw,
	EUA_Erase
};

enum DrawMode
{
	EDM_None,
	EDM_Tile,
	EDM_Entity
};

class MapView : public QGLWidget
{
	Q_OBJECT

	public:
		MapView(QWidget *parent = 0);

		void setTile(Tile *tile);
		void setEntity(std::string entity);
		void setUserAction(UserAction action);
		void setDrawMode(DrawMode mode);

	public slots:
		void showGrid(bool grid);
		void showMiniMap(bool minimap);
	protected:
		void initializeGL();
		void paintGL();
		void resizeGL(int width, int height);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
	private:
		void performAction();
		bool grid;
		bool minimap;
		int mousex;
		int mousey;
		float camerax;
		float cameray;
		bool moving;
		Tile *currenttile;
		std::string currententity;
		UserAction action;
		DrawMode mode;
		bool painting;
		unsigned int entitytexture;
};

#endif
