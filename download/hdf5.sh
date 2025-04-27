cd $CBB_HOME/download
wget -c -4 https://github.com/HDFGroup/hdf5/archive/refs/tags/hdf5-1_12_2.tar.gz
tar -xvzf hdf5-1_12_2.tar.gz
mv hdf5-hdf5-1_12_2/ hdf5-1_12_2/
cd hdf5-1_12_2
./configure --prefix=$DIR/hdf5 --with-zlib=$DIR/grib2 --enable-hl --enable-fortran --enable-parallel MPI_CC=mpicc MPI_CXX=mpicxx
make -j 8
make install
cd $CBB_HOME/download