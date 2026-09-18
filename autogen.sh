#!/bin/sh
# Bootstraps the GNU Autotools build system.
set -e

cd "$(dirname "$0")"

mkdir -p m4 build-aux

# aclocal/autoheader/automake/autoconf/libtoolize in one go.
autoreconf -v -i -f

echo
echo "Autotools files generated.  Now configure with:"
echo "  ./configure --host=<android-triple> [options]"
echo "then:"
echo "  make all"
echo "  make install"