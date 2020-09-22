Catch framework packaging and extensions for ARIEL
=================================================

This is a packaging wrapper for the C++ single-header
[Catch test framework](https://github.com/catchorg/Catch2) for
integration into [ARIEL](https://github.com/JeffersonLab/ARIEL).
It is based on Fermilab Scisoft's Catch package
[v1\_11\_0](https://scisoft.fnal.gov/scisoft/packages/catch).

The Catch framework proper is included as a git submodule.

A top-level CMake list file is provided that interfaces with the
ARIEL build tools.
In addition to installing the Catch header, configuration and
version CMake files as well as the contributed ParseAndAddCatchTests
module are installed. A Fermilab-developed patch that extends that module's
capabilities is applied.

Author:  Ole Hansen (ole@jlab.org)
