# ROOT_JIRA_10504

## Overview

This directory contains the minimal reproducer for the bug described at https://sft.its.cern.ch/jira/projects/ROOT/issues/ROOT-10504.

## Directory structure

* `00README.md`
  This file.
* `bad.h`
  A simple header file demonstrating the issue.
* `buildit.sh`
  Generate and build the dictionary required to demonstrate the issue.
* `classes.h`
  `classes_def.xml`
  Files required to generate the dictionary.
* `gen/`
  Directory containing scripts and associated files to regenerate `bad.h`. See `gen/00README.md` for more details.
* `runit.sh`
  Script to reproduce the issue by invoking ROOT. Provide an (arbitrary) argument to tell the script to pre-load the dictionary before attempting to `#include "bad.h"`. Requires access to a ROOT built with the same compiler and standard used in `buildit.sh`.
* `tidy.sh`
  Script to remove generated and backup files from this directory structure.

### Instructions.

In order to reproduce the error reported in https://sft.its.cern.ch/jira/projects/ROOT/issues/ROOT-10504:

1. Execute `./buildit.sh`.
1. Execute `./runit.sh` (failure demonstrates issue).
1. Execute `./runit.sh dummy` (success demonstrates a complication of the issue).
