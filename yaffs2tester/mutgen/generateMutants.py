import random
import re
import sys

sourcefile = sys.argv[1]
descfile = sys.argv[2]

mutants =  open(descfile).readlines()
msample = random.sample(mutants, 20*len(mutants)/100)  
src = [''] + open(sourcefile).readlines()
print src[14759]
#print len(msample)


for i, l in enumerate(mutants):
    lparts = re.findall('(\d+):(.*)', l)[0]
    lineno = int(lparts[0])
    mutline = lparts[1]
    mutfilename = 'mutant'+ str(i).zfill(6) + '_' + lparts[0] + sourcefile
    mutfile = open(mutfilename, 'w')
    print mutfile

    for j, srcline in enumerate(src):
        if j == lineno:
            mutfile.write(mutline)
        else:
            mutfile.write(srcline)
    

