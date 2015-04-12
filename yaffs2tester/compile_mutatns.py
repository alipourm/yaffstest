import myutils
import glob
import sys

muts = glob.glob(sys.argv[1])
OPTIMIZATION = '-O4'
YAFFS2_COMPILE_OPTIONS = '-DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_YAFFS2 -DCONFIG_YAFFS_PROVIDE_DEFS -DCONFIG_YAFFSFS_PROVIDE_VALUES -I yaffs2/inc -I yaffs2'

i = 0
for m in muts:
    print 'generating:', m
    mutname = m.replace('.c', '')
    print mutname
    status, output =  myutils.exec_cmd('gcc -o {0} {1} testcaserunner.cpp -DNO_COVERAGE  {2} -lstdc++ -ldl'.format(mutname, m, YAFFS2_COMPILE_OPTIONS, OPTIMIZATION))
    if 'error' in output:
        i +=1
    print i
