
python3 --version
pip3 --version
pip3 install pytest pytest-cov
cd $TRAVIS_BUILD_DIR
pip3 install .
pytest python_src/tests/tests.py -s -vv


