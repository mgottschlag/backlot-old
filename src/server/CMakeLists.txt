
include_directories(../../include ../../include/support ../../include/server ${LUA_INCLUDE_DIR})

set(EXECUTABLE_OUTPUT_PATH ../..)

add_executable(server ${SERVER_SRC})
set_target_properties(server PROPERTIES COMPILE_DEFINITIONS SERVER)

if(WIN32)
	target_link_libraries(server enet ws2_32 winmm luabindd ${LUA_LIBRARIES})
else(WIN32)
	target_link_libraries(server enet luabind ${LUA_LIBRARIES})
endif(WIN32)
