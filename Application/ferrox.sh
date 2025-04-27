cd $CBB_HOME/Application/FerroX/Exec/

make clean
make -j 8
mv main* nocomp

make clean
make -j 8 USE_HDF5_ZLIB=TRUE
mv main* zlib

make clean
make -j 8 USE_HDF5_SZADT=TRUE SZ_HOME=$SZ_HOME
mv main* sz

cd $CBB_HOME/Application
