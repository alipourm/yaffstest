#ifndef __YAFFS_TESTER_H

#define __YAFFS_TESTER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <string>

#define __LESS_API

#define STARTUP         0 
#define MOUNT           1
#define MOUNT2          2 
#define UNMOUNT         3
#define UNMOUNT2        4
#define REMOUNT         5
#define OPEN            6
#define CLOSE           7
#define MKDIR           8 
#define RMDIR           9
#define LSEEK           10
#define TRUNCATE        11
#define FTRUNCATE       12
#define STAT            13
#define FSTAT           14
#define LSTAT           15
#define READ            16
#define WRITE           17
#define FREESPACE       18
#define OPENDIR         19
#define READDIR         20
#define REWINDDIR       21
#define CLOSEDIR        22
#define LINK            23
#define SYMLINK         24 
#define READLINK        25
#define UNLINK          26
#define RENAME          27
#define CHMOD           28
#define FCHMOD          29
#define FSYNC           30
#define FDATASYNC       31
#define ACCESS          32
#define DUP             33
#define PREAD           34
#define PWRITE          35
#define UTIME           36
#define FUTIME          37
#define FLUSH           38
#define SYNC            39
#define TOTALSPACE      40
#define INODECOUNT      41
#define NHANDLES        42

#ifndef __LESS_API
    #define SETXATTR        35
    #define LSETXATTR       36
    #define FSETXATTR       37
    #define GETXATTR        38
    #define LGETXATTR       39
    #define FGETXATTR       40
    #define REMOVEXATTR     41
    #define LREMOVEXATTR    42
    #define FREMOVEXATTR    43
    #define LISTXATTR       44
    #define LLISTXATTR      45
    #define FLISTXATTR      46
    #define MAX_API         47
#else
    #define MAX_API 43
#endif

/////////////////////////////////////////////////////
class vtype {
public:
    enum etype{
        eString,
        eInt,
        eUnknown
    };
    enum esubtype{
        ePath,
        eMode,
        eFlag,
        eFd,
        eDd,
        eMaxSize,
        eGeneral,
		eIgnore
    };
    vtype() {
        mtype = eUnknown;
    }
    vtype(const std::string& str, esubtype t) {
        set(str, t);
    }
    vtype(int val, esubtype t)  {
        set(val, t);
    }

    
    void set(const std::string& str, esubtype t) {
        sval = str;
        mtype = eString;
        msubtype = t;
    }
    void set(int val, esubtype t) {
        ival = val;
        mtype = eInt;
        msubtype = t;
    }
    std::string get_str() const {
        return sval;
    }
    int get_int() const {
        return ival;
    }
    etype type() const {
        return mtype;
    }
    esubtype subtype() const {
        return msubtype;
    }
private:
    etype       mtype;
    esubtype    msubtype;
    std::string sval;
    int         ival;
};

class yaffs_api_call {
public:
    yaffs_api_call(int id ) {
        api_id = id;
    }
    void reset() {
        api_id = -1;
        parameters.clear();
    }
    std::string tostring() {
        std::string str;
        stringconverter(true, str);
        return str;
    }
    void fromstring(std::string& str) {
        reset();
        stringconverter(false, str);
    }
    void stringconverter(bool to, std::string& str) {        
        char sbuf[1024];
        if( !to ) {
            strcpy(sbuf, str.c_str());
            char* p = sbuf;
            char* pch = strtok(p, ",");
            int api = atoi(pch);
            api_id = api;
        }

        switch( api_id ) {
            case STARTUP:
            {
                if( to) {
                    sprintf(sbuf, "%d\n", api_id);                    
                }
                else {                    
                }
            }
            break;
            case MOUNT:
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s\n", api_id, parameters[0].get_str().c_str() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    parameters.push_back( vtype(std::string(path), vtype::ePath ) );
                }
            }
            break;
			case MOUNT2:
				{
					if( to ) {
						sprintf(sbuf, "%d,%s,%d\n", api_id, parameters[0].get_str().c_str(), parameters[1].get_int() );
					}
					else {
						char* path = strtok(NULL, ",");
						char* sreadonly = strtok(NULL,",");
						int readonly = atoi(sreadonly);
						parameters.push_back( vtype(std::string(path), vtype::ePath ) );
						parameters.push_back( vtype(readonly, vtype::eGeneral) );
					}
				}
				break;
            case UNMOUNT:
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s\n", api_id, parameters[0].get_str().c_str() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    parameters.push_back( vtype(std::string(path), vtype::ePath ) );
                }
            }
            break;
			case UNMOUNT2:
			{
				if( to ) {
					sprintf(sbuf, "%d,%s,%d\n", api_id, parameters[0].get_str().c_str(), parameters[1].get_int() );
				}
				else {
					char* path = strtok(NULL, ",");
					char* sforce = strtok(NULL, ",");
					int force = atoi(sforce);
					parameters.push_back( vtype(std::string(path), vtype::ePath ) );
					parameters.push_back( vtype(force, vtype::eGeneral) );
				}
			}
			break;
			case REMOUNT:
			{
				if( to ) {
					sprintf(sbuf, "%d,%s,%d,%d\n", api_id, parameters[0].get_str().c_str(), parameters[1].get_int(), parameters[2].get_int() );
				}
				else {
					char* path = strtok(NULL, ",");
					char* sforce = strtok(NULL, ",");
					int force = atoi(sforce);
					char* sreadonly = strtok(NULL,",");
					int readonly = atoi(sreadonly);
					parameters.push_back( vtype(std::string(path), vtype::ePath ) );
					parameters.push_back( vtype(force, vtype::eGeneral ) );
					parameters.push_back( vtype(readonly, vtype::eGeneral ) );
				}
			}
			break;
            case OPEN://int yaffs_open(const YCHAR *path, int oflag, int mode) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s,%d,%d,fd%d\n", 
                        api_id, parameters[0].get_str().c_str(), parameters[1].get_int(), parameters[2].get_int(), parameters[3].get_int() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    char* flag = strtok(NULL, ",");
                    char* mode = strtok(NULL, ",");
                    char* fd   = strtok(NULL, ",");
                    int iflag = atoi(flag);
                    int imode = atoi(mode);
                    int idxfd = atoi( fd+2 );
                    parameters.push_back( vtype(path, vtype::ePath) );
                    parameters.push_back( vtype(iflag, vtype::eFlag) );
                    parameters.push_back( vtype(imode, vtype::eMode));
                    parameters.push_back( vtype(idxfd, vtype::eFd));
                }
            }                
            break;
        case CLOSE://int yaffs_close(int fd) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,fd%d\n", api_id, parameters[0].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    parameters.push_back( vtype(idxfd, vtype::eFd) );
                }                
            }
            break;
        case MKDIR:
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s,%d\n", api_id, parameters[0].get_str().c_str(), parameters[1].get_int() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    char* mode = strtok(NULL, ",");
                    int imode = atoi(mode);
                    parameters.push_back( vtype(std::string(path), vtype::ePath) );
                    parameters.push_back( vtype(imode, vtype::eMode) );
                }                
            }
            break;
        case RMDIR://int yaffs_rmdir(const YCHAR *path) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s\n", api_id, parameters[0].get_str().c_str() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    parameters.push_back( vtype(std::string(path), vtype::ePath ) );
                }
            }
            break;
        case LSEEK://off_t yaffs_lseek(int fd, off_t offset, int whence) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,fd%d,%d,%d\n", api_id, parameters[0].get_int(), parameters[1].get_int(),parameters[2].get_int() );
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    char* offset = strtok(NULL, ",");
                    int ioffset = atoi(offset);
                    char* start = strtok(NULL, ",");
                    int whera = atoi(start);

                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                    parameters.push_back( vtype( ioffset, vtype::eGeneral ) );
                    parameters.push_back( vtype( whera, vtype::eGeneral ) );
                }
            }
            break;
        case TRUNCATE://int yaffs_truncate(const YCHAR *path, off_t new_size);
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s,%d\n",api_id, parameters[0].get_str().c_str(), parameters[1].get_int() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    char* offset = strtok(NULL, ",");
                    int ioffset = atoi(offset);
                    parameters.push_back( vtype( std::string(path), vtype::ePath ) );
                    parameters.push_back( vtype( ioffset, vtype::eGeneral ) );
                }
            }
            break;
        case FTRUNCATE://int yaffs_truncate(int fd, off_t new_size);
            {   
                if( to ) {
                    sprintf(sbuf, "%d,fd%d,%d\n", api_id, parameters[0].get_int(), parameters[1].get_int() );
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    char* offset = strtok(NULL, ",");
                    int ioffset = atoi(offset);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                    parameters.push_back( vtype( ioffset, vtype::eGeneral ) );
                }
            }
            break;
        case STAT://int yaffs_stat(const YCHAR *path, struct yaffs_stat *buf) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s\n", api_id, parameters[0].get_str().c_str() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
                }
            }
            break;
        case FSTAT://int yaffs_fstat(int fd, struct yaffs_stat *buf) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,fd%d\n", api_id, parameters[0].get_int() );
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                }
            }
            break;
        case LSTAT://int yaffs_lstat(const YCHAR *path, struct yaffs_stat *buf) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s\n", api_id, parameters[0].get_str().c_str() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
                }
            }
            break;
        case READ://int yaffs_read(int fd, void *buf, unsigned int nbyte) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,fd%d,%d\n", api_id, parameters[0].get_int(), parameters[1].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    char* nbyte = strtok(NULL, ",");
                    int inbyte = atoi(nbyte);
                    parameters.push_back( vtype(idxfd, vtype::eFd) );
                    parameters.push_back( vtype(inbyte, vtype::eGeneral) );
                }
            }
            break;
        case WRITE://int yaffs_write(int fd, const void *buf, unsigned int nbyte) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,fd%d,%d\n", api_id, parameters[0].get_int(), parameters[1].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    char* nbyte = strtok(NULL, ",");
                    int inbyte = atoi(nbyte);
                    parameters.push_back( vtype(idxfd, vtype::eFd) );
                    parameters.push_back( vtype(inbyte, vtype::eGeneral) );
                }
            }
            break;
        case FREESPACE:
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s\n",api_id, parameters[0].get_str().c_str());   
                }
                else {
                    char* path = strtok(NULL, ",");
                    parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
                }
            }
            break;
        case OPENDIR://yaffs_DIR *yaffs_opendir(const YCHAR *dirname) ;
            {
                if( to ) {
                    sprintf(sbuf, "%d,%s,dd%d\n", OPENDIR, parameters[0].get_str().c_str(), parameters[1].get_int() );
                }
                else {
                    char* path = strtok(NULL, ",");
                    char* dir = strtok(NULL, ",");
                    int idxdir = atoi(dir+2);
                    parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
                    parameters.push_back( vtype( idxdir, vtype::eDd ) );
                }
            }
            break;
        case READDIR://struct yaffs_dirent *yaffs_readdir(yaffs_DIR *dirp) ;
            {
                if( to  ) {
                    sprintf(sbuf, "%d,dd%d\n", READDIR, parameters[0].get_int() );
                }
                else {
                    char* dir = strtok(NULL, ",");
                    int idxdir = atoi(dir+2);
                    parameters.push_back( vtype( idxdir, vtype::eDd) );
                }
            }
            break;
        case REWINDDIR://void yaffs_rewinddir(yaffs_DIR *dirp) ;
            {
                if( to  ) {
                    sprintf(sbuf, "%d,dd%d\n", REWINDDIR, parameters[0].get_int());
                }
                else {
                    char* dir = strtok(NULL, ",");
                    int idxdir = atoi(dir+2);
                    parameters.push_back( vtype( idxdir, vtype::eDd) );

                }
            }
            break;
        case CLOSEDIR://int yaffs_closedir(yaffs_DIR *dirp) ;
            {
                if( to  ) {
                    sprintf(sbuf, "%d,dd%d\n",CLOSEDIR, parameters[0].get_int());
                }
                else {
                    char* dir = strtok(NULL, ",");
                    int idxdir = atoi(dir+2);
                    parameters.push_back( vtype( idxdir, vtype::eDd) );
                }
            }
            break;
        case LINK://int yaffs_link(const YCHAR *oldpath, const YCHAR *newpath);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s,%s\n", LINK, parameters[0].get_str().c_str(), parameters[1].get_str().c_str());
                }
                else {
                    char* oldpath = strtok(NULL, ",");
                    char* newpath = strtok(NULL, ",");
                    parameters.push_back( vtype( std::string(oldpath), vtype::ePath ) );
                    parameters.push_back( vtype( std::string(newpath), vtype::ePath ) );
                }
            }   
            break;
        case SYMLINK://int yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s,%s\n", SYMLINK, parameters[0].get_str().c_str(), parameters[1].get_str().c_str());
                }
                else {
                    char* oldpath = strtok(NULL, ",");
                    char* newpath = strtok(NULL, ",");
                    parameters.push_back( vtype( std::string(oldpath), vtype::ePath ) );
                    parameters.push_back( vtype( std::string(newpath), vtype::ePath ) );
                }
            }
            break;
        case READLINK://int yaffs_readlink(const YCHAR *path, YCHAR *buf, int bufsiz);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s,%d\n", READLINK, parameters[0].get_str().c_str(), parameters[1].get_int());
                }
                else {
                    char* oldpath = strtok(NULL,",");
                    char* size = strtok(NULL, ",");
                    int isize = atoi(size);
                    parameters.push_back( vtype( std::string(oldpath), vtype::ePath ) );
                    parameters.push_back( vtype( isize, vtype::eMaxSize ) );
                }
            }
            break;
        case UNLINK://int yaffs_unlink(const YCHAR *path) ;
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s\n", UNLINK, parameters[0].get_str().c_str());
                }
                else {
                    char* oldpath = strtok(NULL,",");
                    parameters.push_back( vtype( std::string(oldpath), vtype::ePath ) );
                }
            }
            break;
        case RENAME://int yaffs_rename(const YCHAR *oldPath, const YCHAR *newPath) ;
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s,%s\n", RENAME,parameters[0].get_str().c_str(), parameters[1].get_str().c_str());
                }
                else {
                    char* oldpath = strtok(NULL, ",");
                    char* newpath = strtok(NULL, ",");
                    parameters.push_back( vtype( std::string(oldpath), vtype::ePath ) );
                    parameters.push_back( vtype( std::string(newpath), vtype::ePath ) );
                }
            }
            break;
        case CHMOD://int yaffs_chmod(const YCHAR *path, mode_t mode);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s,%d\n",CHMOD,parameters[0].get_str().c_str(), parameters[1].get_int());
                }
                else {
                    char* path = strtok(NULL, ",");
                    char* mode = strtok(NULL, ",");
                    int imode = atoi(mode);
                    parameters.push_back( vtype( std::string(path), vtype::ePath ) );
                    parameters.push_back( vtype( imode, vtype::eMode) );
                }
            }
            break;
        case FCHMOD://int yaffs_fchmod(int fd, mode_t mode);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,fd%d,%d\n", FCHMOD, parameters[0].get_int(), parameters[1].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    char* mode = strtok(NULL, ",");
                    int imode = atoi(mode);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                    parameters.push_back( vtype( imode, vtype::eMode ) );
                }         
            }
            break;

        case FSYNC://int yaffs_fsync(int fd) ;
            {
                if( to  ) {
                    sprintf(sbuf, "%d,fd%d\n", FSYNC, parameters[0].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                }
            }
            break;
        case FDATASYNC://int yaffs_fdatasync(int fd) ;
            {
                if( to  ) {
                    sprintf(sbuf, "%d,fd%d\n", FDATASYNC, parameters[0].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                }
            }
            break;
        case ACCESS://int yaffs_access(const YCHAR *path, int amode);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s,%d\n",ACCESS,parameters[0].get_str().c_str(), parameters[1].get_int());
                }
                else {
                    char* path = strtok(NULL, ",");
                    char* mode = strtok(NULL, ",");
                    int imode = atoi(mode);
                    parameters.push_back( vtype( std::string(path) , vtype::ePath) );
                    parameters.push_back( vtype( imode, vtype::eMode) );
                }
            }
            break;
        case DUP://int yaffs_dup(int fd);
            {
                if( to  ) { 
                    sprintf(sbuf, "%d,fd%d,fd%d\n", DUP, parameters[0].get_int(), parameters[1].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
					char* fd1 = strtok(NULL, ",");
					int idxnewfd = atoi(fd1+2);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
					parameters.push_back( vtype( idxnewfd, vtype::eFd ) );
                }
            }
            break;             
        case PREAD://int yaffs_pread(int fd, void *buf, unsigned int nbyte, unsigned int offset);
            {          
                if( to  ) {
                    sprintf(sbuf, "%d,fd%d,%d,%d\n",PREAD,parameters[0].get_int(),parameters[1].get_int(), parameters[2].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    char* nbyte = strtok(NULL, ",");
                    int inbyte = atoi(nbyte);
                    char* noffset = strtok(NULL, ",");
                    int inoffset = atoi(noffset);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                    parameters.push_back( vtype( inbyte, vtype::eGeneral ) );
                    parameters.push_back( vtype( inoffset, vtype::eGeneral ) );
                }
            }
            break;           
        case PWRITE://int yaffs_pwrite(int fd, const void *buf, unsigned int nbyte, unsigned int offset);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,fd%d,%d,%d\n",PWRITE,parameters[0].get_int(),parameters[1].get_int(), parameters[2].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    char* nbyte = strtok(NULL, ",");
                    int inbyte = atoi(nbyte);
                    char* noffset = strtok(NULL, ",");
                    int inoffset = atoi(noffset);
                    parameters.push_back( vtype( idxfd, vtype::eFd ) );
                    parameters.push_back( vtype( inbyte, vtype::eGeneral ) );
                    parameters.push_back( vtype( inoffset, vtype::eGeneral ) );
                }                
            }
            break;          
        case UTIME://int yaffs_utime(const YCHAR *path, const struct yaffs_utimbuf *buf);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,%s\n", UTIME, parameters[0].get_str().c_str());
                }
                else {
                    char* path = strtok(NULL, ",");
                    parameters.push_back( vtype( std::string(path), vtype::ePath ) );
                }
            }
            break;           
        case FUTIME://int yaffs_futime(int fd, const struct yaffs_utimbuf *buf);
            {
                if( to  ) {
                    sprintf(sbuf, "%d,fd%d\n",FUTIME, parameters[0].get_int());
                }
                else {
                    char* fd = strtok(NULL, ",");
                    int idxfd = atoi(fd+2);
                    parameters.push_back( vtype( idxfd , vtype::eFd));
                }
            }
            break;
		case FLUSH:
			{
				if( to ) {
					sprintf(sbuf, "%d,%d\n",api_id, parameters[0].get_int());   
				}
				else {
					char* sfd = strtok(NULL, ",");
                    int fd = atoi(sfd);
					parameters.push_back( vtype( fd, vtype::eFd ) );
				}
			}
			break;
		case SYNC:
			{
				if( to ) {
					sprintf(sbuf, "%d,%s\n",api_id, parameters[0].get_str().c_str());   
				}
				else {
					char* path = strtok(NULL, ",");
					parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
				}
			}
			break;
		case TOTALSPACE:
			{
				if( to ) {
					sprintf(sbuf, "%d,%s\n",api_id, parameters[0].get_str().c_str());   
				}
				else {
					char* path = strtok(NULL, ",");
					parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
				}
			}
			break;
		case INODECOUNT:
			{
				if( to ) {
					sprintf(sbuf, "%d,%s\n",api_id, parameters[0].get_str().c_str());   
				}
				else {
					char* path = strtok(NULL, ",");
					parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
				}
			}
			break;
		case NHANDLES:
			{
				if( to ) {
					sprintf(sbuf, "%d,%s\n",api_id, parameters[0].get_str().c_str());   
				}
				else {
					char* path = strtok(NULL, ",");
					parameters.push_back( vtype( std::string( path ), vtype::ePath ) );
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
        str = sbuf;
    }

public:
    int api_id;
    std::vector<vtype> parameters;
};

class yaffs_test_case {
public:
    std::vector<yaffs_api_call> api_calls;
public:
    void clear() {
        api_calls.clear();
    }
    void add(const yaffs_api_call call) {
        api_calls.push_back( call );
    }    
public:
    std::string tostring() {
        std::string res;
        for( int i = 0; i < api_calls.size(); i++) {
            std::string tmp = api_calls[i].tostring();
            res.append( tmp );
        }
        return res;
    }
    void fromstrings(std::vector<std::string> strs) {
        clear();
        for( int i = 0; i < strs.size(); i++) {
            yaffs_api_call call(-1);
            call.reset();
            call.fromstring( strs[i] );
            add( call );
        }
    }
};

#endif
