#ifndef __COMMON_DEVICE_UART_H__
#define __COMMON_DEVICE_UART_H__

// #include "commonLib.h"

char common_device_UartOpen(int *fd,char *dev);
char common_device_UartClose(int fd);
int common_device_UartSetAttr(int fd,int nSpeed, int nBits, char nEvent, int nStop);

#endif



