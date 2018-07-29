#!/bin/bash
# Builds BEE with CMake

# Change to the top source directory
ls "`pwd`" | grep "build.sh"
if [ $? -ne 0 ]; then
        cd ..
fi

source config.sh

download_dependencies()
{
        git submodule update --init --recursive
}
build_dependencies()
{
        download_dependencies

        # Build Bullet
        cd lib/bullet3
        cp ../bullet.CMakeLists.txt ./CMakeLists.txt
        cmake .
        make -j5

        # Configure CPython
        cd ../cpython
        if [ ! -f ./pyconfig.h ]; then
                ./configure
                cp pyconfig.h Include/pyconfig.h
        fi
        if [ ! -f ./libpython3.7m.a ]; then
                make -j5
        fi

        cd ../..
}
clean_dependencies()
{
        echo "cleaning dependencies..."

        # Clean Bullet
        cd lib/bullet3
        git clean -fd
        git reset HEAD --hard

        # Clean CPython
        cd ../cpython
        rm pyconfig.h Include/pyconfig.h libpython3.7m.a python
        rm -r build
        git clean -fd
        git reset HEAD --hard

        cd ../..
}

clean()
{
        echo "cleaning dir: $1"

        cd "$1/.."

        rm -r "$1"
}

generate_build_id()
{
        cd "$1"
        cd ..

        files="$(find resources/ -type f -exec grep -Iq . {} \; -and -print)"
        id="$(cat $files | sha256sum | head -c 64)"

        echo "$id"
}
generate_game_id()
{
        bits=6

        cd "$1"
        cd ..

        files="$(find resources/ -type f -exec grep -Iq . {} \; -and -print)"
        sum="$(cat $files | sha256sum | head -c $bits)"
        id="$((0x${sum}))"

        echo "$id"
}
output_game_defines()
{
    cat > "defines.hpp" <<-DEFINES
/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

// The below definitions are auto-generated and all changes will be overwritten
// To change the game name and version, edit config.sh

#ifndef DEFINES_H
#define DEFINES_H 1

#define BEE_BUILD_ID $1
#define BEE_GAME_ID $2

#define GAME_NAME $3
#define GAME_VERSION_MAJOR $4
#define GAME_VERSION_MINOR $5
#define GAME_VERSION_PATCH $6

#endif // DEFINES_H
DEFINES

    diff "defines.hpp" "../defines.hpp" > /dev/null
    if [ $? -ne 0 ]; then
        cp "defines.hpp" "../defines.hpp"
    fi
}

assert_build_type()
{
        if [ -f "$2/Makefile" ]; then
                touch "${2}/last_build_type.txt"
                if [[ $1 != $(cat "${2}/last_build_type.txt") ]]; then
                        rm "$2/Makefile"
                fi
        fi
}

debug()
{
        echo "debug dir: $1"

        mkdir -p "$1"
        cd "$1"

        build_id=$(generate_build_id "$1")
        echo "build id: $build_id"
        game_id=$(generate_game_id "$1")
        echo "game id: $game_id"

        assert_build_type "debug" "$1"
        echo -n "debug" > "$1/last_build_type.txt"

        output_game_defines $build_id $game_id "$game" $version_major $version_minor $version_patch

        cmake -DCMAKE_BUILD_TYPE=Debug -DGAME_NAME="$game" ..

        if [ "$2" == "nomake" ]; then
                return
        fi

        make -j5
        if [ $? -ne 0 ]; then
                echo "Debug build failed!"
                exit 3
        fi

        cd ..
        if [ "$2" == "norun" ]; then
                return
        fi

        if [ "$2" ]; then
                exec $2 "$1/$game"
        else
                exec "$1/$game"
        fi
}
release()
{
        echo "release dir: $1"

        mkdir -p "$1"
        cd "$1"

        build_id=$(generate_build_id "$1")
        echo "build id: $build_id"
        game_id=$(generate_game_id "$1")
        echo "game id: $game_id"

        assert_build_type "release" "$1"
        echo -n "release" > "$1/last_build_type.txt"

        output_game_defines $build_id $game_id "$game" $version_major $version_minor $version_patch

        cmake -DCMAKE_BUILD_TYPE=Release -DGAME_NAME="$game" ..

        if [ "$2" == "nomake" ]; then
                return
        fi

        make -j5
        if [ $? -ne 0 ]; then
                echo "Release build failed!"
                exit 2
        fi

        strip "$game"

        cd ..
        if [ "$2" == "norun" ]; then
                return
        fi

        exec "$1/$game"
}

build_dir="./build"
if [ -n "$2" ]; then
        build_dir="$2"
fi
build_dir="$(readlink -f ${build_dir})"

if [ -z "$1" ] || [ "$1" == "release" ]; then
        build_dependencies
        release "$build_dir"
elif [ "$1" == "nomake" ]; then
        build_dependencies
        if [ -f "${build_dir}/last_build_type.txt" ]; then
                $(cat "${build_dir}/last_build_type.txt") "$build_dir" "nomake"
        else
                release "$build_dir" "nomake"
        fi
elif [ "$1" == "norun" ]; then
        build_dependencies
        if [ -f "${build_dir}/last_build_type.txt" ]; then
                $(cat "${build_dir}/last_build_type.txt") "$build_dir" "norun"
        else
                release "$build_dir" "norun"
        fi
elif [ "$1" == "debug" ]; then
        build_dependencies
        if [ -n "$3" ]; then
                debug "$build_dir" "$3"
        else
                debug "$build_dir"
        fi
elif [ "$1" == "clean" ]; then
        clean_dependencies
        clean "$build_dir"
else
        echo "Invalid argument: \"$1\""
        exit 1
fi

exit 0
