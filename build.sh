#!/bin/sh
# Builds libxtcSerial.so for one or more Android ABIs using GNU Autotools.
#
# Options:
#   --abis=LIST        comma separated ABIs to build.  Default:
#                      armeabi-v7a,arm64-v8a,x86,x86_64
#   --abi=ABI          single ABI (same as --abis with one entry)
#   --ndk=DIR          Android NDK root (default: $ANDROID_NDK_HOME or /opt/android-ndk-r21)
#   --api=N            force Android API level for every ABI
#   --cxx-runtime=MODE C++ runtime linkage: static|shared|system (default: static)
#   --prefix=DIR       configure --prefix (default: $PWD/.dist/<abi>)
#   --out=DIR          where libxtcSerial.so copies land (default: $PWD/prebuilt/<abi>)
#   --jobs=N           make -j level (default: nproc)
#   -h|--help          show this help

set -e

SRC_DIR="$(cd "$(dirname "$0")" && pwd)"

NDK="${NDK:-${ANDROID_NDK_HOME:-/opt/android-ndk-r21}}"
ABIS="armeabi-v7a,arm64-v8a,x86,x86_64"
CXX_RUNTIME="static"
PREFIX=""
OUT="$SRC_DIR/prebuilt"
API=""
JOBS="$(nproc 2>/dev/null || echo 4)"

usage() {
    echo "run: $0 [--abis=LIST] [--ndk=DIR] [--api=N] [--prefix=DIR] [--out=DIR] [--cxx-runtime=MODE]"
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --abis=*)  ABIS="${1#*=}" ;;
        --abi=*)   ABIS="${1#*=}" ;;
        --ndk=*)   NDK="${1#*=}" ;;
        --api=*)   API="${1#*=}" ;;
        --cxx-runtime=*) CXX_RUNTIME="${1#*=}" ;;
        --prefix=*) PREFIX="${1#*=}" ;;
        --out=*)   OUT="${1#*=}" ;;
        --jobs=*)  JOBS="${1#*=}" ;;
        -h|--help) usage; exit 0 ;;
        *) echo "unknown option: $1" >&2; usage >&2; exit 1 ;;
    esac
    shift
done

case "$CXX_RUNTIME" in
    static|shared|system) ;;
    *) echo "invalid --cxx-runtime: $CXX_RUNTIME (static|shared|system)" >&2; exit 1 ;;
esac

if [ ! -x "$SRC_DIR/configure" ]; then
    echo "=== generating configure (autogen.sh) ==="
    (cd "$SRC_DIR" && ./autogen.sh)
fi

BUILD_TRIPLE="$("$SRC_DIR/build-aux/config.guess" 2>/dev/null || echo x86_64-pc-linux-gnu)"

triple_for() {
    case "$1" in
        armeabi-v7a|arm|armv7a) echo armv7a-linux-androideabi ;;
        arm64-v8a|arm64|aarch64) echo aarch64-linux-android ;;
        x86|i686) echo i686-linux-android ;;
        x86_64) echo x86_64-linux-android ;;
        *) echo "$1" ;;
    esac
}

for abi in $(echo "$ABIS" | tr ',' ' '); do
    if [ -z "$abi" ]; then
        continue
    fi
    tuple="$(triple_for "$abi")"
    builddir="$SRC_DIR/build/$tuple"
    pref="${PREFIX:-$SRC_DIR/.dist/$abi}"
    outdir="$OUT/$abi"

    if [ "$abi" = "x86" ]; then
        tuple="i686-linux-android"
    fi

    rm -rf "$builddir"
    mkdir -p "$builddir"

    echo
    echo "=== [${abi}] configuring $tuple (build=$BUILD_TRIPLE, NDK=$NDK) ==="
    configure_args="--build=$BUILD_TRIPLE --host=$tuple --with-ndk=$NDK \
        --with-cxx-runtime=$CXX_RUNTIME --prefix=$pref"
    if [ -n "$API" ]; then
        configure_args="$configure_args --with-android-api=$API"
    fi
    (cd "$builddir" && "$SRC_DIR/configure" $configure_args)

    echo
    echo "=== [${abi}] make all (jobs=$JOBS) ==="
    make -C "$builddir" -j"$JOBS" all

    echo
    echo "=== [${abi}] make install -> $pref ==="
    make -C "$builddir" install
    make -C "$builddir" install-strip 2>/dev/null || true

    echo
    echo "=== [${abi}] copy result -> $outdir ==="
    mkdir -p "$outdir"
    cp -f "$pref/lib/libxtcSerial.so" "$outdir/libxtcSerial.so"
    ls -l "$outdir/libxtcSerial.so"
done

echo
echo "done.  Results under: $OUT"
echo "to build a single ABI manually, run per ABI:"
echo "  ./configure --build=\$BUILD --host=aarch64-linux-android --with-ndk=$NDK --prefix=\$PREFIX"
echo "  make all"
echo "  make install"