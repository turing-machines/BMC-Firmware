/**
 * @file param.h
 * @author wenyi (wenyi0421@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __PARAM_H__
#define __PARAM_H__

typedef struct _env_usb_t
{
    /* data */
    int mode;
    int node;
}env_usb_t;

typedef struct _env_nodeInfo_t
{
    int enable;
}env_nodeInfo_t;


/**
 * @brief all env
 * 
 */
typedef struct _env_ctx_t
{
    env_usb_t usb;
    env_nodeInfo_t nodeen;
}env_ctx_t;

env_ctx_t* env_get_ctx(void);
void init_env(void);
int set_env_usb(env_usb_t* usb); 
int get_env_usb(env_usb_t* usb);


#endif // !__PARAM_H__