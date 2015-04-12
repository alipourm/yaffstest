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
OPTIMIZATION='-O4'
YAFFS2_COMPILE_OPTIONS = '-DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_YAFFS2 -DCONFIG_YAFFS_PROVIDE_DEFS \
                            -DCONFIG_YAFFSFS_PROVIDE_VALUES \
                            -I yaffs2/inc -I yaffs2'
TIMEOUT_LIMIT=6 # in seconds
# please set this manually if you want to use different set of mutants.

MUTANTS_PATH='%s/yaffs2/mutants/*_yaffs2.c'

INSTRUMENTOR_PATH='../tools/cil-1.3.7/bin/cilly'
TCLAP_LIB_PATH='../tools/tclap-1.2.1'
COVERAGE_TOOLS_PATH='../tools/coveragehooks/'

#---------------------------------------------------------------------------------------------------------------------------------
G_WORKING_FOLDER=""
G_TMP_FOLDER=""
# grootfolder is the folder where this script stay
G_ROOT_FOLDER=""

configs=[]
#--------------------------------------------------------------------------------------------------------------------------------
# precompile some modules, so we don't have to compile them again and again.
# precompile yaffs2 oracle versionm, different instrumented versions, and different mutants versions.
#
def preCompile():
    global G_ROOT_FOLDER
    G_ROOT_FOLDER = os.getcwd() 

    # build test case generator
    #myutils.exec_cmd('gcc -o testcasegenerator %s/testcasegenerator.cpp yaffs2/yaffs2.c -DENABLE_GMP -lgmp -I %s/include %s -lstdc++ -ldl'\
    #                            %(G_ROOT_FOLDER, TCLAP_LIB_PATH, YAFFS2_COMPILE_OPTIONS))
    # build yaffs oracle version for function return results.
    #myutils.exec_cmd('gcc -o yaffs2_oracle yaffs2/yaffs2.c testcaserunner.cpp -DNO_COVERAGE %s %s -lstdc++ -ldl'\
    #                        %(YAFFS2_COMPILE_OPTIONS, OPTIMIZATION))                            
    # gcov version for line and function coverage
    #myutils.exec_cmd('gcc -o yaffs2_gcov yaffs2/yaffs2.c testcaserunner.cpp -DNO_COVERAGE %s %s -lstdc++ -ldl'\
    #                        %(YAFFS2_COMPILE_OPTIONS, '--coverage'))

#--------------------------------------------------------------------------------------------------------------------------------     
def generateResultNew(tsfile, yaffs2exe):
    res = ''    
    if os.path.exists('testresult.txt'):
        os.remove('testresult.txt')
    myutils.exec_cmd('timeout %ss %s -start %d -num %d -testcasefile %s'%(TIMEOUT_LIMIT, yaffs2exe, 0, 1, tsfile))
    if( os.path.exists('testresult.txt')):
        res = myutils.readSingleLine('testresult.txt')
        os.remove('testresult.txt')
    if res == '':
        res = 'crash or infinite loop'
    return res
#--------------------------------------------------------------------------------------------------------------------------------- 
def main():
    global G_ROOT_FOLDER
    G_ROOT_FOLDER = os.getcwd()    
   
    now = datetime.datetime.now()
    randseed = now.microsecond
    parser = OptionParser()
    parser.add_option("", "--testcasesdir",  dest="testcasesdir", action="store",      type="string")
    parser.add_option("", "--mutantid",      dest="mutantid",     action="store",      type = "int") 
    parser.add_option("", "--mukillfile",    dest="mukillfile",   action="store",      type="string")
    (options, args) = parser.parse_args()
    # prepare mutant binary
    os.chdir('mutgen')
    myutils.exec_cmd('bash generateMutants.sh yaffs2.c yaffs2mutants %d'%(options.mutantid))
    if os.path.exists('mutant1%06d_yaffs2.c'%(options.mutantid)):
        myutils.exec_cmd('cp mutant1%06d_yaffs2.c ../muyaffs2.c'%(options.mutantid))
    else:
        assert(0)        
    os.chdir(G_ROOT_FOLDER)
    myutils.exec_cmd('gcc -o yaffs2_oracle yaffs2/yaffs2.c testcaserunner.cpp -DNO_COVERAGE %s %s -lstdc++ -ldl'\
                            %(YAFFS2_COMPILE_OPTIONS, OPTIMIZATION))
                            
    myutils.exec_cmd('gcc -o yaffs2_mutant muyaffs2.c testcaserunner.cpp -DNO_COVERAGE %s %s -lstdc++ -ldl'\
                            %(YAFFS2_COMPILE_OPTIONS, OPTIMIZATION))
   
    testcasefiles=glob.glob('%s/ts*.c'%(options.testcasesdir))
    testcasefiles= testcasefiles +glob.glob('%s/ts*.c.cmin'%(options.testcasesdir))
#    print testcasefiles
    testcasefiles.sort()
    myutils.exec_cmd('rm oracle_results.txt mutant_results.txt')
    files = []
    for tsfile in testcasefiles:
        files.append(tsfile.split("/")[-1])
        res = generateResultNew(tsfile, 'yaffs2_oracle')        
        myutils.appendline2file('oracle_results.txt', res)
        
        res = generateResultNew(tsfile, 'yaffs2_mutant')        
        myutils.appendline2file('mutant_results.txt', res)
   
    oracle_results=myutils.getFileLines('oracle_results.txt')
    mutant_results=myutils.getFileLines('mutant_results.txt')
    assert(len(oracle_results)==len(mutant_results))
   
    cmpstr= 'M:' + str(options.mutantid) + ','
    for i in range(0, len(oracle_results)):
        cmpstr += files[i]+':'
        if oracle_results[i] == mutant_results[i]:
            cmpstr += '0,'
        else:
            cmpstr += '1,'
    myutils.appendline2file(options.mukillfile, cmpstr)
#---------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    main()
