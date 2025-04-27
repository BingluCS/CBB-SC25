export test1=1

rm -rf BB/*
cp -r checkpoint/chk000400 BB
cd $CBB_HOME/workDir/warpxDir
mpirun -np $2 $CBB_HOME/Application/warpx_directory/WarpX/$1  inputs-bb &
mpirun_pid=$!
python3 ../monitor.py "$mpirun_pid" warpx_cpu-$1.log  &
wait $mpirun_pid

rm -rf BB/*
cd $CBB_HOME/workDir/ferroxDir
mpirun -np $2 $CBB_HOME/Application/FerroX/Exec/$1  inputs-bb &
mpirun_pid=$!
python3 ../monitor.py "$mpirun_pid" ferrox_cpu-$1.log  &
wait $mpirun_pid

rm -rf BB/*
cp -r checkpoint/chk00240 BB
cd $CBB_HOME/workDir/nyxDir
mpirun -np $2 $CBB_HOME/Application/Nyx/Exec/AMR-density/$1  inputs-bb &
mpirun_pid=$!
python3 ../monitor.py "$mpirun_pid" nyx_cpu-$1.log  &
wait $mpirun_pid

rm -rf BB/*
cp -r checkpoint/chk0000100 BB
cd $CBB_HOME/workDir/maestroexDir
mpirun -np $2 CBB_HOME/Application/MAESTROeX/Exec/test_problems/reacting_bubble/$1  inputs-bb &
mpirun_pid=$!
python3 ../monitor.py "$mpirun_pid" maestroex_cpu-$1.log  &
wait $mpirun_pid