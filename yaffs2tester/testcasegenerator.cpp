// testcasegenerator.cpp : test case generator for yaffs2.

// $Rev: 1639 $:     Revision of last commit
// $Author: zhangch $:  Author of last commit
// $Date: 2012-05-18 22:35:22 -0700 (Fri, 18 May 2012) $:    Date of last commit

//
#include <tclap/CmdLine.h>

#include "yaffstester.h"
#include <algorithm>
#include <functional>
#include <numeric>
#include <map>
#include <utility>
#include <time.h>
#include <stdlib.h>     /* srand, rand */
extern "C" {
#include "yaffsfs.h"
}

int random_seed;
int simulate_power_failure;
extern int yaffs_trace_mask;


//#define ENABLE_GMP
//#define __DIFF_TESTING__

#if defined(ENABLE_GMP)
#include <gmp.h>
#endif

//using namespace std;

#pragma warning( disable : 4996)


#define FILE_SIZE 1024*1024
char rwbuf[FILE_SIZE];



void testcase2concolicsrc(const yaffs_test_case& testcase, std::string& src);
/////////////////////////////////////////////////////
//bool apiinfo[MAX_API];

struct apiinfoentry {
    const char*  name;
    const char*  desc;
    bool   benable;
    double weight;
};
//--------------------------------------------------------------------------------------------------------
struct apiinfoentry apiinfo[] = {
    {"no-startup",     "disable start up commnad",    true, 1.0/MAX_API },
    {"no-mount",       "disable mount commnad",       true, 1.0/MAX_API },
    {"no-mount2",      "disable mount2 commnad",      true, 1.0/MAX_API },
    {"no-unmount",     "disable unmount commnad",     true, 1.0/MAX_API },
    {"no-unmount2",    "disable unmount2 commnad",    true, 1.0/MAX_API },
    {"no-remount",     "disable remount commnad",     true, 1.0/MAX_API },
    {"no-open",        "disable open command",        true, 1.0/MAX_API }, 
    {"no-close",       "disable close command",       true, 1.0/MAX_API }, 
    {"no-mkdir",       "disable mkdir command",       true, 1.0/MAX_API }, 
    {"no-rmdir",       "disable rmdir command",       true, 1.0/MAX_API }, 
    {"no-lseek",       "disable lseek command",       true, 1.0/MAX_API }, 
    {"no-truncate",    "disable truncate command",    true, 1.0/MAX_API }, 
    {"no-ftruncate",   "disable ftruncate command",   true, 1.0/MAX_API }, 
    {"no-stat",        "disable stat command",        true, 1.0/MAX_API }, 
    {"no-fstat",       "disable fstat command",       true, 1.0/MAX_API },  
    {"no-lstat",       "disable lstat command",       true, 1.0/MAX_API }, 
    {"no-read",        "disable read command",        true, 1.0/MAX_API }, 
    {"no-write",       "disable write command",       true, 1.0/MAX_API }, 
    {"no-freespace",   "disable freespace command",   true, 1.0/MAX_API }, 
    {"no-opendir",     "disable opendir command",     true, 1.0/MAX_API }, 
    {"no-readdir",     "disable readdir command",     true, 1.0/MAX_API }, 
    {"no-rewinddir",   "disable rewind command",      true, 1.0/MAX_API }, 
    {"no-closedir",    "disable closedir command",    true, 1.0/MAX_API }, 
    {"no-link",        "disable link command",        true, 1.0/MAX_API }, 
    {"no-symlink",     "disable syslink command",     true, 1.0/MAX_API },  
    {"no-readlink",    "disable readlink command",    true, 1.0/MAX_API }, 
    {"no-unlink",      "disable unlink command",      true, 1.0/MAX_API }, 
    {"no-rename",      "disable rename command",      true, 1.0/MAX_API }, 
    {"no-chmod",       "disable chmod command",       true, 1.0/MAX_API }, 
    {"no-fchmod",      "disable fchmod command",      true, 1.0/MAX_API },
    {"no-fsync",       "disable fsync command",       true, 1.0/MAX_API },       
    {"no-fdatasync",   "disable fdatasync command",   true, 1.0/MAX_API },   
    {"no-access",      "disable access command",      true, 1.0/MAX_API },      
    {"no-dup",         "disable dup command",         true, 1.0/MAX_API },         
    {"no-pread",       "disable pread command",       true, 1.0/MAX_API },       
    {"no-pwrite",      "disable pwrite command",      true, 1.0/MAX_API },      
    {"no-utime",       "disable utime command",       true, 1.0/MAX_API },       
    {"no-futime",      "disable futime command",      true, 1.0/MAX_API },
    {"no-flush",       "disable flush command",       true, 1.0/MAX_API },         
    {"no-sync",        "disable sync command",        true, 1.0/MAX_API },       
    {"no-totalspace",  "disable totalspace command",  true, 1.0/MAX_API },      
    {"no-inodecount",  "disable inodecount command",  true, 1.0/MAX_API },       
    {"no-nhandles",    "disable nhandles command",    true, 1.0/MAX_API },
#ifndef __LESS_API    
    {"no-setxattr",    "disable setxattr command",    true, 1.0/MAX_API },    
    {"no-lsetxattr",   "disable lsetxattr command",   true, 1.0/MAX_API },   
    {"no-fsetxattr",   "disable fsetxattr command",   true, 1.0/MAX_API },   
    {"no-getxattr",    "disable getxattr command",    true, 1.0/MAX_API },    
    {"no-lgetxattr",   "disable lgetxattr command",   true, 1.0/MAX_API },   
    {"no-fgetxattr",   "disable fgetxattr command",   true, 1.0/MAX_API },   
    {"no-removexattr", "disable removexattr command", true, 1.0/MAX_API }, 
    {"no-lremovexattr","disable lremovexatt command", true, 1.0/MAX_API }, 
    {"no-fremovexattr","disable fremovexatt command", true, 1.0/MAX_API }, 
    {"no-listxattr",   "disable listxattr command",   true, 1.0/MAX_API },   
    {"no-llistxattr",  "disable llistxattr command",  true, 1.0/MAX_API },  
    {"no-flistxattr",  "disable flistxattr command",  true, 1.0/MAX_API },
#endif
};
//--------------------------------------------------------------------------------------------------------
#if defined(ENABLE_GMP)
gmp_randstate_t state;
unsigned long rand_int(unsigned long n)
{
    mpz_t m;
    mpz_init(m);
    mpz_set_ui(m, n);

    mpz_t randomnum;
    mpz_init(randomnum);

    mpz_urandomm(randomnum, state, m);

    unsigned long res = mpz_get_ui( randomnum );

    //printf("random number is: %d\n", res);
    mpz_clear( randomnum);
    mpz_clear(m);

    return res;
}
#else
unsigned long rand_int(unsigned long n)
{
    return rand()%n;
}
#endif
//--------------------------------------------------------------------------------------------------------
// generate a string with max length 8
std::string rand_string() {
    char str[256];
    unsigned long len = rand_int(10)+1;
    unsigned long i = 0;
    for(i = 0; i < len; i++) {
        unsigned long index = rand_int(26);
        str[i] = 'a'+(unsigned char)index;
    }
    str[i] = 0;
    return std::string(str);
}
struct _emulfs
{
    std::vector<std::string> avail_dirs;
    std::vector<std::string> avail_files;
    std::vector<int> avail_fd_indices;
    std::vector<int> avail_fds;
    std::vector<int> avail_dd_indices;
    std::vector<yaffs_DIR*> avail_dds;
    std::vector<std::string> avail_lndirs;
    std::vector<std::string> avail_lnfiles;
    std::vector<int> avail_lnfd_indices;
    std::vector<int> avail_lndd_indices;    
    int nextfd_index;
    int nextdd_index;

    void init() {
        avail_dirs.clear();
        avail_files.clear();
        avail_fd_indices.clear();
        avail_fds.clear();
        avail_dds.clear();
        avail_fds.resize( 10000 );
        avail_dd_indices.clear();
        avail_dds.resize( 10000 );
        avail_lndirs.clear();
        avail_lnfiles.clear();
        avail_lnfd_indices.clear();
        avail_lndd_indices.clear();
        avail_dirs.push_back( "/yaffs2" ); 
        avail_dirs.push_back( "/M18-1" ); 
        avail_dirs.push_back( "/ram1" ); 

        nextfd_index = 0;
        nextdd_index = 0;
    }
};

_emulfs emulfs;
//--------------------------------------------------------------------------------------------------------
std::vector<int> avail_apis() {
    std::vector<int> apis;
    for(int i = OPEN; i < MAX_API; i++) {
        if( apiinfo[i].benable )
            apis.push_back(i);
    }
#if 0
    if( emulfs.avail_dirs.size() > 1) {
        if(apiinfo[RMDIR].benable)         { apis.push_back(RMDIR);        }
    }
    if( emulfs.avail_dirs.size() > 0) {
        if(apiinfo[OPEN].benable)          { apis.push_back(OPEN);         }
        if(apiinfo[MKDIR].benable)         { apis.push_back(MKDIR);        }
        if(apiinfo[STAT].benable)          { apis.push_back(STAT);         }
        if(apiinfo[OPENDIR].benable)       { apis.push_back(OPENDIR);      }
        if(apiinfo[LINK].benable)          { apis.push_back(LINK);         }
        if(apiinfo[SYMLINK].benable)       { apis.push_back(SYMLINK);      }
        if(apiinfo[RENAME].benable)        { apis.push_back(RENAME);       }
        if(apiinfo[CHMOD].benable)         { apis.push_back(CHMOD);        }
        if(apiinfo[ACCESS].benable)        { apis.push_back(ACCESS);       }
        if(apiinfo[UTIME].benable)         { apis.push_back(UTIME);        }
        if(apiinfo[FREESPACE].benable)     { apis.push_back(FREESPACE);    }
        if(apiinfo[TOTALSPACE].benable)     { apis.push_back(TOTALSPACE);  }
        if(apiinfo[FREESPACE].benable)     { apis.push_back(FREESPACE);    }
        if(apiinfo[FREESPACE].benable)     { apis.push_back(FREESPACE);    }
#ifndef __LESS_API
        if(apiinfo[SETXATTR].benable)      { apis.push_back(SETXATTR);     }
        if(apiinfo[LSETXATTR].benable)     { apis.push_back(LSETXATTR);    }
        if(apiinfo[GETXATTR].benable)      { apis.push_back(GETXATTR);     }
        if(apiinfo[LGETXATTR].benable)     { apis.push_back(LGETXATTR);    }
        if(apiinfo[REMOVEXATTR].benable)   { apis.push_back(REMOVEXATTR);  }
        if(apiinfo[LREMOVEXATTR].benable)  { apis.push_back(LREMOVEXATTR); }
        if(apiinfo[LISTXATTR].benable)     { apis.push_back(LISTXATTR);    }
        if(apiinfo[LLISTXATTR].benable)    { apis.push_back(LLISTXATTR);   }
#endif
    }
    if( emulfs.avail_files.size() > 0) {
        if(apiinfo[OPEN].benable)          { apis.push_back(OPEN);         }
        if(apiinfo[STAT].benable)          { apis.push_back(STAT);         }
        if(apiinfo[FREESPACE].benable)     { apis.push_back(FREESPACE);    }
        if(apiinfo[LINK].benable)          { apis.push_back(LINK);         }
        if(apiinfo[SYMLINK].benable)       { apis.push_back(SYMLINK);      }
        if(apiinfo[RENAME].benable)        { apis.push_back(RENAME);       }
        if(apiinfo[CHMOD].benable)         { apis.push_back(CHMOD);        }
        if(apiinfo[TRUNCATE].benable)      { apis.push_back(TRUNCATE);     }
        if(apiinfo[ACCESS].benable)        { apis.push_back(ACCESS);       }
        if(apiinfo[UTIME].benable)         { apis.push_back(UTIME);        }
#ifndef __LESS_API
        if(apiinfo[SETXATTR].benable)      { apis.push_back(SETXATTR);     }
        if(apiinfo[LSETXATTR].benable)     { apis.push_back(LSETXATTR);    }
        if(apiinfo[GETXATTR].benable)      { apis.push_back(GETXATTR);     }
        if(apiinfo[LGETXATTR].benable)     { apis.push_back(LGETXATTR);    }
        if(apiinfo[REMOVEXATTR].benable)   { apis.push_back(REMOVEXATTR);  }
        if(apiinfo[LREMOVEXATTR].benable)  { apis.push_back(LREMOVEXATTR); }
        if(apiinfo[LISTXATTR].benable)     { apis.push_back(LISTXATTR);    }
        if(apiinfo[LLISTXATTR].benable)    { apis.push_back(LLISTXATTR);   }
#endif
    }
    if( emulfs.avail_fd_indices.size() > 0) {
        if(apiinfo[CLOSE].benable)         { apis.push_back(CLOSE);        }
        if(apiinfo[LSEEK].benable)         { apis.push_back(LSEEK);        }
        if(apiinfo[FTRUNCATE].benable)     { apis.push_back(FTRUNCATE);    }
        if(apiinfo[FSTAT].benable)         { apis.push_back(FSTAT);        }
        if(apiinfo[READ].benable)          { apis.push_back(READ);         }
        if(apiinfo[WRITE].benable)         { apis.push_back(WRITE);        }
        if(apiinfo[FCHMOD].benable)        { apis.push_back(FCHMOD);       }
        if(apiinfo[FSYNC].benable)         { apis.push_back(FSYNC);        }
        if(apiinfo[FDATASYNC].benable)     { apis.push_back(FDATASYNC);    }
        if(apiinfo[DUP].benable)           { apis.push_back(DUP);          }
        if(apiinfo[PREAD].benable)         { apis.push_back(PREAD);        }
        if(apiinfo[PWRITE].benable)        { apis.push_back(PWRITE);       }
        if(apiinfo[FUTIME].benable)        { apis.push_back(FUTIME);       }
#ifndef __LESS_API        
        if(apiinfo[FSETXATTR].benable)     { apis.push_back(FSETXATTR);    }       
        if(apiinfo[FGETXATTR].benable)     { apis.push_back(FGETXATTR);    }       
        if(apiinfo[FREMOVEXATTR].benable)  { apis.push_back(FREMOVEXATTR); }
        if(apiinfo[FLISTXATTR].benable)    { apis.push_back(FLISTXATTR);   }
#endif
    }
    if( emulfs.avail_dd_indices.size() > 0) {
        if(apiinfo[READDIR].benable)       { apis.push_back(READDIR);      }
        if(apiinfo[REWINDDIR].benable)     { apis.push_back(REWINDDIR);    }
        if(apiinfo[CLOSEDIR].benable)      { apis.push_back(CLOSEDIR);     }
        if(apiinfo[FCHMOD].benable)        { apis.push_back(FCHMOD);       }
        if(apiinfo[FUTIME].benable)        { apis.push_back(FUTIME);       }
#ifndef __LESS_API
#endif
        // fremovexattr can work on this?
    }
    if( emulfs.avail_lndirs.size() > 0) {
        if(apiinfo[OPEN].benable)          { apis.push_back(OPEN);         }
        if(apiinfo[MKDIR].benable)         { apis.push_back(MKDIR);        }
        if(apiinfo[RMDIR].benable)         { apis.push_back(RMDIR);        }
        if(apiinfo[STAT].benable)          { apis.push_back(STAT);         }
        if(apiinfo[LSTAT].benable)         { apis.push_back(LSTAT);        }
        if(apiinfo[FREESPACE].benable)     { apis.push_back(FREESPACE);    }
        if(apiinfo[READLINK].benable)      { apis.push_back(READLINK);     }
        if(apiinfo[UNLINK].benable)        { apis.push_back(UNLINK);       }
        if(apiinfo[SYMLINK].benable)       { apis.push_back(SYMLINK);      }
        if(apiinfo[RENAME].benable)        { apis.push_back(RENAME);       }
        if(apiinfo[CHMOD].benable)         { apis.push_back(CHMOD);        }
        if(apiinfo[ACCESS].benable)        { apis.push_back(ACCESS);       }
        if(apiinfo[UTIME].benable)         { apis.push_back(UTIME);        }
#ifndef __LESS_API
        if(apiinfo[SETXATTR].benable)      { apis.push_back(SETXATTR);     }
        if(apiinfo[LSETXATTR].benable)     { apis.push_back(LSETXATTR);    }
        if(apiinfo[GETXATTR].benable)      { apis.push_back(GETXATTR);     }
        if(apiinfo[LGETXATTR].benable)     { apis.push_back(LGETXATTR);    }
        if(apiinfo[REMOVEXATTR].benable)   { apis.push_back(REMOVEXATTR);  }
        if(apiinfo[LREMOVEXATTR].benable)  { apis.push_back(LREMOVEXATTR); }
        if(apiinfo[LISTXATTR].benable)     { apis.push_back(LISTXATTR);    }
        if(apiinfo[LLISTXATTR].benable)    { apis.push_back(LLISTXATTR);   }
#endif
    }
    if( emulfs.avail_lnfiles.size() > 0) {
        if(apiinfo[OPEN].benable)          { apis.push_back(OPEN);         }
        if(apiinfo[STAT].benable)          { apis.push_back(STAT);         }
        if(apiinfo[LSTAT].benable)         { apis.push_back(LSTAT);        }
        if(apiinfo[FREESPACE].benable)     { apis.push_back(FREESPACE);    }
        if(apiinfo[READLINK].benable)      { apis.push_back(READLINK);     }
        if(apiinfo[UNLINK].benable)        { apis.push_back(UNLINK);       }
        if(apiinfo[CHMOD].benable)         { apis.push_back(CHMOD);        }
        if(apiinfo[TRUNCATE].benable)      { apis.push_back(TRUNCATE);     }
        if(apiinfo[ACCESS].benable)        { apis.push_back(ACCESS);       }
        if(apiinfo[UTIME].benable)         { apis.push_back(UTIME);        }
#ifndef __LESS_API
        if(apiinfo[SETXATTR].benable)      { apis.push_back(SETXATTR);     }
        if(apiinfo[LSETXATTR].benable)     { apis.push_back(LSETXATTR);    }
        if(apiinfo[GETXATTR].benable)      { apis.push_back(GETXATTR);     }
        if(apiinfo[LGETXATTR].benable)     { apis.push_back(LGETXATTR);    }
        if(apiinfo[REMOVEXATTR].benable)   { apis.push_back(REMOVEXATTR);  }
        if(apiinfo[LREMOVEXATTR].benable)  { apis.push_back(LREMOVEXATTR); }
        if(apiinfo[LISTXATTR].benable)     { apis.push_back(LISTXATTR);    }
        if(apiinfo[LLISTXATTR].benable)    { apis.push_back(LLISTXATTR);   }
#endif
    }
    if( emulfs.avail_lnfd_indices.size() > 0) {
        if(apiinfo[CLOSE].benable)         { apis.push_back(CLOSE);        }
        if(apiinfo[LSEEK].benable)         { apis.push_back(LSEEK);        }
        if(apiinfo[FTRUNCATE].benable)     { apis.push_back(FTRUNCATE);    }
        if(apiinfo[FSTAT].benable)         { apis.push_back(FSTAT);        }
        if(apiinfo[READ].benable)          { apis.push_back(READ);         }
        if(apiinfo[WRITE].benable)         { apis.push_back(WRITE);        }
        if(apiinfo[FCHMOD].benable)        { apis.push_back(FCHMOD);       }
        if(apiinfo[FSYNC].benable)         { apis.push_back(FSYNC);        }
        if(apiinfo[FDATASYNC].benable)     { apis.push_back(FDATASYNC);    }
        if(apiinfo[DUP].benable)           { apis.push_back(DUP);          }
        if(apiinfo[PREAD].benable)         { apis.push_back(PREAD);        }
        if(apiinfo[PWRITE].benable)        { apis.push_back(PWRITE);       }
        if(apiinfo[FUTIME].benable)        { apis.push_back(FUTIME);       }
#ifndef __LESS_API
        if(apiinfo[FSETXATTR].benable)     { apis.push_back(FSETXATTR);    }       
        if(apiinfo[FGETXATTR].benable)     { apis.push_back(FGETXATTR);    }       
        if(apiinfo[FREMOVEXATTR].benable)  { apis.push_back(FREMOVEXATTR); }
        if(apiinfo[FLISTXATTR].benable)    { apis.push_back(FLISTXATTR);   }
#endif
    }
    if( emulfs.avail_lndd_indices.size() > 0) {
        if(apiinfo[READDIR].benable)       { apis.push_back(READDIR);      }
        if(apiinfo[REWINDDIR].benable)     { apis.push_back(REWINDDIR);    }
        if(apiinfo[CLOSEDIR].benable)      { apis.push_back(CLOSEDIR);     }
        if(apiinfo[FCHMOD].benable)        { apis.push_back(FCHMOD);       }

        if(apiinfo[FUTIME].benable)        { apis.push_back(FUTIME);       }
#ifndef __LESS_API
#endif
    }
#endif
    std::sort_heap(apis.begin(), apis.end());
    apis.erase(std::unique(apis.begin(), apis.end()), apis.end());
    return apis;
}
//--------------------------------------------------------------------------------------------------------
std::string pick_new_path(std::vector<std::string> dirs, std::vector<std::string> lndirs) {
    std::string newpath;

    int s1 = dirs.size();
    int s2 = lndirs.size();

    int choice = rand_int(s1+s2);
    std::string parent = "";
    if( choice < s1 ) {
        if( dirs[choice] != "??" ) {
            parent  = dirs[choice];
        }
    }
    else {
        if( lndirs[choice-s1] != "??" ) {
            parent  = lndirs[choice-s1];
        }
    }

    newpath = parent+"/";
    newpath += rand_string();

    return newpath;
}
//--------------------------------------------------------------------------------------------------------
int pick_file_mode() {
    int res = 0;

    int val = rand_int(3);
    if( val == 0 ) {
        res = S_IREAD;
    }
    else if( val == 1) {
        res = S_IWRITE;
    }
    else if( val == 2) {
        res = S_IREAD|S_IWRITE;
    }
    return res;
}

int open_flags[] = {
    O_RDONLY,
    O_WRONLY,
    O_RDWR,
    O_EXCL,
    O_TRUNC,
    O_APPEND
};
//--------------------------------------------------------------------------------------------------------
int pick_open_flags() {
    int res = 0;
    int num = sizeof(open_flags)/sizeof(int);
    int howmany = rand_int(num);
    if (howmany == 0)
        howmany = 1;
    for(int i = 0; i < howmany; i++) {
        int choice = rand_int(num);        
        res |= open_flags[choice];      
    }
    return res;
}
//--------------------------------------------------------------------------------------------------------
std::string pick_existing_path(std::vector<std::string>& dirs, std::vector<std::string>& lndirs, bool bcrossoff) {
    int s1 = dirs.size();
    int s2 = lndirs.size();
    std::string parent;
    if( bcrossoff && dirs.size() == 0)
        return std::string("");
    if( bcrossoff && ( dirs[0].compare("/yaffs2") == 0 || dirs[0].compare("/M18-1") == 0 || dirs[0].compare("/ram1") == 0 )) {
        if( s1+s2 == 1 )
            return std::string("");
        int choice = rand_int(s1+s2-1);
        parent = "";
        if( choice < s1-1) {
            parent = dirs[choice+1];
            dirs.erase(dirs.begin()+choice+1);
        }
        else
        {
            parent = lndirs[choice-s1+1];
            lndirs.erase(lndirs.begin()+(choice-s1+1));
        }
    }
    else
    {
        if( s1+s2 == 0)
            return std::string("");
        int choice = rand_int(s1+s2);
        parent = "";
        if( choice < s1 ) {
            if( dirs[choice] != "??" ) {
                parent  = dirs[choice];
                if( bcrossoff) {
                    dirs.erase(dirs.begin()+choice);
                }
            }
        }
        else {
            if( lndirs[choice-s1] != "??" ) {
                parent  = lndirs[choice-s1];
                if( bcrossoff) {
                    lndirs.erase(lndirs.begin()+(choice-s1));
                }
            }
        }
    }

    return parent;
}
//--------------------------------------------------------------------------------------------------------
/* from stackoverflow */
std::string & replace(std::string & subj, std::string old, std::string neu)
{
    size_t uiui = subj.find(old);
    if (uiui != std::string::npos)
    {
        subj.erase(uiui, old.size());
        subj.insert(uiui, neu);
    }
    return subj;
}




//--------------------------------------------------------------------------------------------------------
void normalize(std::vector<double>& vec) {
    double sum = 0.0;
    sum = std::accumulate(vec.begin(), vec.end(), 0.0 );
    for(std::vector<double>::iterator itr = vec.begin(); itr != vec.end(); itr++) {
        *itr /= sum;
    }
}
//--------------------------------------------------------------------------------------------------------
double rand01() {
#ifdef ENABLE_GMP
        double r =  (double)rand_int(1000000)/1000000.0;
#else
        double r = (double)rand()/(double)RAND_MAX;
#endif
    return r;
}
//--------------------------------------------------------------------------------------------------------
/* return api number*/
int pick_api(const char* strategy) {
    int api = -1;
    std::vector<int> apis = avail_apis();
    if( strcmp(strategy, "pure") == 0) {  
        if( apis.size() > 0) {
            int api_index = rand_int( apis.size() );
            api = apis[api_index];
        }
        else {
            api = -1;
        }
    }
    else {
        std::vector<double> weights;
        for(std::vector<int>::const_iterator itr = apis.begin(); itr != apis.end(); itr++) {
            weights.push_back( apiinfo[*itr].weight );
        }
        normalize(weights);
#ifdef ENABLE_GMP
        double r =  (double)rand_int(1000000)/1000000.0;
#else
        double r = (double)rand()/(double)RAND_MAX;
#endif
        double prev = 0.0;
        double sum = 0.0;
        for(unsigned long i = 0; i < weights.size(); i++) {
            sum += weights[i];
            if( r >= prev && r < sum) {
                api = apis[i];
                break;
            }
            else {
                prev = sum;
            }
        }
    }
    //assert( api >= 0 && api < MAX_API );
    return api;
}
//--------------------------------------------------------------------------------------------------------
void adjust_weights_from_string(const char* weightstring ) {
    unsigned long n = MAX_API;
    char buffer[1024];
    strcpy(buffer, weightstring);
    std::vector<int> apiindices;
    char* c = strtok(buffer, ",\n");
    while( c != NULL ) {
        apiindices.push_back( atoi(c) );
        c = strtok(NULL, ",\n");
    }
    for(int i = 0; i < MAX_API; i++) {
        std::vector<int>::iterator itr = std::find(apiindices.begin(), apiindices.end(), i);
        if( itr != apiindices.end() ) {
            apiinfo[i].weight = 0.8/apiindices.size();
        }
        else {
            apiinfo[i].weight = 0.2/(MAX_API-apiindices.size());
        }
    }
}
//--------------------------------------------------------------------------------------------------------
void adjust_weights_order() {
    unsigned long n = MAX_API;
    double delta = 1.0/(n-1);
    unsigned long num = 0;
    for(unsigned long i = 0; i < n; i++ ) {
        apiinfo[i].weight = rand01();
    }
    // while(num < n) {
        // unsigned long i = rand_int(n);
        // if( apiinfo[i].weight < 0.0) {
            // apiinfo[i].weight = 1.0 - num*delta;
            // num++;
        // }
    // }
    // normalize weights
    double sum = 0.0;
    for(unsigned long i = 0; i < n; i++ ) {
        sum += apiinfo[i].weight;
    }
    for(unsigned long i = 0; i < n; i++ ) {
        apiinfo[i].weight /= sum;
    }
}
//--------------------------------------------------------------------------------------------------------
void adjust_weights_manual() {
    /*unsigned long n = MAX_API;

    unsigned long column = rand_int(5);
    double sum = 0.0;
    for(unsigned int i = 0; i < n; i++) {
        sum += api_weights[i][column];
    }

    for(unsigned long i = 0; i < n; i++ ) {
        apiinfo[i].weight = api_weights[i][column]/sum;
    } */   
}
//--------------------------------------------------------------------------------------------------------
void adjust_weights_swarm() {
    unsigned long n = MAX_API;
    unsigned long x = rand_int(n);
    double div = 1.0/(n-1);
    for(unsigned long i = 0; i < n; i++ ) {        
        if( i != x) {
            apiinfo[i].weight = div;
        }
        else {
            apiinfo[i].weight = 0.0;
        }
    }
    // normalize weights
    double sum = 0.0;
    for(unsigned long i = 0; i < n; i++ ) {
        sum += apiinfo[i].weight;
    }
    for(unsigned long i = 0; i < n; i++ ) {
        apiinfo[i].weight /= sum;
    }
}
//--------------------------------------------------------------------------------------------------------
int goahead() {
    int res = 0;
    return 0;
    res = rand_int(10);
    if( res > 0 ) {
        return 1;
    }
    else {
        return 0;
    }
}
//--------------------------------------------------------------------------------------------------------
void gen_tester(FILE* fp, int length, const char* strategy, const char* format) {

    yaffs_test_case testcase;
    
    yaffs_api_call call(STARTUP);
    testcase.add( call );
    call.reset();

    call.api_id = MOUNT;
    call.parameters.push_back( vtype("/yaffs2", vtype::ePath) );
    testcase.add( call );
    call.reset();
    call.api_id = MOUNT;
    call.parameters.push_back( vtype("/M18-1", vtype::ePath) );
    testcase.add( call );
    call.reset();
    call.api_id = MOUNT;
    call.parameters.push_back( vtype("/ram1", vtype::ePath) );
    testcase.add( call );
    call.reset();

    char buf[1024];

    emulfs.init();

    int currlen = 0;
    int count = 0;
    yaffs_start_up();
    yaffs_mount("/yaffs2");
    yaffs_mount("/M18-1");
    yaffs_mount("/ram1");
    while( currlen < length ) {
        if( count++ > 1000000 ) break;
        int api  = pick_api(strategy);        
        switch(api) {
        case OPEN://int yaffs_open(const YCHAR *path, int oflag, int mode) ;
            {
                int choice = rand_int(3);
                if( choice > 0 ){ // new file
                    std::string newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                    int oflag, mode;
                    oflag = pick_open_flags();
                    oflag |= O_CREAT;

                    mode = pick_file_mode();

                    int res = yaffs_open(newpath.c_str(), oflag, mode);
                    if( newpath.size() > 0) {
                        if( res >= 0 || goahead() ) {
                            emulfs.avail_fds[emulfs.nextfd_index] = res;
                        
                            call.api_id = OPEN;
                            call.parameters.push_back( vtype(newpath, vtype::ePath) );
                            call.parameters.push_back( vtype(oflag, vtype::eFlag) );
                            call.parameters.push_back( vtype(mode, vtype::eMode) );
                            call.parameters.push_back( vtype(emulfs.nextfd_index++, vtype::eFd) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%d,%d,fd%d\n", OPEN, newpath.c_str(), oflag, mode, emulfs.nextfd_index++);
                            emulfs.avail_files.push_back( newpath );
                            emulfs.avail_fd_indices.push_back(emulfs.nextfd_index-1);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else if(emulfs.avail_files.size()+emulfs.avail_lnfiles.size() > 0 ){
                    std::string oldpath = ::pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    if( oldpath.length() > 0) {
                        int oflag, mode;
                        oflag = pick_open_flags();
                        mode = pick_file_mode();
                        int res = yaffs_open(oldpath.c_str(), oflag, mode);
                        if( res >= 0 || goahead()) {
                            emulfs.avail_fds[emulfs.nextfd_index] = res;
                            call.api_id = OPEN;
                            call.parameters.push_back( vtype(oldpath, vtype::ePath) );
                            call.parameters.push_back( vtype(oflag, vtype::eFlag) );
                            call.parameters.push_back( vtype(mode, vtype::eMode) );
                            call.parameters.push_back( vtype(emulfs.nextfd_index++, vtype::eFd) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%d,%d,fd%d\n", OPEN, oldpath.c_str(), oflag, mode, emulfs.nextfd_index++);                    
                            emulfs.avail_fd_indices.push_back(emulfs.nextfd_index-1);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
            }
            break;
        case CLOSE://int yaffs_close(int fd) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = -1;
                    fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_close(fd);
                    if( res != -1 || goahead()) {
                        call.api_id = CLOSE;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d\n", CLOSE, fdidxidx);
                        emulfs.avail_fd_indices.erase( emulfs.avail_fd_indices.begin()+fdidxidx);
                        //fputs(buf, fp);
                        currlen++; 
                    }
                }
            }
            break;
        case MKDIR:
            {
                std::string newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                mode_t mode = pick_file_mode();
                int res = yaffs_mkdir(newpath.c_str(), mode);
                if( newpath.size() > 0) {
                    if( res != -1 || goahead()) {
                        call.api_id = MKDIR;
                        call.parameters.push_back( vtype(newpath, vtype::ePath));
                        call.parameters.push_back( vtype(mode, vtype::eMode));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,%s,%d\n", MKDIR, newpath.c_str(), mode);
                        emulfs.avail_dirs.push_back( newpath );
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case RMDIR://int yaffs_rmdir(const YCHAR *path) ;
            {
                if( emulfs.avail_dirs.size() > 0 || emulfs.avail_lndirs.size() > 0 ) {
                    std::string path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, true);
                    if(path.size()>0) {
                        int res = yaffs_rmdir(path.c_str() );
                        if( res != -1 || goahead()) {
                            call.api_id = RMDIR;
                            call.parameters.push_back( vtype(path, vtype::ePath));
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s\n", RMDIR, path.c_str());  
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
            }
            break;
        case LSEEK://off_t yaffs_lseek(int fd, off_t offset, int whence) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int choice = rand_int(3);
                    int start = SEEK_SET;
                    int offset;
                    if( choice == 0 ) {
                        start = SEEK_SET;
                    }
                    else if(choice == 1 ) {
                        start = SEEK_CUR;
                    }
                    else if(choice==2) {
                        start = SEEK_END;
                    }
                    offset = rand_int(FILE_SIZE+FILE_SIZE);
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_lseek(fd, offset, start);
                    if( res != -1 || goahead()) {
                        call.api_id = LSEEK;
                        call.parameters.push_back( vtype(fdidx, vtype::eFd));
                        call.parameters.push_back( vtype(offset, vtype::eGeneral));
                        call.parameters.push_back( vtype(start, vtype::eGeneral) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d,%d\n", LSEEK, fdidxidx, offset, start); 
                        //fputs(buf, fp);
                        currlen++; 
                    }
                }
            }
            break;
        case TRUNCATE://int yaffs_truncate(const YCHAR *path, off_t new_size);
            {
                if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0 ) {
                    std::string oldpath = ::pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    if( oldpath.length() > 0) {
                        int offset = rand_int(FILE_SIZE);
                        int res = yaffs_truncate(oldpath.c_str(), offset);
                        if( res != -1 || goahead()) {
                            call.api_id = TRUNCATE;
                            call.parameters.push_back( vtype(oldpath, vtype::ePath));
                            call.parameters.push_back( vtype(offset, vtype::eGeneral));
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%d\n",TRUNCATE, oldpath.c_str(), offset);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
            }
            break;
        case FTRUNCATE://int yaffs_truncate(int fd, off_t new_size);
            {                
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int offset;
                    offset = rand_int(FILE_SIZE);
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_ftruncate(fd, offset);
                    if( res != -1 || goahead()) {
                        call.api_id = FTRUNCATE;
                        call.parameters.push_back( vtype(fdidx, vtype::eFd));
                        call.parameters.push_back( vtype(offset, vtype::eGeneral));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d\n", FTRUNCATE, fdidxidx, offset);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case STAT://int yaffs_stat(const YCHAR *path, struct yaffs_stat *buf) ;
            {
                int choice = rand_int(2);
                if( choice == 0 ) {
                    std::string path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    struct yaffs_stat stbuf;
                    int res = yaffs_stat(path.c_str(), &stbuf);
                    if( res  != -1 || goahead()) {
                        call.api_id = STAT;
                        call.parameters.push_back( vtype(path, vtype::ePath));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,%s\n", STAT, path.c_str());
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0 ) {
                        std::string path = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                        if( path.length() > 0) {
                            struct yaffs_stat stbuf;
                            int res = yaffs_stat(path.c_str(), &stbuf);
                           if( res  != -1 || goahead()) {
                                call.api_id = STAT;
                                call.parameters.push_back( vtype(path, vtype::ePath));
                                testcase.add( call );
                                call.reset();
                                //sprintf(buf, "%d,%s\n", STAT, path.c_str());
                                //fputs(buf, fp);
                                currlen++;
                            }
                        }
                    }
                }
            }
            break;
        case FSTAT://int yaffs_fstat(int fd, struct yaffs_stat *buf) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    struct yaffs_stat stbuf;
                    int res = yaffs_fstat(fd, &stbuf);
                    if( res != -1 || goahead()) {
                        call.api_id = FSTAT;
                        call.parameters.push_back( vtype(fdidx, vtype::eFd));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d\n",FSTAT, fdidxidx);             
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case LSTAT://int yaffs_lstat(const YCHAR *path, struct yaffs_stat *buf) ;
            {
                int choice = rand_int(2);
                if( choice == 0 ) {
                    if( emulfs.avail_lndirs.size() > 0 ) {
                        int di = -1;
                        di = rand_int(emulfs.avail_lndirs.size());
                        std::string path = emulfs.avail_lndirs[di];
                        struct yaffs_stat stbuf;
                        int res = yaffs_lstat(path.c_str(), &stbuf);
                        if( res != -1 || goahead()) {
                            call.api_id = LSTAT;
                            call.parameters.push_back( vtype(path, vtype::ePath));
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s\n", LSTAT, path.c_str());                
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    if( emulfs.avail_lnfiles.size() > 0 ) {
                        int fi = -1;
                        fi = rand_int(emulfs.avail_lnfiles.size());
                        std::string path = emulfs.avail_lnfiles[fi];
                        struct yaffs_stat stbuf;
                        int res = yaffs_lstat(path.c_str(), &stbuf);
                        if( res != -1 || goahead()) {
                            call.api_id = LSTAT;
                            call.parameters.push_back( vtype(path, vtype::ePath));
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s\n", LSTAT, emulfs.avail_lnfiles[fi].c_str());                      
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
            }
            break;
        case READ://int yaffs_read(int fd, void *buf, unsigned int nbyte) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int nbyte = rand_int(FILE_SIZE);
                    int res = yaffs_read(fd, rwbuf, nbyte);
                    if( res != -1 || goahead()) {
                        call.api_id = READ;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        call.parameters.push_back( vtype( nbyte, vtype::eGeneral ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d\n",READ,fdidxidx,nbyte);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case WRITE://int yaffs_write(int fd, const void *buf, unsigned int nbyte) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int nbyte = rand_int(FILE_SIZE);
                    int res = yaffs_write(fd, rwbuf, nbyte);
                    if( res != -1 || goahead()) {
                        call.api_id = WRITE;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        call.parameters.push_back( vtype( nbyte, vtype::eGeneral ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d\n",WRITE,fdidxidx,nbyte);
                        //fputs(buf, fp);                        
                        currlen++;
                    }
                }
            }
            break;
        case FREESPACE:
            {
                int choice = rand_int(2);
                if( choice == 0 ) {
                    if( emulfs.avail_dirs.size() + emulfs.avail_lndirs.size() > 0) {
                        std::string path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                        int res = yaffs_freespace(path.c_str());
                        if( res != -1 || goahead()) {
                            call.api_id = FREESPACE;
                            call.parameters.push_back( vtype( path, vtype::ePath ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s\n",FREESPACE, path.c_str());                    
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        std::string path = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                        int res = yaffs_freespace(path.c_str());
                        if( res != -1 || goahead()) {
                            call.api_id = FREESPACE;
                            call.parameters.push_back( vtype( path, vtype::ePath ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s\n",FREESPACE,path.c_str());                  
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
            }
            break;
        case OPENDIR://yaffs_DIR *yaffs_opendir(const YCHAR *dirname) ;
            {
                if( emulfs.avail_dirs.size() + emulfs.avail_lndirs.size() > 0) {
                    std::string path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    yaffs_DIR* pdir = yaffs_opendir(path.c_str());
                    if( pdir != NULL || goahead()) {
                        emulfs.avail_dd_indices.push_back(emulfs.nextdd_index);
                        emulfs.avail_dds[emulfs.nextdd_index] = pdir;
                        call.api_id = OPENDIR;
                        call.parameters.push_back( vtype( path, vtype::ePath ) );
                        call.parameters.push_back( vtype( emulfs.nextdd_index, vtype::eDd));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,%s,dd%d\n", OPENDIR, path.c_str(), emulfs.nextdd_index);
                        emulfs.nextdd_index++;
                        //fputs(buf, fp);
                        //emulfs.avail_dd_indices.push_back(emulfs.nextdd_index-1);
                        currlen++;
                    }                    
                }
            }
            break;
        case READDIR://struct yaffs_dirent *yaffs_readdir(yaffs_DIR *dirp) ;
            {
                if( emulfs.avail_dd_indices.size() > 0 ) {
                    int ddptridxidx = rand_int(emulfs.avail_dd_indices.size());
                    int ddptridx = emulfs.avail_dd_indices[ddptridxidx];
                    yaffs_DIR* dirp = emulfs.avail_dds[ddptridx];
                    if( NULL != yaffs_readdir(dirp) || goahead()) {
                        call.api_id = READDIR;
                        call.parameters.push_back( vtype( ddptridx, vtype::eDd));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,dd%d\n", READDIR, ddptridxidx);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case REWINDDIR://void yaffs_rewinddir(yaffs_DIR *dirp) ;
            {
                if( emulfs.avail_dd_indices.size() > 0 ) {
                    int ddptridxidx = rand_int(emulfs.avail_dd_indices.size());
                    int ddptridx = emulfs.avail_dd_indices[ddptridxidx];
                    yaffs_DIR* dirp = emulfs.avail_dds[ddptridx];
                    yaffs_rewinddir(dirp);
                    {
                        call.api_id = REWINDDIR;
                        call.parameters.push_back( vtype( ddptridx, vtype::eDd));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,dd%d\n", REWINDDIR, ddptridxidx);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case CLOSEDIR://int yaffs_closedir(yaffs_DIR *dirp) ;
            {
                if( emulfs.avail_dd_indices.size() > 0 ) {
                    int ddptridxidx = rand_int(emulfs.avail_dd_indices.size());
                    int ddptridx = emulfs.avail_dd_indices[ddptridxidx];
                    yaffs_DIR* dirp = emulfs.avail_dds[ddptridx];
                    int res = yaffs_closedir(dirp);
                    if( res != -1 || goahead()) {
                        call.api_id = CLOSEDIR;
                        call.parameters.push_back( vtype( ddptridx, vtype::eDd));
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,dd%d\n",CLOSEDIR, ddptridxidx);
                        emulfs.avail_dd_indices.erase( emulfs.avail_dd_indices.begin()+ddptridxidx);
                        //fputs(buf, fp);                        
                        currlen++;
                    }
                }
            }
            break;
        case LINK://int yaffs_link(const YCHAR *oldpath, const YCHAR *newpath);
            {
                std::string oldpath, newpath;
                int choice = rand_int(2);
                if( choice == 0 ) {
                    oldpath = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                    if( oldpath.length() > 0 && newpath.length() > 0 ) {
                        int res = yaffs_link(oldpath.c_str(), newpath.c_str());
                        if( res != -1 || goahead()) {
                            call.api_id = LINK;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( newpath, vtype::ePath ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%s\n", LINK, oldpath.c_str(), newpath.c_str());
                            emulfs.avail_lndirs.push_back(newpath);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        oldpath = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles,false);
                        newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                        if( oldpath.length() > 0 && newpath.length() > 0 ) {
                            int res = yaffs_link(oldpath.c_str(), newpath.c_str());
                            if( res != -1 || goahead()) {
                                call.api_id = LINK;
                                call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                                call.parameters.push_back( vtype( newpath, vtype::ePath ) );
                                testcase.add( call );
                                call.reset();
                                //sprintf(buf, "%d,%s,%s\n", LINK, oldpath.c_str(), newpath.c_str());
                                emulfs.avail_lnfiles.push_back(newpath);
                                //fputs(buf, fp);
                                currlen++;
                            }
                        }
                    }
                }
            }   
            break;
        case SYMLINK://int yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath);
            {
                std::string oldpath, newpath;
                int choice = rand_int(2);
                if( choice == 0 ) {
                    oldpath = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                    if( oldpath.length() > 0 && newpath.length() > 0 ) {
                        int res = yaffs_symlink(oldpath.c_str(), newpath.c_str());
                        if( res != -1 || goahead()) {
                            call.api_id = SYMLINK;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( newpath, vtype::ePath ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%s\n", SYMLINK, oldpath.c_str(), newpath.c_str());
                            emulfs.avail_lndirs.push_back(newpath);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        oldpath = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                        newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                        if( oldpath.length() > 0 && newpath.length() > 0 ) {
                            int res = yaffs_symlink(oldpath.c_str(), newpath.c_str());
                            if( res != -1 || goahead()) {
                                call.api_id = SYMLINK;
                                call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                                call.parameters.push_back( vtype( newpath, vtype::ePath ) );
                                testcase.add( call );
                                call.reset();
                                //sprintf(buf, "%d,%s,%s\n", SYMLINK, oldpath.c_str(), newpath.c_str());
                                emulfs.avail_lnfiles.push_back(newpath);
                                //fputs(buf, fp);
                                currlen++;
                            }
                        }
                    }
                }
            }
            break;
        case READLINK://int yaffs_readlink(const YCHAR *path, YCHAR *buf, int bufsiz);
            {
                std::string oldpath, newpath;
                int choice = rand_int(2);
                if( choice == 0 ) {
                    oldpath = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    if( oldpath.length() > 0 ) {
                        int res = yaffs_readlink(oldpath.c_str(), rwbuf, FILE_SIZE);
                        if(res != -1 || goahead()) {
                            call.api_id = READLINK;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( FILE_SIZE, vtype::eMaxSize ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%d\n", READLINK, oldpath.c_str(), FILE_SIZE);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        oldpath = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                        if( oldpath.length() > 0 ) {
                            int res = yaffs_readlink(oldpath.c_str(), rwbuf, FILE_SIZE);
                            if(res != -1 || goahead()) {
                                call.api_id = READLINK;
                                call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                                call.parameters.push_back( vtype( FILE_SIZE, vtype::eMaxSize ) );
                                testcase.add( call );
                                call.reset();
                                //sprintf(buf, "%d,%s,%d\n", READLINK, oldpath.c_str(), FILE_SIZE);
                                //fputs(buf, fp);
                                currlen++;
                            }
                        }
                    }
                }
            }
            break;
        case UNLINK://int yaffs_unlink(const YCHAR *path) ;
            {
                std::string oldpath, newpath;
                int choice = rand_int(2);
                if( choice == 0 ) {
                    oldpath = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, true);
                    if( oldpath.length() > 0 ) {
                        int res = yaffs_unlink(oldpath.c_str());
                        if( res != -1 || goahead()) {
                            call.api_id = UNLINK;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s\n", UNLINK, oldpath.c_str());
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        oldpath = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, true);
                        if( oldpath.length() > 0 ) {
                            int res = yaffs_unlink(oldpath.c_str());
                            if( res != -1 || goahead()) {
                                call.api_id = UNLINK;
                                call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                                testcase.add( call );
                                call.reset();
                                //sprintf(buf, "%d,%s\n", UNLINK, oldpath.c_str());
                                //fputs(buf, fp);
                                currlen++;
                            }
                        }    
                    }                    
                }
            }
            break;
        case RENAME://int yaffs_rename(const YCHAR *oldPath, const YCHAR *newPath) ;
            {
                std::string oldpath, newpath;
                int choice = rand_int(2);
                if( choice == -1 ) {
                    oldpath = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                    if( oldpath.length() > 0 && newpath.length() > 0 ) {
                        int res = yaffs_rename(oldpath.c_str(), newpath.c_str());
                        if( res != -1 || goahead()) {
                            call.api_id = RENAME;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( newpath, vtype::ePath ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%s\n", RENAME,oldpath.c_str(), newpath.c_str());
                            emulfs.avail_lndirs.push_back(newpath);
                            std::vector<std::string>::iterator itr = std::find(emulfs.avail_dirs.begin(), emulfs.avail_dirs.end(), oldpath);
                            if( itr != emulfs.avail_dirs.end()) {
                                emulfs.avail_dirs.erase( itr );
                            }
                            itr = std::find(emulfs.avail_lndirs.begin(), emulfs.avail_lndirs.end(), oldpath);
                            if( itr != emulfs.avail_lndirs.end()) {
                                emulfs.avail_lndirs.erase( itr );
                            }
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    oldpath = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    newpath = pick_new_path(emulfs.avail_dirs, emulfs.avail_lndirs);
                    if( oldpath.length() > 0 && newpath.length() > 0 ) {
                        int res = yaffs_rename(oldpath.c_str(), newpath.c_str());
                        if( res != -1 || goahead()) {
                            call.api_id = RENAME;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( newpath, vtype::ePath ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%s\n", RENAME,oldpath.c_str(), newpath.c_str());
                            emulfs.avail_lndirs.push_back(newpath);
                            std::vector<std::string>::iterator itr = std::find(emulfs.avail_files.begin(), emulfs.avail_files.end(), oldpath);
                            if( itr != emulfs.avail_files.end()) {
                                emulfs.avail_files.erase( itr );
                            }
                            itr = std::find(emulfs.avail_lnfiles.begin(), emulfs.avail_lnfiles.end(), oldpath);
                            if( itr != emulfs.avail_lnfiles.end()) {
                                emulfs.avail_lnfiles.erase( itr );
                            }
                            //fputs(buf, fp);
                            currlen++;
                        }
                        
                    }
                }
            }
            break;
        case CHMOD://int yaffs_chmod(const YCHAR *path, mode_t mode);
            {
                std::string oldpath, newpath;
                int choice = rand_int(2);
                if( choice == 0 ) {
                    oldpath = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    if( oldpath.length() > 0 ) {
                        int mode = pick_file_mode();
                        int res = yaffs_chmod(oldpath.c_str(), mode);
                        if(res != -1 || goahead()) {
                            call.api_id = CHMOD;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( mode, vtype::eMode ) );
                            testcase.add( call );
                            call.reset();
                            sprintf(buf, "%d,%s,%d\n",CHMOD,oldpath.c_str(), mode);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    oldpath = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    if( oldpath.length() > 0) {
                        int mode = pick_file_mode();
                        int res = yaffs_chmod(oldpath.c_str(), mode);
                        if(res != -1 || goahead()) {
                            call.api_id = CHMOD;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( mode, vtype::eMode ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%d\n",CHMOD,oldpath.c_str(), mode);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
            }
            break;
        case FCHMOD://int yaffs_fchmod(int fd, mode_t mode);
            {
                if( emulfs.avail_fd_indices.size() > 0) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int mode = pick_file_mode();
                    int res = yaffs_fchmod(fd, mode);
                    if( res != -1 || goahead()) {
                        call.api_id = FCHMOD;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        call.parameters.push_back( vtype( mode, vtype::eMode ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d\n", FCHMOD, fdidxidx, mode);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }                
            }
            break;

        case FSYNC://int yaffs_fsync(int fd) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_fsync(fd);
                    if( res != -1 || goahead()) {
                        call.api_id = FSYNC;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d\n", FSYNC, fdidxidx);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case FDATASYNC://int yaffs_fdatasync(int fd) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_fdatasync(fd);
                    if( res != -1 || goahead()) {
                        call.api_id = FDATASYNC;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d\n", FDATASYNC, fdidxidx);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case ACCESS://int yaffs_access(const YCHAR *path, int amode);
            {
                std::string oldpath, newpath;
                int choice = rand_int(2);
                if( choice == 0 ) {
                    oldpath = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    if( oldpath.length() > 0 ) {
                        int mode = pick_file_mode();
                        int res = yaffs_access(oldpath.c_str(), mode);
                        if(res != -1 || goahead()) {
                            call.api_id = ACCESS;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( mode, vtype::eMode ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%d\n",ACCESS,oldpath.c_str(), mode);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
                else {
                    oldpath = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    if( oldpath.length() > 0) {
                        int mode = pick_file_mode();
                        int res = yaffs_access(oldpath.c_str(), mode);
                        if(res != -1 || goahead()) {
                            call.api_id = ACCESS;
                            call.parameters.push_back( vtype( oldpath, vtype::ePath ) );
                            call.parameters.push_back( vtype( mode, vtype::eMode ) );
                            testcase.add( call );
                            call.reset();
                            //sprintf(buf, "%d,%s,%d\n",ACCESS,oldpath.c_str(), mode);
                            //fputs(buf, fp);
                            currlen++;
                        }
                    }
                }
            }
            break;
        case DUP://int yaffs_dup(int fd);
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_dup(fd);
                    if( res >= 0 || goahead()) {
                        call.api_id = DUP;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        call.parameters.push_back( vtype( emulfs.nextfd_index, vtype::eFd) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d\n", DUP, fdidxidx);
                        ////fputs(buf, fp);
                        currlen++;
                        
                        emulfs.avail_fds[emulfs.nextfd_index] = res;
                        emulfs.avail_fd_indices.push_back(emulfs.nextfd_index);
                        emulfs.nextfd_index++;
                    }
                }
            }
            break;             
        case PREAD://int yaffs_pread(int fd, void *buf, unsigned int nbyte, unsigned int offset);
            {            
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int nbyte = rand_int(FILE_SIZE);
                    int noffset = rand_int(FILE_SIZE);
                    int res = yaffs_pread(fd, rwbuf, nbyte, noffset);
                    if( res != -1 || goahead()) {
                        call.api_id = PREAD;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        call.parameters.push_back( vtype( nbyte, vtype::eGeneral ) );                            
                        call.parameters.push_back( vtype( noffset, vtype::eGeneral ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d,%d\n",PREAD,fdidxidx,nbyte, noffset);
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;           
        case PWRITE://int yaffs_pwrite(int fd, const void *buf, unsigned int nbyte, unsigned int offset);
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int nbyte = rand_int(FILE_SIZE);
                    int noffset = rand_int(FILE_SIZE);
                    int res = yaffs_pwrite(fd, rwbuf, nbyte, noffset);
                    if( res != -1 || goahead()) {
                        call.api_id = PWRITE;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        call.parameters.push_back( vtype( nbyte, vtype::eGeneral ) );                            
                        call.parameters.push_back( vtype( noffset, vtype::eGeneral ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d,%d,%d\n",PWRITE,fdidxidx,nbyte, noffset);
                        //fputs(buf, fp);                        
                        currlen++;
                    }
                }
            }
            break;          
        case UTIME://int yaffs_utime(const YCHAR *path, const struct yaffs_utimbuf *buf);
            {
                struct yaffs_utimbuf utbuf;
                int choice = rand_int(2);
                std::string path;
                if( choice == 0 ) {
                    path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                }
                else {
                    path = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                }
                if( path.length() > 0) {
                    int res = yaffs_utime(path.c_str(), &utbuf);
                    if( res  != -1 || goahead()) {
                        call.api_id = UTIME;
                        call.parameters.push_back( vtype( path, vtype::ePath ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,%s\n", UTIME, path.c_str());
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;           
        case FUTIME://int yaffs_futime(int fd, const struct yaffs_utimbuf *buf);
            {
                struct yaffs_utimbuf utbuf;
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_futime(fd, &utbuf);
                    if( res != -1 || goahead()) {
                        call.api_id = FUTIME;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        testcase.add( call );
                        call.reset();
                        //sprintf(buf, "%d,fd%d\n",FUTIME, fdidxidx);             
                        //fputs(buf, fp);
                        currlen++;
                    }
                }
            }
            break;
        case FLUSH://int yaffs_flush(int fd) ;
            {
                if( emulfs.avail_fd_indices.size() > 0 ) {
                    int fdidxidx = rand_int(emulfs.avail_fd_indices.size());
                    int fdidx = emulfs.avail_fd_indices[fdidxidx];
                    int fd = emulfs.avail_fds[fdidx];
                    int res = yaffs_flush(fd);
                    if( res != -1 || goahead()) {
                        call.api_id = FLUSH;
                        call.parameters.push_back( vtype( fdidx, vtype::eFd ) );
                        testcase.add( call );
                        call.reset();
                        currlen++;
                    }
                }
            }
            break;
        case SYNC://int yaffs_sync(const YCHAR *path) ;
            {
                int choice = rand_int(2);
                std::string path;
                if( choice == 0 ) {
                    if( emulfs.avail_dirs.size() + emulfs.avail_lndirs.size() > 0) {
                        path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        path = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    }
                }
                if( path.size() > 0) {
                    int res = yaffs_sync(path.c_str());
                    if( res != -1 || goahead()) {
                        call.api_id = SYNC;
                        call.parameters.push_back( vtype( path, vtype::ePath ) );
                        testcase.add( call );
                        call.reset();
                        currlen++;
                    }
                }                
            }
            break;
        case TOTALSPACE://loff_t yaffs_totalspace(const YCHAR *path);
            {
                int choice = rand_int(2);
                std::string path;
                if( choice == 0 ) {
                    if( emulfs.avail_dirs.size() + emulfs.avail_lndirs.size() > 0) {
                        path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        path = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    }
                }
                if( path.size() > 0) {
                    int res = yaffs_totalspace(path.c_str());
                    if( res != -1 || goahead()) {
                        call.api_id = TOTALSPACE;
                        call.parameters.push_back( vtype( path, vtype::ePath ) );
                        testcase.add( call );
                        call.reset();
                        currlen++;
                    }
                }        
            }
            break;
        case INODECOUNT://int yaffs_inodecount(const YCHAR *path);
            {
                int choice = rand_int(2);
                std::string path;
                if( choice == 0 ) {
                    if( emulfs.avail_dirs.size() + emulfs.avail_lndirs.size() > 0) {
                        path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        path = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    }
                }
                if( path.size() > 0) {
                    int res = yaffs_inodecount(path.c_str());
                    if( res != -1 || goahead()) {
                        call.api_id = INODECOUNT;
                        call.parameters.push_back( vtype( path, vtype::ePath ) );
                        testcase.add( call );
                        call.reset();
                        currlen++;
                    }
                }    
            }
            break;
        case NHANDLES://int yaffs_n_handles(const YCHAR *path);
            {
                int choice = rand_int(2);
                std::string path;
                if( choice == 0 ) {
                    if( emulfs.avail_dirs.size() + emulfs.avail_lndirs.size() > 0) {
                        path = pick_existing_path(emulfs.avail_dirs, emulfs.avail_lndirs, false);
                    }
                }
                else {
                    if( emulfs.avail_files.size() + emulfs.avail_lnfiles.size() > 0) {
                        path = pick_existing_path(emulfs.avail_files, emulfs.avail_lnfiles, false);
                    }
                }
                if( path.size() > 0) {
                    int res = yaffs_n_handles(path.c_str());
                    if( res != -1 || goahead()) {
                        call.api_id = NHANDLES;
                        call.parameters.push_back( vtype( path, vtype::ePath ) );
                        testcase.add( call );
                        call.reset();
                        currlen++;
                    }
                }    
            }
            break;
#ifndef __LESS_API            
        case SETXATTR://int yaffs_setxattr(const char *path, const char *name, const void *data, int size, int flags);
            {
            }
            break;        
        case LSETXATTR://int yaffs_lsetxattr(const char *path, const char *name, const void *data, int size, int flags);
            {
            }
            break;       
        case FSETXATTR://int yaffs_fsetxattr(int fd, const char *name, const void *data, int size, int flags);
            {
            }
            break;       
        case GETXATTR://int yaffs_getxattr(const char *path, const char *name, void *data, int size);
            {
            }
            break;        
        case LGETXATTR://int yaffs_lgetxattr(const char *path, const char *name, void *data, int size);
            {
            }
            break;       
        case FGETXATTR://int yaffs_fgetxattr(int fd, const char *name, void *data, int size);
            {
            }
            break;       
        case REMOVEXATTR://int yaffs_removexattr(const char *path, const char *name);
            {
            }
            break;     
        case LREMOVEXATTR://int yaffs_lremovexattr(const char *path, const char *name);
            {
            }
            break;    
        case FREMOVEXATTR://int yaffs_fremovexattr(int fd, const char *name);
            {
            }
            break;    
        case LISTXATTR://int yaffs_listxattr(const char *path, char *list, int size);
            {
            }
            break;      
        case LLISTXATTR://int yaffs_llistxattr(const char *path, char *list, int size);
            {
            }
            break;      
        case FLISTXATTR://int yaffs_flistxattr(int fd, char *list, int size);
            {
            }
            break;
#endif
        default:
            break;
        }        
    }
    if( strcmp(format, "concolic") == 0) {
        std::string csrc;
        testcase2concolicsrc(testcase, csrc);
        fputs( csrc.c_str(), fp);
    }
    else {
        fputs(testcase.tostring().c_str(), fp);
    }
    //yaffs_unmount("/yaffs2");
}
//--------------------------------------------------------------------------------------------------------
void gen_testers(FILE* fp, int numcases, int length, const char* strategy, const char* format, const char* weightstring) {
    int i = 0;
    for(; i < numcases; i++) {
        if( i % 2 == 0) {
            if( strcmp(strategy, "swarm") == 0) {
                adjust_weights_swarm();
            }
            else if( strcmp(strategy, "order") == 0) {
                adjust_weights_manual();
            }
            else if( strcmp(strategy, "weights") == 0) {
                adjust_weights_from_string(weightstring);
            }
        }
        gen_tester(fp, length, strategy, format);
        if( strcmp(format, "concolic") != 0) {
            //fputs("========================================================\n", fp);
        }
    }
}

//--------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    /* parse command line options */

    TCLAP::CmdLine* p= new TCLAP::CmdLine("YAFFS2 test case generator", ' ', "0.99");
    TCLAP::CmdLine& cmd = *p;
    TCLAP::ValueArg<int>    argRandseed("", "randseed", "seed for random generator", false, 0, "int");
    TCLAP::ValueArg<int>    argStartIdx("", "startidx", "start index of test cases", false, 0, "int");
    TCLAP::ValueArg<int>    argEndIdx("", "endidx", "end index of test cases",  false,0 , "int");
    TCLAP::ValueArg<int>    argEachSize("", "eachsize", "The number of test cases in each *.c file", false, 1, "int");
    TCLAP::ValueArg<std::string> argDir("", "dir", "The directory where you want want to put the generated test cases", false, "./", "std::string");
    TCLAP::ValueArg<int>    argTCLength("", "tclen", "The length of test cases", true, 100, "int");
    TCLAP::ValueArg<std::string> argStrategy("", "strategy", "Pure random, swarm, or weighted random", false, "pure", "std::string");
    TCLAP::ValueArg<std::string> argFormat("", "format", "concolic or txt call sequence", false, "nonconcolic", "std::string");
    TCLAP::ValueArg<std::string> argWeightString("", "weightstring", "weight string", false, "", "std::string");

    for( int i = 0; i < MAX_API; i++) {
        TCLAP::SwitchArg* p = new TCLAP::SwitchArg( "", apiinfo[i].name, apiinfo[i].desc, true);
        cmd.add( *p );
    }

    cmd.add( argRandseed );
    cmd.add( argStartIdx );
    cmd.add( argEndIdx );
    cmd.add( argEachSize );
    cmd.add( argDir );
    cmd.add( argTCLength );
    cmd.add( argStrategy );
    cmd.add( argFormat );
    cmd.add( argWeightString );

    cmd.parse( argc, argv );        
    
    std::list<TCLAP::Arg*>&argList = cmd.getArgList();
    std::list<TCLAP::Arg*>::const_iterator iter = argList.begin();
 
    for( ; iter != argList.end(); iter++) {
        for( int i = 0; i < MAX_API; i++) {
            if( strcmp( apiinfo[i].name, (*iter)->getName().c_str()) == 0) {
                TCLAP::SwitchArg* p = dynamic_cast<TCLAP::SwitchArg*>( *iter);
                if( p != NULL ) {
                    if( p->getValue() == false ) {
                        apiinfo[i].benable = false;
			//			printf("%s", )
                   }
                }
                break;
            }
        }
    }
    
    int startidx;
    int endidx;
    int each_size;
    const char* dir = 0;
    int length = 0;
    srand(time(NULL));
    int randseed  = rand();
//    randseed = argRandseed.getValue();
    startidx = argStartIdx.getValue();
    endidx = argEndIdx.getValue();
    //numcases = argNumCases.getValue();
    each_size = argEachSize.getValue();
    dir = argDir.getValue().c_str();
    length = argTCLength.getValue();
    const char* strategy = argStrategy.getValue().c_str();
    const char* format = argFormat.getValue().c_str();
    const char* weightstring = argWeightString.getValue().c_str();

    //    printf("randseed = %d\n", randseed);
#if defined(ENABLE_GMP)
    gmp_randinit_default(state);
    gmp_randseed_ui(state, randseed);
    
#else
    srand(randseed);
#endif

    for(int i = startidx; i <= endidx /* yes less than or equal */; i++ ) {
        char filename[1024];
        sprintf(filename, "%s/ts%06d.c", dir, i);
        FILE* fp = stdout; //fopen(filename, "w+");
        if( fp ) {
            gen_testers(fp, 1, length, strategy, format, weightstring);
            fclose(fp);
            printf("Generated test cases file %s.\n", filename);
        }
        else {
            printf("Cannot write test cases to file %s.\n", filename);
        }
    }

#if defined(ENABLE_GMP)
    gmp_randclear(state);
#endif

    return 0;       
}
//--------------------------------------------------------------------------------------------------------
int decideit(int n) {
    if( rand()%n == n-1)
        return 1;
    else
        return 0;
}
//--------------------------------------------------------------------------------------------------------
char sep(int i) {
    return i == 0 ? ' ' : ',';
}
//--------------------------------------------------------------------------------------------------------
std::string make_call(std::string funcname, yaffs_api_call call, bool hasret, int methodidx,
    std::map< std::pair<int, int>, bool> concolicvars, int expos, std::string extrastr) {
        std::ostringstream os;
        if( hasret ) {
            vtype::esubtype subtype = call.parameters[call.parameters.size()-1].subtype();
            if( subtype == vtype::eFd ) {
                os << "    int fd" << call.parameters[call.parameters.size()-1].get_int() << "=" << funcname << "(";
            }
            else if( subtype == vtype::eDd ) {
                os << "    yaffs_DIR* dd" << call.parameters[call.parameters.size()-1].get_int() << "=" << funcname << "(";
            }
        }
        else {
            os << "    " << funcname << "(";
        }

        int numparams = hasret ? call.parameters.size() -1 : call.parameters.size();
        for(int k = 0; k < numparams; k++) {
            if( expos == k ) {
                if( k == 0 ) {
                    os << extrastr << ",";
                }
                else {
                    os << "," << extrastr;
                }
            }
            if( concolicvars.find( std::make_pair(methodidx, k) ) != concolicvars.end() ) {
                os << sep(k) << "var_" << methodidx << "_" << k;
            }
            else {
                if( call.parameters[k].type() == vtype::eInt) {
                    if( call.parameters[k].subtype() == vtype::eDd) {
                        os << sep(k) << "dd" << call.parameters[k].get_int();
                    }
                    else if( call.parameters[k].subtype() == vtype::eFd) {
                        os << sep(k) << "fd" << call.parameters[k].get_int();
                    }
                    else {
                        os << sep(k) << call.parameters[k].get_int();
                    }
                }
                else if(call.parameters[k].type() == vtype::eString) {
                    os << sep(k) << "\"" << call.parameters[k].get_str() << "\"";
                }
            }
        }
        if( expos == numparams ) {
            os << ", " << extrastr;
        }
        os << ");\n";

        return os.str();
}
//--------------------------------------------------------------------------------------------------------
void testcase2concolicsrc(const yaffs_test_case& testcase, std::string& src) {
    // generate header part
    const char* p1 = "#include <yaffsfs.h>\n"
        "#include <crest.h>\n"
        "\n"
        "int random_seed;\n"
        "int simulate_power_failure;\n"
        "extern int yaffs_trace_mask;\n"
        "\n"
        "#define MAXSIZE 10000\n"
        "char rwbuf[MAXSIZE];\n"
        "struct yaffs_stat st_buf;\n"
        "struct yaffs_utimbuf utbuf;\n"
        "\n"
        "void CREST_string(char buffer[], int maxsize) {\n"
        "    int size;\n"
        "    CREST_int(size);\n"
        "    if( size == 0 ) {\n"
        "        size += 1;\n"
        "     }\n"
        "    size = size%maxsize;\n"
        "    int i;\n"
        "    for(i = 0; i < size; i++) {\n"
        "        unsigned char c;\n"
        "        CREST_unsigned_char(c);\n"
        "        buffer[i] = c;\n"
        "    }\n"
        "    buffer[i] = 0;\n"
        "}\n"
        "\n";
    src.append(p1);  

    // generate function calls
    char sbuf[1024];
    const char* p2 =    "void concolictest() {\n"
                "\n";                
    src.append(p2);


    // vary function parameters using concolic inputs
    // concolic variables, true - string, false - int
    srand(time(NULL));
    std::map< std::pair<int, int>, bool> concolicvars;
    for( int i = 0; i < testcase.api_calls.size(); i++) {
        yaffs_api_call call = testcase.api_calls[i];
        for( int j = 0; j < call.parameters.size(); j++) {
            vtype v = call.parameters[j];
            if( v.subtype() == vtype::ePath && decideit(10) ) {
                concolicvars.insert( std::make_pair( std::make_pair(i, j), true) );
            }
            else if( (v.subtype() == vtype::eMode || v.subtype() == vtype::eFlag || v.subtype() == vtype::eGeneral) && decideit(10) ) {
                concolicvars.insert( std::make_pair(std::make_pair(i, j), false));
            }
        }
    }

    std::map< std::pair<int, int>, bool>::const_iterator itr = concolicvars.begin();
    for( ; itr != concolicvars.end(); itr++) {
        std::ostringstream os;
        int i = itr->first.first;
        int j = itr->first.second;
        if( itr->second == true ) {
            os << "    char var_" << i << "_" << j << "[256];\n";
            os << "    CREST_string( var_" << i << "_" << j << ", 256);\n";
        }
        else {
            os << "    int var_" << i << "_" << j << ";\n";
            os << "    CREST_int( var_" << i << "_" << j << ");\n";
        }
        src.append( os.str() );
    }
    src.append("\n");

    for(int i= 0; i < testcase.api_calls.size(); i++) {
        yaffs_api_call call = testcase.api_calls[i];
#if 1
        switch( call.api_id ) {
            case STARTUP:
                {
                    std::string callstr = make_call("yaffs_start_up", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case MOUNT:
                {   
                    std::string callstr = make_call("yaffs_mount", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case MOUNT2:
                {   
                    std::string callstr = make_call("yaffs_mount2", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;            
            
            case UNMOUNT:
                {   
                    std::string callstr = make_call("yaffs_unmount", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case UNMOUNT2:
                {
                    std::string callstr = make_call("yaffs_unmount2", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case REMOUNT:
                {   
                    std::string callstr = make_call("yaffs_remount", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case OPEN:
                {
                    std::string callstr = make_call("yaffs_open", call, true, i, concolicvars, -1, "");
                    src.append( callstr );           
                }
                break;
            case CLOSE:
                {
                    std::string callstr = make_call("yaffs_close", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case MKDIR:
                {
                    std::string callstr = make_call("yaffs_mkdir", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case RMDIR:
                {
                    std::string callstr = make_call("yaffs_rmdir", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case LSEEK:
                {
                    std::string callstr = make_call("yaffs_lseek", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case TRUNCATE:
                {
                    std::string callstr = make_call("yaffs_truncate", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case FTRUNCATE:
                {
                    std::string callstr = make_call("yaffs_ftruncate", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;                    
            case STAT:
                {
                    std::string callstr = make_call("yaffs_stat", call, false, i, concolicvars, 1, "&st_buf");
                    src.append( callstr );
                }
                break;
            case FSTAT:
                {
                    std::string callstr = make_call("yaffs_fstat", call, false, i, concolicvars, 1, "&st_buf");
                    src.append( callstr );
                }
                break; 
            case LSTAT:
                {
                    std::string callstr = make_call("yaffs_lstat", call, false, i, concolicvars, 1, "&st_buf");
                    src.append( callstr );
                }
                break;
            case READ:
                {
                    std::string callstr = make_call("yaffs_read", call, false, i, concolicvars, 1, "rwbuf");
                    src.append( callstr );
                }
                break;
            case WRITE:
                {
                    std::string callstr = make_call("yaffs_write", call, false, i, concolicvars, 1, "rwbuf");
                    src.append( callstr );
                }
                break;
            case FREESPACE:
                {
                    std::string callstr = make_call("yaffs_freespace", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case OPENDIR:
                {
                    std::string callstr = make_call("yaffs_opendir", call, true, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case READDIR:
                {
                    std::string callstr = make_call("yaffs_readdir", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case REWINDDIR:
                {
                    std::string callstr = make_call("yaffs_rewinddir", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case CLOSEDIR:
                {
                    std::string callstr = make_call("yaffs_closedir", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case LINK:
                {
                    std::string callstr = make_call("yaffs_link", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case SYMLINK:
                {
                    std::string callstr = make_call("yaffs_symlink", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break; 
            case READLINK:
                {
                    std::string callstr = make_call("yaffs_readlink", call, false, i, concolicvars, 1, "rwbuf");
                    src.append( callstr );
                }
                break;
            case UNLINK:
                {
                    std::string callstr = make_call("yaffs_unlink", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case RENAME:
                {
                    std::string callstr = make_call("yaffs_rename", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case CHMOD:
                {
                    std::string callstr = make_call("yaffs_chmod", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case FCHMOD:
                {
                    std::string callstr = make_call("yaffs_fchmod", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case FSYNC:
                {
                    std::string callstr = make_call("yaffs_fsync", call, false, i, concolicvars, -1, "");
                    src.append( callstr );                          
                }
                break;
            case FDATASYNC:
                {
                    std::string callstr = make_call("yaffs_fdatasync", call, false, i, concolicvars, -1, "");
                    src.append( callstr );                           
                }
                break;
            case ACCESS:
                {
                    std::string callstr = make_call("yaffs_access", call, false, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case DUP:
                {
                    std::string callstr = make_call("yaffs_dup", call, true, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case PREAD:
                {
                    std::string callstr = make_call("yaffs_pread", call, false, i, concolicvars, 1, "rwbuf");
                    src.append( callstr );
                }
                break;
            case PWRITE:
                {
                    std::string callstr = make_call("yaffs_pwrite", call, false, i, concolicvars, 1, "rwbuf");
                    src.append( callstr );
                }
                break;
            case UTIME://int yaffs_utime(const YCHAR *path, const struct yaffs_utimbuf *buf);
                {
                    std::string callstr = make_call("yaffs_utime", call, false, i, concolicvars, 1, "&utbuf");
                    src.append( callstr );                    
                }
                break;           
            case FUTIME://int yaffs_futime(int fd, const struct yaffs_utimbuf *buf);
                {
                    std::string callstr = make_call("yaffs_futime", call, false, i, concolicvars, 1, "&utbuf");
                    src.append( callstr );
                }
                break;
            case FLUSH:
                {
                    std::string callstr = make_call("yaffs_flush", call, true, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case SYNC:
                {
                    std::string callstr = make_call("yaffs_sync", call, true, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case TOTALSPACE:
                {
                    std::string callstr = make_call("yaffs_totalspace", call, true, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case INODECOUNT:
                {
                    std::string callstr = make_call("yaffs_inodecount", call, true, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
            case NHANDLES:
                {
                    std::string callstr = make_call("yaffs_n_handles", call, true, i, concolicvars, -1, "");
                    src.append( callstr );
                }
                break;
#ifndef __LESS_API                
            case SETXATTR://int yaffs_setxattr(const char *path, const char *name, const void *data, int size, int flags);
                {
                }
                break;        
            case LSETXATTR://int yaffs_lsetxattr(const char *path, const char *name, const void *data, int size, int flags);
                {
                }
                break;       
            case FSETXATTR://int yaffs_fsetxattr(int fd, const char *name, const void *data, int size, int flags);
                {
                }
                break;       
            case GETXATTR://int yaffs_getxattr(const char *path, const char *name, void *data, int size);
                {
                }
                break;        
            case LGETXATTR://int yaffs_lgetxattr(const char *path, const char *name, void *data, int size);
                {
                }
                break;       
            case FGETXATTR://int yaffs_fgetxattr(int fd, const char *name, void *data, int size);
                {
                }
                break;       
            case REMOVEXATTR://int yaffs_removexattr(const char *path, const char *name);
                {
                }
                break;     
            case LREMOVEXATTR://int yaffs_lremovexattr(const char *path, const char *name);
                {
                }
                break;    
            case FREMOVEXATTR://int yaffs_fremovexattr(int fd, const char *name);
                {
                }
                break;    
            case LISTXATTR://int yaffs_listxattr(const char *path, char *list, int size);
                {
                }
                break;      
            case LLISTXATTR://int yaffs_llistxattr(const char *path, char *list, int size);
                {
                }
                break;      
            case FLISTXATTR://int yaffs_flistxattr(int fd, char *list, int size);
                {
                }
                break; 
#endif                
            default:
                break;
            }
#endif
    }
    src.append("}\n");
    // generate main function
    const char* p3 =    "void main() {\n"
                "    concolictest();\n"
                "    return 0;\n"
                "}\n"
                "\n";
    src.append(p3);
}
