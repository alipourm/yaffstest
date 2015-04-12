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
#---------------------------------------------------------------------------------------------------------------------------------
# Global variables
OPTIMIZATION='-O2'
YAFFS2_COMPILE_OPTIONS = '-DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_YAFFS2 -DCONFIG_YAFFS_PROVIDE_DEFS \
                            -DCONFIG_YAFFSFS_PROVIDE_VALUES \
                            -I yaffs2/inc -I yaffs2 -O2'
TIMEOUT_LIMIT=2 # in seconds
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


#mutants=glob.glob('./yaffs2/mutants/500_b*/*_yaffs2.c')
#mutants.sort()
#for mutant in mutants:
#    lines = myutils.getFileLines(mutant)
#    for i in range(0, len(lines)):
#        if lines[i].find('struct yaffs_dev m18_1Dev;') >= 0:
#            lines[i] = 'struct yaffs_dev m18_1Dev;\r\n'
#        if lines[i].find('yaffs_add_device(&ram1Dev);') >= 0:
#            lines[i] = '\tyaffs_add_device(&ram1Dev);\r\n#endif\r\n'
#        if lines[i].find('yaffs_add_device(&m18_1Dev);') >= 0:
#            lines[i+1] = '\r\n'
#    
#    myutils.appendlines2filenonewline('%s.fixed'%(mutant),lines)

#mutants=glob.glob('./yaffs2/mutants/500_b*/*_yaffs2.c')
#mutants.sort()
#for mutant in mutants:
#    myutils.exec_cmd('diff %s %s.fixed >> dr.txt'%(mutant, mutant))
mutants=glob.glob('./yaffs2/mutants/500_b*/*_yaffs2.c.fixed')
mutants.sort()
for mutant in mutants:
    myutils.exec_cmd('mv %s %s'%(mutant, mutant.replace('.fixed', '')))