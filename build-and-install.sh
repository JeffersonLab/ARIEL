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
PACKAGES="buildtools CLHEP range catch hep_concurrency cetlib_except cetlib \
fhiclcpp fhiclpy messagefacility canvas canvas_root_io gallery \
art art_root_io critic toyExperiment cmake"

# Basic check for prerequisites
if ! which root-config > /dev/null 2>&1; then
    echo 'Cannot find ROOT. Please source <where-ROOT-is>/bin/thisroot.(c)sh'
    exit 1
fi

TOPDIR="$PWD"
buildinfo="$TOPDIR/.buildinfo"
[[ -r "$buildinfo" ]] && . "$buildinfo"

# Default build type = Release
BUILDTYPE="-DCMAKE_BUILD_TYPE=Release"
POSITIONAL=()
while [[ $# -gt 0 ]]; do
    case "$1" in
        --debug)
            # Build Debug version
            BUILDTYPE="-DCMAKE_BUILD_TYPE=Debug"
            shift
            ;;
        --reldeb)
            # Build Release With Debug Info version
            BUILDTYPE="-DCMAKE_BUILD_TYPE=RelWithDebInfo"
            shift
            ;;
        --release)
            # Build Release version
            BUILDTYPE="-DCMAKE_BUILD_TYPE=Release"
            shift
            ;;
        --configure)
            # Force configuration to run, even if already configured
            do_config=1
            shift
            ;;
        --clean)
            # Start clean
            do_clean=1
            shift
            ;;
        --builddir)
            # Set build location
            BUILDDIR="$2"
            shift 2
            ;;
        --help)
            echo ./build-and-install.sh [--debug|--release] [--help] INSTALL_DIR
            exit 0
            ;;
        *)
            POSITIONAL+=("$1")
            shift
            ;;
    esac
done
set -- "${POSITIONAL[@]}"

# Installation location
if [[ $# -gt 0 ]]; then
    INSTALLDIR="$1"
fi
if [[ -z $INSTALLDIR ]]; then
    echo ERROR: must give installation directory: ./build-and-install.sh INSTALL_DIR
    exit 1
fi
# Build location
if [[ -z $BUILDDIR ]]; then
    BUILDDIR="$TOPDIR/ARIEL-build"
fi
configstat="$BUILDDIR/.configured"
[[ -n $do_config ]] && rm -f "$configstat"

echo Using BUILDDIR="$BUILDDIR", INSTALLDIR="$INSTALLDIR"

# This is probably the easiest way to help CMake find the installed packages ...
export PATH="$INSTALLDIR/bin:$PATH"

# Create/clean build directory
[[ -n $do_clean ]] && rm -rf "$BUILDDIR"
mkdir -p "$BUILDDIR"

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
    for D in "$TOPDIR/externals" "$TOPDIR/src" "$TOPDIR/examples" "$TOPDIR"; do
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

    case "$PKG" in
    CLHEP)
        BUILDOPTS="-DCLHEP_BUILD_CXXSTD=-std=c++17"
        ;;
    range)
        BUILDOPTS="-DRANGES_CXX_STD=17 -DRANGES_BUILD_CALENDAR_EXAMPLE=NO -DRANGES_ASSERTIONS=NO -DRANGES_DEBUG_INFO=NO -DRANGE_V3_TESTS=NO -DRANGE_V3_EXAMPLES=NO -DRANGE_V3_PERF=NO"
        ;;
    *)
        BUILDOPTS=""
        ;;
    esac

    if [ -n "$do_config" -o ! -e "$configstat" ]; then
        echo cmake $GENERATOR $BUILDTYPE $BUILDOPTS -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" "$SRCDIR"
        cmake $GENERATOR $BUILDTYPE $BUILDOPTS -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" "$SRCDIR"
        [ $? -ne 0 ] && exit 1
    fi
    $BUILDPROG install
    [ $? -ne 0 ] && exit 1
done
echo \# ARIEL build completed $(date) >> $buildinfo
touch "$configstat"

cd "$TOPDIR"
