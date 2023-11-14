
python3 --version
pip3 --version
pip3 install -r requirements.txt
pip3 install -r requirements-dev.txt
cd $TRAVIS_BUILD_DIR
pip3 install .
pytest python_src/tests/tests.py -s -vv


