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

#include "EditorWindow.hpp"
#include "Game.hpp"
#include "TileSet.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	// Initialize game data
	std::string gamedirectory = "";
	if (argc == 2)
	{
		gamedirectory = argv[1];
	}
	else
	{
		gamedirectory = QFileDialog::getExistingDirectory(0,
			"Choose a game directory", ".").toAscii().constData();
	}
	if ((gamedirectory == "") || !Game::get().init(gamedirectory))
	{
		QMessageBox::critical(0, "No valid game directory available.",
			"Could not open the game data because no valid game directory was "
			"passed to the program.");
		return -1;
	}
	TileSet::loadAll();

	EditorWindow window;
	window.show();
	return app.exec();
}
