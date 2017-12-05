#ifndef DS_H
#define DS_H

#include "ds_global.h"

enum MediaType{
    MediaTypeUnknown = 0,
    MediaTypeAudio =   1,
    MediaTypeVideo =   2,
};

enum SrcType{
    SRC_TYPE_NUKNOWN   = 0,
    SRC_TYPE_FILE      = 1,
    SRC_TYPE_LMIC      = 2,
};

extern "C" {
    DSSHARED_EXPORT int libversion(void);
    DSSHARED_EXPORT int libchar(void);
    DSSHARED_EXPORT int libtrace(int);

    DSSHARED_EXPORT int libinit(const char* xml, void* task, void** ctx);
    DSSHARED_EXPORT int libstop(void* ctx);
    DSSHARED_EXPORT int liboper(int media_type, int data_type, int opt, void* param, void * ctx);
}

#endif // DS_H
