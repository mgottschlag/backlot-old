
cmake_minimum_required(VERSION 2.6)

project(backlot)

set(SRC
src/main.cpp
src/Engine.cpp
src/Graphics.cpp
src/Input.cpp
src/Map.cpp
src/Texture.cpp
src/TileSet.cpp
src/Audio.cpp
src/Server.cpp
src/Client.cpp
)

set(CMAKE_CXX_FLAGS "-Wall")

include_directories(include)
add_executable(backlot ${SRC})
target_link_libraries(backlot SDL SDL_image GL glut)
