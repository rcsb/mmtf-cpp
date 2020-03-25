
pip install pytest pytest-cov
cd $TRAVIS_BUILD_DIR
pip install .
pytest python_src/tests/tests.py -s -vv


