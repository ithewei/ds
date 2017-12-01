#ifndef DS_GLOBAL_H
#define DS_GLOBAL_H

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

#include <ook/trace>
#include <ook/fourcc.h>

#include <ook/apps/transcoder/service/service.h>
#include <ook/apps/transcoder/service/director/director_service.h>

#include <ook/xmlparser>

#include <ook/separator>
#include <ook/macro.h>
#include <ook/tools/chkpath.h>
#include <ook/tools/strtool>
#include <ook/tools/strconv>

#include <ook/sock/http_client.h>
#include <ook/cycbuf2>
#include <ook/sys/call>

extern "C" {
    #include <ook/codecs/avdef.h>
}

#define LAYOUT_TYPE_ONLY_OUTPUT         0
#define LAYOUT_TYPE_ONLY_MV             0
#define LAYOUT_TYPE_OUTPUT_AND_MV       1

#define OPERATION_TYPE_MOUSE   0
#define OPERATION_TYPE_TOUCH   1

#endif // DS_GLOBAL_H
