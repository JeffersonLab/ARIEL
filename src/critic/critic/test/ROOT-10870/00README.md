# Reproducer for [ROOT-10870](https://sft.its.cern.ch/jira/projects/ROOT/issues/ROOT-10870)

## Prerequisites

* ROOTSYS must be set in the environment, or root-config must be in PATH.
* A `pythonX.Y` executable matching the version of Python used to build ROOT must be available in PATH.
* The compilers whose names match the output of `root-config --cc`, _etc._ must be the same versions as those used to compile ROOT.
* /bin/bash must be available.
* ROOT must have been compiled with CMAKE_CXX_STANDARD=17.
* The parent directory to the one in which this README resides must be writable as the build system enforces an out-of-source build.

### Building and running the test

From the directory in which this README resides:

* `./run_cmd ./build_clean`
* `./run_cmd ./test`

If appropriate, one may run `build` instead of `build_clean`.

Example output to demonstrating failure:

```console
$ ./run_cmd build_clean
INFO: Executing build_clean with ROOTSYS=/home/greenc/work/cet-is/test-products/root/v6_22_00b/Linux64bit+3.10-2.17-e20-p383b-prof
INFO: Install prefix = /scratch/greenc/products
INFO: CETPKG_TYPE = Prof

------------------------------------
INFO: Stage cmake.
------------------------------------
...
------------------------------------
INFO: Stage build successful.
------------------------------------

$ ./run_cmd test
INFO: Executing test with ROOTSYS=/home/greenc/work/cet-is/test-products/root/v6_22_00b/Linux64bit+3.10-2.17-e20-p383b-prof
+ exec python3.8 /scratch/greenc/tmp/r10870-reproducer/r10870_t.py
Default-constructed Handle<int>
- Type: <class 'cppyy.CPPExcInstance'>
- Boolean value: True
Default-constructed Handle<DataObj>
- Type: <class 'cppyy.CPPExcInstance'>
- Boolean value: True
 *** Break *** segmentation violation
    __boot()
    import os
 *** Break *** segmentation violation
    __boot()
    import os
$ 
```
