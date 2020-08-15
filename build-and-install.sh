#!/bin/bash
#
# Quick and dirty build and install script for JERM.
# Intended to be replaced with a top-level CMakeLists.txt eventually.
#
# Usage:
#  ./build-and-install.sh [INSTALL_DIR]
#
# 22-Jul-2020  Ole Hansen

# Packages to build. The exact order matters.
PACKAGES="buildtools range catch cetlib_except cetlib hep_concurrency fhiclcpp \
messagefacility canvas canvas_root_io gallery art toyExperiment"

TOPDIR="$PWD"
buildinfo="$TOPDIR/.buildinfo"

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
echo \# JERM build started $(date) > $buildinfo
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
    ncpu=$(nproc)
    BUILDPROG="make -j$ncpu"
fi

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
    cmake $GENERATOR -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" "$SRCDIR"
    $BUILDPROG install
    if [ $? -ne 0 ]; then
        exit 1
    fi
done

cd "$TOPDIR"
