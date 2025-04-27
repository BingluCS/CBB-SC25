cd $CBB_HOME/download
wget -c -4 https://github.com/madler/zlib/archive/refs/tags/v1.2.12.tar.gz
tar -xvzf v1.2.12.tar.gz
cd zlib-1.2.12/
./configure --prefix=$DIR/grib2
make -j 8
make install

cd $CBB_HOME/download
wget https://www2.mmm.ucar.edu/wrf/OnLineTutorial/compile_tutorial/tar_files/libpng-1.2.50.tar.gz
tar xzvf libpng-1.2.50.tar.gz     #or just .tar if no .gz present
cd libpng-1.2.50
./configure --prefix=$DIR/grib2
make -j 8
make install

cd $CBB_HOME/download
wget https://www2.mmm.ucar.edu/wrf/OnLineTutorial/compile_tutorial/tar_files/jasper-1.900.1.tar.gz
tar xzvf jasper-1.900.1.tar.gz  
cd jasper-1.900.1
./configure --prefix=$DIR/grib2
make -j 8
make install
cd $CBB_HOME/download
