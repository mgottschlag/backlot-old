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

#include "ScoreBoard.hpp"
#include "Preferences.hpp"
#include "Game.hpp"
#include "Font.hpp"

#include <GL/gl.h>

namespace backlot
{
	ScoreBoard &ScoreBoard::get()
	{
		static ScoreBoard board;
		return board;
	}
	ScoreBoard::~ScoreBoard()
	{
	}

	void ScoreBoard::setVisible(bool visible)
	{
		this->visible = visible;
	}
	bool ScoreBoard::isVisible()
	{
		return visible;
	}

	void ScoreBoard::render()
	{
		if (!visible)
			return;
		// Setup camera
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		const Vector2I &size = Preferences::get().getResolution();
		glOrtho(0, size.x, size.y, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		// Size
		int x = size.x / 2 - 200;
		int width = 400;
		// Background
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glColor3f(0.25, 0.25, 0.25);
		glBegin(GL_QUADS);
			glVertex2f(x, 0);
			glVertex2f(x + width, 0);
			glVertex2f(x + width, 400);
			glVertex2f(x, 400);
		glEnd();
		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_BLEND);
		// Get font
		FontPointer font = Font::get("menu");
		if (!font)
			return;
		// Header
		font->render("Scoreboard:", Vector2I(x + 10, 0));
		font->render("Name:", Vector2I(x + 10, 30));
		font->render("Score:", Vector2I(x + 160, 30));
		// Get all players
		EntityListPointer players = Game::get().getEntities(std::string("player"));
		for (int i = 0; i < players->getSize(); i++)
		{
			EntityPointer player = players->getEntity(i);
			int score = player->getProperty("score")->getInt();
			char scorestr[10];
			snprintf(scorestr, 10, "%d", score);
			std::string name = player->getProperty("name")->getString();
			font->render(name, Vector2I(x + 10, 50 + i * 20));
			font->render(scorestr, Vector2I(x + 160, 50 + i * 20));
		}
		// Restore camera
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	ScoreBoard::ScoreBoard()
	{
		visible = false;
	}
}
