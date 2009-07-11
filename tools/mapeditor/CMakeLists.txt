
project(mapeditor)

cmake_minimum_required(VERSION 2.4.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")

find_package(Qt4 REQUIRED)


set(QT_USE_OPENGL TRUE)
include(${QT_USE_FILE})

set(SRC
	src/main.cpp
	src/EditorWindow.cpp
	src/MapView.cpp
	src/TilePreview.cpp
	src/Map.cpp
	src/Game.cpp
	src/TileSet.cpp
	src/Tile.cpp
	src/NewMapDialog.cpp
	src/OpenMapDialog.cpp
	../../src/support/tinystr.cpp
	../../src/support/tinyxml.cpp
	../../src/support/tinyxmlerror.cpp
	../../src/support/tinyxmlparser.cpp
)

set(MOC_H
	include/EditorWindow.hpp
	include/MapView.hpp
	include/TilePreview.hpp
	include/Map.hpp
	include/NewMapDialog.hpp
	include/OpenMapDialog.hpp
)

QT4_WRAP_CPP(MOC_SRC ${MOC_H})

set(QRC
	MapEditor.qrc
)

QT4_ADD_RESOURCES(QRC_SRC ${QRC})

set(UIS
	ui/MainWindow.ui
	ui/NewDialog.ui
	ui/OpenDialog.ui
)

QT4_WRAP_UI(UIS_H ${UIS})

include_directories(include ../../include/support ${CMAKE_CURRENT_BINARY_DIR} ${QT_QTOPENGL_INCLUDE_DIR})

add_executable(mapeditor ${SRC} ${UIS_H} ${MOC_SRC} ${QRC_SRC})
target_link_libraries(mapeditor ${QT_LIBRARIES} ${QT_QTOPENGL_LIBRARIES})
