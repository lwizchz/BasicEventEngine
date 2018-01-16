Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>

[![Build Status](https://travis-ci.org/piluke/BasicEventEngine.svg?branch=master)](https://travis-ci.org/piluke/BasicEventEngine)

![In-Game Screenshot](https://github.com/piluke/BasicEventEngine/raw/master/screenshot.png)

BasicEventEngine is an event-driven game engine which the user can interface
with via the Sprite, Sound, Background, Font, Path, Timeline, Mesh, Light,
Object, and Room classes. The Room class is used to organize the resources for
each section of the game and the Object class is used to act upon the events
from the main loop. A small example is included in the resources directory.

The available events are the virtual functions of the Object class which are on
lines 100 to 128 in the file ``bee/resources/object.hpp``

The software is in early alpha so do not expect any sort of stability. I'm
slowly adding more content to the wiki so be sure to check that out first but
if you still have any questions or comments, feel free to email me. Also feel
free to contribute to the engine code or make feature requests, preferably via
Github. And above all, report bugs! :)

## How to use on Linux

This program can compile under Arch Linux, Ubuntu 15.10, and Windows 10 but at
the moment support will only be offered for Linux.

1. [Intall git-lfs][1] so that you can download the resources for the example.

        # For Arch Linux, it's available in the AUR
        pacaur -y git-lfs

2. Install the required libraries:

        # Arch Linux
        sudo pacman -S cmake sdl2 sdl2_image sdl2_ttf sdl2_mixer sdl2_net glew glm freeglut libxmu assimp
        # Ubuntu
        sudo apt-get install cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-net-dev libglew-dev libglm-dev freeglut3-dev libxmu-dev libassimp-dev

3. Download the additional library submodules:

        git submodule update --init --recursive

4. *(Optional)* Edit the file ``resources/resources.hpp`` in order to add or remove resources which are located in their corresponding subdirectory.

5. Compile and run the program with the following commands:

        # Build with debug mode in the build/ subdirectory
        ./build.sh debug build

See [the wiki page][2] for additional uses of `./build.sh`:

## How to use on Windows

Even though this program can compile on Windows 10, I am not offering any
support for any Windows platform because it is terribly annoying to get
anything to compile correctly. If you dare attempt to compile on Windows, then
follow the below instructions.

*Compiling on Windows 10 is actually much easier now with the latest CMake and Visual Studio.*

1. *(Recommended)* Clone the project with [Github Desktop][3] for Windows to be able to easily download the resources for the example.

2. Install the required libraries from their official websites: SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, SDL2_net, GLEW, GLM, and ASSIMP.
The current project setup assumes that each of these libraries has its DLL in the same directory as the executable (e.g. ``win/bin``), its header file in a ``win/include`` subdirectory, and its .lib file in ``win/lib``
Feel free to download the libraries from [here][4] and extract them to ``win/``.

3. [Install CMake][5] and [Visual Studio Community 2017][6] (or your favorite CMake-compatible IDE).

4. *(Optional)* Edit the file ``resources/resources.hpp`` in order to add or remove resources which are located in their corresponding subdirectory.

5. Build Bullet by running ``lib/bullet3/build_visual_studio_without_pybullet_vr.bat`` and then compiling BulletCollision, BulletDynamics, BulletSoftBody, and LinearMath from the generated Visual Studio Solution.

6. Open the CMake GUI and set the appropriate source and build directories, e.g. ``C:\Users\Luke\Documents\GitHub\BasicEventEngine`` and ``C:\Users\Luke\Documents\GitHub\BasicEventEngine\build``.

7. Configure the project and select the appropriate build system, e.g. ``Visual Studio 15 2017``, and then Generate the project.

8. Open the Visual Studio solution file from ``build/BasicEventEngine.sln``.

9. Build the ALL_BUILD target generated by CMake.

10. Run the compiled executable by copying it to the directory which contains the required resources and DLLs, e.g. ``win/bin``.

This project is under the MIT License so feel free to do basically whatever you want with the code.

[1]: https://git-lfs.github.com/                               "Git LFS instructions"
[2]: https://github.com/piluke/BasicEventEngine/wiki/build.sh  "BEE Wiki for build.sh"
[3]: https://desktop.github.com/                               "Github Desktop for Windows"
[4]: https://lukemontalvo.us/BasicEventEngine/win.zip          "Windows Library Files"
[5]: https://cmake.org/download/                               "CMake"
[6]: https://www.visualstudio.com/                             "Visual Studio"
