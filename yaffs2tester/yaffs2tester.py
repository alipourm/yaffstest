#!/usr/bin/python

from operator import or_
from optparse import OptionParser
from time import localtime, strftime
import datetime
import getopt, sys
import glob
import itertools
import myutils
import os
import random
import re
import shutil
import signal
import string
import subprocess
import time
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


    # build yaffs oracle version for function return results.
    #myutils.exec_cmd('gcc -o yaffs2_oracle yaffs2/yaffs2.c testcaserunner.cpp -DNO_COVERAGE %s %s -lstdc++ -ldl'\
    #                        %(YAFFS2_COMPILE_OPTIONS, OPTIMIZATION))                            
    # gcov version for line and function coverage
    myutils.exec_cmd('gcc -o yaffs2_gcov yaffs2/yaffs2.c testcaserunner.cpp -DNO_COVERAGE %s %s -lstdc++ -ldl'\
                            %(YAFFS2_COMPILE_OPTIONS, '--coverage'))

#--------------------------------------------------------------------------------------------------------------------------------     
def generateTestCases(startidx, endidx, eachsize, tslength, dir, configfile, randseed, strategy, format):
    # build test case generator
    myutils.exec_cmd('gcc -o testcasegenerator %s/testcasegenerator.cpp yaffs2/yaffs2.c -DENABLE_GMP -lgmp -I %s/include %s -lstdc++ -ldl'\
                                %(G_ROOT_FOLDER, TCLAP_LIB_PATH, YAFFS2_COMPILE_OPTIONS))    
    if not os.path.exists('testcasegenerator'):      
      assert(0)
                                 
    global configs
    # only load configs file once
    if configfile != '' and configs == []:
        configs = myutils.getFileLines(configfile)
        for i in range(len(configs)):
            configs[i] = configs[i].replace('\n', '')
    if format == None:
        format = 'nonconcolic'
    os.chdir(G_TMP_FOLDER)
    myutils.ensuredir('%s/%06d'%(dir, tslength))
    os.chdir(G_TMP_FOLDER)
    for i in range(startidx, endidx+1):
        disabledapilist = ''
        if len(configs) > 0:
            disabledapilist = configs[i % (len(configs))]
        if strategy == None:
            myutils.exec_cmd('%s/testcasegenerator --randseed %d --startidx %d --endidx %d --eachsize %d --dir %s/%06d --tclength %d --format %s %s'\
                                %(G_ROOT_FOLDER,random.randint(0,20000000), i, i, eachsize, dir,  tslength, tslength, format, disabledapilist))
        else:
            myutils.exec_cmd('%s/testcasegenerator --randseed %d --startidx %d --endidx %d --eachsize %d --dir %s/%06d --tclength %d --strategy %s --format %s %s'\
                        %(G_ROOT_FOLDER,random.randint(0,sys.maxint), i, i, eachsize, dir,  tslength, tslength, strategy, format, disabledapilist))
    os.chdir(G_ROOT_FOLDER)
#---------------------------------------------------------------------------------------------------------------------------------
def generateCoverageInfo(startidx, endidx, eachsize, tslength, dir, suitesize):    
    os.chdir(G_TMP_FOLDER)
    if not os.path.exists( './blockcount' ):
        myutils.exec_cmd('cp -f %s/blockcount ./'%(G_ROOT_FOLDER) )
    if not os.path.exists( './branchcount' ):
        myutils.exec_cmd('cp -f %s/branchcount ./'%(G_ROOT_FOLDER) )
    if not os.path.exists( './cfginfofile' ):
        myutils.exec_cmd('cp -f %s/cfginfofile ./'%(G_ROOT_FOLDER) )
    if not os.path.exists( './blockpredcount' ):
        myutils.exec_cmd('cp -f %s/blockpredcount ./'%(G_ROOT_FOLDER) )
    if not os.path.exists( './stmtpredcount' ):
        myutils.exec_cmd('cp -f %s/stmtpredcount ./'%(G_ROOT_FOLDER) )

    #instypes = ['block','branch', 'path', 'acyclicpath','pred', 'predStmt']
    instypes = [ 'path', 'acyclicpath']
    myutils.ensuredir('%s/coverage/path'%(G_WORKING_FOLDER))
    myutils.ensuredir('%s/coverage/acyclicpath'%(G_WORKING_FOLDER))
    for instype in instypes:
        #starttime=time.time()
        for tsidx in range(startidx, endidx+1):
            tsfile = '%s/testcases/%06d/ts%06d.c'%(G_WORKING_FOLDER, tslength, tsidx)                
            #covdbname = '%s/coverage/%s.%06d.cov'%(G_WORKING_FOLDER, 'tpath', tsidx/suitesize)        
            if instype == 'path' or instype == 'acyclicpath':
                covdbname='%s/coverage/%s/path%06d.cov'%(G_WORKING_FOLDER,instype,tsidx)
                myutils.exec_cmd('%s/yaffs2_%s -start %d -num %d -c -instype %s -testcasefile %s -covdbname %s'\
                                %(G_ROOT_FOLDER, instype, 0, 1, 'path', tsfile, covdbname))
                if instype == 'acyclicpath':
                    myutils.exec_cmd('mv %s/acyclicpath.cov %s'%(G_TMP_FOLDER, covdbname))
            else:
                covdbname='%s/coverage/%s.cov'%(G_WORKING_FOLDER, instype)
                myutils.exec_cmd('%s/yaffs2_%s -start %d -num %d -c -instype %s -testcasefile %s -covdbname %s'\
                                %(G_ROOT_FOLDER, instype, 0, 1, instype, tsfile, covdbname))
        #endtime=time.time()

        #print ('time is %f'%(endtime-starttime) )     

    os.chdir(G_ROOT_FOLDER)
#--------------------------------------------------------------------------------------------------------------------------------- 
def generateResultNew(tsfile, yaffs2exe):
    os.chdir(G_TMP_FOLDER)
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
def generateOracleResults(startidx, endidx, eachsize, tslength):
    oracle_app = '%s/yaffs2_oracle'%(G_ROOT_FOLDER)
    for i in range(startidx, endidx+1):
        tsfile = '%s/testcases/%06d/ts%06d.c'%(G_WORKING_FOLDER, tslength, i)
        res = generateResultNew(tsfile, oracle_app)        
        myutils.appendline2file('%s/oracle_res/testresult#%06d.txt'%(G_WORKING_FOLDER, tslength), res)
        
#--------------------------------------------------------------------------------------------------------------------------------- 
def generateMuResults(startidx, endidx, eachsize, tslength, savetemp):
    myutils.ensuredir('%s/mu_res'%(G_WORKING_FOLDER))
    for i in range(startidx, endidx+1):
        tsfile = '%s/testcases/%06d/ts%06d.c'%(G_WORKING_FOLDER, tslength, i)
        mutants=glob.glob(MUTANTS_PATH%(G_ROOT_FOLDER))
        mutants.sort()
        ress = []
        for mutant in mutants:
            mu_app =os.path.dirname(mutant)+'/'+os.path.basename(mutant).split('_')[0]
            res = generateResult(tsfile, mu_app)
            ress.append( res )
            #print res 
        muresfile='%s/mu_res/testresult#%06d#ts%06d.txt'%(G_WORKING_FOLDER, tslength, i)
        myutils.appendlines2file(muresfile, ress)
        generateMuKillResults(i, eachsize, tslength)
        #if savetemp != True:
        #    myutils.exec_cmd('rm -rf %s'%(muresfile))
#---------------------------------------------------------------------------------------------------------------------------------
def generateMuKillResults(i, eachsize, tslength):
    myutils.ensuredir('%s/mu_kill'%(G_WORKING_FOLDER))
    oracle_res = '%s/oracle_res/testresult#%06d.txt'%(G_WORKING_FOLDER, tslength)
    oracle_file = open(oracle_res)
    oracle_lines = oracle_file.readlines()
    #cmpstrs = []
    #for i in range(startidx, endidx+1):
    mu_res='%s/mu_res/testresult#%06d#ts%06d.txt'%(G_WORKING_FOLDER, tslength, i)
    mu_file = open(mu_res)
    mu_lines = mu_file.readlines()
    cmpstr = []        
    for mu_line in mu_lines:
        if len(mu_line) > 100:
          if oracle_lines[i] == mu_line:
              cmpstr.append(0)
          else:
              cmpstr.append(1)
    #cmpstrs.append(cmpstr)
    mu_file.close()
    oracle_file.close()
    myutils.appendArrayNoNewline('%s/mu_kill/kill%06d.txt'%(G_WORKING_FOLDER,tslength), cmpstr)
    return cmpstr    
      
#--------------------------------------------------------------------------------------------------------------------------------
def main():
    global G_ROOT_FOLDER
    G_ROOT_FOLDER = os.getcwd()    
   
    now = datetime.datetime.now()
    randseed = now.microsecond
    parser = OptionParser()
    parser.add_option("", "--precompile",    dest="precompile",   action="store_true", default=False)
    parser.add_option("", "--ignore",        dest="ignore",       action="store_true", default=False)
    parser.add_option("", "--gentestcases",  dest="gentestcases", action="store_true", default=False)
    parser.add_option("", "--strategy",      dest="strategy",     action="store",      type='string')
    parser.add_option("", "--format",        dest="format",       action="store",      type='string')
    parser.add_option("", "--tslength",      dest="tslength",     action="store",      type = "int")
    parser.add_option("", "--configfile",    dest="configfile",   action="store",      type='string')
      
    parser.add_option("", "--startidx",      dest="startidx",     action="store",      type = "int") 
    parser.add_option("", "--endidx",        dest="endidx",       action="store",      type = "int")
    parser.add_option("", "--suitesize",     dest="suitesize",    action="store",      type = "int")    
     
    parser.add_option("", "--coverage",      dest="coverage",     action="store_true", default=False)
    parser.add_option("", "--tracedb",       dest="tracedb",      action="store",      type = "string")
    parser.add_option("", "--mutantskill",   dest="mutantskill",  action="store_true", default=False)
    parser.add_option("", "--savetemp",      dest="savetemp",     action="store_true", default=False)
    
    parser.add_option("", "--timelimit",     dest="timelimit",    action="store",      type = "int")
   
    parser.add_option("", "--workingfolder", dest="workingfolder",action="store",      type = "string")
    parser.add_option("", "--randseed",      dest="randseed",     action="store",      type = "int")
        
    (options, args) = parser.parse_args()
    
    # precompile/instrument some common modules.
    if options.precompile == True:
        preCompile()
        return
           
    if options.randseed != None:
        randseed = options.randseed
    
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
        
    # log current command
    myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), strftime("%Y-%m-%d %H:%M:%S", localtime()))   
    myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), string.join(sys.argv[0:], " "))    
    
    configfile = ''
    if options.configfile != None:
        configfile = options.configfile   
    
    singlesize = 1
    if options.startidx != None and options.endidx != None:
        startidx = options.startidx
        endidx = options.endidx

        timer = myutils.Timer()
        if options.gentestcases == True:
            myutils.ensuredir('%s/testcases'%(G_WORKING_FOLDER))
            timer.start()             
            generateTestCases(startidx, endidx, singlesize, tslength, '%s/testcases'%(G_WORKING_FOLDER), \
                        configfile, randseed, options.strategy, options.format)
            timer.end()
            myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), 'Generating test cases took %d seconds.'%(timer.elapsed()))
        if options.coverage == True:
            myutils.ensuredir('%s/coverage'%(G_WORKING_FOLDER)) 
            timer.start()            
            generateCoverageInfo(startidx, endidx, singlesize, tslength, '%s/coverage'%(G_WORKING_FOLDER), options.suitesize )
            timer.end()
            myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), 'Generating coverage information took %d seconds.'%(timer.elapsed()))
            
        if options.mutantskill == True:
            timer.start()
            myutils.ensuredir( '%s/oracle_res'%(G_WORKING_FOLDER) )
            generateOracleResults(startidx, endidx, singlesize, tslength)
            timer.end()
            myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), 'Generating oracle results took %d seconds.'%(timer.elapsed()))
            timer.start() 
            generateMuResults(startidx, endidx, options.suitesize, tslength, options.savetemp)
            timer.end()
            myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), 'Generating mutants results took %d seconds.'%(timer.elapsed()))           

        
    myutils.rmfiles( [G_TMP_FOLDER] )
    
    myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), strftime("%Y-%m-%d %H:%M:%S", localtime()))
    numofdash = len(string.join(sys.argv[0:], " "))
    strdash = '-'*numofdash
    myutils.appendline2file("%s/cmdhistory.txt"%(G_WORKING_FOLDER), strdash)

#---------------------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    main()
