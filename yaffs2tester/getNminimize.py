import sys
import os
import subprocess

DEST = "/nfs/guille/groce/users/alipour/yaffsresults/"
ORIGIN = "/nfs/guille/groce/users/alipour/yaffstests/"
tcName = sys.argv[1]

tc = int(tcName) - 1

def cmd(cmdLine):
    subprocess.call([cmdLine + ">& /dev/null"], shell = True)

def cmd_show(cmdLine):
    subprocess.call([cmdLine], shell = True)


tcName = 'ts' + str(tc).zfill(6) + ".c"
print "minimizing", tcName
cminName = tcName + ".cmin"
cmd2 = "python yaffs2DD.py " + ORIGIN + "testcases/001000/" + tcName + "  -l " +  DEST+cminName
cmd(cmd2)
