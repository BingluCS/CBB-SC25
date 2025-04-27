cd $DIR/SZ_ADT
mkdir install
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=$DIR/SZ_ADT/install -DBUILD_HDF5_FILTER=ON ..
export SZ_HOME=$DIR/SZ_ADT/install 
make -j 8
make install
cd $DIR
