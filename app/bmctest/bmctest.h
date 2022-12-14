/**
 * @file bmctest.h
 * @author wenyi (wenyi0421@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef __BMCTEST_H__
#define __BMCTEST_H__

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



int ctrl_usbconnet(int mode,int node);
int get_node_power(int num);
int node_power(int num,int status);
void RTL_Reset(void);
char* get_nodeType(int num);

int uart_write(int index,char *data,int len);

#endif // !__BMCTEST_H__
