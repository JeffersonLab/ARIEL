#!/bin/bash
#
# Quick and dirty build and install script for ARIEL.
# Intended to be replaced with a top-level CMakeLists.txt eventually.
#
# Usage:
#  ./build-and-install.sh [INSTALL_DIR]
#
# 22-Jul-2020  Ole Hansen

# Packages to build. The exact order matters.
PACKAGES="buildtools range catch hep_concurrency cetlib_except cetlib fhiclcpp \
fhiclpy messagefacility canvas canvas_root_io gallery art art_root_io critic \
toyExperiment"

# Basic check for prerequisites
if ! which root-config > /dev/null; then
    echo 'Cannot find ROOT. Please source <where-ROOT-is>/bin/thisroot.(c)sh'
    exit 1
fi

TOPDIR="$PWD"
buildinfo="$TOPDIR/.buildinfo"

case "$1" in
    --debug)
        # Build Debug version
        BUILDTYPE="-DCMAKE_BUILD_TYPE=Debug"
        shift
        ;;
    --release)
        # Build Release version
        BUILDTYPE="-DCMAKE_BUILD_TYPE=Release"
        shift
        ;;
    --help)
        echo ./build-and-install.sh [--debug|--release] [--help] INSTALL_DIR
        exit 0
        ;;
esac

# Installation location
if [[ $# -eq 0 ]]; then
    if [[ -r "$buildinfo" ]]; then
        source "$buildinfo"
    else
        echo ERROR: must give installation directory: ./build-and-install.sh INSTALL_DIR
        exit 1
    fi
else
    BUILDDIR="$TOPDIR/tmp-build"
    INSTALLDIR="$1"
fi
echo Using BUILDDIR="$BUILDDIR", INSTALLDIR="$INSTALLDIR"

# This is probably the easiest way to help CMake find the installed packages ...
export PATH="$INSTALLDIR/bin:$PATH"

# Create/clean build directory
if [ "x$do_clean" != "x" -o ! -e "$BUILDDIR" ]; then
    rm -rf "$BUILDDIR"
    mkdir "$BUILDDIR"
fi

# Write .buildinfo so other scripts know what we did
echo \# ARIEL build started $(date) > $buildinfo
echo INSTALLDIR=\"$INSTALLDIR\" >> $buildinfo
echo BUILDDIR=\"$BUILDDIR\" >> $buildinfo

# Use ninja if available
for N in ninja-build ninja; do
    if which $N > /dev/null; then
        BUILDPROG="$(which $N)"
        GENERATOR="-GNinja"
        break
    fi
done
if [[ -z "$BUILDPROG" ]]; then
    if which nproc > /dev/null; then
        ncpu=$(nproc)
    else
        echo NOTICE: Cannot find nproc. Assuming 2 CPUs. Install coreutils to fix
        sleep 3
        ncpu=2
    fi
    BUILDPROG="make -j$ncpu"
fi

ostype=$(uname -s)
module_path_base="$INSTALLDIR/lib"
# ROOTSYS might not be set if ROOT is installed in system directories
ld_path_base="${ROOTSYS:+$ROOTSYS/lib}"
# Add /usr/local/lib to library path
# except on macOS, where Homebrew packages interfere
if [ "$ostype" != "Darwin" ]; then
    ld_path_base="${ld_path_base:+$ld_path_base:}/usr/local/lib"
fi
include_path_base="$INSTALLDIR/include:/usr/local/include"

# Build and install the packages.
# The current CMake scripts only work with installed packages.
for PKG in $PACKAGES; do
    echo ========= Building $PKG =========
    cd "$BUILDDIR"
    if [ ! -e $PKG ]; then
        mkdir $PKG
    fi
    cd $PKG
    unset SRCDIR
    for D in "$TOPDIR/src" "$TOPDIR/examples" "$TOPDIR"; do
        if [[ -d "$D/$PKG" ]]; then
            SRCDIR="$D/$PKG"
            break;
        fi
    done
    if [[ -z "$SRCDIR" ]]; then
        echo Cannot find source for $PKG. Exiting.
        exit 2
    fi

    export CET_PLUGIN_PATH="$PWD/lib:$module_path_base"
    export ROOT_INCLUDE_PATH="$SRCDIR${include_path_base:+${SRCDIR:+:}$include_path_base}"
    # checkClassVersion requires the library path set at build time
    if [ "$ostype" = "Darwin" ]; then
        export DYLD_LIBRARY_PATH="$CET_PLUGIN_PATH${ld_path_base:+:$ld_path_base}"
    else
        export LD_LIBRARY_PATH="$CET_PLUGIN_PATH${ld_path_base:+:$ld_path_base}"
    fi

    echo cmake $GENERATOR $BUILDTYPE -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" "$SRCDIR"
    cmake $GENERATOR $BUILDTYPE -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" "$SRCDIR"
    $BUILDPROG install
    if [ $? -ne 0 ]; then
        exit 1
    fi
done

cd "$TOPDIR"
