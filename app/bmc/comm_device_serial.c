/********************************************************************************
* File                :
* Intro               :
* Author              : 
* Version             : v1.0
* Notes               :
* Change History      :
Date              Version        Changed By          Changes
* --------------------------------------------------------------------------------
2018-12-20        v0.1                             create

* Copyright(c)
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "comm_device_serial.h"

/*************************************************************************
*                        Function implementation                         *
**************************************************************************/
/**************************************************************
**Describe: 打开串口节结
**Input: int *fd:打开的设备节点的文件描述符 ； 
         char *dev：设备节点的名称 
**Ouput:
**Return: 0:success  -1 :Fail
*************************************************************************/
char common_device_UartOpen(int *fd,char *dev)
{
    *fd = open(dev, O_RDWR|O_NOCTTY|O_NDELAY);   

    if((*fd) < 0)
    {
    	printf("\r\n common_device_UaritOpen Error! Dev=%s \r\n",dev);
	    return -1;
    }
	else
	{
		return 0;
	}
}

/**************************************************************
**Describe: 关闭串口节结
**Input: int fd: 要关闭的设备节点的文件描述符 ； 
**Ouput:
**Return: 0:success  -1 :Fail
**************************************************************/
char common_device_UartClose(int fd)
{
    int ret=0;  /*0:表示成功； -1 ：表示失败*/
	ret = close(fd);

	if(ret == -1)
	{
      printf("\r\n common_device_UaritClose Error! fd=%d \r\n",fd);
	}
	return  ret;
}

/**************************************************************
**Describe: 设置串口属性
**Input:  int fd：文件描述符    int nSpeed：波特率  int nBits：数据位
          char nEvent:奇偶校验   int nStop：停止位          
**Ouput:
**Return:   0->Sucess    -1 ->Fail
**************************************************************/
int common_device_UartSetAttr(int fd,int nSpeed, int nBits, char nEvent, int nStop) 
{
	struct termios newtio,oldtio;   
	
	if(tcgetattr( fd,&oldtio)!=0) /*保存现有串口参数设置*/	
	{	
	 printf("tcgetattr(fd,&oldtio) -> %d\n",tcgetattr( fd,&oldtio));	
	return -1;	
	}   
	memset( &newtio, 0,sizeof( newtio ) );	
	
	newtio.c_cflag  |=  CLOCAL | CREAD;	 
	newtio.c_cflag &= ~CSIZE; 
	
	/*设置停止位*/   
	switch( nBits )	
	{ 
	case 5:	
	newtio.c_cflag |= CS5;	  /*Every char is 5BIT */
	break;	
	case 6:	
	newtio.c_cflag |= CS6;	  /*Every char is 6BIT */
	break;
	case 7:	
	newtio.c_cflag |= CS7;	  /*Every char is 7BIT */
	break;	
	case 8:	
	newtio.c_cflag |= CS8;	  /*Every char is 8BIT */
	break;	
	}  
	
	/*设置奇偶校验位*/   
	switch( nEvent )	 
	{   
	case 'o':  
	case 'O': //奇数	 
	newtio.c_cflag |= PARENB;   
	newtio.c_cflag |= PARODD;   
	newtio.c_iflag |= (INPCK | ISTRIP);	 
	break;	
	case 'e':  
	case 'E': //偶数	 
	newtio.c_iflag |= (INPCK | ISTRIP);	 
	newtio.c_cflag |= PARENB;   
	newtio.c_cflag &= ~PARODD;	
	break;  
	case 'n':  
	case 'N':  //无奇偶校验位   
	newtio.c_cflag &= ~PARENB;	
	break;  
	default:	
	break;  
	}  
	
	/*设置波特率*/   
	switch( nSpeed )	
	{   
	case 2400:   
	cfsetispeed(&newtio, B2400);   
	cfsetospeed(&newtio, B2400);   
	break;	
	case 4800:   
	cfsetispeed(&newtio, B4800);   
	cfsetospeed(&newtio, B4800);   
	break;	
	case 9600:   
	cfsetispeed(&newtio, B9600);   
	cfsetospeed(&newtio, B9600);   
	break;	
	case 19200:   
	cfsetispeed(&newtio, B19200);   
	cfsetospeed(&newtio, B19200);   
	break;	
	case 38400:   
	cfsetispeed(&newtio, B38400);   
	cfsetospeed(&newtio, B38400);   
	break;	
	case 57600:   
	cfsetispeed(&newtio, B57600);   
	cfsetospeed(&newtio, B57600);   
	break;	
	case 115200:	 
	cfsetispeed(&newtio, B115200);	
	cfsetospeed(&newtio, B115200);	
	break;	
	case 460800:	 
	cfsetispeed(&newtio, B460800);	
	cfsetospeed(&newtio, B460800);	
	break;	
	default:	 
	cfsetispeed(&newtio, B9600);   
	cfsetospeed(&newtio, B9600);   
	break;   
	}   
	
	/*设置停止位*/   
	if( nStop == 1 )	 
	newtio.c_cflag &=  ~CSTOPB;	 
	else if ( nStop == 2 )   
	newtio.c_cflag |=  CSTOPB;	
	
	/*设置等待时间和最小接收字符*/   
	newtio.c_cc[VTIME]  = 0;	 
	newtio.c_cc[VMIN] = 0; 
	
	/*处理未接收字符*/   
	tcflush(fd,TCIFLUSH);   
	/*激活新配置*/   
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)   
	{   
	  perror("com set error");   
	return -1;	
	}   
	 // printf("set done!\n");   
	return 0;   
}








