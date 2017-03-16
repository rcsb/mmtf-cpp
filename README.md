
The <b>m</b>acro<b>m</b>olecular <b>t</b>ransmission <b>f</b>ormat
([MMTF](http://mmtf.rcsb.org)) is a binary encoding of biological structures.

This repository holds the C++-03 compatible API, encoding and decoding
libraries. The MMTF specification can be found
[here](https://github.com/rcsb/mmtf/blob/HEAD/spec.md/).

## Prerequisites for using MMTF in C++

You need the headers of the MessagePack C++ library (version 2.1.1 or newer).
If you do not have them on your machine already, you can download the "include"
directory from the
[MessagePack GitHub repository](https://github.com/msgpack/msgpack-c).

## How to use MMTF

You only need to include the mmtf.hpp header in your code to use MMTF.
For instance a minimal example to load an MMTF file looks as follows:

```C
#include <mmtf.hpp>

int main(int argc, char** argv) {
    mmtf::StructureData data;
    mmtf::decodeFromFile(data, "test.mmtf");
    return 0;
}
```

The C++ MMTF library is header only so you do not need to compile it. If you
have a source file named `demo.cpp` (e.g. including the code above), you can
generate an executable `demo.exe` as follows:

```bash
g++ -I<MSGPACK_INCLUDE_PATH> -I<MMTF_INCLUDE_PATH> demo.cpp -o demo.exe
```

Here, `<MSGPACK_INCLUDE_PATH>` and `<MMTF_INCLUDE_PATH>` are the paths to the
"include" directories of MessagePack and this library respectively.

## Examples and tests

The examples folder contains tests and examples for you to run.
To test the library on your machine, you can run:
```bash
./test_compile_and_traverse.sh <MSGPACK_INCLUDE_PATH>
```
If it concludes stating that the test has passed, it is all good.

Example codes:
- demo.cpp: Loads an MMTF file and checks internal consistency using
            mmtf::StructureData::hasConsistentData.
- traverse.cpp: Loads an MMTF file and dumps it in human-readable forms.
                The json-like output is used in test_compile_and_traverse.sh.

Furthermore, we have:
- data folder: MMTF test set
- out_json_ref.tar.gz: Reference output generated using the
                       [mmtf-c library](https://github.com/rcsb/mmtf-c).
                       This is used in test_compile_and_traverse.sh.
- compile_and_run.sh: Convenience script to compile and run an executable with
                      a single argument. E.g. with demo.cpp:
```bash
./compile_and_run.sh <MSGPACK_INCLUDE_PATH> demo.cpp "./data/173D.mmtf"
```
- compile_and_run_all.sh: Similar to compile_and_run.sh, but processing all
                          .mmtf files in the data folder. E.g. with demo.cpp:
```bash
./compile_and_run_all.sh <MSGPACK_INCLUDE_PATH> demo.cpp
```

## Code documentation

You can generate a [doxygen](http://www.doxygen.org) based documentation of the
library by calling `doxygen` in the docs folder. You will need doxygen 1.8.11 or
later for that. Open `docs/html/index.html` to see the generated documentation.
