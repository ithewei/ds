#ifndef DS_GLOBAL_H
#define DS_GLOBAL_H

#ifdef WIN32
#include <process.h>
#endif

#include <QtCore/qglobal.h>

#if defined(DS_LIBRARY)
#  define DSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DSSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <string>
#include <deque>
#include <map>
#include <vector>

#include <ook/apps/transcoder/service/service.h>
#include <ook/apps/transcoder/service/director/director_service.h>
#include <ook/fourcc.h>
#include <ook/fourccdef.h>

#include <ook/xmlparser>
#include <ook/separator>
#include <ook/macro.h>
#include <ook/tools/strtool>
#include <ook/sys/call>

extern "C" {
    #include <ook/codecs/avdef.h>
}

#endif // DS_GLOBAL_H
