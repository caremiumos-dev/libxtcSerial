dnl configure.ac -- libxtcSerial Android JNI serial library
dnl GNU Autotools (autoconf/automake/libtool) build for Android NDK.
dnl One libxtcSerial.so per configure invocation (plain, unversioned).

AC_PREREQ([2.69])
AC_INIT([libxtcSerial], [1.1.0], [], [libxtcSerial],
        [https://github.com/xtc/libxtcSerial])

AC_CONFIG_SRCDIR([src/xtcSerial.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

dnl ==== options (values are parsed to $with_* by configure's option
dnl loop before any macro body runs) =====================================

AC_ARG_WITH([ndk],
  [AS_HELP_STRING([--with-ndk=DIR],
     [Android NDK root.  Defaults to $ANDROID_NDK_HOME or /opt/android-ndk-r21])],
  [AS_IF([test -n "$withval" && test "x$withval" != xyes && test "x$withval" != xno],
         [NDK_ROOT="$withval"], [NDK_ROOT="${ANDROID_NDK_HOME:-/opt/android-ndk-r21}"])],
  [NDK_ROOT="${ANDROID_NDK_HOME:-/opt/android-ndk-r21}"])

AC_ARG_WITH([android-api],
  [AS_HELP_STRING([--with-android-api=N],
     [Android API/platform level.  Default: 16 for 32-bit ABIs, 21 for 64-bit])],
  [ANDROID_API="$withval"],
  [ANDROID_API=""])

AC_ARG_WITH([cxx-runtime],
  [AS_HELP_STRING([--with-cxx-runtime=MODE],
     [C++ runtime linkage.  MODE is one of:
        static  - statically link libc++ (default, self contained)
        shared  - dynamically link libc++_shared.so
        system  - link bionic's libstdc++.so stub])],
  [CXX_RUNTIME="$withval"],
  [CXX_RUNTIME="static"])
AS_CASE(["$CXX_RUNTIME"],
  [static|shared|system], [],
  [AC_MSG_ERROR([invalid --with-cxx-runtime "$CXX_RUNTIME": use static, shared or system])])

dnl ==== target platform and local toolchain =============================
dnl configure fills $host_alias/$target_alias from --host/--target before
dnl any macro body executes.  AM_INIT_AUTOMAKE below AC_REQUIREs AC_PROG_CC,
dnl so the compiler defaults MUST be established here, before it.

AS_IF([test -n "$host_alias"], [XTARGET="$host_alias"],
      [test -n "$target_alias"], [XTARGET="$target_alias"],
      [AC_MSG_ERROR([no Android target selected.  Configure with
       --host=armv7a-linux-androideabi   (armeabi-v7a)
       --host=aarch64-linux-android      (arm64-v8a)
       --host=i686-linux-android         (x86)
       --host=x86_64-linux-android       (x86_64)
     (the equivalent --target=TRIPLE also works)])])

dnl if only --target was given, make it the configure --host as well so
dnl autoconf/libtool treat the build as cross compilation.
AS_IF([test -n "$target_alias" && test -z "$host_alias"],
      [host_alias="$target_alias"])

AC_ARG_VAR([CXX], [C++ cross compiler (default: NDK clang++)])
AC_ARG_VAR([CC],  [C cross compiler (default: NDK clang)])
AC_ARG_VAR([AR],  [Archiver])
AC_ARG_VAR([RANLIB], [ranlib])
AC_ARG_VAR([STRIP], [strip])

ABI=""
CXX_PREFIX=""
AR_PREFIX=""
XC_CXXFLAGS=""
AS_CASE(["$XTARGET"],
  [aarch64*|arm64*],
    [ABI="arm64-v8a"
     CXX_PREFIX="aarch64-linux-android"
     AR_PREFIX="aarch64-linux-android"
     AS_IF([test -z "$ANDROID_API"], [ANDROID_API=21])],
  [x86_64*],
    [ABI="x86_64"
     CXX_PREFIX="x86_64-linux-android"
     AR_PREFIX="x86_64-linux-android"
     AS_IF([test -z "$ANDROID_API"], [ANDROID_API=21])],
  [i[[3-6]]86*|x86*],
    [ABI="x86"
     CXX_PREFIX="i686-linux-android"
     AR_PREFIX="i686-linux-android"
     AS_IF([test -z "$ANDROID_API"], [ANDROID_API=16])],
  [armv7a-*|arm*-linux-androideabi|arm*-linux-android],
    [ABI="armeabi-v7a"
     CXX_PREFIX="armv7a-linux-androideabi"
     AR_PREFIX="arm-linux-androideabi"
     AS_IF([test -z "$ANDROID_API"], [ANDROID_API=16])
     XC_CXXFLAGS="-march=armv7-a -mfpu=neon -mfloat-abi=softfp -mthumb"],
  [*],
    [AC_MSG_ERROR([unrecognized target "$XTARGET".  Use one of
       armv7a-linux-androideabi / aarch64-linux-android /
       i686-linux-android / x86_64-linux-android])])

AS_IF([test -d "$NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64"],
      [NDK_TOOLCHAIN_DIR="$NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64"],
      [AC_MSG_ERROR([NDK prebuilt toolchain "linux-x86_64" not found under
                     $NDK_ROOT/toolchains/llvm/prebuilt.  Check --with-ndk.])])
ANDROID_SYSROOT="$NDK_TOOLCHAIN_DIR/sysroot"
AS_IF([test ! -d "$ANDROID_SYSROOT/usr/include"],
      [AC_MSG_ERROR([NDK sysroot not found under $ANDROID_SYSROOT.  Check --with-ndk.])])

AS_IF([test -z "$CC"],
      [CC="$NDK_TOOLCHAIN_DIR/bin/${CXX_PREFIX}${ANDROID_API}-clang"])
AS_IF([test -z "$CXX"],
      [CXX="$NDK_TOOLCHAIN_DIR/bin/${CXX_PREFIX}${ANDROID_API}-clang++"])
AS_IF([test -z "$AR" && test -n "$AR_PREFIX"],
      [AR="$NDK_TOOLCHAIN_DIR/bin/${AR_PREFIX}-ar"])
AS_IF([test -z "$RANLIB" && test -n "$AR_PREFIX"],
      [RANLIB="$NDK_TOOLCHAIN_DIR/bin/${AR_PREFIX}-ranlib"])
AS_IF([test -z "$STRIP" && test -n "$AR_PREFIX"],
      [STRIP="$NDK_TOOLCHAIN_DIR/bin/${AR_PREFIX}-strip"])
AS_IF([test ! -x "$CXX" -o ! -x "$CC"],
      [AC_MSG_ERROR([NDK compilers not found: $CC / $CXX.  Install the NDK at
                     $NDK_ROOT or pass --with-ndk=DIR / CC= CXX=.])])

dnl ==== canonical platforms (must precede AM_INIT_AUTOMAKE's option work) ==

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_USE_SYSTEM_EXTENSIONS

dnl ==== programs ========================================================

AC_LANG([C])
AC_PROG_CC
AC_LANG([C++])
AC_PROG_CXX
AM_PROG_AR
AC_PROG_RANLIB
LT_INIT([disable-static])

dnl libtool auto-injects the detected C++ runtime (-lc++ and friends) into
dnl every C++ link through $postdeps_CXX.  The runtime is managed explicitly
dnl in XC_LDFLAGS via --with-cxx-runtime instead, so drop libtool's version
dnl here (crtbegin/crtend are kept via predep_/postdep_objects_CXX).
postdeps_CXX=""

dnl ==== link settings for the shared JNI library ========================

AC_MSG_CHECKING([C++ runtime linkage])
XC_LIBS="-llog -ldl -lm -lc"
XC_LDFLAGS=""
AS_CASE(["$CXX_RUNTIME"],
  [static], [XC_LDFLAGS="-nostdlib++ -l:libc++_static.a"],
  [shared], [XC_LDFLAGS="-nostdlib++ -lc++_shared"],
  [system], [XC_LDFLAGS="-nostdlib++ -lstdc++"])
AC_MSG_RESULT([$CXX_RUNTIME ($XC_LDFLAGS)])

AC_SUBST([ABI])
AC_SUBST([ANDROID_API])
AC_SUBST([NDK_ROOT])
AC_SUBST([ANDROID_SYSROOT])
AC_SUBST([XC_CXXFLAGS])
AC_SUBST([XC_LIBS])
AC_SUBST([XC_LDFLAGS])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 src/Makefile])
AC_OUTPUT

AC_MSG_NOTICE([])
AC_MSG_NOTICE([libxtcSerial $PACKAGE_VERSION configured])
AC_MSG_NOTICE([  ABI            : $ABI])
AC_MSG_NOTICE([  host/target    : $host])
AC_MSG_NOTICE([  build          : $build])
AC_MSG_NOTICE([  Android API    : $ANDROID_API])
AC_MSG_NOTICE([  NDK root       : $NDK_ROOT])
AC_MSG_NOTICE([  CXX            : $CXX])
AC_MSG_NOTICE([  C++ runtime    : $CXX_RUNTIME])
AC_MSG_NOTICE([  cross compiling : $cross_compiling])
AC_MSG_NOTICE([  output file    : libxtcSerial.so])
AC_MSG_NOTICE([  build with     : make all   then  make install])
AC_MSG_NOTICE([])