Exercises for Additional Art Facilities.
----------------------------------------

1. Command-line options.

  * Compare --debug-config and --config-out by executing, from the $CETPKG_BUILD directory:

    * art --debug-config=dc.cfg -c FirstProducer/producer1.fcl

    * art --config-out=co.cfg -c FirstProducer/producer1.fcl

  * Peruse the available command-line options:

    * art -h

  * Starting with the --debug-config exercise above and using the original dc.cfg for comparison, determine the configuration injected by the following options:

    * --process-name Oopsie

    * -n 7

    * --nskip 7

    * --TFileName hist.root

    * --no-output

    * --trace

  * Peruse the output produced by the following commands:

    * art --print-available-modules

    * art --print-available-modules

    * art --service-description TimeTracker

    * art --service-description Conditions

    * art --module-description RootOutput

    * art --module-description ConcatenateIntersections


2. config_dumper and sam_metadata_dumper.

  * config_dumper

    From the $CETPKG_BUILD/inputFiles directory, try the following invocations of config_dumper and understand the output:

    * config_dumper -h

    * config_dumper input01.art

    * config_dumper -M input01.art

    * config_dumper -S input01.art

    * config_dumper -P input01.art

    * config_dumper -M --filter=inspectHits input01.art

    * Compare and contrast co.cfg with the output of:

      config_dumper -P ${ART_WORKBOOK_OUTPUT}/concatenateIntersections.art

  * sam_metadata_dumper

    From the $CETPKG_BUILD/inputFiles directory, try the following invocations of config_dumper and understand the output:

    * sam_metadata_dumper -h

    * sam_metadata_dumper input01.art

    * sam_metadata_dumper *.art

    * sam_metadata_dumper -H *.art


3. TimeTracker and MemoryTracker

  * Make a .fcl file with the following contents:

########################################################################
#include "fcl/input01.fcl"

services.MemoryTracker: { }
services.TimeTracker: { }
########################################################################

  * Modify the configuration to write a (different) database output file for each service. If you are on OS X, use the memoryTracker.db in this directory.

  * Open the TimeTracker DB file in sqlite3, and ascertain the path name and time of the longest-executing module for the longest executing event (with event number and time). EXTRA CREDIT: do it in one SQL statement. 

  * If you have time, obtain the list of tables and schema for the MemoryTracker database. Also, if you are on Linux, configure for a full summary printout.

  * Useful information to get you started:

    * Invoke sqlite3: sqlite3 <db>

    * Obtain list of tables: .tables

    * Obtain table structure: .schema [<table>]

    * Print info from one or more tables:

      SELECT <exp>[,<exp>]... FROM <table> [<alias>][JOIN <table> [alias] ON <boolean-exp>]...
        [WHERE <boolean-exp>] [ORDER BY <col> [DESC][,<col> [DESC]]..] [LIMIT <num>];
