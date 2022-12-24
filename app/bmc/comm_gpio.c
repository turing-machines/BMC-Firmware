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
#include <pthread.h>



#include "comm_gpio.h"

/*************************************************************************
*                        global variable                                 *
**************************************************************************/
static pthread_mutex_t gpio_lock = PTHREAD_MUTEX_INITIALIZER;


/*************************************************************************
*                        Function implementation                         *
**************************************************************************/
/**************************************************************
**Describe: Open operation gpio node,and set the direction
**Input:  gpio_num Operation Gpio num; 
           direct:  0->output   1-> input
**Ouput:
**Return: 0:success  -1 :Fail
*************************************************************************/
char common_device_gpioOpen(unsigned int gpio_num,bool direct)
{   
	FILE *fp;
	char file_name[50];

	
	pthread_mutex_lock(&gpio_lock);

	//open operation giop node  
	sprintf(file_name, "/sys/class/gpio/export");
	fp = fopen(file_name, "w");
	if (fp == NULL) 
	{
		   printf("Cannot open %s.\n", file_name);
		   return -1;
	}		
    fprintf(fp, "%d", gpio_num);
    fclose(fp);

    //set gpio direct
    sprintf(file_name, "/sys/class/gpio/gpio%d/direction", gpio_num);
    fp = fopen(file_name, "rb+");
    if (fp == NULL) 
	{
            printf("Cannot open %s.\n", file_name);
            return -1;
    }
	if(direct == 0)
	{
   		fprintf(fp, "out");
	}
	else
	{
        fprintf(fp, "in");
	}
	
    fclose(fp);

	pthread_mutex_unlock(&gpio_lock);

	return 0;
}

/**************************************************************
**Describe: Close operation gpio node
**Input:  gpio_num Operation Gpio num; 
**Ouput:
**Return: 0:success  -1 :Fail
**************************************************************/
char common_device_gpioClose(unsigned int gpio_unm)
{
	FILE *fp;
	char file_name[50];

    sprintf(file_name, "/sys/class/gpio/unexport");
	fp = fopen(file_name, "w");
	if(fp == NULL) 
	{
		printf("Cannot open %s.\n", file_name);
		return -1;
	}
	fprintf(fp, "%d", gpio_unm);
	fclose(fp);

	return 0;
}

/**************************************************************
**Describe: Set gpio output value
**Input:   gpio_num Operation Gpio num; 
           value:  0->L   1-> H
**Ouput:
**Return: 0:success  -1 :Fail
**************************************************************/
char common_device_gpioWirte(unsigned int gpio_num,bool value)
{
	FILE *fp;
    char file_name[50];
	unsigned char buf[2];

	
	pthread_mutex_lock(&gpio_lock);

   //set gpio H or L
    sprintf(file_name, "/sys/class/gpio/gpio%d/value", gpio_num);
    fp = fopen(file_name, "rb+");
    if (fp == NULL) 
	{
            printf("Cannot open %s.\n", file_name);
            return -1;
    }
 
	if (value == GPIO_H)
	   strcpy(buf,"1");
	else
	   strcpy(buf,"0");

	fwrite(buf, sizeof(char), sizeof(buf) - 1, fp);
	
	//printf("%s: gpio%d_%d = %s\n", __func__,gpio_num/8, gpio_num%8, buf);
	
	fclose(fp);

	pthread_mutex_unlock(&gpio_lock);
	
  return 0;
}

/**************************************************************
**Describe: Get gpio input value
**Input:   gpio_num Operation Gpio num;           
**Ouput:
**Return: value:  0->L   1-> H     -1 ->Fail
**************************************************************/
char common_device_gpioRead(unsigned int gpio_num)
{
	FILE *fp;
	char file_name[50];
	unsigned char buf[2];

	
	pthread_mutex_lock(&gpio_lock);

	sprintf(file_name, "/sys/class/gpio/gpio%d/value", gpio_num);
	fp = fopen(file_name, "rb+");
	if (fp == NULL) {
	printf("Cannot open %s.\n", file_name);
	return -1;
	}
	memset(buf, 0, 2);
	fread(buf, sizeof(char), sizeof(buf) - 1, fp);
	
	//printf("%s: gpio%d_%d = %d  \n", __func__,gpio_num/8, gpio_num%8, buf[0]-48);
	
    fclose(fp);

	pthread_mutex_unlock(&gpio_lock);

	if(buf[0] == '1')	
	   return GPIO_H;
	else if(buf[0] == '0')
	   return GPIO_L;
	else
	   return -1;	
}