
project(mapeditor)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)

set(QT_USE_OPENGL TRUE)
include(${QT_USE_FILE})

set(SRC
	src/main.cpp
	src/EditorWindow.cpp
	src/MapView.cpp
	src/TilePreview.cpp
)

set(MOC_H
	include/EditorWindow.hpp
	include/MapView.hpp
	include/TilePreview.hpp
)

#qt4_automoc(${SRC})
QT4_WRAP_CPP(MOC_SRC ${MOC_H})

set(UIS
	MainWindow.ui
)

QT4_WRAP_UI(UIS_H ${UIS})

include_directories(include ${CMAKE_CURRENT_BINARY_DIR} ${QT_QTOPENGL_INCLUDE_DIR})

add_executable(mapeditor ${SRC} ${UIS_H} ${MOC_SRC})
target_link_libraries(mapeditor ${QT_LIBRARIES} ${QT_QTOPENGL_LIBRARIES})
