
python3 --version
pip3 --version
pip3 install pytest pytest-cov --user
cd $TRAVIS_BUILD_DIR
pip3 install . --user
pytest python_src/tests/tests.py -s -vv


