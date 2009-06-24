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
#include "Player.hpp"
#include "Menu.hpp"
#include "Graphics.hpp"

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
		bool keyschanged = false;
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
								Menu::getActiveMenu()->setActive(false);
								break;
							default:
								Graphics::get().getGuichanInput()->pushInput(event);
								break;
						}
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
							keyschanged = true;
							currentkeys |= EKM_Up;
							break;
						case SDLK_s:
							keyschanged = true;
							currentkeys |= EKM_Down;
							break;
						case SDLK_a:
							keyschanged = true;
							currentkeys |= EKM_Left;
							break;
						case SDLK_d:
							keyschanged = true;
							currentkeys |= EKM_Right;
							break;
						case SDLK_ESCAPE:
						{
							MenuPointer menu = Menu::get("main");
							menu->setActive(true);
						}
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
							keyschanged = true;
							currentkeys &= ~EKM_Up;
							break;
						case SDLK_s:
							keyschanged = true;
							currentkeys &= ~EKM_Down;
							break;
						case SDLK_a:
							keyschanged = true;
							currentkeys &= ~EKM_Left;
							break;
						case SDLK_d:
							keyschanged = true;
							currentkeys &= ~EKM_Right;
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
							keyschanged = true;
							currentkeys |= EKM_Shoot;
							shooting = true;
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
							keyschanged = true;
							currentkeys &= ~EKM_Shoot;
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
		if (shooting && !(currentkeys & EKM_Shoot))
		{
			shootonce = true;
		}
		// Send input to player
		PlayerPointer localplayer = Player::getLocalPlayer();
		if (localplayer)
		{
			if (keyschanged || shootonce)
			{
				// Send keyboard info
				if (shooting)
					localplayer->sendKeys(currentkeys | EKM_Shoot);
				else
					localplayer->sendKeys(currentkeys);
				if (shootonce)
				{
					shootonce = false;
				}
			}
			if (rotationchanged)
			{
				// Send mouse info
				localplayer->sendRotation(rotation);
			}
		}
		return true;
	}

	Input::Input()
	{
		currentkeys = 0;
	}
}
