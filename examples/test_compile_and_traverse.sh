#!/bin/bash

if [ "$1" == "" ]; then
  echo "USAGE: test_compile_and_traverse.sh <MSGPACK_INCLUDE_PATH>"
  echo "-> compare compile_and_traverse_all.sh with out_json_ref.tar.gz"
  exit 1
fi

# compile
./compile_and_traverse_all.sh $1

# get reference set
tar zxvf out_json_ref.tar.gz

# compare
COMPARSION=$(diff out out_json_ref)
if [ "$COMPARSION" == "" ]; then
  echo "======================================================================"
  echo "TEST PASSED"
  echo "======================================================================"
else
  echo "======================================================================"
  echo "!!!! TEST FAILED !!!!!!"
  echo "Manually compare folders out and out_json_ref to see what failed!"
  echo "======================================================================"
fi
