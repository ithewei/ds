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

class HRingBuffer
{
public:
    HRingBuffer(int size, int num = 10){
        _size = size;
        _num = num;

        size_t total = (1+size)*num;
        _ptr = (char*)malloc(total);
        memset(_ptr, 0, total);

        read_index = 0;
        write_index = 0;

        policy = POLICY_DISCARD;
    }

    ~HRingBuffer(){
        if (_ptr){
            free(_ptr);
            _ptr = NULL;
        }
    }

    int size() {return _size;}

    char* read(){
        char* ret = get(read_index);

        if (*ret == USED){
            read_index = (read_index + 1)%_num;
            *ret = UNUSED;
            return ret+1;
        }

        return NULL;
    }

    char* write(){
        char* ret = get(write_index);
        if (*ret == USED){
            if (policy == POLICY_DISCARD){
                return NULL;
            }
            // edge out read_index
            read_index = (read_index+1)%_num;
            //qDebug("edge out read_index");
        }

        write_index = (write_index+1)%_num;
        *ret = USED;

        return ret+1;
    }

    char* get(int index){
        if (index < 0 || index >= _num)
            return NULL;
        return _ptr + index*(1+_size);
    }

private:
    int _size;
    int _num;

    char* _ptr;

    int read_index;
    int write_index;

    int policy;
};

#endif // HRINGBUFFER_H
