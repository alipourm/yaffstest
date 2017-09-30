#!/bin/bash
#PBS -t 1-180
#PBS -N reducer-js
#PBS -o /home/maalipou/nodeoutput
#PBS -e /home/maalipou/nodeoutput

module load python python/2.7
SUT=js
SUTEXTENSION=.js

tc=/project/alipour/test-reduction-data/testcases/$SUT/tc$PBS_ARRAYID$SUTEXTENSION

cd $TMPDIR
cp -r /home/maalipou/code/test-reduction/ .
cd test-reduction
for i in  95 90 80 70 60 50 
do
    python phased-reduction.py -sut $SUT -tc $tc -c $i -out1 $tc.$i.1 -out2 $tc.$i.2 -log $tc.$i.log
done

rm -rf $TMPDIR

