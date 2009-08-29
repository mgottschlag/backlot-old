
cmake_minimum_required(VERSION 2.6)
project(backlot)

if(WIN32)
	set(LUA_INCLUDE_DIR C:\\Lua\\5.1\\include)
	set(LUA_LIBRARIES lua51)
	set(SDL_INCLUDE_DIR C:/MinGW/include/SDL)
	add_definitions(-DGUICHAN_DLL_IMPORT)
else(WIN32)
	find_package(Lua51)
	find_package(SDL)
endif(WIN32)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")

set(SRC_SHARED
../Preferences.cpp
../Map.cpp
../TileSet.cpp
../Buffer.cpp
../Script.cpp
../Timer.cpp
../entity/EntityTemplate.cpp
../entity/EntityList.cpp
../entity/Property.cpp
../support/tinystr.cpp
../support/tinyxml.cpp
../support/tinyxmlerror.cpp
../support/tinyxmlparser.cpp
)

set(CLIENT_SRC
${SRC_SHARED}
main.cpp
Engine.cpp
graphics/Graphics.cpp
Input.cpp
graphics/Texture.cpp
Animation.cpp
Audio.cpp
Sound.cpp
Music.cpp
Effect.cpp
Server.cpp
Client.cpp
graphics/QuadBatch.cpp
graphics/Camera.cpp
graphics/RenderTarget.cpp
HUD.cpp
HUDElement.cpp
menu/Menu.cpp
graphics/Font.cpp
menu/GuichanFont.cpp
menu/Dialog.cpp
menu/ListModel.cpp
menu/SimpleButton.cpp
menu/MenuStyle.cpp
menu/MenuElement.cpp
menu/MenuButton.cpp
menu/MenuImage.cpp
menu/MenuImageButton.cpp
entity/Entity.cpp
Game.cpp
entity/EntityImage.cpp
SplashScreen.cpp
../support/ScreenPosition.cpp
)

set(SERVER_SRC
${SRC_SHARED}
main.cpp
Engine.cpp
Server.cpp
Client.cpp
Game.cpp
entity/Entity.cpp
entity/EntityState.cpp
)

add_subdirectory(src/client)
add_subdirectory(src/server)
