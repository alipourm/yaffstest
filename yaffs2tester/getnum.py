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
import locale

locale.setlocale(locale.LC_ALL, 'en_US')
rate = range(1,11)
times=[]
for i in rate:
    cmdhistory='50KSwarm%d00/cmdhistory.txt'%(i)
    lines = myutils.getFileLines(cmdhistory)
    gentime = 0
    for line in lines:
        if line.find('total test cases generated') >= 0:
            s = re.sub("\D", "", line)
            gentime = gentime + int(s)        
            times.append(gentime)

for time in times:
    print ('%d'%(time))




