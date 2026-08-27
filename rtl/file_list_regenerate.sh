#!/usr/bin/env bash

# Generate file-list file for the entire processor/SoC using GHDL's elaborate option.

set -e
cd $(dirname "$0")

# top entity
TOP=astrarv32_top

# file-list file
FLIST=file_list_core.f

# rtl path placeholder
PLACEHOLDER="\$ASTRARV32_HOME/rtl"

# temporary GHDL project
mkdir -p ~build
ghdl -i --work=astrarv32 --workdir=~build core/*.vhd

# elaborate design and build file list
echo "Regenerating $FLIST ..."
ghdl --elab-order --work=astrarv32 --workdir=~build $TOP > ~$FLIST
while IFS= read -r line; do
  echo "$PLACEHOLDER/$line"
done < ~$FLIST > $FLIST

# clean-up temporaries
rm -rf ~build ~$FLIST
