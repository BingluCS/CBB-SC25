cd $CBB_HOME/Application/MAESTROeX/Exec/test_problems/reacting_bubble

make clean
make -j 8 USE_HDF5_ZLIB=FALSE
mv Maestro3d* nocomp

make clean
make -j 8 USE_HDF5_ZLIB=TRUE
mv Maestro3d* zlib

make clean
make -j 8 USE_HDF5_SZADT=TRUE SZ_HOME=$SZ_HOME
mv Maestro3d* sz

cd $CBB_HOME/Application
