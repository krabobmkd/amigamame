// Vic Krb Ferry, Feb 2024
// from amiga

#ifndef __stdargs
#define __stdargs
#endif


#include <proto/dos.h>

// from mame:
extern "C" {
    #include "osdepend.h"
    #include "fileio.h"
    #include "unzip.h"
    #include "hash.h"
}
//from this project
//#include "file.h"
// from project from this repos, static zlib:
#include "zlib.h"

#include <string>
// for memcpy
#include <string.h>
#include <cstdlib>
#include <stdio.h>
#include <cstdarg>
#include <vector>
#include <sstream>

#include <stdio.h>

#include "amiga106_config.h"

extern struct DosLibrary    *DOSBase;

using namespace std;

// api tracing tools
const vector<string> filetypenames={
	"RAW",
	"ROM",
	"IMAGE",
	"IMAGE_DIFF",
	"SAMPLE",
	"ARTWORK",
	"NVRAM",
	"HIGHSCORE",
	"HIGHSCORE_DB",
	"CONFIG",
	"INPUTLOG",
	"STATE",
	"MEMCARD",
	"SCREENSHOT",
	"MOVIE",
	"HISTORY",
	"CHEAT",
	"LANGUAGE",
	"CTRLR",
	"INI",
	"COMMENT",
	"DEBUGLOG",
	"HASH",
	"end"
};

// internal amiga file type
enum
{
    AFT_ROM, // ROM and sample
    AFT_SAMPLE,
    AFT_USER, // writable, configs, ...
};

// shortlist this enum:
int getAmigaFileType(int mameFileType)
{
    if(mameFileType == FILETYPE_ROM )
    {
        return AFT_ROM;
    }
    if(mameFileType == FILETYPE_SAMPLE )
    {
        return AFT_SAMPLE;
    }
    return AFT_USER;
}


const char *osdfiletypeName(int itype) {
    if(itype<0 || itype>= (int)filetypenames.size()) return "errortype";
    return filetypenames[itype].c_str();
}



stringstream &glog() {
    static stringstream _glog;
    return _glog;
}

struct _osd_file {
    _osd_file() :_hdl(NULL) {}
    ~_osd_file() { close(); }
    inline int open(const char *pFilepath, const char *access) {
        if(_hdl) close();
        _hdl = fopen(pFilepath,access);
        return (int)(_hdl != NULL);
    }
    inline void close() {
        if(_hdl) fclose(_hdl);
        _hdl = NULL;
    }
    inline int seek(int offset, int whence) {

        if(!_hdl) return 0;
        return fseek(_hdl,offset,whence);
    }
    inline int tell() {
        if(!_hdl) return 0;
        return ftell(_hdl);
    }
    inline int read(void *ptr,int size )
    {
        if(!_hdl) return 0;
        return fread(ptr,1,size,_hdl);
    }
    inline int write(const void *ptr,size_t size )
    {
        if(!_hdl) return 0;
        return fwrite(ptr,1,size,_hdl);
    }
    /* Return 1 if we're at the end of file */
    inline int eof()
    {
        if(!_hdl) return 1;
        return feof(_hdl);
    }
protected:
    FILE *_hdl;
};



int get_path_info(const char *fullpath)
{
    BPTR hdl = Open(fullpath, MODE_OLDFILE);
    if(!hdl)
    {
        return PATH_NOT_FOUND;
    }
    struct FileInfoBlock *fib = (struct FileInfoBlock *)AllocDosObject(DOS_FIB, NULL);
    if(!fib){
        Close(hdl);
        return PATH_NOT_FOUND;
    }
    ExamineFH(hdl,fib);
    int res = ((fib->fib_DirEntryType<0)?PATH_IS_FILE:PATH_IS_DIRECTORY);
    FreeDosObject(DOS_FIB,fib);
    Close(hdl);
    return res;
}

// would create directory recusively, return if ok.
int assumeDirectory(const char *fullpath)
{
    int dirtype = get_path_info(fullpath);
    if(dirtype == PATH_IS_DIRECTORY) return PATH_IS_DIRECTORY;

    // if not...
    std::string sfullpath(fullpath);
    size_t i = sfullpath.rfind("/");
    if(i != string::npos)
    {
        string sparent =sfullpath.substr(0,i);
        int iparent = assumeDirectory(sparent.c_str());
        if(iparent != PATH_IS_DIRECTORY) return PATH_NOT_FOUND;
    }
    BPTR l = CreateDir(fullpath);
    if(l) {
        UnLock(l);
        return PATH_IS_DIRECTORY;
    }
    return PATH_NOT_FOUND;
}

/* Return the number of paths for a given type */
int osd_get_path_count(int pathtype)
{

    switch( getAmigaFileType(pathtype))
    {
        case AFT_ROM: return 1;
        case AFT_USER: return 1;
        default: return 0;
    }
}

void composeFilePath(int pathtype, int pathindex, const char *filename, std::string &p)
{
    MameConfig &conf = getMainConfig();

    switch( getAmigaFileType(pathtype))
    {
        case AFT_ROM: p = conf.getRomsDir(); break;
        case AFT_USER:
        {
            // where configs are written should be only one dir, no search.
            p = conf.getUserDir();
            assumeDirectory(p.c_str()); // would makedir if not done.
        } break;
        default: p.clear(); break;
    }
    if(p.length()>0 && p.back() != ':' && p.back() != '/') p+= '/';
    p += filename;

}

osd_file *osd_fopen(int pathtype, int pathindex, const char *filename, const char *mode, osd_file_error *error)
{
    if(error) *error = FILEERR_FAILURE;
    string spath;
    composeFilePath(pathtype,pathindex,filename,spath);

// printf("osd_fopen: mode:%s file:%s\n",mode,spath.c_str());

    _osd_file *posd = new _osd_file();
    if(!posd) return NULL;
    if(! posd->open(spath.c_str(),mode))
    {
       //  printf("fail\n");
        delete posd;
        return NULL;
    }
    if(error) *error = FILEERR_SUCCESS;

    return posd;
}

// return bytes read.

UINT32 osd_fread(_osd_file *file, void *buffer, UINT32 length)
{
    if(!file) return 0;
#ifdef PRINTOSDFILESYSTEMCALLS
    //printf("osd_fread: l:%d %s\n",length,file->cname());
#endif
    return file->read(buffer,length);
}

/* Seek within a file */
int osd_fseek(_osd_file *file, INT64 offset, int whence)
{
    if(!file) return -1;
    #ifdef PRINTOSDFILESYSTEMCALLS
        //printf("osd_fseek: ofs:%d t:%d %s\n",offset,whence,file->cname());
    #endif
    return ((_osd_file*)file)->seek(offset,whence);
}

/* Close an open file */
void osd_fclose(_osd_file *file)
{
    if(!file) return;
    delete file; // destructor does the job.
}

/* Return current file position */
UINT64 osd_ftell(_osd_file *file)
{
    if(!file) return 0;
#ifdef PRINTOSDFILESYSTEMCALLS
     //printf("osd_ftell:%s\n",file->cname());
#endif

    return file->tell();
}

/* Write bytes to a file */
UINT32 osd_fwrite(osd_file *file, const void *buffer, UINT32 length)
{
 if(!file) return 0;
   return file->write(buffer,length);
}

// 1 if end of file (or not opened?)
int osd_feof(osd_file *file)
{
     if(!file) return 1;
   return file->eof();
}

/* These values are returned by osd_get_path_info */
/*
enum
{
	PATH_NOT_FOUND,
	PATH_IS_FILE,
	PATH_IS_DIRECTORY
};*/
/* Get information on the existence of a file */
int osd_get_path_info(int pathtype, int pathindex, const char *filename)
{
    string spath;
    composeFilePath(pathtype,pathindex,filename,spath);

    return get_path_info(spath.c_str());
}

/* Create a directory if it doesn't already exist */
// VF: no need actually.
//int osd_create_directory(int pathtype, int pathindex, const char *dirname)
//{
//    int
//    return 0;
//}

