#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <linux/reboot.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "comm_gpio.h"
#include "t113_gpio.h"

#include "webserver.h"
#include "param.h"
#include "bmc.h"

#include "version.h"

#include "comm_device_serial.h"

#include <tpi_rs.h>
#define RESET_LED GPIO_PIN_PG(9)
#define SYS_RESET GPIO_PIN_PG(11)
#define POWER_DETECT GPIO_PIN_PG(10)
#define POWER_BOARD GPIO_PIN_PG(15)
char nodeType[4][128] = {"unknown", "unknown", "unknown", "unknown"};

#define NEW_USB 

NODE_CTX_t g_node_ctx[4];


/**
 * @brief 0- unknown
 *         1- raspberry
 *          2- jetson
 *          3-
 */

char *allNodeType[] =
    {
        "unknown",
        "raspberry pi",
        "jetson"};

int allNodeInfo[4] = {0};

char *serail_path[4] = {"/dev/ttyS2", "/dev/ttyS1", "/dev/ttyS4", "/dev/ttyS5"};

char *dev_keys[] = {"raspberry", "ttyTHS"};

int analysis_nodeType(int fd, int num)
{
    if (fd < 0)
        return 0;
    int ret;
    char *tmp = NULL;
    char buff[256];
    memset(buff, 0, sizeof(buff));
    ret = read(fd, buff, sizeof(buff));
    printf("recv num:%d,len:%d,buff=%s\n", num, ret, buff);
    if (strstr(buff, dev_keys[0]))
    {
        allNodeInfo[num] = 1;
        sprintf(nodeType[num], "[raspberry pi]");
        printf("get raspberry dev\n");
    }
    if ((tmp = strstr(buff, dev_keys[1])))
    {
        *tmp = 0;
        allNodeInfo[num] = 2;
        sprintf(nodeType[num], "[jetson]-%s", buff);
        printf("get jetson dev\n");
    }
    return 0;
}


int analysis_nodeType_proc(char *data, int num)
{
    int ret;
    char *tmp = NULL;
    char buff[256];
    memset(buff, 0, sizeof(buff));
    strcpy(buff,data);
    if (strstr(buff, dev_keys[0]))
    {
        allNodeInfo[num] = 1;
        sprintf(nodeType[num], "[raspberry pi]");
        printf("get raspberry dev\n");
    }
    if ((tmp = strstr(buff, dev_keys[1])))
    {
        *tmp = 0;
        allNodeInfo[num] = 2;
        sprintf(nodeType[num], "[jetson]-%s", buff);
        printf("get jetson dev\n");
    }
    return 0;
}


void *sigle_uart_thread(void *arg)
{
    pthread_detach(pthread_self());

    int nodenum = *(int *)arg;
    // printf("arg = %d\n",nodenum);
    printf("start uart thread node num = %d----\n", nodenum);

    fd_set sfd;
    int i;
    int fd = 0;
    bool serial_status = false;
    env_nodeInfo_t *nodeInfo = &env_get_ctx()->nodeen;
    sleep(1);
    while (true)
    {
        // get uart enable
        if (nodeInfo->enable)
        {
            if (!serial_status) // open serial
            {
                if (0 != common_device_UartOpen(&fd, serail_path[nodenum]))
                {
                    fd = -1;
                    printf("open %s faild\n", serail_path[nodenum]);
                    return;
                    // continue;
                }
                common_device_UartSetAttr(fd, 115200, 8, 'n', 1);
                serial_status = true;
            }
            else // check login info
            {
                int ret = 0;
                FD_ZERO(&sfd);

                FD_SET(fd, &sfd);
                struct timeval tt;
                tt.tv_sec = 1;
                tt.tv_usec = 0;
                ret = select(fd + 1, &sfd, NULL, NULL, &tt);
                if (ret > 0)
                {
                    if (FD_ISSET(fd, &sfd))
                    {
                        analysis_nodeType(fd, nodenum);
                    }
                }
            }
        }
        else
        {
            if (serial_status)
            {
                if (fd > 0)
                {
                    common_device_UartClose(fd);
                }
                serial_status = false;
                sprintf(nodeType[nodenum], "unknown");
            }
            usleep(100 * 1000);
        }
    }
    return;
}



int add_buff(NODE_BUFF_CTX_t* bctx,char* data)
{
    int len = 0;
    if(NULL==bctx||NULL==data)
        return -1;
    len = strlen(data);
    pthread_mutex_lock(&bctx->lock);
    if(bctx->offset+len >= BUFF_MAX_SIZE)
    {
        memset(bctx->buff,0,BUFF_MAX_SIZE);
        bctx->offset = 0;
    }
    strcpy(&bctx->buff[bctx->offset],data);
    bctx->offset += len;
    pthread_mutex_unlock(&bctx->lock);
    return 0;
}

int get_buff(int index,char* data,int len)
{
    int ret = 0;
    if(index<0||index>3)
        return -1;
    NODE_BUFF_CTX_t* bctx = &g_node_ctx[index].data;
    if(NULL==bctx||NULL==data||len<10)
        return -1;
    pthread_mutex_lock(&bctx->lock);
    if(bctx->offset<len)
    {
        strcpy(data,bctx->buff);
        memset(bctx->buff,0,bctx->offset);
        ret = bctx->offset;
        bctx->offset = 0;
        
    }
    else
    {
        strncpy(data,bctx->buff,len-1);
        data[len-1] = 0;
        memset(bctx->buff,0,bctx->offset);
        bctx->offset = 0;
        ret = len-1;
    }
    pthread_mutex_unlock(&bctx->lock);
    return ret;
}


int uart_write(int index,char *data,int len)
{
    char* enter = "\r\n";
    int ret = 0;
    if(index<0||index>3)
        return -1;
    if(NULL==data||len<0)
        return -2;    
    int fd = g_node_ctx[index].fd;
    if(fd<0)
    {
        printf("uart_write fd < 0\n");
        return -1;
    }
    ret = write(fd,data,len);
    write(fd,enter,strlen(enter));
    return ret;
}

void *sigle_uart_thread_proc(void *arg)
{
    pthread_detach(pthread_self());

    int nodenum = *(int *)arg;
    // printf("arg = %d\n",nodenum);
    printf("start uart thread node num = %d----\n", nodenum);

    fd_set sfd;
    int i;
    char buff[1024];
    int fd = g_node_ctx[nodenum].fd;
    if(fd<0)
    {
        printf("fd < 0\n");
        return ;
    }
    int ret = 0;
    sleep(1);
    while (true)
    {
        // get uart enable
        FD_ZERO(&sfd);
        FD_SET(fd, &sfd);
        struct timeval tt;
        tt.tv_sec = 1;
        tt.tv_usec = 0;
        ret = select(fd + 1, &sfd, NULL, NULL, &tt);
        if (ret > 0)
        {
            if (FD_ISSET(fd, &sfd))
            {
                memset(buff,0,sizeof(buff));
                ret = read(fd, buff, sizeof(buff));
                add_buff(&g_node_ctx[nodenum].data,buff);
                // printf("recv num:%d,len:%d,buff=%s\n", nodenum, ret, buff);
                analysis_nodeType_proc(buff, nodenum);
            }
        }
    }
    return;
}

char *get_nodeType(int num)
{
    // return allNodeType[allNodeInfo[num]];
    return nodeType[num];
}

void node_uart_init()
{
    int i;
    int fd = -1;
    for (i = 0; i < 4; ++i)
    {
        memset(&g_node_ctx[i], 0, sizeof(NODE_CTX_t));
        fd = -1;
        g_node_ctx[i].index = i;
        strncpy(g_node_ctx[i].path, serail_path[i], sizeof(g_node_ctx[i].path));
        if (0 != common_device_UartOpen(&fd, serail_path[i]))
        {
            fd = -1;
            g_node_ctx[i].fd = fd;
            printf("open %s faild\n", serail_path[i]);
            // return ;
            continue;
        }
        common_device_UartSetAttr(fd, 115200, 8, 'n', 1);
        g_node_ctx[i].fd = fd;
        pthread_mutex_init(&g_node_ctx[i].data.lock, NULL);
    }
}

int main(int argc, char *argv[])
{
    printf("Hello bmc start build time = %s-%s\n", __TIME__, __DATE__);
    printf("bmc version:v%s\n", BMCVERSION);

    tpi_initialize();

    int ret = -1;
    int i;
    pthread_t pid, piduart;
    init_env();

    node_uart_init();


    int chn[4] = {0, 1, 2, 3};
    for (i = 0; i < 4; ++i)
    {
        ret = pthread_create(&piduart, NULL, sigle_uart_thread_proc, &chn[i]);
    }

    printf("pthread end\n");
    webstart();
    return 0;
}
