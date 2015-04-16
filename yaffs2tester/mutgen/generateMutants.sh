#!/bin/bash
# generateMutants.sh -- generate mutants of file
# Copyright (c) 2005 Jamie Andrews (andrews [[at]] csd.uwo.ca)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
#
# Changes:
#   None (first public release).
#   08/22/2011 by Chaoqiang Zhang
#   Randomly generate some mutants using user specified number.

if test $# = 3
then
  SOURCE=$1
  MutantNums=$2
  MutantIdx=$3
  Idx=$(($MutantIdx-1)) 
else
  echo Usage: $0 source-filename num-of-mutants
  #exit 1
fi

DESCS=${SOURCE}.MutantDescs.txt

echo Current directory: `pwd`
echo Source file: $SOURCE

echo Generating mutant descriptions into file ${DESCS}...

index=0
while read line ; do
    #Mutants[$index]="${line%?}"
    Mutants[$index]="${line}"
    index=$(($index+1))
done < $MutantNums

Mutant=0
# Starting with code 1000001 because that makes it much easier for
# shell scripts that are going to step through all mutants.
#echo ${#stringZ}
MutantCode=1000000
#Idx=0
#while test $Idx -lt $TotalMutants
#do
  Mutant=${Mutants[$Idx]}
  echo $Mutant
  #echo ${#Mutant}
  Idx=`expr $Idx + 1`
  #Mutant=`expr $Mutant + 1`
  #echo $Mutant
  MutantCode=`expr $MutantCode + $Idx`
  echo $MutantCode
  LineNo=`tail -n +$Mutant $DESCS | head -1 | cut -f 1 -d:`
  LineNoM1=`expr $LineNo - 1`
  LineNoP1=`expr $LineNo + 1`
  # echo LineNoM1 $LineNoM1 LineNoP1 $LineNoP1
  MutantFilename=mutant${MutantCode}_${SOURCE}

  echo Generating mutant source file $MutantFilename with mutated line $LineNo
  head -$LineNoM1 $SOURCE >$MutantFilename
  tail -n +$Mutant $DESCS | head -1 | cut -f 2- -d: >>$MutantFilename
  tail -n +$LineNoP1 $SOURCE >>$MutantFilename
#done

#echo Number of mutants generated in `pwd`: $Mutant

exit 0
