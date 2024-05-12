export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/qt@5/lib"
export CPPFLAGS="-I/opt/homebrew/opt/qt@5/include"
export PKG_CONFIG_PATH="/opt/homebrew/opt/qt@5/lib/pkgconfig"
export OTR_OSX_ARCH=arm64
export BASE_INSTALL_DIR=`pwd/..`

brew install cmake qt@5 opencv libomp create-dmg 
brew install imagemagick
brew install --cask opencat
brew reinstall ffmpeg@6
cp /opt/homebrew/Cellar/libpng/1.6.43/lib/libpng16.16.dylib /usr/local/lib/

cd ..
rm -rf build
cmake \
-DOpenCV_DIR=/opt/homebrew/opt/opencv/lib/cmake/ \
-DONNXRuntime_LIBRARY=$BASE_INSTALL_DIR/onnxruntime-osx-universal2-1.17.3/lib/libonnxruntime.dylib \
-DONNXRuntime_INCLUDE_DIR=$BASE_INSTALL_DIR/onnxruntime-osx-universal2-1.17.3/include \
-DOpenMP_CXX_FLAG="-Xclang -fopenmp" \
-DOpenMP_CXX_INCLUDE_DIR=/opt/homebrew/opt/libomp/include \
-DOpenMP_CXX_LIB_NAMES=libomp \
-DOpenMP_C_FLAG="-Xclang -fopenmp" \
-DOpenMP_C_INCLUDE_DIR=/opt/homebrew/opt/libomp/include \
-DOpenMP_C_LIB_NAMES=libomp \
-DOpenMP_libomp_LIBRARY=/opt/homebrew/opt/libomp/lib/libomp.dylib \
-DSDK_XPLANE=$BASE_INSTALL_DIR/SDK \
-S . -B build --toolchain cmake/apple.cmake

cd build
make install
