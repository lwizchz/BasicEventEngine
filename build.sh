#!/bin/bash
# Builds BEE with CMake

game="BasicEventEngine"

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

        if [ ! -f "$1/Makefile" ]; then
                cmake -DCMAKE_BUILD_TYPE=Debug -DGAME_NAME="$game" -DBEE_BUILD_ID=$build_id -DBEE_GAME_ID=$game_id ..
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

        if [ ! -f "$1/Makefile" ]; then
                cmake -DCMAKE_BUILD_TYPE=Release -DGAME_NAME="$game" -DBEE_BUILD_ID=$build_id -DBEE_GAME_ID=$game_id ..
        fi

        make -j5
        if [ $? -ne 0 ]; then
                echo "Release build failed!"
                exit 2
        fi

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
        release "$build_dir"
elif [ "$1" == "norun" ]; then
        if [ -f "${build_dir}/last_build_type.txt" ]; then
                $(cat "${build_dir}/last_build_type.txt") "$build_dir" "norun"
        else
                release "$build_dir" "norun"
        fi
elif [ "$1" == "debug" ]; then
        if [ -n "$3" ]; then
                debug "$build_dir" "$3"
        else
                debug "$build_dir"
        fi
elif [ "$1" == "clean" ]; then
        clean "$build_dir"
else
        echo "Invalid argument: \"$1\""
        exit 1
fi

exit 0
