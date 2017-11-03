cmake_minimum_required(VERSION 3.0)

set(ENGINE_VERSION_MAJOR "0")
set(ENGINE_VERSION_MINOR "1")
set(ENGINE_VERSION_RELEASE "4")

if (GAME_NAME)
	project("${GAME_NAME}" CXX C)

	set(GAME_VERSION_MAJOR "${GAME_VERSION_MAJOR}")
	set(GAME_VERSION_MINOR "${GAME_VERSION_MINOR}")
	set(GAME_VERSION_RELEASE "${GAME_VERSION_RELEASE}")
else()
	project("BasicEventEngine" CXX C)

	set(GAME_VERSION_MAJOR "${ENGINE_VERSION_MAJOR}")
	set(GAME_VERSION_MINOR "${ENGINE_VERSION_MINOR}")
	set(GAME_VERSION_RELEASE "${ENGINE_VERSION_RELEASE}")
endif()

# Generate a unique BEE_GAME_ID in ./build.sh
if (NOT BEE_BUILD_ID)
	set(BEE_BUILD_ID "0")
endif()
if (NOT BEE_GAME_ID)
	set(BEE_GAME_ID "11111111")
endif()

set(engine_defines "-DBEE_VERSION_MAJOR=${ENGINE_VERSION_MAJOR} -DBEE_VERSION_MINOR=${ENGINE_VERSION_MINOR} -DBEE_VERSION_RELEASE=${ENGINE_VERSION_RELEASE}")
set(game_defines "-DBEE_BUILD_ID=${BEE_BUILD_ID} -DBEE_GAME_ID=${BEE_GAME_ID}")
set(game_defines "${game_defines} -DGAME_NAME=${PROJECT_NAME} -DGAME_VERSION_MAJOR=${GAME_VERSION_MAJOR} -DGAME_VERSION_MINOR=${GAME_VERSION_MINOR} -DGAME_VERSION_RELEASE=${GAME_VERSION_RELEASE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${engine_defines}")

set(warns "")

include_directories("lib/doctest/doctest/" "lib/bullet3/src")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/lib/")

if (WIN32) # Include Windows libraries manually
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SRC}/win/include")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\"")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /MT")

	add_subdirectory("bee")

	add_subdirectory("tests")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${game_defines}")

	add_executable(${PROJECT_NAME} main.cpp resources/resources.hpp)
	target_link_libraries(${PROJECT_NAME} BEE)
	target_link_libraries(${PROJECT_NAME} BEE_Tests)

	# Include SDL2 libraries
	foreach(l SDL2_image SDL2_ttf SDL2_mixer SDL2_net SDL2)
		target_link_libraries(${PROJECT_NAME} "${SRC}/win/lib/${l}.lib")
	endforeach(l)

	# Include OpenGL libraries
	set(GLEW_INCLUDE_DIR ${SRC}/win/include)
	set(GLEW_LIBRARY ${SRC}/win/lib/glew32s.lib)
	set(GLUT_INCLUDE_DIR ${SRC}/win/include)
	set(GLUT_glut_LIBRARY ${SRC}/win/lib/freeglut.lib)

	set(ASSIMP_INCLUDE_DIRECTORIES ${SRC}/win/include)
	set(ASSIMP_LIBRARIES ${SRC}/win/lib/libassimp.dll.a)

	set(BULLET_DIR ${${PROJECT_NAME}_SOURCE_DIR}/lib/bullet3)
	set(BULLET_INCLUDE_DIRS ${BULLET_DIR}/src)
	set(
		BULLET_LIBRARIES

		${BULLET_DIR}/bin/BulletDynamics.lib
		${BULLET_DIR}/bin/BulletCollision.lib
		${BULLET_DIR}/bin/LinearMath.lib
	)

	find_package(GLEW REQUIRED)
	find_package(GLUT REQUIRED)
	find_package(OpenGL REQUIRED)

	include_directories(${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRECTORIES} ${OPENGL_INCLUDE_DIRECTORIES} ${ASSIMP_INCLUDE_DIRECTORIES} ${BULLET_INCLUDE_DIRECTORIES})
	target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${ASSIMP_LIBRARIES} ${BULLET_LIBRARIES})
else() # Include Unix libraries automatically
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(warns "-Wall -Wextra -Wpedantic -Wshadow -Wno-unused-parameter")
	endif()

	set(packaged_lib_dir "-Wl,-rpath='\$ORIGIN/lib/'") # ./lib (relative to the executable) will be search for dynamic library loading
	set(CMAKE_CXX_FLAGS_DEBUG "${packaged_lib_dir} -g -pg -pipe -std=c++14 ${warns} -D_REENTRANT")
	set(CMAKE_CXX_FLAGS_RELEASE "${packaged_lib_dir} -O2 -pipe -std=c++14 ${warns} -D_REENTRANT -DNDEBUG")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
	endif()

	# Enable link time optimization if it is supported
	if(GXX_HAS_LTO_FLAG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
	endif()

	add_subdirectory("bee")

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_subdirectory("tests")
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${game_defines}")

	add_subdirectory("resources")
	add_executable(${PROJECT_NAME} main.cpp)
	target_link_libraries(${PROJECT_NAME} BEE)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(${PROJECT_NAME} BEE_Tests)
	endif()
	target_link_libraries(${PROJECT_NAME} Resources)

	# Include SDL2 related libraries
	include(FindPkgConfig)

	pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
	pkg_search_module(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
	pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)
	pkg_search_module(SDL2NET REQUIRED SDL2_net>=2.0.0)
	pkg_search_module(SDL2 REQUIRED sdl2>=2.0.0)

	include_directories(${SDL2IMAGE_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR} ${SDL2MIXER_INCLUDE_DIR} ${SDL2NET_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} )
	target_link_libraries(${PROJECT_NAME} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${SDL2NET_LIBRARIES} ${SDL2_LIBRARIES})

	# Include OpenGL related libraries
	find_package(GLEW REQUIRED)
	find_package(GLUT REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(ASSIMP REQUIRED)

	set(BULLET_SRC ${${PROJECT_NAME}_SOURCE_DIR}/lib/bullet3/src)

	set(BULLET_INCLUDE_DIRS ${BULLET_SRC})
	set(
		BULLET_LIBRARIES

		${BULLET_SRC}/BulletDynamics/libBulletDynamics.a
		${BULLET_SRC}/BulletCollision/libBulletCollision.a
		${BULLET_SRC}/LinearMath/libLinearMath.a
	)

	include_directories(${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRECTORIES} ${OPENGL_INCLUDE_DIRECTORIES} ${ASSIMP_INCLUDE_DIRECTORIES} ${BULLET_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${ASSIMP_LIBRARIES} ${BULLET_LIBRARIES})
endif()
