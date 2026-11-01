#!/bin/sh
# Check the exported symbols against the NDK map.
set -eu

here=$(cd "$(dirname "$0")" && pwd)
src=$(cd "$here/.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

KNOWN_MISSING="_dist_code _length_code _tr_align _tr_flush_bits _tr_flush_block _tr_init _tr_stored_block _tr_tally"

cmake -S "$src" -B "$tmp" -DZLIB_COMPAT=ON -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF \
    -DWITH_GTEST=OFF -DWITH_BENCHMARKS=OFF -DWITH_FUZZERS=OFF >/dev/null
cmake --build "$tmp" --target zlib >/dev/null 2>&1 || cmake --build "$tmp" >/dev/null
lib=$(ls "$tmp"/libz.so.1.* "$tmp"/libz.so 2>/dev/null | head -n1)

# Extract the global symbols from the version map.
sed 's/#.*//' "$src/libz.map.txt" | awk '
    /\{/ { inblock=1; local=0; next }
    /\}/ { inblock=0; next }
    /local:/ { local=1; next }
    /global:/ { next }
    inblock && !local { gsub(/[ \t;]/, ""); if ($0 != "") print }' | sort -u > "$tmp/want.txt"
nm -D --defined-only "$lib" | awk '{print $3}' | sed 's/@.*//' | sort -u > "$tmp/have.txt"

missing=$(comm -23 "$tmp/want.txt" "$tmp/have.txt" | tr '\n' ' ')
extra=$(comm -13 "$tmp/want.txt" "$tmp/have.txt" | grep -v '^ZLIB_' | tr '\n' ' ')
unexpected=""
for s in $missing; do
    case " $KNOWN_MISSING " in *" $s "*) ;; *) unexpected="$unexpected $s" ;; esac
done

echo "In libz.map.txt, not exported (expected, hidden by zlib-ng): $(echo "$missing" | wc -w) symbols"
echo "Exported beyond libz.map.txt (not NDK API): $extra"
if [ -n "$unexpected" ]; then
    echo "ERROR: NDK symbols missing from the build:$unexpected" >&2
    exit 1
fi
echo "OK"
