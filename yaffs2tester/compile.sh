#!/bin/bash
#PBS -t 1-500
#PBS -N yaffs-mutation
#PBS -o /home/maalipou/nodeoutput
#PBS -e /home/maalipou/nodeoutput



cd /home/maalipou/testsubjects/yaffstest/yaffs2tester


#PBS_ARRAYID=10
let START=PBS_ARRAYID*100-100
let END=PBS_ARRAYID*100
echo $END
echo $START
for NUM in `seq ${START} ${END}` 
do
    compile $NUM
done




compile () {
    NUM=$1
    MUTANTFILE=yaffs-mutants.txt    
    MUT=tail -n+$NUM $MUTANTFILE | head -n1
    OUT=$mut.exe
    YAFFS2_COMPILE_OPTIONS="-DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_YAFFS2 -DCONFIG_YAFFS_PROVIDE_DEFS -DCONFIG_YAFFSFS_PROVIDE_VALUES -I yaffs2/inc -I yaffs2"
    TCLAP_LIB_PATH=../tools/tclap-1.2.1
    gcc -o $OUT  -DNO_COVERAGE $YAFFS2_COMPILE_OPTIONS -lstdc++ -ldl -O4 $MUT  testcaserunner.cpp 
    echo  $YAFFS2_COMPILE_OPTIONS
    echo $2
}