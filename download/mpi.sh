cd $CBB_HOME/download
wget https://www.mpich.org/static/downloads/4.0.2/mpich-4.0.2.tar.gz
tar -xvzf mpich-4.0.2.tar.gz
cd mpich-4.0.2
./configure --prefix=$DIR/mpich
make -j 8
make install
cd $CBB_HOME/download
