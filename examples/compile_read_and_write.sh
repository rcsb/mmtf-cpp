#!/bin/bash

# compile
#./compile_read_and_write.sh $1(in mmtf) $2(out mmtf)
g++ -I"../../msgpack-c/include" -I"../include" -O2 -o read_and_write.exe read_and_write.cpp  && \
  ./read_and_write.exe $1 $2
