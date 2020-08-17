#!/bin/bash

[[ -n "$DEBUG" ]] && set -x

TEST_PGM=fhicl-expand
TEST_ARGS="--lookup-policy=nonabsolute"
WORKDIR=`mktemp -d ${TMPDIR:-/tmp}/${TEST_PGM}.XXXXXXXXXX`
[[ -n "$WORKDIR" ]] && [[ -d "$WORKDIR" ]] || [[ -w "$WORKDIR" ]] || exit 1

export FHICL_FILE_PATH="$WORKDIR"

# Clean up if we're not debugging.
trap "if [[ -n \"$DEBUG\" ]]; then echo \"$WORKDIR\"; else [[ -d \"$WORKDIR\" ]] && rm -rf \"$WORKDIR\"; fi" EXIT

OUTPUT_FILE=${WORKDIR}/out.txt

rm -rf ${OUTPUT_FILE}
${TEST_PGM} ${TEST_ARGS} > ${OUTPUT_FILE} <<EOF
EOF
STATUS=$?
[[ ${STATUS}           ]] || exit ${STATUS}
[[ -r ${OUTPUT_FILE}   ]] || exit 11
[[ ! -s ${OUTPUT_FILE} ]] || exit 12

rm -rf ${OUTPUT_FILE}
${TEST_PGM} ${TEST_ARGS} - > ${OUTPUT_FILE} <<EOF
hello
EOF
STATUS=$?
[[ ${STATUS}         ]] || exit ${STATUS}
[[ -r ${OUTPUT_FILE} ]] || exit 21
[[ -s ${OUTPUT_FILE} ]] || exit 22

rm -rf ${OUTPUT_FILE}
${TEST_PGM} ${TEST_ARGS} a b c > ${OUTPUT_FILE}
STATUS=$?
[[ ${STATUS} == 3 ]] || exit ${STATUS}

rm -rf ${OUTPUT_FILE}
F1=${WORKDIR}/F1.txt
F1A=${WORKDIR}/F1A.txt
F2=${WORKDIR}/F2/F2.txt
F3=${WORKDIR}/F3.txt
FEXPECTED=${WORKDIR}/expected.txt
cat - > ${F1} <<EOF
hello
EOF
cat - > ${F1A} <<EOF
Hi
EOF
mkdir -p `dirname "${F2}"`
cat - > ${F2} <<EOF
#include "${F1##*/}"
there
EOF
cat - > ${F3} <<EOF
#include "${F1A##*/}"
#include "${F2##*/}"
moo
oink
EOF
cat - > ${FEXPECTED} <<EOF
Hi
hello
there
moo
oink
EOF
export FHICL_FILE_PATH="$WORKDIR/F2:$WORKDIR"
${TEST_PGM} ${TEST_ARGS} ${F3} > ${OUTPUT_FILE}
STATUS=$?
[[ ${STATUS}          ]] || exit ${STATUS}
cmp ${FEXPECTED} ${OUTPUT_FILE} || exit 31

exit 0
