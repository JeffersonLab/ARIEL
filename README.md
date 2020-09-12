# JERM

A software environment providing a collection of APIs and services for processing nuclear and particle physics event data.

This is a modified version of Fermilab's
[*art*](https://art.fnal.gov/) event processing framework and related
tools.  The main modifications are with the build system.  All
packages in this repository can be build from source without the use
of Fermilab's UPS package manager, in the same way as other standard
CMake-based software distributions.

This repository contains the source code of the
[*art*](https://cdcvs.fnal.gov/redmine/projects/art/wiki/Series_306/),
[*gallery*](https://cdcvs.fnal.gov/redmine/projects/gallery/wiki/Series_115/)
and [*critic*](https://cdcvs.fnal.gov/redmine/projects/critic/wiki/)
suites with small patches, mainly to accommodate the modified build
system and additional platforms and compilers. At present, the build
system uses a heavily modified version of Fermilab's
[cetbuildtools](https://cdcvs.fnal.gov/redmine/projects/cetbuildtools/wiki/).

This release is based on *art* 3.06.03.

### System Requirements

Minimum system requirements/dependencies (approximate, newer is better):

* Recent Linux or macOS. Examples: CentOS 8, macOS 10.14
* C++17 compiler (gcc 8, clang 10, Xcode 11)
* [CMake](https://cmake.org/) 3.12
* [ROOT](https://root.cern/) 6.12.6
* [Boost](https://www.boost.org/) 1.66
* [SQLite](https://www.sqlite.org/) 3.20.1
* [cppunit](https://www.freedesktop.org/wiki/Software/cppunit/) 1.13.2
* [CLHEP](https://proj-clhep.web.cern.ch/proj-clhep/) 2.3.4.6
* [TBB](https://www.threadingbuildingblocks.org/) 2018_U2
* Python 3.6
* Perl 5.18

Additional dependencies on macOS:

* [GNU getopt](https://github.com/karelzak/util-linux/) (any version)

ROOT must have been compiled with the exact same compiler that will be used for building JERM.

Generally, dependencies can have any version at or above the minimum specified;  an exact version match is not required. One can thus upgrade JERM without necessarily having to pull in upgrades of various large dependencies as well. (This is the typical behavior for Unix software distributions.) If JERM versions and dependency versions are to be locked, this should be handled with a suitable external package manager.

The following packages are included in this repository as submodules because they are often not available through package managers:

* [catch](https://github.com/catchorg/Catch2/) 2.13.1
* [range-v3](https://github.com/ericniebler/range-v3/) 0.10.5

There is no need to install these two components separately.

### Building

* Install dependencies listed above
* Clone this repository and `cd` to the repository root directory:

    `git clone https://github.com/hansenjo/JERM.git`
    `cd JERM`

* Pick an installation location. Currently, the build process requires installing each subpackage immediately after building.
* For convenience, set an environment variable that points to the top of the installation tree. For example

   `export JERM=/usr/local/JERM`

* Run the build script. The script expects the installation location as its first argument:

   `./build-and-install.sh $JERM`

   This will take a while. Expect 10-20 minutes on a modern 8-core machine.
* Run the tests to ensure a correct build:

   `./run-tests.sh`

* Add the installation location to your environment. For example

   `export PATH=$JERM/bin:$PATH`
   `export LD_LIBRARY_PATH=$JERM/lib:$LD_LIBRARY_PATH`

   As usual, use `DYLD_LIBRARY_PATH` instead of `LD_LIBRARY_PATH` on macOS.

### Using the workbook

See `workbook/README.md`
 
### Repository Organization

The _base_ branch contains selected point releases of the unmodified Fermilab sources. _develop_ contains patches and additions to those sources. JERM releases correspond to tags on the _develop_ branch.

### About the name

JERM = JLab Event Reconstruction Manager. The germ of an idea for a good, reusable software environment.

### License

Standard 3-clause BSD. See in the included LICENSE files.

Author:  Ole Hansen (ole@jlab.org)
