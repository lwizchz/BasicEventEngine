Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>

[![Build Status](https://travis-ci.org/piluke/BasicEventEngine.svg?branch=master)](https://travis-ci.org/piluke/BasicEventEngine)

BasicEventEngine is an event-driven game engine which the user can interface with via the Sprite, Sound, Background, Font, Path, Object, and Room classes. The Room class is used to organize the resources for each section of the game and the Object class is used to act upon the events from the main loop. A small example is included with all non-engine code in the resources directory.

The available events are the virtual functions of the Object class which are on lines 101 to 127 in the file bee/resource_structures/object.hpp

The software is in early alpha so do not expect any sort of stability. If you have any questions or comments, feel free to email me. Also feel free to contribute to the engine code, either by email or via github.

## How to use on Linux

This program can compile under Arch Linux, Ubuntu 15.10, and Windows 10 but at the moment support will only be offered for Linux.

1. [Intall git-lfs][1] so that you can download the resources for the example.

        # For Arch Linux, it's available in the AUR
        pacaur -y git-lfs

2. Install the required libraries:

        # Arch Linux
        sudo pacman -S cmake sdl2 sdl2_image sdl2_ttf sdl2_mixer sdl2_net glew glm freeglut libxmu
        # Ubuntu
        sudo apt-get install cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-net-dev libglew-dev libglm-dev freeglut3-dev libxmu-dev

3. Download the additional library submodules:

        git submodule update --init --recursive

4. *(Optional)* Edit the file "resources/resources.hpp" in order to add or remove resources which are located in their corresponding subdirectory.

5. Create a build directory and compile the program with the following commands:

        mkdir build
        cd build
        cmake ..
        make

6. Run the program by moving back to the top directory:

        cd ..
        ./build/BasicEventEngine

## How to use on Windows *(Outdated)*

Even though this program can compile on Windows 10, I am not offering any support for any Windows platform because it is terribly annoying to get anything to compile correctly. If you dare attempt to compile on Windows, then follow the below instructions. Hopefully in the near future I'll be able to get CMake to work properly on Windows.

1. [Download and install Git LFS][2] for Windows to be able to download the resources for the example.

2. Install the required libraries from their official websites: SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, SDL2_net, GLEW, GLM.
The project file assumes that each of these libraries has its DLL in the same directory as the executable (e.g. win/bin) and has its header file in a win/include subdirectory

3. [Install Dev-C++][3] then use it to open the project file BasicEventEngine.dev

4. Edit the file "resources/resources.hpp" in order to add or remove resources which are located in their corresponding subfolder.

5. Compile and run the program with Execute>Compile & Run

This project is under the MIT License so feel free to do basically whatever you want with the code.

[1]: https://git-lfs.github.com/                                "Git LFS instructions"
[2]: https://github.com/github/git-lfs/releases/latest          "Git LFS for Windows"
[3]: http://orwelldevcpp.blogspot.com/                          "Dev-C++"
