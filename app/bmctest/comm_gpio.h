/********************************************************************************
* File                :
* Intro               :
* Author              : 
* Version             : v1.0
* Notes               :
* Change History      :
Date              Version        Changed By          Changes
* --------------------------------------------------------------------------------
2018-12-20        v0.1                        create

* Copyright(c)
*******************************************************************************/


#ifndef __COMM_GPIO_H__
#define __COMM_GPIO_H__

#include <stdbool.h>

#define GPIO_IN   1
#define GPIO_OUT  0

#define GPIO_H    1
#define GPIO_L    0


char common_device_gpioOpen(unsigned int gpio_num,bool direct);
char common_device_gpioClose(unsigned int gpio_unm);
char common_device_gpioWirte(unsigned int gpio_num,bool value);
char common_device_gpioRead(unsigned int gpio_num);


#endif
