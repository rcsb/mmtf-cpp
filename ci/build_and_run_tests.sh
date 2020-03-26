cd $TRAVIS_BUILD_DIR
mkdir build && cd build
$CMAKE_CONFIGURE cmake $CMAKE_ARGS $CMAKE_EXTRA ..
make -j2
ctest -j2 --output-on-failure
bash $TRAVIS_BUILD_DIR/ci/travis-test-example.sh
cd $TRAVIS_BUILD_DIR
