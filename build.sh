#!/bin/bash
# Builds BEE with CMake

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

        cd ../..
}
clean_dependencies()
{
        echo "cleaning dependencies..."

        # Clean Bullet
        cd lib/bullet3
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

        cmake \
                -DCMAKE_BUILD_TYPE=Debug \
                -DGAME_NAME="$game" \
                -DBEE_BUILD_ID=$build_id \
                -DBEE_GAME_ID=$game_id \
                -DGAME_VERSION_MAJOR=$version_major \
                -DGAME_VERSION_MINOR=$version_minor \
                -DGAME_VERSION_RELEASE=$version_release \
                ..

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

        cmake \
                -DCMAKE_BUILD_TYPE=Release \
                -DGAME_NAME="$game" \
                -DBEE_BUILD_ID=$build_id \
                -DBEE_GAME_ID=$game_id \
                -DGAME_VERSION_MAJOR=$version_major \
                -DGAME_VERSION_MINOR=$version_minor \
                -DGAME_VERSION_RELEASE=$version_release \
                ..

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
