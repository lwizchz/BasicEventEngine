#!/bin/bash
# Packages the generated executable with the required libraries and resources

source config.sh

build_dir="build"
if [ -n "$1" ]; then
        build_dir="$1"
fi

package_dir="pkg"
if [ "$package_dir" != "." ]; then
	mkdir -p $package_dir
fi

package_file="$game-$version_major.$version_minor.$version_patch-build$(date +%Y.%m.%d).tar.gz"
if [ -n "$2" ]; then
        package_file="$2"
fi

if [ -d "$game" ]; then
	echo "Failed to package: temporary directory \"$game\" already exists"
	exit 1
fi

./build.sh norun "$build_dir"

mkdir "$game"
mkdir -p "$game/bee/"

cp "$build_dir/$game" "$game/$game"
strip "$game/$game"

cp -r "./resources/" "$game/"
cp -r "./cfg/" "$game/"
cp -r "./bee/resources" "$game/bee/"

# Copy dynamic libraries
mkdir "$game/lib"
#libraries="$(ldd $build_dir/$game | grep so | sed -e '/^[^\t]/ d' | sed -e 's/\t//' | sed -e 's/.*=..//' | sed -e 's/ (0.*)//' | uniq)"
libraries="
libassimp.so.4
libfreetype.so.6
libGLEW.so.2.1
libpcre.so.1
libpng16.so.16
libpthread.so.0
libSDL2_image-2.0.so.0
libSDL2_mixer-2.0.so.0
libSDL2_net-2.0.so.0
libSDL2_ttf-2.0.so.0
libz.so.1
"
for l in $libraries; do
	cp "/usr/lib/$l" "$game/lib/"
done

# Copy Python libraries
mkdir -p "$game/lib/cpython/build"
cp -r "./lib/cpython/build/lib.linux-x86_64-3.7" "$game/lib/cpython/build"
cp -r "./lib/cpython/Lib" "$game/lib/cpython/"
cp "./lib/cpython/libpython3.7m.a" "$game/lib/cpython/"
rm -r "$game/lib/cpython/Lib/test/data" # Remove bulky test module data

tar czf "$package_dir/$package_file" "$game/"
echo "$game packaged in \"$package_dir/$package_file\"!"

if [ "$(cat $build_dir/last_build_type.txt)" == "debug" ]; then
        echo "Warning: packaged a debug build, was this desired?"
fi

rm -r "$game"

exit 0
