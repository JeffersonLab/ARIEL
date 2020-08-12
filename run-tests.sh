#!/bin/bash
#
# Script to run JERM tests
# Intended to be replaced with CTest eventually.
#
# Usage:
#  ./run-tests.sh
#
# 23-Jul-2020  Ole Hansen

TOPDIR="$PWD"
buildinfo="$TOPDIR/.buildinfo"

if [ ! -r "$buildinfo" ]; then
    echo ERROR: Must run build-and-install.sh first
    exit 1
fi

# Pick up INSTALLDIR and BUILDDIR from last build
source "$buildinfo"

# Check if installation location exists
if [ -z "$INSTALLDIR" -o ! -d "$INSTALLDIR" -o ! -r "$INSTALLDIR" ]; then
    echo Error opening installation directory $INSTALLDIR
    exit 1
fi
# Check build location top directory. This is where the test executables are.
if [ -z "$BUILDDIR" -o ! -d "$BUILDDIR" -o ! -r "$BUILDDIR" ]; then
    echo Error opening build directory $BUILDDDIR
    exit 1
fi

case "$1" in
    --debug)
	debug=1
	shift
	;;
    --help)
	echo ./run-tests.sh [--debug] [--help] [module1 [module2 [module3...]]]
	exit 0
	;;
esac

ostype=$(uname -s)
path_root="$INSTALLDIR/bin:$PATH"
if [ "$ostype" = "Darwin" ]; then
    # On macOS, assume Homebrew's GNU getopt
    path_root="/usr/local/opt/gnu-getopt/bin:$path_root"
fi
module_path_root="$INSTALLDIR/lib"
ld_path_root="${ROOTSYS:+$ROOTSYS/lib}"
# Add /usr/local/lib to library path
# except on macOS, where Homebrew packages interfere
if [ "$ostype" != "Darwin" ]; then
    ld_path_root="${ld_path_root:+$ld_load_path:}/usr/local/lib"
fi
export ROOT_INCLUDE_PATH="$INSTALLDIR/include:/usr/local/include"

# Packages for which to run tests
if [ $# -eq 0 ]; then
    PACKAGES="buildtools cetlib_except cetlib hep_concurrency fhiclcpp \
messagefacility canvas canvas_root_io art"
else
    PACKAGES="$*"
fi

# Run tests for each requested package
ncpu=$(nproc)
for PKG in $PACKAGES; do
    echo ========= Testing $PKG =========
    cd "$BUILDDIR"
    if [ ! -e $PKG ]; then
        echo "Package $PKG does not exist. Skipping."
        continue
    fi
    cd $PKG
    export PATH="$PWD/bin:$path_root"
    export CET_MODULE_PATH="$PWD/lib:$module_path_root"
    export FHICL_FILE_PATH="."
    if [ "$ostype" = "Darwin" ]; then
        export DYLD_LIBRARY_PATH="$CET_MODULE_PATH${ld_path_root:+:$ld_path_root}"
        export CET_TEST_LIBPATH="$DYLD_LIBRARY_PATH"
    else
        export LD_LIBRARY_PATH="$CET_MODULE_PATH${ld_path_root:+:$ld_path_root}"
    fi

    if [ -z "$debug" ]; then
	ctest -j$ncpu
    else
	ctest -VV
    fi
    if [ $? -ne 0 ]; then
        exit 1;
    fi
done

cd "$TOPDIR"
