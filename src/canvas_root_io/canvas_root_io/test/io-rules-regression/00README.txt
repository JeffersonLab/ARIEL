These tests were inaugurated to investigate and reproduce the isssue reported to LArSoft as https://cdcvs.fnal.gov/redmine/issues/23013, eventually reported to ROOT as https://sft.its.cern.ch/jira/projects/ROOT/issues/ROOT-10271.

Although part of canvas-root-io, this entire directory can be turned into a standalone reproducer in conjunction with cetmodules with the make_standalone_tarball script in the standalone/ directory, e.g.:

  standalone/make_standalone_tarball -v v1_04_00 ~/test-tarball.tar.bz2

If not specified, the cetmodules version defaults to v1_04_00.

The tarball will contain a single eponymous directory, within which will be:

  build/
  cetmodules/
  io-rules-regression/
  run_standalone*

1. Ensure that suitable versions of CMake and ROOT (and dependencies—including the compiler) are available. This will likely be done by setting up the relevant UPS products, but anything that puts the right things in PATH, ROOTSYS, CMAKE_PREFIX_PATH etc. should work.
2. Ensure CC and CXX have appropriate values in the environment if necessary.
3. Execute the script run_standalone to build and run the tests under build/.

--
Chris Green, 2019-08-15.
