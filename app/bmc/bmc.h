/**
 * @file bmc.h
 * @author wenyi (wenyi0421@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef __BMC_H__
#define __BMC_H__

#include <pthread.h>


#define BUFF_MAX_SIZE       (64*1024)

typedef struct _NODE_BUFF_CTX_t
{
    pthread_mutex_t lock;
    int offset;
    char buff[BUFF_MAX_SIZE];
}NODE_BUFF_CTX_t;


typedef struct _NODE_CTX_t
{
    int index;
    int fd;
    char path[64];
    NODE_BUFF_CTX_t data;
}NODE_CTX_t;

char* get_nodeType(int num);

int get_buff(int index,char* data,int len);

int uart_write(int index,char *data,int len);

#endif // !__BMC_H__
