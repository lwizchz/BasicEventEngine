BasicEventEngine is an event-driven game engine which the user can interface with via the Sprite, Sound, Background, Font, Path, Object, and Room classes. The Room class is used to organize the resources for each section of the game and the Object class is used to act upon the events from the main loop. A small example is included with all non-engine code in the resources directory.

The available events are the virtual functions of the Object class which are on lines 59 to 80 in the file bee/resource_structures/object.hpp

The software is in early alpha so do not expect any sort of stability. If you have any questions or comments, feel free to email me. Also feel free to contribute to the engine code, either by email or via github.

## How to use

This program has only been compiled under Arch Linux and Ubuntu 15.04 and at the moment, no official support will be given to other platforms.
**Note that as of Aug 2015 I am unable to build it under Ubuntu but you may have better luck**

1. Install the required libraries:

        # Arch Linux
        sudo pacman -S sdl2 sdl2_image sdl2_ttf sdl2_mixer sdl2_net
        # Ubuntu
        sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-net-dev

2. (Optional) Install ChaiScript [from their website][1] in order to use scripting.

3. Download the example resource files [from my website][2] and extract the tarball to the same folder, keeping the folder structure.

	I'm waiting on acceptance into GitHub's large file storage closed beta, after which I will include the example resources with the rest of the project.

4. Edit the file "resources/resources.hpp" in order to add or remove resources which are located in their corresponding subfolder.

5. Compile the program with the `make` command, optionally the `run` target can be passed to it in order to automatically run the program after a successful compilation.

This project is under the MIT License so feel free to do basically whatever you want with the code.

[1]: https://github.com/ChaiScript/ChaiScript/releases          "ChaiScript download"
[2]: http://lukemontalvo.us/BasicEventEngine/resources.tar.gz	"example resources"
