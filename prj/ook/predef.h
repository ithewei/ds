#ifndef __OOK_PREDEF_H__
#define __OOK_PREDEF_H__

#if defined(WIN32)
#pragma warning(disable: 4786)
#endif

#if !defined(WINCE) && (_MSC_VER > 1200)
#pragma warning(disable: 4996) // strtok
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32

	#define PATHSEPARATOR 				"\\"
	
	#if !defined(WINCE) && (_MSC_VER > 1200) // for VC6.0 _MSC_VER == 1200

	#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x501
	#endif
	
	#define __strcpy(a, b, c) 			strcpy_s(a, b, c)
	#define __strncpy(a, b, c, d) 		strncpy_s(a, b, c, d)
	#define __strcat(a, b, c) 			strcat_s(a, b, c)
	#define __strtok(a, b, c) 			strtok_s(a, b, c)
	#define __memcpy(a, b, c, d)		memcpy_s(a, b, c, d)
	#define __memmove(a, b, c, d)		memmove_s(a, b, c, d)
	#define __snprintf(a, b, fmt, ...) 	_snprintf_s(a, b, b, fmt, __VA_ARGS__)

	#define __gets(a, b)				gets_s(a)
	#define __ftime 					_ftime_s
		
	#else

	#define __strcpy(a, b, c) 			strcpy(a, c)
	#define __strncpy(a, b, c, d) 		strncpy(a, c, d)
	#define __strcat(a, b, c) 			strcat(a, c)
	#define __strtok(a, b, c) 			strtok(a, c)
	#define __memcpy(a, b, c, d)		memcpy(a, c, d)
	#define __memmove(a, b, c, d)		memmove(a, c, d)
	#define __snprintf 					_snprintf
	
	#define __gets(a, b)				gets(a)
	#define __ftime 					_ftime
		
	#endif // end of #if _MSC_VER > 1200

#else

	#define PATHSEPARATOR 				"/"

	#define __strcpy(a, b, c) 			strcpy(a, c)
	#define __strncpy(a, b, c, d) 		strncpy(a, c, d)
	#define __strcat(a, b, c) 			strcat(a, c)
	#define __strtok(a, b, c) 			strtok(a, c)
	#define __memcpy(a, b, c, d)		memcpy(a, c, d)
	#define __memmove(a, b, c, d)		memmove(a, c, d)
	#define __snprintf 					snprintf

	#define __gets(a, b)				gets(a)	
	#define __ftime 					_ftime

#endif // end of #ifdef WIN32

#endif
