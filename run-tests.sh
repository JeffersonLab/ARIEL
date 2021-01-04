#!/bin/bash
#
# Script to run ARIEL tests
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
path_base="$INSTALLDIR/bin:$PATH"
if [ "$ostype" = "Darwin" ]; then
    # On macOS, assume Homebrew's GNU getopt
    path_base="/usr/local/opt/gnu-getopt/bin:$path_base"
fi
module_path_base="$INSTALLDIR/lib"
ld_path_base="${ROOTSYS:+$ROOTSYS/lib}"
include_path_base="$INSTALLDIR/include:/usr/local/include"

# Packages for which to run tests
if [ $# -eq 0 ]; then
    PACKAGES="buildtools hep_concurrency cetlib_except cetlib fhiclcpp \
messagefacility canvas canvas_root_io art art_root_io gallery critic"
else
    PACKAGES="$*"
fi

# Number of logical CPUs on this system
ncpu=$(getconf _NPROCESSORS_ONLN)

# Run tests for each requested package
for PKG in $PACKAGES; do
    echo ========= Testing $PKG =========
    cd "$BUILDDIR"
    if [ ! -e $PKG ]; then
        echo "Package $PKG does not exist. Skipping."
        continue
    fi
    cd $PKG
    export PATH="$PWD/bin:$path_base"
    export CET_PLUGIN_PATH="$PWD/lib:$module_path_base"
    export FHICL_FILE_PATH="."
    if [ "$ostype" = "Darwin" ]; then
        export DYLD_LIBRARY_PATH="$CET_PLUGIN_PATH${ld_path_base:+:$ld_path_base}"
        export CET_TEST_LIBPATH="$DYLD_LIBRARY_PATH"
    else
        export LD_LIBRARY_PATH="$CET_PLUGIN_PATH${ld_path_base:+:$ld_path_base}"
    fi
    # Get the package's source directory from CMakeCache.txt
    # Obviously this will trivially go away in a unified build
    if [ -r CMakeCache.txt ]; then
        SRCDIR="$(grep "$PKG"_SOURCE_DIR CMakeCache.txt | awk -F= '{print $2}')"
    fi
    export ROOT_INCLUDE_PATH="$SRCDIR${include_path_base:+${SRCDIR:+:}$include_path_base}"
    if [ -z "$debug" ]; then
        ctest -j$ncpu
    else
        ctest -VV --rerun-failed
    fi
    if [ $? -ne 0 ]; then
        exit 1;
    fi
done

cd "$TOPDIR"
