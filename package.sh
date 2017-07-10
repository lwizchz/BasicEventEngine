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

package_file="$game-$version_major.$version_minor.$version_release-build$(date +%Y.%m.%d).tar.gz"
if [ -n "$2" ]; then
        package_file="$2"
fi

if [ -d "$game" ]; then
	echo "Failed to package: temporary directory \"$game\" already exists"
	exit 1
fi

./build.sh norun "$build_dir"

mkdir "$game"
cp "$build_dir/$game" "$game/$game"
cp -r "./resources/" "$game/"
cp -r "./cfg/" "$game/"

# Copy dynamic libraries
mkdir "$game/lib"
#libraries="$(ldd $build_dir/$game | grep so | sed -e '/^[^\t]/ d' | sed -e 's/\t//' | sed -e 's/.*=..//' | sed -e 's/ (0.*)//' | uniq)"
libraries="
libassimp.so.3
libfreetype.so.6
libGLEW.so.2.0
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

tar czf "$package_dir/$package_file" "$game/"
echo "$game packaged in \"$package_dir/$package_file\"!"

rm -r "$game"

exit 0
