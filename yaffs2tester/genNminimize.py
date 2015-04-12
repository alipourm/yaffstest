import sys
import os
import subprocess

DEST = "/nfs/guille/groce/users/alipour/yaffsresults/"

first_index_str = sys.argv[1]

first_index = int(first_index_str)

gen = 100


def cmd(cmdLine):
    subprocess.call([cmdLine + ">& /dev/null"], shell = True)

def cmd_show(cmdLine):
    subprocess.call([cmdLine], shell = True)




cmd1 = "python yaffs2tester.py --gentestcases --startidx " + str(first_index) + " --endidx " + str(first_index+gen)+ " --tslength 1000 --workingfolder testCases"
cmd_show(cmd1)
for i in range(first_index, first_index + gen + 1):
    tcName = 'ts' + str(i).zfill(6) + ".c"
    print "minimizing", tcName
    cminName = tcName + ".cmin"
    fminName = tcName + ".fmin"
    cmd2 = "python yaffs2DD.py testCases/testcases/001000/" + tcName + "  -l " +  cminName
    cmd(cmd2)
      
    # cmd3 = "python yaffs2DD.py testCases/testcases/001000/" + tcName + "  -f " +  fminName
    # md(cmd3)

    cmd("sleep 1")

    cmd4 = "cp " + "testCases/testcases/001000/" + tcName + " " + DEST
    cmd5 = "cp "+ cminName + " " + DEST
    #cmd6 = "cp "+ fminName + " " + DEST
    cmd_show(cmd4)
    cmd_show(cmd5)
    cmd7 = "echo " + tcName + ">> /nfs/stak/students/a/alipour/public_html/yaffsRep.txt"
    cmd_show(cmd7)
    

