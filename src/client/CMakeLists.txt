
include_directories(../../include ../../include/support ../../include/client ${LUA_INCLUDE_DIR} ${SDL_INCLUDE_DIR})

set(EXECUTABLE_OUTPUT_PATH ../..)

add_executable(backlot ${CLIENT_SRC})
set_target_properties(backlot PROPERTIES COMPILE_DEFINITIONS CLIENT)

if(WIN32)
	target_link_libraries(backlot SDL SDL_image SDL_mixer guichan_opengl guichan_sdl guichan opengl32 glu32 glew32 enet ws2_32 winmm luabindd ${LUA_LIBRARIES})
else(WIN32)
	target_link_libraries(backlot SDL SDL_image SDL_mixer guichan_opengl guichan_sdl guichan GL glut GLEW enet luabind ${LUA_LIBRARIES})
endif(WIN32)
