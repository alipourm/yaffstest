#!/usr/bin/python

import os
import shutil
import subprocess
import time
import signal
import re
import getopt, sys
import glob
from optparse import OptionParser
from time import localtime, strftime
import string
import datetime
import myutils
import itertools
from operator import or_
#---------------------------------------------------------------------------------------------------------------------------------
# Global variables
OPTIMIZATION='-O2'
YAFFS2_COMPILE_OPTIONS = '-DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_YAFFS2 -DCONFIG_YAFFS_PROVIDE_DEFS \
                            -DCONFIG_YAFFSFS_PROVIDE_VALUES \
                            -I yaffs2/inc -I yaffs2 -O2 -fprofile-arcs -ftest-coverage'
TIMEOUT_LIMIT=6 # in seconds
# please set this manually if you want to use different set of mutants.

MUTANTS_PATH='%s/yaffs2/mutants/500_b*/*_yaffs2.c'

INSTRUMENTOR_PATH='../../tools/cil-1.3.7/bin/cilly'
TCLAP_LIB_PATH='../../tools/tclap-1.2.1'
COVERAGE_TOOLS_PATH='../../tools/coveragehooks/'

#---------------------------------------------------------------------------------------------------------------------------------
G_WORKING_FOLDER=""
G_TMP_FOLDER=""
# grootfolder is the folder where this script stay
G_ROOT_FOLDER=""

configs=[]

#---------------------------------------------------------------------------------------------------------------------------------
def mkArray(size):
    arr = []
    for i in range(size):
        arr.append(0)
    return arr
#---------------------------------------------------------------------------------------------------------------------------------
def generateCoverageInfo(startidx, endidx, tslength):    

    myutils.exec_cmd('gcc -o yaffs2_gcov yaffs2/yaffs2.c testcaserunner.cpp -DNO_COVERAGE %s %s -lstdc++ -ldl'\
                            %(YAFFS2_COMPILE_OPTIONS, OPTIMIZATION))

    # load mutants' lines information
    mutants = mkArray(1000)
    
    lines = myutils.getFileLines('yaffs2/mutants/m.txt')
    
    for idx in range(len(lines)):
        line = lines[idx]
        parts = line.split(':')
        lineno = parts[1].strip()
        #print(lineno)
        #print(idx)
        if len(lineno) > 0:
            mutants[idx] = int(lineno)
    #print mutants
    #return
    for tsidx in range(startidx, endidx+1):
        myutils.exec_cmd('rm yaffs2.c.gcov')
        tsfile = '%s/testcases/%06d/ts%06d.c'%(G_WORKING_FOLDER, tslength, tsidx)               

        myutils.exec_cmd('%s/yaffs2_gcov -start %d -num %d -testcasefile %s'%(G_ROOT_FOLDER, 0, 1, tsfile))
        myutils.exec_cmd('gcov yaffs2.c')
        uncovered = mkArray(14761)
        getCoveredLines('yaffs2.c.gcov', uncovered)
        #print uncovered
        flags = mkArray(1000)
        for i in range(1000):
            if uncovered[ mutants[i]-1 ] == 1:
                flags[i] = 1
        myutils.appendArray('filtertable', flags)              
            

    os.chdir(G_ROOT_FOLDER)
#--------------------------------------------------------------------------------------------------------------------------------
# return an array which record which lines are executable but not executed(take not executable code as executed)
def getCoveredLines(filename, arr):
    lines = myutils.getFileLines(filename)
    for idx in range(len(lines)):
        line = lines[idx]
        parts = line.split(':')
        if parts[0].find('#####')>=0:
            lineno = parts[1].strip()
            arr[int(lineno)-1] = 1 # not covered      
#--------------------------------------------------------------------------------------------------------------------------------
def main():
    global G_ROOT_FOLDER
    G_ROOT_FOLDER = os.getcwd()    
   
    now = datetime.datetime.now()
    randseed = now.microsecond
    parser = OptionParser()
   
    parser.add_option("", "--tslength",      dest="tslength",     action="store",      type = "int")
      
    parser.add_option("", "--startidx",      dest="startidx",     action="store",      type = "int") 
    parser.add_option("", "--endidx",        dest="endidx",       action="store",      type = "int")  
     
   
   
    parser.add_option("", "--workingfolder", dest="workingfolder",action="store",      type = "string")

        
    (options, args) = parser.parse_args()
    
        
    tslength = 200
    if options.tslength != None:
        tslength = options.tslength
           
    # set path related stuff, everthing is absolute.
    
    global G_WORKING_FOLDER
    if options.workingfolder != None:        
        if os.path.isabs(options.workingfolder) == True:
            G_WORKING_FOLDER = options.workingfolder
        else:
            G_WORKING_FOLDER = os.path.abspath(options.workingfolder)
            
    myutils.ensuredir(G_WORKING_FOLDER)
        
    global G_TMP_FOLDER
    G_TMP_FOLDER = '__tmp__%d'%(os.getpid())    
    G_TMP_FOLDER = os.path.abspath(G_TMP_FOLDER)
    myutils.ensuredir( G_TMP_FOLDER )             
        
        
    generateCoverageInfo(options.startidx, options.endidx, options.tslength) 
    

#---------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    main()
