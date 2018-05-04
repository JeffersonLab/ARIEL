#!/bin/bash

status=0

prefix=$CETPKG_SOURCE/test/MessageService
refsubdir=$prefix/output-references/ELdestinationTester

for testdir in `find -type d -wholename "*test/MessageService/*.d"`
do

    # Get reference directory
    dirSuffix=${testdir#*messagefacility_}
    dirId=${dirSuffix%_t.d}
    refdir=$refsubdir/$dirId

    # If testdir is not empty, redact timestamps, which will not be
    # the same wrt reference
    if test "$(ls -A "$testdir")"; then
      for file in $testdir/*
      do
          sed -r -f $prefix/filter-timestamps.sed $file > tmp
          mv tmp $file
      done
    else
        if [ -d ${refdir} ]; then
            echo Directory $testdir should not be empty!  It should contain:
            echo `ls ${refdir}/`
        else continue
        fi
    fi

    # Compare outputs
    diff -rq $testdir $refdir

done

exit $status
