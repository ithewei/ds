#ifdef WIN32

#define ATOI64(a) _atoi64(a)

#define APPENDSEPARTOR(str) { if(str[str.length() - 1] != '\\') str += "\\"; }

#define GETCURRPATH(a) \
	char pathbuf[1024]; \
	GetModuleFileName(NULL, pathbuf, 1024); \
	a = pathbuf; \
	int pos = a.find_last_of("\\"); \
	if(pos > 0) \
	{ \
		a.resize(pos); \
		SetCurrentDirectory(a.c_str()); \
	}
	
#define GETCURRPATH2(a, b) \
	char pathbuf[1024]; \
	GetModuleFileName(NULL, pathbuf, 1024); \
	b = a = pathbuf; \
	int pos = (int)a.find_last_of("\\"); \
	if(pos > 0) \
	{ \
		a.resize(pos); \
		SetCurrentDirectory(a.c_str()); \
	}
		
#else

#define ATOI64(a) atoll(a)

#define APPENDSEPARTOR(str) { if(str[str.length() - 1] != '/') str += "/"; }

#define GETCURRPATH(a) \
	a = argv[0]; \
	int pos = a.find_last_of("/"); \
	if(pos > 0) \
		a.resize(pos);

#define GETCURRPATH2(a, b) \
	b = a = argv[0]; \
	int pos = a.find_last_of("/"); \
	if(pos > 0) \
		a.resize(pos);
	
#endif

// GETCFGPATH
#ifdef WIN32

#define GETCFGPATH(a, p, n, b, c) \
    a = p; \
	a += "\\"; \
	if(b) \
	{ \
		a += "..\\"; \
		a += c; \
		a += "\\"; \
	} \
	if(strlen(n) > 0) \
		a += n;

#define GETDIRPATH(a, p, b, c) \
    a = p; \
	a += "\\"; \
	if(b) \
	{ \
		a += "..\\"; \
		a += c; \
	} \
	else \
		a += c;

#else

#define GETCFGPATH(a, p, n, b, c) \
	a = p; \
	a += "/"; \
	if(b) \
	{ \
		a += "../"; \
		a += c; \
		a += "/"; \
	} \
	if(strlen(n) > 0) \
		a += n;

#define GETDIRPATH(a, p, b, c) \
    a = p; \
	a += "/"; \
	if(b) \
	{ \
		a += "../"; \
		a += c; \
	} \
	else \
		a += c;
	
#endif

#define FGETS_r(a, l) fgets(a, l, stdin)
#define FGETS_d(a) a[strlen(a) - 1] = 0;
