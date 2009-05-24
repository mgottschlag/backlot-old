
cmake_minimum_required(VERSION 2.6)

project(backlot)

set(SRC
src/main.cpp
src/Engine.cpp
src/Preferences.cpp
src/Graphics.cpp
src/Input.cpp
src/Map.cpp
src/Texture.cpp
src/TileSet.cpp
src/Audio.cpp
src/Sound.cpp
src/Music.cpp
src/Server.cpp
src/Client.cpp
src/QuadBatch.cpp
src/Camera.cpp
src/support/tinystr.cpp
src/support/tinyxml.cpp
src/support/tinyxmlerror.cpp
src/support/tinyxmlparser.cpp
)

set(CMAKE_CXX_FLAGS "-Wall")

include_directories(include include/support)
add_executable(backlot ${SRC})
target_link_libraries(backlot SDL SDL_image SDL_mixer GL glut GLEW)

