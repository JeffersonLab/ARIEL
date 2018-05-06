JERM buildtools
===============

This is a significantly modified version of Fermilab's
[cetbuildtools](https://cdcvs.fnal.gov/redmine/projects/cetbuildtools/wiki),
for which the CMake build systems of all Fermilab CET
software is designed.

This package eliminates the dependence on the UPS package
manager. Dependencies are implicitly checked at CMake time via
find\_package, find\_library and pkg-config calls and similar, as in
plain CMake projects. Furthermore, projects are no longer tied to an
exact version of a dependency, but at most to a minimum version,
avoiding unnecessary installation of multiple, often dozens of,
versions of essentially the same dependency software. Dependencies
available as system libraries (e.g. sqlite3) or as
separately-installed third-party software (e.g. ROOT) may be used in
the same way as would be the case with other software builds.  This
comes at the cost of having to check binary compatibility of
dependencies manually and/or possibly having to recompile user
projects more frequently.

Installation locations follow standard Unix conventions (but details
can be customized).  By default, a single "flavor" (platform/bitness)
is assumed, but placement of binaries in platform-dependent
subdirectories is also supported.

If support for UPS packaging and CET conventions is not needed, in
principle these tools are also no longer necessary to build CET
software like art. However, without the build tool wrapper, all
CMakeLists.txt of all CET products would have to be painstakingly
modified by hand. It is certainly significantly easier to modify only
the buildtool layer than each product. Additionally, doing so allows
developers familiar with the Fermilab development environment to
continue developing user and application software in a familiar way
as described in the applicable documentation.

Author:  Ole Hansen (ole@jlab.org)
