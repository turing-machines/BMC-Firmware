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
#include "bmctest.h"

#include "comm_device_serial.h"

#define RTL_RESET GPIO_PIN_PG(13)
#define SYS_LED GPIO_PIN_PG(8)
#define RESET_LED GPIO_PIN_PG(9)
#define SYS_RESET GPIO_PIN_PG(11)
#define POWER_DETECT GPIO_PIN_PG(10)
#define POWER_BOARD GPIO_PIN_PG(15)

#define POWER_EN GPIO_PIN_PD(3)
#define MODE1_EN GPIO_PIN_PD(7)
#define MODE2_EN GPIO_PIN_PD(6)
#define MODE3_EN GPIO_PIN_PD(5)
#define MODE4_EN GPIO_PIN_PD(4)

#define PORT1_EN GPIO_PIN_PD(11)
#define PORT2_EN GPIO_PIN_PD(10)
#define PORT3_EN GPIO_PIN_PD(9)
#define PORT4_EN GPIO_PIN_PD(8)

#define PORT1_RST GPIO_PIN_PD(0)
#define PORT2_RST GPIO_PIN_PD(20)
#define PORT3_RST GPIO_PIN_PD(21)
#define PORT4_RST GPIO_PIN_PD(22)

#define PORT1_USB_VBUS GPIO_PIN_PD(19)
#define PORT2_USB_VBUS GPIO_PIN_PD(18)
#define PORT3_USB_VBUS GPIO_PIN_PD(17)
#define PORT4_USB_VBUS GPIO_PIN_PD(16)

#define PORT1_RPIBOOT GPIO_PIN_PD(15)
#define PORT2_RPIBOOT GPIO_PIN_PD(14)
#define PORT3_RPIBOOT GPIO_PIN_PD(12)
#define PORT4_RPIBOOT GPIO_PIN_PD(13)

#define USB_SEL1 GPIO_PIN_PG(1)
#define USB_SEL2 GPIO_PIN_PG(0)
#define USB_OE1 GPIO_PIN_PG(2)
#define USB_OE2 GPIO_PIN_PG(3)

#define USB_SWITCH GPIO_PIN_PG(5)
#define USB_PWEN GPIO_PIN_PG(4)

char nodeType[4][128] = {"unknown", "unknown", "unknown", "unknown"};

#define NEW_USB 


NODE_CTX_t g_node_ctx[4];

void gpio_out_init(unsigned int num, bool val)
{
    common_device_gpioOpen(num, GPIO_OUT);
    common_device_gpioWirte(num, val);
}

void usb_boot_init(void)
{
    gpio_out_init(PORT1_USB_VBUS, GPIO_H);
    gpio_out_init(PORT2_USB_VBUS, GPIO_H);
    gpio_out_init(PORT3_USB_VBUS, GPIO_H);
    gpio_out_init(PORT4_USB_VBUS, GPIO_H);
    gpio_out_init(PORT1_RPIBOOT, GPIO_H);
    gpio_out_init(PORT2_RPIBOOT, GPIO_H);
    gpio_out_init(PORT3_RPIBOOT, GPIO_H);
    gpio_out_init(PORT4_RPIBOOT, GPIO_H);
}

void usb0_default_mode(void)
{
    gpio_out_init(USB_SEL1, GPIO_L);
    gpio_out_init(USB_SEL2, GPIO_H);
    gpio_out_init(USB_OE1, GPIO_L);
    gpio_out_init(USB_OE2, GPIO_H);
    gpio_out_init(USB_SWITCH, GPIO_L);
    // gpio_out_init(USB_PWEN, GPIO_H);
    // // gpio_out_init(USB_PWEN, GPIO_L);
#ifdef NEW_USB
    gpio_out_init(USB_PWEN, GPIO_L);
#else
    gpio_out_init(USB_PWEN, GPIO_H);
#endif 
}

int g_usb_node[4][4] = {
    {0, 0, 1, 1},
    {1, 0, 1, 1},
    {1, 1, 0, 0},
    {1, 1, 1, 0}};
void usb_connet_node(int num)
{
    gpio_out_init(USB_SEL1, g_usb_node[num][0]);
    gpio_out_init(USB_OE1, g_usb_node[num][1]);
    gpio_out_init(USB_SEL2, g_usb_node[num][2]);
    gpio_out_init(USB_OE2, g_usb_node[num][3]);
}

int g_usb_mode[4] = {PORT1_RPIBOOT, PORT2_RPIBOOT, PORT3_RPIBOOT, PORT4_RPIBOOT};
void usb_connet_mode(int num, int val)
{
    gpio_out_init(g_usb_mode[num], val);
}

int ctrl_usbconnet(int mode, int node)
{
    if (node < 0 || node > 3)
    {
        printf("node %d is error\n", node);
        return -1;
    }
    usb_connet_node(node);
    if (0 == mode) // host
    {
        // gpio_out_init(USB_PWEN, GPIO_H);
        // // gpio_out_init(USB_PWEN, GPIO_L);
#ifdef NEW_USB
    gpio_out_init(USB_PWEN, GPIO_L);
#else
    gpio_out_init(USB_PWEN, GPIO_H);
#endif 
        gpio_out_init(PORT1_RPIBOOT, GPIO_H);
        gpio_out_init(PORT2_RPIBOOT, GPIO_H);
        gpio_out_init(PORT3_RPIBOOT, GPIO_H);
        gpio_out_init(PORT4_RPIBOOT, GPIO_H);
    }
    else if (1 == mode) // slave
    {
#ifdef NEW_USB
    gpio_out_init(USB_PWEN, GPIO_H);
#else
    gpio_out_init(USB_PWEN, GPIO_L);
#endif         
        
        // 
        usb_connet_mode(node, GPIO_L);
    }
    return 0;
}

void bmc_init(void)
{
    printf("bmc init start \n");
    common_device_gpioOpen(SYS_RESET, GPIO_OUT);
    common_device_gpioWirte(SYS_RESET, GPIO_L);
    common_device_gpioOpen(SYS_RESET, GPIO_IN);

    // common_device_gpioOpen(POWER_DETECT, GPIO_OUT);
    // common_device_gpioWirte(POWER_DETECT, GPIO_L);
    common_device_gpioOpen(POWER_DETECT, GPIO_IN);

    common_device_gpioOpen(POWER_BOARD, GPIO_IN);

    /* init led */
    common_device_gpioOpen(SYS_LED, GPIO_OUT);
    common_device_gpioWirte(SYS_LED, GPIO_H);
    common_device_gpioOpen(RESET_LED, GPIO_OUT);
    common_device_gpioWirte(RESET_LED, GPIO_L);

    usb_boot_init();
    usb0_default_mode();
    gpio_out_init(RTL_RESET, GPIO_H);

    gpio_out_init(POWER_EN, GPIO_L);
    gpio_out_init(MODE1_EN, GPIO_L);
    gpio_out_init(MODE2_EN, GPIO_L);
    gpio_out_init(MODE3_EN, GPIO_L);
    gpio_out_init(MODE4_EN, GPIO_L);

    gpio_out_init(PORT1_EN, GPIO_H);
    gpio_out_init(PORT2_EN, GPIO_H);
    gpio_out_init(PORT3_EN, GPIO_H);
    gpio_out_init(PORT4_EN, GPIO_H);

    gpio_out_init(PORT1_RST, GPIO_L);
    gpio_out_init(PORT2_RST, GPIO_L);
    gpio_out_init(PORT3_RST, GPIO_L);
    // gpio_out_init(PORT4_RST,GPIO_L);
    printf("bmc init end \n");
}
int g_node_power_map[4][3] = {
    {MODE1_EN, PORT1_EN, PORT1_RST},
    {MODE2_EN, PORT2_EN, PORT2_RST},
    {MODE3_EN, PORT3_EN, PORT3_RST},
    {MODE4_EN, PORT4_EN, PORT4_RST}};

int get_atx_power()
{
    if (common_device_gpioRead(POWER_EN) == GPIO_H)
        return 1;
    else
        return 0;
}

int set_atx_power()
{
    common_device_gpioWirte(POWER_EN, GPIO_H);
    usleep(100 * 1000);
    common_device_gpioWirte(SYS_LED, GPIO_L);
}

void RTL_Reset(void)
{
    common_device_gpioWirte(RTL_RESET, GPIO_L);
    sleep(1);
    common_device_gpioWirte(RTL_RESET, GPIO_H);
}

int node_power(int num, int status)
{
    if (0 == status)
    {
        common_device_gpioWirte(g_node_power_map[num][1], GPIO_H);
        usleep(100 * 1000);
        common_device_gpioWirte(g_node_power_map[num][0], GPIO_L);
        usleep(100 * 1000);
        if (118 != g_node_power_map[num][2])
            common_device_gpioWirte(g_node_power_map[num][2], GPIO_L);
    }
    else // on
    {
        common_device_gpioWirte(g_node_power_map[num][0], GPIO_H);
        usleep(100 * 1000);
        common_device_gpioWirte(g_node_power_map[num][1], GPIO_L);
        usleep(100 * 1000);
        if (118 != g_node_power_map[num][2])
            common_device_gpioWirte(g_node_power_map[num][2], GPIO_H);

        if (get_atx_power() == 0)
        {
            set_atx_power();
        }
    }
}

int get_node_power(int num)
{
    if (common_device_gpioRead(g_node_power_map[num][0]) == GPIO_H &&
        common_device_gpioRead(g_node_power_map[num][1]) == GPIO_L)
        return 1;
    else
        return 0;
}

void poweron(void)
{
    common_device_gpioWirte(POWER_EN, GPIO_H);
    usleep(100 * 1000);
    common_device_gpioWirte(SYS_LED, GPIO_L);

    node_power(0, 1);
    sleep(1);
    node_power(1, 1);
    sleep(1);
    node_power(2, 1);
    sleep(1);
    node_power(3, 1);
    sleep(1);

}

void poweroff(void)
{

    sleep(1);
    node_power(3, 0);
    sleep(1);
    node_power(2, 0);
    sleep(1);
    node_power(1, 0);
    sleep(1);
    node_power(0, 0);

    common_device_gpioWirte(POWER_EN, GPIO_L);
    usleep(100 * 1000);
    common_device_gpioWirte(SYS_LED, GPIO_H);
}

bool get_key_status(unsigned int key, bool status, int t)
{
    bool ret = false;
    int count = 0;
    time_t st = time(NULL);
    time_t cur = 0;
    while (status == common_device_gpioRead(key))
    {
        usleep(500 * 1000);
        cur = time(NULL);
        if (abs(cur - st) >= t)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

bool get_power_status()
{
    bool ret = false;
    int count = 0;
    // time_t st = time(NULL);
    // time_t cur = 0;
    while (GPIO_H == common_device_gpioRead(POWER_DETECT) || GPIO_L == common_device_gpioRead(POWER_BOARD))
    {
        usleep(100 * 1000);
        count++;

        if (count >= 5)
        {
            ret = true;
            break;
        }
    }
    return ret;
}


/**
 * @brief 按键控制线程
 * TODO：led灯控制
 * @param arg
 * @return void*
 */
void *key_thread(void *arg)
{
    // todo
    int i;
    pthread_detach(pthread_self());
    printf("start key thread----\n");
    int power_flag = false;

    bool reset_led_status = GPIO_H;

    sleep(1);
    while (true)
    {
        // if (true == get_key_status(POWER_DETECT, GPIO_H, 2))
        if (true == get_power_status())
        {
            if (false == power_flag)
            {
                power_flag = true;
                printf("poweron----------->\n");
                poweron();
            }
            else
            {
                power_flag = false;
                printf("poweroff-----------<\n");
                for (i = 0; i < 4; ++i)
                {
                    sprintf(nodeType[i], "unknown");
                }
                poweroff();
            }
            /* wait key up */
            while (true == get_power_status())
            {
                usleep(500 * 1000);
            }

        }
        if (true == get_key_status(SYS_RESET, GPIO_H, 2))
        {
            printf("-----will reboot -------------\n");
            power_flag = false;
            poweroff();
            int led_count = 10;
            while (led_count--)
            {
                reset_led_status = !reset_led_status;
                common_device_gpioWirte(RESET_LED, reset_led_status);
                usleep(300 * 1000);
            }
            reboot();
            system("reboot");
        }
        sleep(1);
    }
    return;
}

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
    printf("Hello bmct start build time = %s-%s------>\n", __TIME__, __DATE__);
    int ret = -1;
    int i;
    pthread_t pid, piduart;
    init_env();
    bmc_init();
    printf("pthread start \n");
    ret = pthread_create(&pid, NULL, key_thread, NULL);
    if (ret)
    {
        printf("pthread_create error\n");
        return -1;
    }

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