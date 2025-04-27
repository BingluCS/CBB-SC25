#!/bin/bash
wget -c -4 https://github.com/LLNL/sundials/archive/refs/tags/v6.7.0.tar.gz
tar xvf v6.7.0.tar.gz
mv sundials-6.7.0/ sundials
cd sundials
mkdir builddir instdir
INSTALL_PREFIX=$(pwd)/instdir
cd builddir
cmake \
-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}  \
-DCMAKE_INSTALL_LIBDIR=lib \
-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
-DCMAKE_C_COMPILER=$(which gcc)  \
-DCMAKE_CXX_COMPILER=$(which g++)   \
-DCMAKE_CUDA_HOST_COMPILER=$(which g++)    \
-DEXAMPLES_INSTALL_PATH=${INSTALL_PREFIX}/examples \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_C_FLAGS_RELEASE="-O3 -DNDEBUG" \
-DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG"  \
-DCUDA_ENABLE=OFF  \
-DMPI_ENABLE=OFF  \
-DOPENMP_ENABLE=ON   \
-DF2003_INTERFACE_ENABLE=OFF   \
-DSUNDIALS_INDEX_SIZE:INT=32 ../
make -j8
make install -j8
