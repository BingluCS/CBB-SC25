cd warpx_directory
git clone https://github.com/ECP-WarpX/picsar.git
git clone https://github.com/ECP-WarpX/warpx-data.git

cd WarpX
make clean
make -j 8 
mv main3d* nocomp

make clean
make -j 8 USE_HDF5_ZLIB=TRUE
mv main3d* zlib

make clean
make -j 8 USE_HDF5_SZADT=TRUE SZ_HOME=$SZ_HOME
mv main3d* sz

cd $CBB_HOME/Application
