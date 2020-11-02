## Workbook

The *art* [workbook](doc/art-workbook-v0_91.pdf) is a 500+ document written for beginning C++ programmers who wish to learn about writing art plugins and modules. It comes with various exercises and example code, which can be found in this directory.

The workbook document is out of date at this time. Some information refers to older versions of *art* or the workbook code. Any references to the UPS package manager, buildtool etc. should be ignored. Neverthless, the bulk of the material about programming in C++ and writing art modules is still applicable. Just take things with a grain of salt. Updates will be forthcoming.

Build the workbook code as follows:

~~~~~~~~~~bash
module use [ariel-install-prefix]/modulefiles
module load ariel
mkdir -p ~Analysis/art-workbook/{build,output}
cd ~Analysis/art-workbook/build
export ART_WORKBOOK_OUTPUT=~/Analysis/art-workbook/output
module load workbook
cmake $ARIEL/workbook
make -j$(nproc)
~~~~~~~~~~

Then run the examples in the `fcl` directory like so:

`art -c fcl/FirstModule/first.fcl`

etc.
