PFS_path=$(readlink -f ./PFS)
BB_path=$(readlink -f ./BB)
CBB_path=$(readlink -f ./CBB)

export PFS=$(readlink -f ./PFS)
export BB=$(readlink -f ./BB)
unset test1
inputs="inputs-bb"

increment=$((10 * 1024 * 1024 * 1024))
if [ "$1" = "warpx" ]; then
    app_path=$CBB_HOME/Application/warpx_directory/WarpX/$2
    chk=chk000400
    increment=$((25 * 1024 * 1024 * 1024))
elif [ "$1" = "ferrox" ]; then
    app_path=$CBB_HOME/Application/FerroX/Exec/$2
    increment=$((25 * 1024 * 1024 * 1024))
elif [ "$1" = "nyx" ]; then
    app_path=$CBB_HOME/Application/Nyx/Exec/AMR-density/$2
    increment=$((10 * 1024 * 1024 * 1024))
    chk=chk00240
elif [ "$1" = "maestroex" ]; then
    app_path=$CBB_HOME/Application/MAESTROeX/Exec/test_problems/reacting_bubble/$2
    increment=$((10 * 1024 * 1024 * 1024))
    chk=chk0000100
else
    echo "error: $1"
    return 1
fi

iterations=8
type=$2
export CBB_class=$2
cd ${1}Dir
for ((j=0; j<=iterations; j++))
do
    i=$((j * increment))
    export BBCapacity=$i
    rm -rf $PFS_path/*
    rm -rf $BB_path/*
    if [ $2 = "cbb" ]; then
        type="nocomp"
        inputs="inputs-cbb"
        rm -rf $CBB_path/*
        cp -r  $CBB_HOME/workDir/sim_bb/$1/* $BB_path/
        cp -r  cp -r $CBB_HOME/workDir/checkpoint/$chk $CBB_path/
        cp -r $CBB_HOME/workDir/sim_bb/chk000400 $PFS_path
    # elif [ $2 = "szcbb" ]; then
    #     type="nocomp"
    #     inputs="inputs-cbb"
    #     rm -rf $CBB_path/*
    #     cp -r  $CBB_HOME/workDir/sim_bb_sz/$1/* $BB_path/
    else
        if [ $1 != "ferrox" ]; then
            cp -r $CBB_HOME/workDir/checkpoint/$chk $PFS_path/
        fi
        inputs="inputs-bb"
    fi
    mpirun -np 8 $app_path  inputs-bb >> $1-$2-$j
done
cd $CBB_HOME/workDir