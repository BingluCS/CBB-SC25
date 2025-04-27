export DIR=$CBB_HOME/Libs
export JASPERLIB=$DIR/grib2/lib
export JASPERINC=$DIR/grib2/include
export LDFLAGS=-L$DIR/grib2/lib
export CPPFLAGS=-I$DIR/grib2/include
export PATH=$DIR/netcdf/bin:$PATH
export NETCDF=$DIR/netcdf
export HDF5=$DIR/hdf5
export LD_LIBRARY_PATH=$DIR/netcdf/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$DIR/hdf5/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$DIR/grib2/lib:$LD_LIBRARY_PATH

export LIBRARY_PATH=$DIR/grib2/lib:$LIBRARY_PATH
export C_INCLUDE_PATH=$DIR/grib2/include:$C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=$DIR/grib2/include:$CPLUS_INCLUDE_PATH


export PATH=$DIR/mpich/bin:$PATH 
export MPI_DIR=$DIR/mpich
export LD_LIBRARY_PATH=$MPI_DIR/lib:$LD_LIBRARY_PATH
export MANPATH=$MPI_DIR/share/man:$MANPATH
export C_INCLUDE_PATH=$MPI_DIR/include:$C_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=$MPI_DIR/include:$CPLUS_INCLUDE_PATH

export SZ_HOME=$DIR/SZ_ADT/install
# export SZ3_HOME=$DIR/orisz3/install
export HDF5_PLUGIN_PATH=$SZ_HOME/lib
export LD_LIBRARY_PATH=$SZ_HOME/lib:$LD_LIBRARY_PATH
