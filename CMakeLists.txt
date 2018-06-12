cmake_minimum_required(VERSION 3.0)

if(COMMAND cmake_policy)
	if(POLICY CMP0072)
		cmake_policy(SET CMP0072 NEW)
	endif(POLICY CMP0072)
endif(COMMAND cmake_policy)

project("${GAME_NAME}" CXX C)

set(warns "")

include_directories("lib/doctest/doctest/" "lib/bullet3/src" "lib/cpython/Include")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/lib/")

if (WIN32) # Include Windows libraries manually
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SRC}/win/include")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\"")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /MT")

	add_subdirectory("bee")

	add_subdirectory("tests")

	add_subdirectory("resources")
	add_executable(${PROJECT_NAME} main.cpp)
	target_link_libraries(${PROJECT_NAME} BEE)
	target_link_libraries(${PROJECT_NAME} BEE_Tests)
	target_link_libraries(${PROJECT_NAME} Resources)

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
		set(warns "${warns} -D_GLIBCXX_ASSERTIONS -fasynchronous-unwind-tables -fexceptions -fstack-clash-protection -fstack-protector-strong")
		set(warns "${warns} -Wl,-z,defs -Wl,-z,now -Wl,-z,relro")
	endif()

	set(packaged_lib_dir "-Wl,-rpath='\$ORIGIN/lib/'") # ./lib (relative to the executable) will be searched for dynamic library loading
	set(CMAKE_CXX_FLAGS_DEBUG "${packaged_lib_dir} -g -pg -pipe -std=c++14 ${warns} -D_REENTRANT")
	set(CMAKE_CXX_FLAGS_RELEASE "${packaged_lib_dir} -O2 -pipe -std=c++14 ${warns} -D_REENTRANT -DNDEBUG -D_FORTIFY_SOURCE=2")
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
	find_package(Threads REQUIRED)

	set(BULLET_SRC ${${PROJECT_NAME}_SOURCE_DIR}/lib/bullet3/src)
	set(BULLET_INCLUDE_DIRS ${BULLET_SRC})
	set(
		BULLET_LIBRARIES

		${BULLET_SRC}/BulletDynamics/libBulletDynamics.a
		${BULLET_SRC}/BulletCollision/libBulletCollision.a
		${BULLET_SRC}/LinearMath/libLinearMath.a
	)

	set(CPYTHON_DIR ${${PROJECT_NAME}_SOURCE_DIR}/lib/cpython/)
	set(
		CPYTHON_LIBRARIES

		${CPYTHON_DIR}/libpython3.7m.a
		${CMAKE_DL_LIBS}
		${CMAKE_THREAD_LIBS_INIT}
		util
	)

	include_directories(${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRECTORIES} ${OPENGL_INCLUDE_DIRECTORIES} ${ASSIMP_INCLUDE_DIRECTORIES})
	target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${ASSIMP_LIBRARIES} ${BULLET_LIBRARIES} ${CPYTHON_LIBRARIES})
endif()
