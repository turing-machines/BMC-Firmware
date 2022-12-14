/**
 * @file env.c
 * @author wenyi (wenyi0421@163.com)
 * @brief
 * @version 0.1
 * @date 2022-09-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "cjson/cJSON.h"
#include "param.h"

/********************/
#define CONFIG_PATH "/mnt/config"

#define USB_FILE_PATH "usb.json"
#define NODEINFOEN_FILE_PATH "nodeinfoen.json"
/**********************/

static env_ctx_t g_env_ctx;
env_ctx_t* env_get_ctx(void)
{
    return &g_env_ctx;
}

void init_env(void)
{
    memset(&g_env_ctx,0,sizeof(g_env_ctx));
    get_env_usb(&g_env_ctx.usb);
    get_env_nodeInfo(&g_env_ctx.nodeen);
} 

#define json_get_int(obj, key, val)                                           \
    do                                                                        \
    {                                                                         \
        if (NULL != obj)                                                      \
        {                                                                     \
            cJSON *tmp = cJSON_GetObjectItem(obj, key);                       \
            if (NULL != tmp)                                                  \
            {                                                                 \
                val = tmp->valueint;                                          \
            }                                                                 \
            else                                                              \
            {                                                                 \
                printf("json_get_int key:%s is NULL\n", key ? key : "error"); \
            }                                                                 \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            printf("json_get_int obj is NULL,key:%s\n", key ? key : "error"); \
        }                                                                     \
    } while (0)

#define json_get_str(obj, key, val, len)                                      \
    do                                                                        \
    {                                                                         \
        if (NULL != obj)                                                      \
        {                                                                     \
            cJSON *tmp = cJSON_GetObjectItem(obj, key);                       \
            if (NULL != tmp && NULL != tmp->valuestring)                      \
            {                                                                 \
                strncpy(val, tmp->valuestring, len);                          \
            }                                                                 \
            else                                                              \
            {                                                                 \
                printf("json_get_str key:%s is NULL\n", key ? key : "error"); \
            }                                                                 \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            printf("json_get_str obj is NULL,key:%s\n", key ? key : "error"); \
        }                                                                     \
    } while (0)

static int writefile(char *name, char *data, int len)
{
    char path[128] = {0};
    int ret = 0;
    if (NULL == name || NULL == data || len < 0)
    {
        printf("error todo\n");
        return -1;
    }
    snprintf(path, sizeof(path), "%s/%s", CONFIG_PATH, name);
    FILE *fp = NULL;
    fp = fopen(path, "w+");
    if (NULL == fp)
    {
        printf("fopen file error \n");
        return -2;
    }
    ret = fwrite(data, sizeof(char), len, fp);
    fflush(fp);
    fclose(fp);
    if (ret != len)
    {
        printf("fwrite ret = %d != len = %d\n", ret, len);
    }
    return 0;
}

static int readfile(char *name, char *data, int *len)
{
    char path[128] = {0};
    int ret = 0;
    if (NULL == name || NULL == data || len < 0)
    {
        printf("error todo\n");
        return -1;
    }
    snprintf(path, sizeof(path), "%s/%s", CONFIG_PATH, name);
    FILE *fp = NULL;
    fp = fopen(path, "r");
    if (NULL == fp)
    {
        printf("fopen file error \n");
        return -2;
    }
    ret = fread(data, sizeof(char), *len, fp); // todo if len > 4096 fixme
    fflush(fp);
    fclose(fp);
    *len = ret;
    return 0;
}

int set_env_usb(env_usb_t *usb)
{
    cJSON *pRoot = cJSON_CreateObject();

    /*--json body--*/

    cJSON_AddNumberToObject(pRoot, "mode", usb->mode);
    cJSON_AddNumberToObject(pRoot, "node", usb->node);
    /*--json end--*/
    char *szJSON = cJSON_Print(pRoot);
    printf("%s\n", szJSON);
    writefile(USB_FILE_PATH, szJSON, strlen(szJSON));
    cJSON_Delete(pRoot);
    free(szJSON);
    szJSON = NULL;
    return 0;
}

int get_env_usb(env_usb_t *usb)
{
    char buff[4096] = {0};
    int len = 4096;
    readfile(USB_FILE_PATH, buff, &len);
    cJSON *pRoot = cJSON_Parse(buff);
    cJSON *pitem = NULL;
    if (NULL == pRoot)
    {
        printf("get env %s no json\n", USB_FILE_PATH);
        // todo load back env
    }
    else
    {
        json_get_int(pRoot, "mode", usb->mode);
        json_get_int(pRoot, "node", usb->node);
    }
    return 0;
}




int set_env_nodeInfo(env_nodeInfo_t *info)
{
    cJSON *pRoot = cJSON_CreateObject();

    /*--json body--*/

    cJSON_AddNumberToObject(pRoot, "enable", info->enable);
    /*--json end--*/
    char *szJSON = cJSON_Print(pRoot);
    printf("%s\n", szJSON);
    writefile(NODEINFOEN_FILE_PATH, szJSON, strlen(szJSON));
    cJSON_Delete(pRoot);
    free(szJSON);
    szJSON = NULL;
    return 0;
}

int get_env_nodeInfo(env_nodeInfo_t *info)
{
    char buff[4096] = {0};
    int len = 4096;
    readfile(NODEINFOEN_FILE_PATH, buff, &len);
    cJSON *pRoot = cJSON_Parse(buff);
    cJSON *pitem = NULL;
    if (NULL == pRoot)
    {
        printf("get env %s no json\n", NODEINFOEN_FILE_PATH);
        // todo load back env
    }
    else
    {
        json_get_int(pRoot, "enable", info->enable);
    }
    return 0;
}
