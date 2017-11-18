#ifndef HRINGBUFFER_H
#define HRINGBUFFER_H

#include "qtheaders.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define CAN_WRITE   0
#define CAN_READ    1

#define DISCARD_WHEN_NO_CAN_WRITE   0

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

        long long total = (1+size)*num; // 1 for flag : CAN_WRITE or CAN_READ
        _ptr = (char*)malloc(total);
        memset(_ptr, 0, total); // init CAN_WRITE

        read_index = 0;
        write_index = 0;
    }

    ~HRingBuffer(){
        if (_ptr){
            free(_ptr);
            _ptr = NULL;
        }
    }

    char* read(){
        char* ret = get(read_index);

        if (*ret == CAN_READ){
            read_index = (read_index + 1)%_num;
            *ret = CAN_WRITE;
            return ret+1;
        }

        return NULL;
    }

    char* write(){
        char* ret = get(write_index);
        if (*ret == CAN_READ){
            if (DISCARD_WHEN_NO_CAN_WRITE){
                return NULL;
            }
            // edge out read_index
            read_index = (read_index+1)%_num;
            //qDebug("edge out read_index");
        }

        write_index = (write_index+1)%_num;
        *ret = CAN_READ;

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
};

#endif // HRINGBUFFER_H
