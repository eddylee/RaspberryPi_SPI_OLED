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
#include <string.h>
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
#include "fb.h"
#include "gpiolib.h"

bool oled_init(int fd);
bool oled_sendPixel(int fd,int pixel);
bool oled_paintFullScreen(int fd,int color);
bool oled_setCommandMode(bool mode);
bool oled_transmitFullScreen(int fd,unsigned char* pixmap);
void oled_main(void);

bool oled_init(int fd)
{
    unsigned char init_data[] = {
	0xae,

	// Row/Col Address
	0x75,0x00,0x3f,
	0x15,0x00,0x5f,

	// Contrast
	0xa0,0x74,
	0xa1,0x00,
	0xa2,0x00,
	0xa4,
	0xa8,0x3f,
	//0xad,0x8f,
	//0xb0,0x1a,
	//0xb1,0x74,
	//0xb3,0xd0,
	//0x8a,0x81,
	//0x8b,0x82,
	//0x8c,0x83,
	//0xbb,0x3e,
	//0xbe,0x3e,
	0x87,0x0f,
	0x81,0x80,
	0x82,0x80,
	0x83,0x80,
	0xaf
    };

    int result;
  
    oled_setCommandMode(true);

    result = write(fd,init_data,sizeof(init_data));
    if(result < 0){
	printf("%s : data write error\n",__func__);
	return(false);
    }

    oled_setCommandMode(false);
  
    return(true);
}



int main(int argc,char** argv)
{
    bool result;

    result = gpiolib_init();
    if(result != true){
	printf("gpiolib init error\n");
	return(-1);
    }
    gpiolib_setDirectionGpio17(false);

    result = fb_init();
    if(result != true){
	printf("fb init error\n");
	return(-1);
    }

    oled_main();
}

void oled_main(void)
{
    int fd;
    int result;
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    //uint32_t speed = 4000000;
    uint32_t speed = 8000000; // on the spec,SCLK max freq=4[MHz]
    uint16_t delay;

    fd = open("/dev/spidev0.0",O_RDWR);
    if(fd == -1){
	printf("Device open error\n");
	return;
    }

    /*
      set SPI read/write mode
    */
    result = ioctl(fd,SPI_IOC_WR_MODE,&mode);
    if(result < 0){
	printf("error(L%d)\n",__LINE__);
	return;
    }
    result = ioctl(fd,SPI_IOC_RD_MODE,&mode);
    if(result < 0){
	printf("error(L%d)\n",__LINE__);
	return;
    }

    result = ioctl(fd,SPI_IOC_WR_BITS_PER_WORD,&bits);
    if(result < 0){
	printf("error(L%d)\n",__LINE__);
	return;
    }
    result = ioctl(fd,SPI_IOC_RD_BITS_PER_WORD,&bits);
    if(result < 0){
	printf("error(L%d)\n",__LINE__);
	return;
    }

    result = ioctl(fd,SPI_IOC_WR_MAX_SPEED_HZ,&speed);
    if(result < 0){
	printf("error(L%d)\n",__LINE__);
	return;
    }
    result = ioctl(fd,SPI_IOC_RD_MAX_SPEED_HZ,&speed);
    if(result < 0){
	printf("error(L%d)\n",__LINE__);
	return;
    }

    oled_init(fd);

    {
	unsigned char buff[96*64*2];
	bool r;


	while(1){
	    r = fb_getTopLeftBuff_for_OLED(buff,96,64);
	    if(r == false){
		printf("%s : fb_getTopLeftBuffer_for_OLED returns error\n",__func__);
		return;
	    }
	    r = oled_transmitFullScreen(fd,buff);
	    if(r == false){
		printf("%s : oled_transmitFullScreen returns error\n",__func__);
		return;
	    }
	}
    }

    close(fd);
}

bool oled_sendPixel(int fd,int color)
{
    bool ret = true;
    unsigned char data[2];

    data[0] = color >> 8;
    data[1] = color & 0xff;
  
    if(write(fd,data,sizeof(data)) < 0){
	ret = false;
    }

    return(ret);
}

bool oled_paintFullScreen(int fd,int color)
{
    int i;
    unsigned char buff[64*96*2];
    unsigned char* p;

    p = buff;

    for(i=0;i<sizeof(buff);i+=2){
	buff[i] = color >> 8;
	buff[i+1] = color & 0xff;
    }

    oled_transmitFullScreen(fd,buff);
}

bool oled_transmitFullScreen(int fd,unsigned char* pixmap)
{
    int i;
    bool result;
    for(i=0;i<6;i++){
	result = write(fd,pixmap + 2048*i,2048);
	if(result < 0){
	    printf("%s : write error\n",__func__);
	    return(false);
	}
    }
    return(true);
}

bool oled_setCommandMode(bool mode)
{
    return (gpiolib_setDataGpio17(!mode));
}
