/*
  OLED Control sample for SPI bus
  This code is designed to work with Raspberry Pi and
  OLED by aitendo(http://www.aitendo.com/product/2099)

  Coded by Yasuhiro ISHII,4/15/2013

  This software is distributed under Apache 2.0 license


  memo:
  D/C# : GPIO17(P1-09)
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "gpiolib.h"

bool gpiolib_init(void);
bool gpiolib_setDirectionGpio17(bool in_xout);
bool gpiolib_setDataGpio17(bool level);

#define BCM2708_PERI_BASE	(0x20000000)
#define GPIO_BASE		(BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE		(1024*4)

#define INP_GPIO(g)		*(gpio_mem+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)		*(gpio_mem+((g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_SET		*(gpio_mem+7)
#define GPIO_CLR		*(gpio_mem+10)

static volatile unsigned* gpio_mem = NULL;

bool gpiolib_init(void)
{
    int memfd;
    bool ret = false;

    memfd = open("/dev/mem",O_RDWR|O_SYNC);
    if(memfd < 0){
	printf("%s : memory open error\n",__func__);
    } else {
	gpio_mem = mmap(NULL,BLOCK_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,memfd,GPIO_BASE);
	close(memfd);
	ret = true;
    }
    return(ret);
}

bool gpiolib_setDirectionGpio17(bool in_xout)
{
    bool ret = false;

    if(gpio_mem != NULL){
	INP_GPIO(17);
	if(in_xout == false){
	    OUT_GPIO(17);
	    ret = true;
	}
    }
    return(ret);
}

bool gpiolib_setDataGpio17(bool level)
{
    bool ret = false;

    if(gpio_mem != NULL){
	if(level == true){
	    GPIO_SET = 1 << 17;
	} else {
	    GPIO_CLR = 1 << 17;
	}
    }
    return(ret);
}

