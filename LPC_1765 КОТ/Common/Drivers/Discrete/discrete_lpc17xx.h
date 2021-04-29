#ifndef DISCRETE_H
#define DISCRETE_H

#include <stdint.h>
#include "config.h"
#if(_DISCRETE_)										  


#define MAX_DISCRETE_DATA_OFFSET   (NUMBER_OF_CH_DIS * 8)


// Буфер приема - передачи
typedef struct  tagbuf_st  
{
    uint8_t in;
    uint8_t out;
    uint8_t d_buf[DISCRETE_BUF_SIZE];
}DISCRET_BUF;

typedef struct __attribute__ ((__packed__)) tagDiscreteData
{
    uint8_t value[NUMBER_OF_CH_DIS];
    uint16_t DataPoint;
}DiscreteData;

// Объект
typedef struct tagDISCRETE_INFO
{
    uint8_t DataPoint;
    uint8_t ReadMode;
    DISCRET_BUF buf[NUMBER_OF_CH_DIS];
}DISCRETE_INFO;


#endif // _DISCRETE_INPUT_

#endif

