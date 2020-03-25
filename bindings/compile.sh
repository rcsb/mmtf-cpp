
export PYTHONPATH=:/home/danpf/miniconda3/envs/py3/lib/python37.zip:/home/danpf/miniconda3/envs/py3/lib/python3.7:/home/danpf/miniconda3/envs/py3/lib/python3.7/lib-dynload:/home/danpf/.local/lib/python3.7/site-packages:/home/danpf/miniconda3/envs/py3/lib/python3.7/site-packages:/home/danpf/git/dimaiolab
c++ -O3 -Wall -shared -std=c++17 -fPIC `python3 -m pybind11 --includes` -I../pybind11/include -I../msgpack-c/include -I../include  bindings.cpp -o mmtf_cpp`python3-config --extension-suffix`
pytest tests.py -s -vv
