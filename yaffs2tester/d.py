def getLineCoverage(filename):
    s=''
    f = open(filename)
    lines = f.readlines()
    f.close()
    for line in lines:
        parts=line.split(':')
        p0 = parts[0]
        p0=p0.strip()
        if p0 != '-' and p0 != '#####':
            s += parts[1].strip()
            s += ','
    #print(s);
    return s

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



#print(getLineCoverage('yaffs2.c.gcov'))
print(getFunctionCoverage('xx'))
