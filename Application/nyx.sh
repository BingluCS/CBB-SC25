cd $CBB_HOME/Application

cd Nyx/subprojects
. build-sun.sh

cd $CBB_HOME/Application/Nyx/Exec/AMR-density

make clean
make -j 8 USE_HDF5_ZLIB=FALSE
mv Nyx3d* nocomp

make clean
make -j 8 USE_HDF5_ZLIB=TRUE
mv Nyx3d* zlib

make clean
make -j 8 USE_HDF5_SZADT=TRUE SZ_HOME=$SZ_HOME
mv Nyx3d* sz

cd $CBB_HOME/Application
