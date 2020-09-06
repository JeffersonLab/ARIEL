#!/bin/bash
########################################################################
# tidy.sh
#
# Remove generaated and temporary files annd directories.
########################################################################
prog=${BASH_SOURCE[0]##*/}

(return 0 2>/dev/null) && \
  { echo "ERROR: $prog should be executed, not sourced." 1>&2; return 1; }

# Fail on error.
set -e

# Execute in the script directory.
cd "$(dirname $(realpath ${BASH_SOURCE[0]}))"

# Clean up unwanted files.
rm -rfv lib
rm -vf gen/*~ *~ *.cpp *.cpp.o *.log *.out *.err
