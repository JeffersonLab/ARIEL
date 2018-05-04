#!/bin/bash
# Program name
prog=${0##*/}
# ======================================================================
function usage() {
    cat 1>&2 <<EOF
usage: $prog <top-dir>
EOF
}

get_this_dir()
{
    ( cd / ; /bin/pwd -P ) >/dev/null 2>&1
    if (( $? == 0 )); then
        pwd_P_arg="-P"
    fi
    reldir=`dirname ${0}`
    thisdir=`cd ${reldir} && /bin/pwd ${pwd_P_arg}`
}

function cmake_file() {
    local F=$1
    printf "$F ... "
    # Fix CMakeLists.txt
    perl -wapi\~ -f ${thisdir}/fix_cmake_canvas_root_io.pl "${F}"
    #>/dev/null 2>&1 && rm -f "${F}~"
}

# ======================================================================
# Check number of supplied arguments
if [[ "$#" -ne 1 ]] ; then
    echo "Incorrect number of arguments specified."
    usage
    exit 1
fi

TOP=${1}

get_this_dir

# ======================================================================
# Run scripts to update

TMP=`mktemp -t update_sources.sh.XXXXXX`
trap "rm $TMP* 2>/dev/null" EXIT

for F in `find $TOP \( \( -name .svn -o -name .git -o -name CVS \) -prune \) -o \( -name CMakeLists.txt -o -name '*.cmake' \) -print`
do
    if grep -q -e "find_ups_product(\s*canvas_root_io" $F; then
        echo "File: $F already has the correct dependencies"
        continue
    fi
    cmake_file "$F"
    echo
done
