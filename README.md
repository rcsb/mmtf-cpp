<!--- Batch image URLs generated at https://shields.io -->
[![Release](https://img.shields.io/github/release/rcsb/mmtf-cpp.svg?style=flat)](https://github.com/rcsb/mmtf-cpp/releases)
[![License](https://img.shields.io/github/license/rcsb/mmtf-cpp.svg?style=flat)](https://github.com/rcsb/mmtf-cpp/blob/master/LICENSE)
[![Build Status (Travis)](https://img.shields.io/travis/rcsb/mmtf-cpp/master.svg?style=flat)](https://travis-ci.org/rcsb/mmtf-cpp)
[![Build Status (AppVeyor)](https://img.shields.io/appveyor/ci/rcsb/mmtf-cpp/master.svg?style=flat)](https://ci.appveyor.com/project/rcsb/mmtf-cpp)

The <b>m</b>acro<b>m</b>olecular <b>t</b>ransmission <b>f</b>ormat
([MMTF](http://mmtf.rcsb.org)) is a binary encoding of biological structures.

This repository holds the C++-03 compatible API, encoding and decoding
libraries. The MMTF specification can be found
[here](https://github.com/rcsb/mmtf/blob/HEAD/spec.md/).

## Prerequisites for using MMTF in C++

You need the headers of the MessagePack C++ library (version 2.1.5 or newer).
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

For your more complicated projects, a `CMakeLists.txt` is included for you.


### Python bindings

The C++ MMTF library now can build python bindings using pybind11.  To use them
you must have A) a c++11 compatible compiler and B) python >= 3.6

```python
from mmtf_cppy import StructureData
import numpy as np
import math


def rotation_matrix(axis, theta):
    """
    Return the rotation matrix associated with counterclockwise rotation about
    the given axis by theta radians.
    from https://stackoverflow.com/a/6802723
    """
    axis = np.asarray(axis)
    axis = axis / math.sqrt(np.dot(axis, axis))
    a = math.cos(theta / 2.0)
    b, c, d = -axis * math.sin(theta / 2.0)
    aa, bb, cc, dd = a * a, b * b, c * c, d * d
    bc, ad, ac, ab, bd, cd = b * c, a * d, a * c, a * b, b * d, c * d
    return np.array([[aa + bb - cc - dd, 2 * (bc + ad), 2 * (bd - ac)],
                     [2 * (bc - ad), aa + cc - bb - dd, 2 * (cd + ab)],
                     [2 * (bd + ac), 2 * (cd - ab), aa + dd - bb - cc]])


theta = 1.2
axis = [0, 0, 1]

sd = StructureData("my_favorite_structure.mmtf")
sd.atomProperties["pymol_colorList"] = [1 if x % 2 == 0 else 5 for x in sd.xCoordList]
xyz = np.column_stack((sd.xCoordList, sd.yCoordList, sd.zCoordList))
xyz_rot = rotation_matrix(axis, theta).dot(xyz.T).T
sd.xCoordList, sd.yCoordList, sd.zCoordList = np.hsplit(xyz_rot, 3)
sd.write_to_file("my_favorite_structure_rot.mmtf")

```



## Installation
You can also perform a system wide installation with `cmake` and `ninja` (or `make`).  
To do so:
```bash
mkdir build
cd build
cmake -G Ninja ..
sudo ninja install
```

`cmake` automatically sets the installation directory to `/usr/local/include`, if you want to install it to another `*include/` directory
run `cmake` with the command:
```bash
cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/home/me/local ..
```
Be aware that `/include` is added to the end of `DCMAKE_INSTALL_PREFIX` and that is where your files are installed (i.e. the above would install at `/home/me/local/include/`).


## Examples and tests

To build the tests + examples we recommend using the following lines:

```bash
# download Catch2 testing framework, msgpack-c, and the mmtf-spec test-dataset
git submodule update --init --recursive
mkdir build
cd build
cmake -G Ninja -DBUILD_TESTS=ON -Dmmtf_build_local=ON -Dmmtf_build_examples=ON ..
ninja
chmod +x ./tests/mmtf_tests
./tests/mmtf_tests
```

Example codes:
- mmtf_demo.cpp: Loads an MMTF file and checks internal consistency using
            mmtf::StructureData::hasConsistentData.
```bash
./examples/mmtf_demo ../mmtf_spec/test-suite/mmtf/173D.mmtf
```
- traverse.cpp: Loads an MMTF file and dumps it in human-readable forms.
```bash
./examples/traverse ../mmtf_spec/test-suite/mmtf/173D.mmtf
./examples/traverse ../mmtf_spec/test-suite/mmtf/173D.mmtf json
./examples/traverse ../mmtf_spec/test-suite/mmtf/173D.mmtf print
```

- print_as_pdb.cpp: Loads an MMTF file and prints it in pdb format.
```bash
./examples/print_as_pdb ../mmtf_spec/test-suite/mmtf/173D.mmtf
```

## Benchmark

Using the following simple code:
```cpp
#include <mmtf.hpp>


int main(int argc, char** argv)
{
        for (int i=1; i<argc; ++i) {
                mmtf::StructureData sd;
                mmtf::decodeFromFile(sd, argv[i]);
        }
}
```
compiled via:
```
g++ -Ofast -Immtf/include  -Imsgpack/include  decode_all_mmtf.cpp
```

We are able to load 153,987 mmtf files (current size of the pdb) or 14.3GB from an SSD in 211.3 seconds (averaged over 4 runs with minimal differences between the runs).

## Code documentation

You can generate a [doxygen](http://www.doxygen.org) based documentation of the
library by calling `doxygen` in the docs folder. You will need doxygen 1.8.11 or
later for that. Open `docs/html/index.html` to see the generated documentation.
