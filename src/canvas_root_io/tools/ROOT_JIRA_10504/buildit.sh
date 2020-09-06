#!/bin/bash
########################################################################
# buildit.sh
#
# Generate and build the dictionary required to demonstrate the issue.
########################################################################
prog=${BASH_SOURCE[0]##*/}

(return 0 2>/dev/null) && \
  { echo "ERROR: $prog should be executed, not sourced." 1>&2; return 1; }

# Fail on command error.
set -e

# Echo commands.
set -x

# Execute in the script directory.
cd "$(dirname $(realpath ${BASH_SOURCE[0]}))"

# Ensure the existence of a directory for the dictionary and associated files.
mkdir -p lib

# Generate the dictionary.
genreflex classes.h -s classes_def.xml --fail_on_warnings --noIncludePaths -l lib/libcanvas_root_io_Dictionaries_std_dict.so --rootmap-lib=libcanvas_root_io_Dictionaries_std_dict.so --rootmap=lib/libcanvas_root_io_Dictionaries_std_dict.rootmap -o canvas_root_io_Dictionaries_std_dict.cpp

# Compile the dictionary source.
g++ -Dcanvas_root_io_Dictionaries_std_dict_EXPORTS -I"$(root-config --incdir)" -g -O0 -Werror -pedantic -std=c++17 -Wall -Werror=return-type -Wextra -Wno-long-long -Winit-self -Wno-unused-local-typedefs -Woverloaded-virtual -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -fPIC -o canvas_root_io_Dictionaries_std_dict.cpp.o -c canvas_root_io_Dictionaries_std_dict.cpp

# Link the dictionary into a shared library.
g++ -fPIC -g -O0 -Werror -pedantic -std=c++17 -Wall -Werror=return-type -Wextra -Wno-long-long -Winit-self -Wno-unused-local-typedefs -Woverloaded-virtual -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -Wl,--no-undefined -shared -Wl,-soname,libcanvas_root_io_Dictionaries_std_dict.so -o lib/libcanvas_root_io_Dictionaries_std_dict.so canvas_root_io_Dictionaries_std_dict.cpp.o -Wl,-rpath,`pwd`/lib: -L"$(root-config --libdir)" -lCore
