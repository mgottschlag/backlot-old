
cmake_minimum_required(VERSION 2.6)

project(backlot)

set(CLIENT_SRC
src/client/main.cpp
src/client/Engine.cpp
src/Preferences.cpp
src/client/Graphics.cpp
src/client/Input.cpp
src/Map.cpp
src/client/Texture.cpp
src/TileSet.cpp
src/client/Audio.cpp
src/client/Sound.cpp
src/client/Music.cpp
src/client/Server.cpp
src/client/Client.cpp
src/client/QuadBatch.cpp
src/client/Camera.cpp
src/Buffer.cpp
src/client/Player.cpp
src/support/tinystr.cpp
src/support/tinyxml.cpp
src/support/tinyxmlerror.cpp
src/support/tinyxmlparser.cpp
)

set(CMAKE_CXX_FLAGS "-Wall")

include_directories(include include/support)
add_executable(backlot ${CLIENT_SRC})
set_target_properties(backlot PROPERTIES COMPILE_DEFINITIONS CLIENT)
set_target_properties(backlot PROPERTIES COMPILE_FLAGS -Iinclude/client)

IF(WIN32)
	target_link_libraries(backlot SDL SDL_image SDL_mixer opengl32 glu32 glew32 enet ws2_32 winmm)
ELSE(WIN32)
	target_link_libraries(backlot SDL SDL_image SDL_mixer GL glut GLEW enet)
ENDIF(WIN32)

set(SERVER_SRC
src/server/main.cpp
src/server/Engine.cpp
src/Preferences.cpp
src/Map.cpp
src/TileSet.cpp
src/server/Server.cpp
src/server/Client.cpp
src/server/Player.cpp
src/Buffer.cpp
src/support/tinystr.cpp
src/support/tinyxml.cpp
src/support/tinyxmlerror.cpp
src/support/tinyxmlparser.cpp
)

add_executable(server ${SERVER_SRC})
set_target_properties(server PROPERTIES COMPILE_DEFINITIONS SERVER)
set_target_properties(server PROPERTIES COMPILE_FLAGS -Iinclude/server)
target_link_libraries(server enet)

IF(WIN32)
	target_link_libraries(server enet ws2_32 winmm)
ELSE(WIN32)
	target_link_libraries(server enet)
ENDIF(WIN32)
