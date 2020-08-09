#! /bin/bash
#
# Make symbolic links needed by the .fcl files in these exercises.
#
# 1) Make a symlink to the directory that holds the input files.
# 2) Make an output directory. There are two options:
#    a) Make it in ${ART_WORKBOOK_OUTPUT_BASE} and sym link to it.
#    b) Make it is a real subdirectory of cwd.
#

if [ -z "$TOYEXPERIMENT_DIR" ]; then
  echo "You cannot run this script until the toyExperiment ups product has been setup."
  exit 1
fi

# Name of link that will be made in this directory
inputLink="inputFiles"

# The directory in which we expect to find the input files
inputDir=${TOYEXPERIMENT_DIR}/inputFiles

# Are the input files present?
if [ ! -e ${inputDir} ]; then
  echo "Cannot find the directory that contains the input files: " ${inputdir}
  echo "Please make sure that the directory exists and rerun this script"
  exit 1
fi

# If the symlink already exists ...
if [ ! -e "${inputLink}" ] && [ ! -h "${inputLink}" ]; then
  ln -s ${inputDir} ${inputLink}
elif [ -h "${inputLink}" ]; then
  existingLink=$(readlink ${inputLink})
  if [ "${existingLink}" != "${inputDir}" ]; then
     echo "${inputLink} exists and is a symlink to ${existingLink}".
     echo "It should be a link to ${inputDir}"
     echo "Please rename/remove it and rerun this script"
     exit 1
  fi
else
  echo "The file ${inputLink} exists but is not a link to ${inputDir}"
  echo "Please rename/remove it and rerun this script"
  exit 1
fi

# The name of the output directory; either a real directory or a symlink to one.
outputName="output"

# Ensure that the output directory or symlink exists.
if [ -n "${ART_WORKBOOK_OUTPUT_BASE}" ]; then

  # Case 1: make output directory in ART_WORKBOOK_OUTPUT_BASE and symlink to it.
  export ART_WORKBOOK_OUTPUT=${ART_WORKBOOK_OUTPUT_BASE}/$(whoami)/art_workbook_output
  if [ ! -e $ART_WORKBOOK_OUTPUT ]; then
    mkdir -p ${ART_WORKBOOK_OUTPUT}
  fi
  if [ ! -d ${ART_WORKBOOK_OUTPUT} ]; then
    echo "Error making the output directory " ${ART_WORKBOOK_OUTPUT}
    echo "There is already a file with that name and it is not a directory."
    echo "Please rename/remove this file and rerun this script"
    exit 1
  fi

  if [ ! -e "${outputName}" ] && [ ! -h "${outputName}" ]; then
    ln -s ${ART_WORKBOOK_OUTPUT} ${outputName}
  elif [ -h ${outputName} ]; then
    existingLink=$(readlink ${outputName})
    if [ "${existingLink}" != "${ART_WORKBOOK_OUTPUT}" ]; then
       echo "Error: ${outputName} exists and is a symlink to ${existingLink}".
       echo "It should be a link to ${ART_WORKBOOK_OUTPUT}"
       echo "Please rename/remove it and rerun this script"
       exit 1
    fi
  else
    echo "Error: ${outputName} exists but is not a link to ${ART_WORKBOOK_OUTPUT}."
    echo "Please rename/remove it and rerun this script"
    exit 1
  fi

else

  # Case 2: make the directory in cwd.
  if [ ! -e ${outputName} ] && [ ! -h ${outputName} ]; then
    mkdir ${outputName}
  fi
  if [ ! -d  ${outputName} ]; then
    echo "Error making the output directory " ${outputName}
    echo "There is already a file with that name and it is not a directory."
    echo "Please rename/remove this file and rerun this script"
    exit 1
  fi

fi
