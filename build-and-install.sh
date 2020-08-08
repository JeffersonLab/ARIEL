#!/bin/bash
#
# Quick and dirty build and install script for JERM.
# Intended to be replaced with a top-level CMakeLists.txt eventually.
#
# Usage:
#  ./build-and-install.sh INSTALL_DIR
#
# 22-Jul-2020  Ole Hansen

TOPDIR="$PWD"

# Installation location
if [ $# -eq 0 ]; then
    echo ERROR: must give installation directory: ./build-and-install.sh INSTALL_DIR
    exit 1
fi
INSTALLDIR="$1"
# This is probably the easiest way to help CMake find the installed packages ...
export PATH="$INSTALLDIR/bin:$PATH"

# Sources to build. The exact order matters.
PACKAGES="buildtools range catch cetlib_except cetlib hep_concurrency fhiclcpp messagefacility canvas canvas_root_io art"

# Create build directory
BUILDDIR="$TOPDIR/tmp-build"
echo "$INSTALLDIR" "$BUILDDIR"
if [ "x$do_clean" != "x" -o ! -e "$BUILDDIR" ]; then
    rm -rf "$BUILDDIR"
    mkdir "$BUILDDIR"
fi

# Write .buildinfo so other scripts know what we did
buildinfo="$TOPDIR/.buildinfo"
echo \# JERM build started $(date) > $buildinfo
echo INSTALLDIR=\"$INSTALLDIR\" >> $buildinfo
echo BUILDDIR=\"$BUILDDIR\" >> $buildinfo

# Build and install the packages.
# The current CMake scripts only work with installed packages.
ncpu=$(nproc)
for PKG in $PACKAGES; do
    echo ========= Building $PKG =========
    cd "$BUILDDIR"
    if [ ! -e $PKG ]; then
        mkdir $PKG
    fi
    cd $PKG
    if [ $PKG != buildtools ]; then
        SRCDIR="$TOPDIR/src/$PKG"
    else
        SRCDIR="$TOPDIR/$PKG"
    fi
    cmake -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" "$SRCDIR"
    make -j$ncpu install
    if [ $? -ne 0 ]; then
        exit 1
    fi
done

cd "$TOPDIR"
