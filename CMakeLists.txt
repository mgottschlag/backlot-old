
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

set(CLIENT_SRC
src/client/main.cpp
src/client/Engine.cpp
src/Preferences.cpp
src/client/Graphics.cpp
src/client/Input.cpp
src/Map.cpp
src/client/Texture.cpp
src/TileSet.cpp
src/client/Animation.cpp
src/client/Audio.cpp
src/client/Sound.cpp
src/client/Music.cpp
src/client/Effect.cpp
src/client/Server.cpp
src/client/Client.cpp
src/client/QuadBatch.cpp
src/client/Camera.cpp
src/Buffer.cpp
src/client/Player.cpp
src/client/Weapon.cpp
src/client/Bullet.cpp
src/Script.cpp
src/client/Menu.cpp
src/client/Font.cpp
src/client/GuichanFont.cpp
src/support/tinystr.cpp
src/support/tinyxml.cpp
src/support/tinyxmlerror.cpp
src/support/tinyxmlparser.cpp
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")

include_directories(include include/support ${LUA_INCLUDE_DIR})
add_executable(backlot ${CLIENT_SRC})
set_target_properties(backlot PROPERTIES COMPILE_DEFINITIONS CLIENT)
set_target_properties(backlot PROPERTIES COMPILE_FLAGS "-Iinclude/client -I${SDL_INCLUDE_DIR}")

if(WIN32)
	target_link_libraries(backlot SDL SDL_image SDL_mixer guichan_opengl guichan_sdl guichan opengl32 glu32 glew32 enet ws2_32 winmm ${LUA_LIBRARIES})
else(WIN32)
	target_link_libraries(backlot SDL SDL_image SDL_mixer guichan_opengl guichan_sdl guichan GL glut GLEW enet ${LUA_LIBRARIES})
endif(WIN32)

set(SERVER_SRC
src/server/main.cpp
src/server/Engine.cpp
src/Preferences.cpp
src/Map.cpp
src/TileSet.cpp
src/server/Server.cpp
src/server/Client.cpp
src/server/Player.cpp
src/server/Weapon.cpp
src/Buffer.cpp
src/server/Bullet.cpp
src/Script.cpp
src/support/tinystr.cpp
src/support/tinyxml.cpp
src/support/tinyxmlerror.cpp
src/support/tinyxmlparser.cpp
)

add_executable(server ${SERVER_SRC})
set_target_properties(server PROPERTIES COMPILE_DEFINITIONS SERVER)
set_target_properties(server PROPERTIES COMPILE_FLAGS -Iinclude/server)
target_link_libraries(server enet)

if(WIN32)
	target_link_libraries(server enet ws2_32 winmm ${LUA_LIBRARIES})
else(WIN32)
	target_link_libraries(server enet ${LUA_LIBRARIES})
endif(WIN32)
