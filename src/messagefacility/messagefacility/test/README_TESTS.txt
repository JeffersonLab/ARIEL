The tests in this directory tree as of 2010/11/10 have been extracted
from the CMS MessageFacility system:

* Tests in test/Utilities have been removed where they test things that
  are no longer defined or used in messagefacility.

* The remaining tests in test/Utilities do not constitute reliable unit
  tests of the code in messagefacility/Utilities. They do work however,
  so have been left operating.

* Tests that were in the top-level test/ directory previously have been
  relegated to test/Integation. As noted by the directory name, these
  are not unit tests. They are built but not executed as they require
  configuration files to be converted to fhiclcpp.

* Tests in and under test/MessageService are neither built nor executed
  as they have multiple dependencies on the wider framework, and many
  tests actually require CmsRun.

Discussions are required to determine a test methodology for this
package going forward, including unit, integration and regression tests.

2010/11/10 CG.

* All Python-based tests in test/MessageService have been removed in
  favor of FHiCL-based tests.  The .fcl files are now in
  test/MessageService/fhicl-files and the reference files these
  configurations produce are in test/MessageService/output-references.

2014/11/24 KK.
