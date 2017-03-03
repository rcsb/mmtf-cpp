#!/bin/bash

if [ "$2" == "" ]; then
  echo "USAGE: compile_and_run_all.sh <MSGPACK_INCLUDE_PATH> <MAIN_FILE>"
  echo "-> for MAIN_FILE = test.cpp, we compile test.exe and run it for all"
  echo "   .mmtf files in data"
  exit 1
fi

MAIN_FILENAME="${2%.*}"

./compile_target.sh $1 $MAIN_FILENAME.cpp

FILES="./data/*.mmtf"
for FILE in $FILES
do
  echo "Processing $FILE..."
  ./$MAIN_FILENAME.exe $FILE
done
