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

* Linux or macOS. Examples: CentOS 7, macOS 10.12
* C++11 compiler (gcc 4.8, clang 5, Xcode 9)
* [CMake](https://cmake.org/) 3.10
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

These dependencies must be installed separately on the target system, *e.g.* through a local package manager or in a central installation. Generally, dependencies can have any version at or above the minimum specified;  an exact version match is not required. One can thus upgrade *art* without necessarily having to pull in upgrades of various large dependencies as well. (This is the expected behavior for Unix software distributions.) If *art* and dependency versions are to be locked, this should be handled with a suitable external package manager.

The following packages are included in this repository as submodules because they are often not available through package managers:

* [catch](https://github.com/catchorg/Catch2/) 2.13.1
* [range-v3](https://github.com/ericniebler/range-v3/) 0.10.5

There is no need to install these two components separately.

### Repository Organization

The _base_ branch contains selected point releases of the unmodified Fermilab sources. _develop_ contains patches and additions to those sources. JERM releases correspond to tags on the _develop_ branch.

### About the name

JERM = JLab Event Reconstruction Manager. The germ of an idea for a good, reusable software environment.

### License

Standard 3-clause BSD. See in the included LICENSE files.

Author:  Ole Hansen (ole@jlab.org)
