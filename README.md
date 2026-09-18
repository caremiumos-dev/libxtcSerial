# libxtcSerial

Reimplementation of the prebuilt `libxtcSerial.so` Android JNI serial-port
library using **C++ and GNU Autotools** (Autoconf / Automake / Libtool).
No CMake, no ndk-build.

Provides the same exported symbols and behaviour as the original binary:

| C++ function            | JNI method   | Java signature                       |
|-------------------------|--------------|--------------------------------------|
| `native_open`           | `open_port`  | `(Ljava/lang/String;I)I`             |
| `native_write`          | `write_port` | `(I[B)I`                             |
| `native_read`           | `read_port`  | `(ILjava/nio/ByteBuffer;I)I`         |
| `native_close`          | `close_port` | `(I)V`                               |
| `JNI_OnLoad`            | —            | `RegisterNatives` on `com.android.internal.app.OpenFeatureSecureCheck` |

The log tag is `[F][serail]` (verbatim, including the typo, to match the
original).

## License

GNU General Public License, version 3 — see `LICENSE`.

## Requirements

- GNU Autotools: `autoconf`, `automake`, `libtoolize`, `autoreconf`
- Android NDK r21+ (default `/opt/android-ndk-r21`)

## Building

### All four ABIs (recommended)

```sh
./build.sh                        # armeabi-v7a, arm64-v8a, x86, x86_64
./build.sh --abis=armeabi-v7a,arm64-v8a
./build.sh --abis=armeabi-v7a --ndk=/opt/android-ndk-r21 --out=/tmp/out
./build.sh --cxx-runtime=shared   # or static | system
```

Results are copied to `prebuilt/<abi>/libxtcSerial.so`.

### Single ABI, manually

```sh
./autogen.sh
./configure \
    --build=$(./build-aux/config.guess) \
    --host=aarch64-linux-android \
    --with-ndk=/opt/android-ndk-r21 \
    --prefix=/tmp/xtc
make all
make install
```

`--host` and `--target` are interchangeable; the triple selects the ABI:

| ABI           | triple                     | default API |
|---------------|----------------------------|-------------|
| armeabi-v7a   | `armv7a-linux-androideabi` | 16          |
| arm64-v8a     | `aarch64-linux-android`    | 21          |
| x86           | `i686-linux-android`       | 16          |
| x86_64        | `x86_64-linux-android`     | 21          |

### configure options

- `--prefix=DIR`      install prefix (`$prefix/lib/libxtcSerial.so`)
- `--build=TRIPLE`    build machine (detected with `config.guess` if omitted)
- `--host=TRIPLE`     run-on machine (alias: `--target`)
- `--with-ndk=DIR`    NDK root (default `$ANDROID_NDK_HOME` or `/opt/android-ndk-r21`)
- `--with-android-api=N` override the default API level
- `--with-cxx-runtime=static|shared|system`
  - `static`  statically links libc++ (default, self contained)
  - `shared`  links `libc++_shared.so`
  - `system`  links bionic's `libstdc++.so` stub

Environment overrides `CC`, `CXX`, `AR`, `RANLIB`, `STRIP` are honoured.

## Layout

```
configure.ac        top-level autoconf file
Makefile.am         top-level automake file
src/Makefile.am     libxtcSerial.la rules
src/xtcSerial.cpp   JNI implementation
autogen.sh          autoreconf bootstrap
build.sh            multi-ABI driver
build/<triple>/     out-of-tree per-ABI build trees
prebuilt/<abi>/     built .so copies
```