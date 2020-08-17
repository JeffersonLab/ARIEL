#!/bin/bash
########################################################################
# runit.sh
#
# Invoke ROOT to demonstrate the issue autoparsing bad.h.
########################################################################

cd "$(dirname $(realpath ${BASH_SOURCE[0]}))"
LD_LIBRARY_PATH=./lib${LD_LIBRARY_PATH+:${LD_LIBRARY_PATH}} root.exe -l <<EOF
${1+gSystem->Load(\"libcanvas_root_io_Dictionaries_std_dict.so\")}
#include "bad.h"
EOF
