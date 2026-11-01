#!/bin/sh
# Regenerate the zlib-compat headers used by Soong.
set -eu

here=$(cd "$(dirname "$0")" && pwd)
src=$(cd "$here/.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

cmake -S "$src" -B "$tmp" \
    -DZLIB_COMPAT=ON \
    -DBUILD_TESTING=OFF \
    -DWITH_GTEST=OFF \
    -DWITH_BENCHMARKS=OFF \
    -DWITH_FUZZERS=OFF >/dev/null

for f in zlib.h zconf.h zlib_name_mangling.h gzread_mangle.h; do
    cp "$tmp/$f" "$here/include/$f"
done
echo "Regenerated: $(ls "$here/include" | tr '\n' ' ')"
