#!/bin/bash

TEST_PGM=inc-expand
WORKDIR=`mktemp -d ${TMPDIR:-/tmp}/${TEST_PGM}.XXXXXXXXXX`
[[ -n "$WORKDIR" ]] && [[ -d "$WORKDIR" ]] || [[ -w "$WORKDIR" ]] || exit 1

# Clean up if we're not debugging.
[[ -z "$DEBUG" ]] && trap "[[ -d \"$WORKDIR\" ]] && rm -rf \"$WORKDIR\"" EXIT

OUTPUT_FILE=${WORKDIR}/out.txt

rm -rf ${OUTPUT_FILE}
${TEST_PGM} > ${OUTPUT_FILE} <<EOF
EOF
STATUS=$?
[[ ${STATUS}           ]] || exit ${STATUS}
[[ -r ${OUTPUT_FILE}   ]] || exit 11
[[ ! -s ${OUTPUT_FILE} ]] || exit 12

rm -rf ${OUTPUT_FILE}
${TEST_PGM} - > ${OUTPUT_FILE} <<EOF
hello
EOF
STATUS=$?
[[ ${STATUS}         ]] || exit ${STATUS}
[[ -r ${OUTPUT_FILE} ]] || exit 21
[[ -s ${OUTPUT_FILE} ]] || exit 22

rm -rf ${OUTPUT_FILE}
${TEST_PGM} a b c > ${OUTPUT_FILE}
STATUS=$?
[[ ${STATUS} == 3 ]] || exit ${STATUS}

rm -rf ${OUTPUT_FILE}
F1=${WORKDIR}/F1.txt
F2=${WORKDIR}/F2.txt
F3=${WORKDIR}/F3.txt
FEXPECTED=${WORKDIR}/expected.txt
cat - > ${F1} <<EOF
hello
EOF
cat - > ${F2} <<EOF
#include "${F1}"
there
EOF
cat - > ${F3} <<EOF
#include "${F2}"
moo
oink
EOF
cat - > ${FEXPECTED} <<EOF
hello
there
moo
oink
EOF
${TEST_PGM} ${F3} > ${OUTPUT_FILE}
STATUS=$?
[[ ${STATUS}          ]] || exit ${STATUS}
cmp ${FEXPECTED} ${OUTPUT_FILE} || exit 31

exit 0
