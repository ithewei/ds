#ifdef WIN32

#define FCLOSE_(h) CloseHandle(h);

#define FREAD__(h, b, s, r)\
	{\
		r = -1;\
		DWORD readsize = 0;\
		if(ReadFile((HANDLE)h, b, s, &readsize, NULL))\
			r = (int)readsize;\
	}

#define FSEEK__(h, pos, r)\
	{\
		LARGE_INTEGER li;\
		li.QuadPart = pos;\
		li.LowPart  = SetFilePointer((HANDLE)h, li.LowPart, &li.HighPart, FILE_BEGIN);\
   		if(li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)\
      		r = -1;\
   		else\
			r = 0;\
	}

#define FSEEK_END__(h, pos, r)\
	{\
		LARGE_INTEGER li;\
		li.QuadPart = pos;\
		li.LowPart  = SetFilePointer((HANDLE)h, li.LowPart, &li.HighPart, FILE_END);\
   		if(li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)\
      		r = -1;\
   		else\
			r = 0;\
	}

#define FTELL__(h, s)\
	{\
		DWORD sizeh = 0;\
		DWORD sizel = GetFileSize((HANDLE)h, &sizeh);\
		if(sizel != INVALID_FILE_SIZE)\
			s  = ((uint64_t)sizeh << 32) + sizel;\
	}

#define FREAD_(h, b, s, r) r = (int)fread(b, 1, s, (FILE *)h);

#define FTELL_(h, r) r = ftell((FILE *)h);
#define FSEEK_(h, pos, r) r = fseek((FILE *)h, (size_t)pos, SEEK_SET);
#define FSEEK_END_(h, pos, r) r = fseek((FILE *)h, (size_t)pos, SEEK_END);

#define GETFILETIME(f, a, t) { \
	WIN32_FIND_DATA fd; \
	HANDLE hd = FindFirstFile(f, &fd); \
	if(hd != INVALID_HANDLE_VALUE) { \
		switch(a) { \
		case 0x01: \
			t = (fd.ftLastWriteTime.dwHighDateTime << 24)  + fd.ftLastWriteTime.dwLowDateTime; \
			break; \
		case 0x02: \
			t = (fd.ftLastAccessTime.dwHighDateTime << 24) + fd.ftLastAccessTime.dwLowDateTime; \
			break; \
		default: \
			t = (fd.ftCreationTime.dwHighDateTime << 24)   + fd.ftCreationTime.dwLowDateTime; \
			break; \
		} \
		FindClose(hd); \
	} \
}

#define GETFILETIMEANDSIZE(f, a, t, s) { \
	WIN32_FIND_DATA fd; \
	HANDLE hd = FindFirstFile(f, &fd); \
	if(hd != INVALID_HANDLE_VALUE) { \
		switch(a) { \
		case 0x01: \
			t = (fd.ftLastWriteTime.dwHighDateTime << 24)  + fd.ftLastWriteTime.dwLowDateTime; \
			break; \
		case 0x02: \
			t = (fd.ftLastAccessTime.dwHighDateTime << 24) + fd.ftLastAccessTime.dwLowDateTime; \
			break; \
		default: \
			t = (fd.ftCreationTime.dwHighDateTime << 24)   + fd.ftCreationTime.dwLowDateTime; \
			break; \
		} \
		s = (fd.nFileSizeHigh << 24) + fd.nFileSizeLow; \
		FindClose(hd); \
	} \
}

#else

#define FCLOSE_(h) fclose((FILE *)h);
#define FREAD_(h, b, s, r) r = (int)fread(b, 1, s, (FILE *)h);

#ifdef __USE_FILE_OFFSET64

///#warning "#define __USE_FILE_OFFSET64 in filemacro.h"

#define FTELL_(h, r) r = ftello((FILE *)h);
#define FSEEK_(h, pos, r) r = fseeko((FILE *)h, (off_t)pos, SEEK_SET);
#define FSEEK_END_(h, pos, r) r = fseeko((FILE *)h, (off_t)pos, SEEK_END);

#define STRU_STAT struct stat64
#define FILE_STAT(a, b) stat64(a, b)

#else

#define FTELL_(h, r) r = ftell((FILE *)h);
#define FSEEK_(h, pos, r) r = fseek((FILE *)h, pos, SEEK_SET);
#define FSEEK_END_(h, pos, r) r = fseek((FILE *)h, pos, SEEK_END);

#define STRU_STAT struct stat
#define FILE_STAT(a, b) stat(a, b)

#endif

#define GETFILETIME(f, a, t) { \
	STRU_STAT st; \
	if(FILE_STAT(f, &st) == 0) \
	{ \
		switch(a) \
		{ \
		case 0x01: \
			t = st.st_mtime; \
			break; \
		case 0x02: \
			t = st.st_atime; \
			break; \
		default: \
			t = st.st_ctime; \
			break; \
		} \
	} \
}

#define GETFILETIMEANDSIZE(f, a, t, s) { \
	STRU_STAT st; \
	if(FILE_STAT(f, &st) == 0) \
	{ \
		switch(a) \
		{ \
		case 0x01: \
			t = st.st_mtime; \
			break; \
		case 0x02: \
			t = st.st_atime; \
			break; \
		default: \
			t = st.st_ctime; \
			break; \
		} \
		s = st.st_size; \
	} \
}
	
#endif

#define GETFILENAME(f, fn) \
{ \
	int pos = f.find_last_of("\\"); \
	if(pos < 1) \
		pos = f.find_last_of("/"); \
	if(pos > 0) fn = f.c_str() + pos + 1; \
	else fn = f.c_str(); \
}

#define GETFILETYPE(f, ft) \
{ \
	int pos = f.find_last_of("."); \
	if(pos > 0) ft = f.c_str() + pos + 1; \
}

#define GETFILEPATH(f, pn) \
{ \
	int pos = f.find_last_of("\\"); \
	if(pos < 1) \
		pos = f.find_last_of("/"); \
	if(pos > 0) pn = f.substr(0, pos + 1); \
	else pn = ""; \
}

#define GETFILESUBPATH(f, pn, idx) \
{ \
	std::string strf = f; \
	if(idx < 0) \
	{ \
		int i, j = -idx; \
		for(i = 0; i < j; i++) \
		{ \
			int pos = 0;\
			pos = strf.find_last_of("\\");\
			if(pos < 1)\
				pos = strf.find_last_of("/");\
			if(pos < 0)\
				return;\
			strf.resize(pos);\
			pn = strf;\
		} \
	} \
}

