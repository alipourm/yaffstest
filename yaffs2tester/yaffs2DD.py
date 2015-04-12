
# Using delta debugging on yaffs2 input
# Usage:
# python yaffs2DD.py {orig test case file} {-f|-l for function based or line based} {output minimal test case file name}
import DD
import commands
import string
import subprocess
import myutils
import os
import sys

testcaseinput=sys.argv[1]
covtype=sys.argv[2] # -f or -l
outputminimal=sys.argv[3]

newinput='input.c'
cmdline='yaffs2_gcov -start 0 -num 1 -testcasefile %s'%(testcaseinput)
origcov=''
#--------------------------------------------------------
def getFunctionCoverage(filename):
    s=''
    f=open(filename)
    lines=f.readlines()
    f.close()
    currfunc=''
    for line in lines:
        if line.find('Function') == 0:
            currfunc=line.split(' ')[1].strip('\n')
        elif currfunc != '':
            if line.find('Lines executed:') == 0:
                percentage=float(line.split(':')[1].split('%')[0])
                if percentage > 0.00:
                    s+=currfunc
                    s+=','
                currfunc=''
    return s

#--------------------------------------------------------------

def getBranchCoverage(filename):
    s=''
    f=open(filename)
    lines=f.readlines()
    f.close()
    currfunc=''
    for line in lines:
        if line.find('Function') == 0:
            currfunc=line.split(' ')[1].strip('\n')
        elif currfunc != '':
            if line.find('Lines executed:') == 0:
                percentage=float(line.split(':')[1].split('%')[0])
                if percentage > 0.00:
                    s+=currfunc
                    s+=','
                currfunc=''
    return s


#---------------------------------------
def getLineCoverageNew(filename):
    s=""
    f = open(filename)
    lines = f.readlines()
    f.close()
    for l in lines:
        l = l.strip()
        ls = l.split(':');
#        print l
        if (ls[0] != "-") and (ls[0] != "#####"):
#            print ls
           # if (":" in ls[1]):
            s += (ls[1].split(":"))[0]
            s += ","
#    print s

    return s



#--------------------------------------------------------
def getLineCoverage(filename):
    return getLineCoverageNew(filename)
if False:
    s=''
    f = open(filename)
    lines = f.readlines()
    f.close()
    for line in lines:
        newline=line.strip()
        if len(newline) > 0:
            if newline[0] != '-' and newline[0] != '#':
                s+=newline.split(':')[1].strip()
                s+=','
        #parts=line.split(':')
        #p0 = parts[0]
        #p0=p0.strip()
        #if p0 != '-' and p0 != '#####':
        #    s += parts[1].strip()
        #    s += ','
    #print("------------------\n");
    #print(s);
#    return s
#--------------------------------------------------------



#--------------------------------------------------------

class MyDD(DD.DD):
    def __init__(self):
        DD.DD.__init__(self)
        self.iteration = 0

    def _test(self, deltas):
        # Build input
        input = ""
        for (index, character) in deltas:
            input = input + character

        # Write input to `input.c'
        out = open(newinput, 'w')
        out.write(input)
        out.close()

        #print self.coerce(deltas)

        if deltas == []:
            return self.PASS

        # Invoke yaffs2
        cmd='rm *.gcov *.gcda funccoverage'
        myutils.exec_cmd(cmd)
        cmd='yaffs2_gcov -start 0 -num 1 -testcasefile %s'%(newinput)
        myutils.exec_cmd(cmd)
        if os.path.exists('yaffs2.gcda'):
            myutils.exec_cmd('gcov -f yaffs2.c > funccoverage')
            if os.path.exists('yaffs2.c.gcov'):
                if covtype == '-l':
                    newcov=getLineCoverage('yaffs2.c.gcov')
                else:
                    newcov=getFunctionCoverage('funccoverage') 
                if newcov == origcov:
                    myutils.exec_cmd('cp {0} {1}'.format(newinput,  testcaseinput + '.' + str(self.iteration).zfill(5) + '.cmin'))
                    self.iteration += 1
                    return self.FAIL
                else:
                    return self.PASS
            else:
                return self.UNRESOLVED
        else:
            self.UNRESOLVED

    def coerce(self, deltas):
        # Pretty-print the configuration
        input = ""
        for (index, character) in deltas:
            input = input + character
        return input

#--------------------------------------------------------
if __name__ == '__main__':
    # Load deltas from `bug.c'
    deltas = []
    index = 1

    for line in open(testcaseinput):
        #print ("Line = " + line)
        deltas.append((index, line))
        index = index + 1

    myutils.exec_cmd('python yaffs2tester.py --precompile')
    cmd='rm *.gcov *.gcda funccoverage'
    myutils.exec_cmd(cmd)
    myutils.exec_cmd('yaffs2_gcov -start 0 -num 1 -testcasefile %s'%(testcaseinput))
    myutils.exec_cmd('gcov -f yaffs2.c > funccoverage')
    if covtype == '-l':
        origcov=getLineCoverage('yaffs2.c.gcov')
    elif covtype == '-f':
        origcov=getFunctionCoverage('funccoverage')
    mydd = MyDD()
    
    print "Simplifying failure-inducing input..."
    c = mydd.ddmin(deltas)              # Invoke DDMIN
    #print "The 1-minimal failure-inducing input is", mydd.coerce(c)
    print "Removing any element will make the failure go away."

    # Write input to `input.c'
    out = open(outputminimal, 'w')
    out.write(mydd.coerce(c))
    out.close()

    # print
    
    # print "Isolating the failure-inducing difference..."
    # (c, c1, c2) = mydd.dd(deltas)    # Invoke DD
    # print "The 1-minimal failure-inducing difference is", c
    # print mydd.coerce(c1), "passes,", mydd.coerce(c2), "fails"




# Local Variables:
# mode: python
# End:
