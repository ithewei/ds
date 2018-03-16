#ifndef HRINGBUFFER_H
#define HRINGBUFFER_H

#include "qtheaders.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define UNUSED      0
#define USED        1

#define POLICY_DISCARD  1
#define POLICY_COVER    2

/**
 * @note: use in multi thread, please lock for read and write.
 *
 *
**/

struct frame_info{
    unsigned int ts;
    char* data;
    unsigned int len;

    frame_info(){
        ts = 0;
        data = NULL;
        len = 0;
    }
};

// flag=1byte + ts=4bytes + data=_size(bytes)
#define PRE_INFO_LEN    5

class HRingBuffer
{
public:
    HRingBuffer(int size, int num = 10){
        _size = size;
        _num = num;

        size_t total = (PRE_INFO_LEN+size)*num;
        _ptr = (char*)malloc(total);
        memset(_ptr, 0, total);

        read_index = 0;
        write_index = 0;
        readable_num = 0;

        policy = POLICY_DISCARD;
    }

    ~HRingBuffer(){
        if (_ptr){
            free(_ptr);
            _ptr = NULL;
        }
    }

    // we don't memcpy in read and write for flexibility, for exsample, copy YV12 to YUV420P
    // so use in multi thread, please lock for read and write and whole memcpy
    frame_info read(){
        frame_info fi;
        char* ret = get(read_index);

        if (*ret == USED){
            read_index = (read_index + 1)%_num;
            *ret = UNUSED;
            readable_num--;

            fi.ts = *(unsigned int *)(ret+1);
            fi.data = ret+PRE_INFO_LEN;
            fi.len = _size;
        }

        return fi;
    }

    frame_info write(){
        frame_info fi;
        char* ret = get(write_index);
        if (*ret == USED){
            if (policy == POLICY_DISCARD){
                return fi;
            }
            // edge out read_index
            read_index = (read_index+1)%_num;
            //qDebug("edge out read_index");
        }

        write_index = (write_index+1)%_num;
        *ret = USED;
        readable_num++;
        if (readable_num > _num)
            readable_num = _num;

        fi.data = ret+PRE_INFO_LEN;
        fi.len = _size;
        return fi;
    }

    int readable(){
        return readable_num;
    }

    int size(){
        return _size;
    }

    int num(){
        return _num;
    }

private:
    char* get(int index){
        if (index < 0 || index >= _num)
            return NULL;
        return _ptr + index*(PRE_INFO_LEN+_size);
    }

private:
    int _size;
    int _num;

    char* _ptr;

    int read_index;
    int write_index;
    int readable_num;

    int policy;
};

#endif // HRINGBUFFER_H
