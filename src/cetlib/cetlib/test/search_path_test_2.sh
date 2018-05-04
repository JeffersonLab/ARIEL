#!/bin/bash

export TMP_DIR=`mktemp -d /tmp/${0##*/}.XXXXXXXXXX`

[[ -z "$DEBUG" ]] && trap "[[ -d \"$TMP_DIR\" ]] && rm -rf \"$TMP_DIR\"" EXIT

T1="libthis_is_one.so libanother_one.so"
T2="libthis_is_two.so libanother_two.so"
T3="libthis_is_three.so"
T_ALL="$T1 $T2 $T3"

for name in ${T_ALL}
do
  touch "${TMP_DIR}/${name}"
done

echo `pwd`

search_path_test_2 "lib.*one.so"   TMP_DIR; test $? -eq 2 || exit 1
search_path_test_2 "lib.*two.so"   TMP_DIR; test $? -eq 2 || exit 2
search_path_test_2 "lib.*three.so" TMP_DIR; test $? -eq 1 || exit 3
search_path_test_2 "lib.*four.so"  TMP_DIR; test $? -eq 0 || exit 4

exit 0
