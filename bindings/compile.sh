c++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -I../pybind11/include -I../msgpack-c/include -I../include  bindings.cpp -o example`python3-config --extension-suffix`
python mmtf_t.py
