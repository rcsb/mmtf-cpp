#!/bin/bash

if [ "$1" == "" ]; then
  echo "USAGE: compile_and_traverse_all.sh <MSGPACK_INCLUDE_PATH>"
  echo "-> traverse all mmtf files and write to out subfolder"
  exit 1
fi

# compile
./compile_target.sh $1 traverse.cpp

# get output folder
mkdir -p out

# traverse all
FILES="./data/*.mmtf"
for FILE in $FILES
do
  MMTF_FILENAME=$(basename $FILE)
  # OUT_FILENAME_PDB="./out/${MMTF_FILENAME%.*}.pdb"
  # OUT_FILENAME_TXT="./out/${MMTF_FILENAME%.*}.txt"
  OUT_FILENAME_JSON="./out/${MMTF_FILENAME%.*}.json"
  echo "Processing $MMTF_FILENAME..."
  # ./traverse.exe $FILE > $OUT_FILENAME_PDB
  # ./traverse.exe $FILE txt > $OUT_FILENAME_TXT
  ./traverse.exe $FILE json > $OUT_FILENAME_JSON
done
