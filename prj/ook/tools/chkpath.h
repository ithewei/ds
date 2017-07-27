#ifndef __OOK_CHECKPATH_H__
#define __OOK_CHECKPATH_H__

#ifdef WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ook/separator>
#endif

/*
	if file exist return 0, otherwise return -1
 */
inline int job_check_path(const char * path)
{
#ifdef WIN32
	return _access(path, 0);
#elif !defined(__em8622l__)
	return access(path, F_OK);
#endif
	return -1;
}

/*
	if dir exist return 0, 
	if create successs return 1,
	otherwise return < 0
 */
inline int job_check_dir(const char * ptr, bool bfile, bool bcreate)
{
	if(!ptr || strlen(ptr) == 0)
		return -1;
	std::string path = ptr;
	if(bfile)
	{
	#ifdef WIN32
		const char * p = strrchr(ptr, '\\');
	#else
		const char * p = strrchr(ptr, '/');
	#endif
		if(!p)
			return -2;
		char bak = *p;
		*(char *)p = 0;
		path = ptr;	
		*(char *)p = bak;
	}
	///printf("job_check_path::path=<%s>\n", path.c_str());
#ifdef WIN32
	DWORD dwAttr = GetFileAttributes(path.c_str());
	if((dwAttr == -1) || ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0))
	{
		if(!bcreate)
			return -3;
		if(!CreateDirectory(path.c_str(), NULL))
			return -4;
		return 1;
	}
	return 0;
#else
	separator sept(path.c_str(), "/");
	int size = sept.size();
	if(size == 0)
		return -1;
	int retv = 0;
	path = sept[0];
	for(int i = 1; i < size; i++)
	{
		path += "/";
		path += sept[i];
		///printf("path=<%s>\n", path.c_str());
		DIR * pdir = opendir(path.c_str());
		if(pdir)
		{
			closedir(pdir);
			continue;
		}
		if(!bcreate)
			return -3;
		// modify @ 2017/05/09
		///int r = mkdir(path.c_str(), 0x777);
		int r = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		///printf("mkdir=%d\n", r);
		if(r != 0)
		{
			printf("job_check_dir::mkdir<%s> error\n", path.c_str());
			return -4;
		}
		retv = 1;
	}
	return retv;
#endif
}

inline int remove_dir(const char * path)
{
#ifdef WIN32
	SHFILEOPSTRUCT FileOp;
	FileOp.fFlags            = FOF_NOCONFIRMATION;
	FileOp.hNameMappings     = NULL;
	FileOp.hwnd              = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom             = path;
	FileOp.pTo               = NULL;
	FileOp.wFunc             = FO_DELETE;
	return SHFileOperation(&FileOp);
#else
	std::string cmd = "rm ";
	cmd += path;
	cmd += " -rf";
	return system(cmd.c_str());
#endif
}

#endif
