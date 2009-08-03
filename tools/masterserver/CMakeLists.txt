
project(masterserver)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)
set(QT_USE_NETWORK TRUE)
set(QT_USE_OPENGL FALSE)
set(QT_USE_GUI FALSE)

include(${QT_USE_FILE})

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")

set(SRC
	src/main.cpp
	src/Server.cpp
	src/Client.cpp
)

set(MOC_H
	include/Client.hpp
)

QT4_WRAP_CPP(MOC_SRC ${MOC_H})


include_directories(include moc ${CMAKE_CURRENT_BINARY_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_INCLUDE_DIR})

add_executable(masterserver  ${SRC} ${MOC_SRC})
target_link_libraries(masterserver ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})

