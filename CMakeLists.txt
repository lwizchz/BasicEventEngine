cmake_minimum_required(VERSION 3.0)
project("BasicEventEngine" CXX)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_RELEASE "1")

# Generate a unique BEE_GAME_ID
### TODO
set(BEE_GAME_ID "11111111")

set(warns "-Wall -Wextra -Wpedantic -Wshadow -Wno-unused-parameter -Wno-unused-variable")
set(defines "-DBEE_GAME_ID=${BEE_GAME_ID} -DBEE_VERSION_MAJOR=${VERSION_MAJOR} -DBEE_VERSION_MINOR=${VERSION_MINOR} -DBEE_VERSION_RELEASE=${VERSION_RELEASE}")
set(CMAKE_CXX_FLAGS "-g -pg -pipe -std=c++11 ${warns} -D_REENTRANT ${defines}")

# Enable link time optimization if it is supported
if(GXX_HAS_LTO_FLAG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(bee)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} BEE)

# Include SDL2 libraries
include(FindPkgConfig)

pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)
pkg_search_module(SDL2NET REQUIRED SDL2_net>=2.0.0)
pkg_search_module(SDL2 REQUIRED sdl2>=2.0.0)

include_directories(${SDL2IMAGE_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR} ${SDL2MIXER_INCLUDE_DIR} ${SDL2NET_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} )
target_link_libraries(${PROJECT_NAME} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${SDL2NET_LIBRARIES} ${SDL2_LIBRARIES})

# Include OpenGL libraries
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRECTORIES} ${OPENGL_INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
