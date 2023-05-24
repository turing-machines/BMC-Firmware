/**
 * @file webserver.c
 * @author wenyi (wenyi0421@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <sys/vfs.h>    //statfs
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/reboot.h>


#include "inc/goahead.h"
#include "cjson/cJSON.h"
#include "param.h"
#include "bmc.h"
#include "version.h"

#include <stdbool.h>

#include <tpi_rs.h>

#define  SD_PATH "/mnt/sdcard"


static int Go_asp_test_disp( int eid, Webs *wp, int argc, char **argv)
{
    int i = 0;
    printf("-----------argc %d\n", argc);

    for (i = 0; i < argc; i++)
        printf("argv[%d] [%s]\n", i, argv[i]);



    // asp_test_Db();

    websWrite(wp, "%s", argv[0]);
    //websFlush(wp, 0);
    //websDone(wp);
    return 3;
}

static void Go_asp_test_Action(Webs *wp)
{
    char    *name, *address;

    name = websGetVar(wp, "name", NULL);
    address = websGetVar(wp, "address", NULL);
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html><body><h2>name: %s, address: %s</h2></body></html>\n", name, address);
    websFlush(wp, 0);
    websDone(wp);
}



static int app_webS_PrintJsonErr(Webs *wp, int code,char *errstr)
{

    /*--json start--*/
    cJSON* pRoot = cJSON_CreateObject();
    cJSON* pArray = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "response", pArray);
    cJSON* pItem = NULL;


    /*--json body--*/
    pItem = cJSON_CreateObject();
	cJSON_AddNumberToObject(pItem, "code", code);
	cJSON_AddStringToObject(pItem, "result", errstr);
    cJSON_AddItemToArray(pArray, pItem);


    /*--json end--*/
    char* szJSON = cJSON_Print(pRoot);
    // printf("%s\n", szJSON);

    websWrite(wp, szJSON);
    websFlush(wp, 0);
    websDone(wp);

    cJSON_Delete(pRoot);

    return 0;

}

bool check_sdcard_mount_status(void)
{
    char tmp[4096]={0};
    FILE *pp=NULL;
    if (access("/dev/mmcblk0p1", 0) && access("/dev/mmcblk0", 0)) {
        return false;
    }
    pp = popen("mount | grep mmcblk", "r");
    if (!pp) {
        return false;
    }
    fgets(tmp, 4096, pp);
    pclose(pp);
    if (!strstr(tmp, SD_PATH)) {
        return false;
    }
    return true;
}


static int get_sdcard_info(Webs* wp)
{
    unsigned long long total;
    unsigned long long free;
    unsigned long long use;

    struct statfs hfs = {0};
	unsigned long long block_size = 0;
    
    if(check_sdcard_mount_status())
    {
        if(statfs(SD_PATH,&hfs))
        {
            printf("get sd error\n");
            return -1;
        }
    }
	
	block_size = hfs.f_bsize;
	total = (hfs.f_blocks * block_size)>>20;
	free = (hfs.f_bavail * block_size)>>20;
	use = total - free;

    /*--json start--*/
    cJSON* pRoot = cJSON_CreateObject();
    cJSON* pArray = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "response", pArray);
    cJSON* pItem = NULL;


    /*--json body--*/
    pItem = cJSON_CreateObject();
	cJSON_AddNumberToObject(pItem, "total", total);
	cJSON_AddNumberToObject(pItem, "free", free);
    cJSON_AddNumberToObject(pItem, "use", use);
    cJSON_AddItemToArray(pArray, pItem);


    /*--json end--*/
    char* szJSON = cJSON_Print(pRoot);
    // printf("%s\n", szJSON);

    websWrite(wp, szJSON);
    websFlush(wp, 0);
    websDone(wp);

    cJSON_Delete(pRoot);

	return 0;
}


static int get_usbmode(Webs* wp)
{
    /*--json start--*/
    cJSON* pRoot = cJSON_CreateObject();
    cJSON* pArray = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "response", pArray);
    cJSON* pItem = NULL;

    
    env_usb_t* usb = &(env_get_ctx()->usb);

    /*--json body--*/
    pItem = cJSON_CreateObject();
	cJSON_AddNumberToObject(pItem, "mode", usb->mode);  
	cJSON_AddNumberToObject(pItem, "node", usb->node);  
    cJSON_AddItemToArray(pArray, pItem);


    /*--json end--*/
    char* szJSON = cJSON_Print(pRoot);
    // printf("%s\n", szJSON);

    websWrite(wp, szJSON);
    websFlush(wp, 0);
    websDone(wp);

    cJSON_Delete(pRoot);
    return 0;
}


static int set_usbmode(Webs* wp)
{
    char* mode = NULL;
    char* node = NULL;
    mode = websGetVar(wp, "mode", NULL);
    node = websGetVar(wp, "node", NULL);
    // env_usb_t usb;
    if(NULL == node || NULL == mode)
    {
        app_webS_PrintJsonErr(wp,400,"mode or node is null");
        return -1;
    }
    env_usb_t* usb = &(env_get_ctx()->usb);
    usb->mode = atoi(mode);
    usb->node = atoi(node);
    set_env_usb(usb);
    tpi_usb_mode(usb->mode,usb->node);
    return 0;
}

static int get_nodepower(Webs* wp)
{
    /*--json start--*/
    cJSON* pRoot = cJSON_CreateObject();
    cJSON* pArray = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "response", pArray);
    cJSON* pItem = NULL;


    /*--json body--*/
    pItem = cJSON_CreateObject();
	cJSON_AddNumberToObject(pItem, "node1", tpi_get_node_power(0));  
	cJSON_AddNumberToObject(pItem, "node2", tpi_get_node_power(1));  
    cJSON_AddNumberToObject(pItem, "node3", tpi_get_node_power(2));  
	cJSON_AddNumberToObject(pItem, "node4", tpi_get_node_power(3));  
    cJSON_AddItemToArray(pArray, pItem);


    /*--json end--*/
    char* szJSON = cJSON_Print(pRoot);
    printf("json ret:%s\n", szJSON);

    websWrite(wp, szJSON);
    websFlush(wp, 0);
    websDone(wp);

    cJSON_Delete(pRoot);
    return 0;
}

static int get_nodeInfoType(Webs* wp)
{
    /*--json start--*/
    cJSON* pRoot = cJSON_CreateObject();
    cJSON* pArray = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "response", pArray);
    cJSON* pItem = NULL;

    /*--json body--*/
    pItem = cJSON_CreateObject();
	cJSON_AddStringToObject(pItem, "node1", get_nodeType(0));  
	cJSON_AddStringToObject(pItem, "node2", get_nodeType(1));  
    cJSON_AddStringToObject(pItem, "node3", get_nodeType(2));  
	cJSON_AddStringToObject(pItem, "node4", get_nodeType(3));  
    cJSON_AddItemToArray(pArray, pItem);


    /*--json end--*/
    char* szJSON = cJSON_Print(pRoot);
    printf("json ret:%s\n", szJSON);

    websWrite(wp, szJSON);
    websFlush(wp, 0);
    websDone(wp);

    cJSON_Delete(pRoot);
    return 0;
}



int comm_getMacAddr(char* ethname, char *macaddr, size_t len)
{
    int fd;
    char buffer[20];
    struct ifreq ifr;

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        strncpy(ifr.ifr_name, ethname, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
        {
            snprintf(buffer, 20, "%02x:%02x:%02x:%02x:%02x:%02x",
                     (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                     (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                     (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                     (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                     (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                     (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
        }
        else
        {
            close(fd);
            return(-1);
        }
    }
    else
    {
        return(-1);
    }

    if (strlen(buffer) > len - 1)
    {
		close(fd);
        return(-1);
    }
    strncpy(macaddr, buffer, len);

    close(fd);
    return(0);
}



int comm_getIpAddr(char* ethname, char *ipaddr, size_t len)
{
    int fd;
    char buffer[20];
    struct ifreq ifr;
    struct sockaddr_in *addr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        strncpy(ifr.ifr_name, ethname, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
        {
            addr = (struct sockaddr_in *) & (ifr.ifr_addr);
            inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
        }
        else
        {
            close(fd);
            return(-1);
        }
    }
    else
    {
        perror("st_comm_net_card_getIpAddr error :");
        return(-1);
    }

    if (strlen(buffer) > len - 1)
    {
		close(fd);
        return(-1);
    }
    strncpy(ipaddr, buffer, len);
    close(fd);
    return(0);
}




static int get_otherInfo(Webs* wp)
{
    /*--json start--*/
    cJSON* pRoot = cJSON_CreateObject();
    cJSON* pArray = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "response", pArray);
    cJSON* pItem = NULL;

    /*--json body--*/
    char mac[24];
    char ip[24];
    comm_getMacAddr("eth0",mac,sizeof(mac));
    comm_getIpAddr("eth0",ip,sizeof(ip));

    pItem = cJSON_CreateObject();
	cJSON_AddStringToObject(pItem, "version", BMCVERSION);  
	cJSON_AddStringToObject(pItem, "buildtime", BUILDTIME);  
    cJSON_AddStringToObject(pItem, "ip", ip);  
	cJSON_AddStringToObject(pItem, "mac", mac);  
    cJSON_AddItemToArray(pArray, pItem);

    /*--json end--*/
    char* szJSON = cJSON_Print(pRoot);
    // printf("json ret:%s\n", szJSON);

    websWrite(wp, szJSON);
    websFlush(wp, 0);
    websDone(wp);

    cJSON_Delete(pRoot);
    return 0;
}

static int get_uartString(Webs* wp)
{
    char buff[BUFF_MAX_SIZE]={0};
    char *node =NULL;
    int num = 0;
    node = websGetVar(wp, "node", NULL);
    if(NULL!=node)
    {
        num = atoi(node);
    }

    /*--json start--*/
    cJSON* pRoot = cJSON_CreateObject();
    cJSON* pArray = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "response", pArray);
    cJSON* pItem = NULL;

    /*--json body--*/
    get_buff(num,buff,BUFF_MAX_SIZE);
    pItem = cJSON_CreateObject();
	cJSON_AddStringToObject(pItem, "uart", buff);  
    cJSON_AddItemToArray(pArray, pItem);


    /*--json end--*/
    char* szJSON = cJSON_Print(pRoot);
    // printf("json ret:%s\n", szJSON);

    websWrite(wp, szJSON);
    websFlush(wp, 0);
    websDone(wp);

    cJSON_Delete(pRoot);
    return 0;
}



static int set_uartCmd(Webs* wp)
{
    int ret = 0;
    char* cmd = NULL;
    cmd = websGetVar(wp, "cmd", NULL);
    char *node =NULL;
    int num = 0;
    node = websGetVar(wp, "node", NULL);
    if(NULL!=node)
    {
        num = atoi(node);
    }
    if(num<0||num>3)
    {
        return -1;
    }
    if(cmd!=NULL)
    {
        ret = uart_write(num,cmd,strlen(cmd));
        printf("uart write cmd = %s ret = %d\n",cmd,ret);
    }
    
    return 0;
}



static int set_nodepower(Webs* wp)
{
    int i;
    char* var[] = {"node1","node2","node3","node4"};
    char* node1 = NULL;
    char* node = NULL;
    node1 = websGetVar(wp, "node1", NULL);
    node = websGetVar(wp, "node", NULL);
    // env_usb_t usb;
    for(i=0; i<4; ++i)
    {
        node = NULL;
        node = websGetVar(wp, var[i], NULL);
        if(NULL!=node && atoi(node)!=tpi_get_node_power(i))
        {
            tpi_node_power(i,atoi(node));
            printf("set node %d,var = %d\n",i,atoi(node));        
        }
    }
    return 0;
}

static int set_network(Webs* wp)
{
    char* cmd = NULL;
    cmd = websGetVar(wp, "cmd", NULL);
    if(cmd!=NULL)
    {
        if(0==strcmp(cmd,"reset"))
        {
            tpi_rtl_reset();
        }
    }
    
    return 0;
}

static int set_nodeinfo(Webs* wp)
{
    char* en = NULL;
    en = websGetVar(wp, "enable", NULL);
    if(en!=NULL)
    {
        env_nodeInfo_t *info = &env_get_ctx()->nodeen;
        info->enable = atoi(en);
        set_env_nodeInfo(info);
    }
    return 0;
}


char* updateFilePath = "/tmp/update.bin";
static int get_ubi_num(void)
{
    int ret = -1;
    char cmd[128]={0};
    char line[1024]={0};
    snprintf(cmd,sizeof(cmd),"mount");
    FILE* pp = popen(cmd,"r");
    if(pp)
    {
        while (fgets(line, 1024, pp)) {
            if (strstr(line, "ubi0_5") || strstr(line, "ubi0_6")) {
                if(strstr(line, "ubi0_5"))
                    ret = 6;
                if(strstr(line, "ubi0_6"))
                    ret = 5;
                break;
            }
        }
        pclose(pp);
    }
    return ret;
}

static const char* swupdate_file = "/mnt/sdcard/update.bin";
static int swupdate_cmd(void)
{
    char cmd[256] = {0};
    char line[1024] = {0};
    bool update_ret = false;
    bool env_status = false;
    int ver = get_ubi_num();
    if(-1==ver)
    {
        printf("error\n");
        return -1;
    }
    snprintf(cmd,sizeof(cmd),"swupdate -i %s -e stable,upgrade_ubi%d",swupdate_file,ver);
    FILE* pp = popen(cmd,"r");
    if(pp)
    {
        while (fgets(line, 1024, pp)) {
            if (strstr(line, "SWUPDATE successful") ) {
                update_ret = true;
                break;
            }
        }
        pclose(pp);
        pp = NULL;
    }
    // sleep(5);
    if(update_ret)
    {
        int count = 0;
        char ubi[16];
        // char printenv[128];
        // snprintf(printenv,sizeof(printenv),"fw_printenv ");

        // snprintf(ubi,sizeof(ubi),"ubi0_%d",ver);
        //update bootenv
        // snprintf(cmd,sizeof(cmd),"fw_setenv nand_root %s",ubi);
        // snprintf(cmd,sizeof(cmd),"/etc/env.sh");
        // printf(" %s\n",cmd);
        // system(cmd);
        // while(env_status == false)
        // {
        //     system(cmd);
        //     usleep(50*1000);
        //     // snprintf(cmd,sizeof(cmd),"fw_printenv ",);
        //     pp = popen(printenv,"r");
        //     if(pp)
        //     {
        //         while (fgets(line, 1024, pp)) {
        //             if (strstr(line, ubi) ) {
        //                 env_status = true;
        //                 break;
        //             }
        //         }
        //         pclose(pp);
        //     }
        //     sleep(1);
        //     if(count++ > 10)
        //     {
        //         printf("--------------UPDATE faild--------------------\n");
        //         printf("=================system will reboot ====================\n");
        //         sleep(3);
        //         system("reboot");
        //     }
        // }
        sleep(1);
        printf("--------------UPDATE successful--------------------\n");
    }
    else
    {
        printf("--------------UPDATE faild--------------------\n");
    }
    printf("=================system will reboot ====================\n");
    sprintf(cmd,"rm %s",swupdate_file);
    system(cmd);
    sleep(3);
    system("reboot");
    // reboot();
}

static bool isMountSDcard(char* mountPoint)
{
    if(NULL == mountPoint)
        return false;
    int fd = -1;
    char buff[1024];
    if((fd = open("/proc/mounts",O_RDONLY)) < 0)
    {
        printf("open /proc/mounts error\n");
        return false;
    }
    while(read(fd,buff,1024)>0)
    {
        if(strstr(buff,mountPoint))
        {
            close(fd);
            return true;
        }
    }
    close(fd);
    return false;
}



static void uploadFirmware(Webs *wp)
{
    WebsKey         *s;
    WebsUpload      *up;

    char    cmd[128];
   	char    sessionIdBuf[128] = {0};
	int 	sessionIdBufLen = sizeof(sessionIdBuf);


	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteHeader(wp, "Content-Type", "text/plain");

    websWriteEndHeaders(wp);

    if(!isMountSDcard("/mnt/sdcard"))
    {
        websWrite(wp,"{\"response\":[{\"result\":\"err:no sdcard\"}]}");
        websDone(wp);
        return ;
    }
    
    if (scaselessmatch(wp->method, "POST")) {
        for (s = hashFirst(wp->files); s; s = hashNext(wp->files, s)) {
            up = (WebsUpload*)(s->content.value.symbol);
            if (rename(up->filename, updateFilePath) < 0) {
                error("Cannot rename uploaded file: %s to %s, errno %d", up->filename, updateFilePath, errno);
            }
        }
        websWrite(wp, "\r\nVARS:\r\n");
        for (s = hashFirst(wp->vars); s; s = hashNext(wp->vars, s)) {
            websWrite(wp, "%s=%s\r\n", s->name.value.string, s->content.value.string);
        }
    }
    sprintf(cmd,"mv %s %s;sync",updateFilePath,swupdate_file);
    system(cmd);
	websWrite(wp,"{\"response\":[{\"result\":\"ok\"}]}");
    websDone(wp);
	printf("upload Filename=%s\n",up->clientFilename);
    swupdate_cmd();
	
}

static void req_flash_node(Webs *wp)
{
    WebsKey         *s;
    WebsUpload      *up;

    char    *node = NULL;
    char    *node_flashing_file = NULL;
    uint8_t node_id;
    flashing_result res;

    node = websGetVar(wp, "node", NULL);
    if (!node)
    {
        app_webS_PrintJsonErr(wp, 400, "No node specified");
        return;
    }

    node_flashing_file = websGetVar(wp, "file", NULL);
    if (!node_flashing_file)
    {
        app_webS_PrintJsonErr(wp, 400, "No file specified");
        return;
    }

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");

    websWriteEndHeaders(wp);

    if (!isMountSDcard("/mnt/sdcard"))
    {
        websWrite(wp,"{\"response\":[{\"result\":\"err:no sdcard\"}]}");
        websDone(wp);
        return;
    }

    // NOTE: code for uploading files (as in uploadFirmware()) omitted, because can't simply
    // configure upload path without recompiling.

    node_id = atoi(node);

    res = tpi_flash_node(node_id, node_flashing_file);

    if (res == FR_SUCCESS)
        websWrite(wp,"{\"response\":[{\"result\":\"ok\"}]}");
    else
        websWrite(wp,"{\"response\":[{\"result\":\"flashing failure: %d\"}]}", res);

    websDone(wp);
}

static void bmcdemo(Webs *wp)
{
    char    *pOpt   = NULL;
	char    *pType  = NULL;

    char json_result_buff[1024] = {0};

    char* url = NULL;
    url = websGetUrl(wp);
    printf("bmcdemo url=%s\n",url);

    pOpt = websGetVar(wp, "opt", NULL);
    pType = websGetVar(wp, "type", NULL);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");
    websWriteEndHeaders(wp);

    if(NULL==pOpt || NULL == pType)
    {
        app_webS_PrintJsonErr(wp,400,"opt type error");
        return;
    }
    
    if(strcasecmp(pOpt,"get") == 0)
    {
        printf("this get\n");
        if(0==strcasecmp(pType,"sdcard"))
        {
            get_sdcard_info(wp);
        }
        else if(0==strcasecmp(pType,"usb"))
        {
            get_usbmode(wp);
        }
        else if(0==strcasecmp(pType,"power"))
        {
            get_nodepower(wp);
        }
        else if(0==strcasecmp(pType,"nodeinfo"))
        {
            get_nodeInfoType(wp);
        }
        else if(0==strcasecmp(pType,"uart"))
        {
            get_uartString(wp);
        }
        else if(0==strcasecmp(pType,"other"))
        {
            get_otherInfo(wp);
        }
        // strcpy(json_result_buff,"{\"response\":[{\"result\":\"ok\"}]}");
        // websWrite(wp, "%s", json_result_buff);
        // websFlush(wp, 0);
        // websDone(wp);
    }
    else if(0==strcasecmp(pOpt,"set"))
    {
        printf("this set\n");
        if(0==strcasecmp(pType,"firmware"))
        {
            uploadFirmware(wp);
        }
        if(0==strcasecmp(pType,"flash"))
        {
            req_flash_node(wp);
        }
        if(0==strcasecmp(pType,"sdcard"))
        {
            //todo format
            // get_sdcard_info(wp);
            printf("todo format \n");
        }
        else if(0==strcasecmp(pType,"usb"))
        {
            if(set_usbmode(wp))
                return ;
        }
        else if(0==strcasecmp(pType,"power"))
        {
            set_nodepower(wp);
        }
        else if(0==strcasecmp(pType,"network"))
        {
            set_network(wp);
        }
        else if(0==strcasecmp(pType,"nodeinfo"))
        {
            set_nodeinfo(wp);
        }
        else if(0==strcasecmp(pType,"uart"))
        {
            set_uartCmd(wp);
        }

        strcpy(json_result_buff,"{\"response\":[{\"result\":\"ok\"}]}");
        websWrite(wp, "%s", json_result_buff);
        websFlush(wp, 0);
        websDone(wp);
    }
    else
    {
        app_webS_PrintJsonErr(wp,401,"opt error");
    }
}

static go_asp_func_t g_func_reg[] =
{
    {1, "app_WebS_Jst_test", Go_asp_test_disp, NULL},
    {2, "app_WebS_Act_test", NULL, Go_asp_test_Action},
    {2, "bmc", NULL, bmcdemo},
};


int webstart()
{
    printf("start webserver -\n");
    start_goahead("/mnt", "/mnt/var/www", "http://*:80", "/mnt/route.txt", "/mnt/auth.txt", g_func_reg, sizeof(g_func_reg) / sizeof(go_asp_func_t));
    return 0;
}




