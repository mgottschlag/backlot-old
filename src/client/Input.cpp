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

#include "Input.hpp"
#include "NetworkData.hpp"
#include "Preferences.hpp"
#include "menu/Menu.hpp"
#include "graphics/Graphics.hpp"
#include "Game.hpp"

#include <SDL/SDL.h>
#include <math.h>
#include <iostream>
#include <guichan/sdl/sdlinput.hpp>

namespace backlot
{
	Input &Input::get()
	{
		static Input input;
		return input;
	}
	Input::~Input()
	{
	}

	bool Input::update()
	{
		// Handle SDL events
		SDL_Event event;
		float rotation = 0;
		bool rotationchanged = false;
		bool shooting = false;
		while (SDL_PollEvent(&event))
		{
			// Disable game input in the menu
			if (Menu::getActiveMenu())
			{
				switch(event.type)
				{
					case SDL_QUIT:
						return false;
					case SDL_KEYDOWN:
						// Key was pressed down
						switch(event.key.keysym.sym)
						{
							case SDLK_ESCAPE:
								//Menu::getActiveMenu()->setActive(false);
								break;
							default:
								Graphics::get().getGuichanInput()->pushInput(event);
								break;
						}
						break;
					default:
						Graphics::get().getGuichanInput()->pushInput(event);
						break;
				}
				continue;
			}
			// Game input
			switch(event.type)
			{
				case SDL_QUIT:
					return false;
				case SDL_KEYDOWN:
					// Key was pressed down
					switch(event.key.keysym.sym)
					{
						// WSAD
						case SDLK_w:
							sendKey("up", 1);
							break;
						case SDLK_s:
							sendKey("down", 1);
							break;
						case SDLK_a:
							sendKey("left", 1);
							break;
						case SDLK_d:
							sendKey("right", 1);
							break;
						case SDLK_ESCAPE:
							sendKey("exit", 1);
							break;
						// Other keys
						default:
							break;
					}
					break;
				case SDL_KEYUP:
					// Key was left up
					switch(event.key.keysym.sym)
					{
						// WSAD
						case SDLK_w:
							sendKey("up", 0);
							break;
						case SDLK_s:
							sendKey("down", 0);
							break;
						case SDLK_a:
							sendKey("left", 0);
							break;
						case SDLK_d:
							sendKey("right", 0);
							break;
						// Other keys
						default:
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					// Mouse button was pressed down
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							shootonce = false;
							shooting = true;
							sendKey("shoot", 1);
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					// Mouse button was pressed down
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							// Check whether the button was left up instantly
							// and also shoot in this case
							if (!shooting)
								sendKey("shoot", 0);
							else
								shootonce = true;
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEMOTION:
				{
					int x = event.motion.x;
					int y = event.motion.y;
					rotationchanged = true;
					// Get rotation relative to the center of the window
					const Vector2I &size = Preferences::get().getResolution();
					rotation = atan2(x - size.x / 2, size.y / 2 - y);
					break;
				}
				default:
					break;
			}
		}
		// Stop shooting if the button was only pressed shortly
		if (!shooting && shootonce)
		{
			sendKey("shoot", 0);
			shootonce = false;
		}
		// Send new rotation.
		if (rotationchanged)
		{
			EntityPointer entity = Game::get().getInputTarget();
			if (entity)
			{
				// Call script callback
				ScriptPointer script = entity->getScript();
				if (script && script->isFunction("on_mouse_input"))
					script->callFunction("on_mouse_input", rotation * 180 / 3.1415);
			}
		}
		return true;
	}

	Input::Input()
	{
		shootonce = false;
	}

	void Input::sendKey(std::string key, int state)
	{
		EntityPointer entity = Game::get().getInputTarget();
		if (entity.isNull())
			return;
		// Call script callback
		ScriptPointer script = entity->getScript();
		if (script->isFunction("on_keyboard_input"))
			script->callFunction("on_keyboard_input", key, state);
	}
}
