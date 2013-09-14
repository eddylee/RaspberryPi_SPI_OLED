/*
  OLED Control sample for SPI bus
  This code is designed to work with Raspberry Pi and
  OLED by aitendo(http://www.aitendo.com/product/2099)

  Coded by Yasuhiro ISHII,4/15/2013

  This software is distributed under Apache 2.0 license
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "fb.h"

bool fb_init(void);
bool fb_getTopLeftBuff_for_OLED(unsigned char* buff,int size_x,int size_y);
static void fb_memconvert(unsigned char* src,unsigned char* dest,int size);

volatile unsigned char* fb_mem;

#define OLED_SIZE_X		96
#define OLED_SIZE_Y		64

static int screen_size_x = 0;
static int screen_size_y = 0;
static int screen_bpp = 0;
static int screen_line_length = 0;

bool fb_init(void)
{
    int fb_fd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    fb_fd = open("/dev/fb0",O_RDWR);
    if(fb_fd < 0){
	printf("framebuffer open error\n");
	return(false);
    }

    if(ioctl(fb_fd,FBIOGET_VSCREENINFO,&vinfo)){
	printf("screen information retrieve error\n");
	return(false);
    }

    if(ioctl(fb_fd,FBIOGET_FSCREENINFO,&finfo)){
      printf("err\n");
      return(false);
    }

    screen_size_x = vinfo.xres;
    screen_size_y = vinfo.yres;
    screen_bpp = vinfo.bits_per_pixel;
    screen_line_length = finfo.line_length;

    printf("%dx%d,%dbpp line_len=%d(xres*bpp/8=%d)\n",vinfo.xres,vinfo.yres,vinfo.bits_per_pixel,finfo.line_length,vinfo.xres*screen_bpp/8);

    if(vinfo.bits_per_pixel != 16){
	printf("This color mode is not supported.\nOnly 16bpp is supported currently\n");
	return(false);
    }

    fb_mem = (volatile unsigned char*)mmap(
	NULL,
	(screen_line_length*screen_size_y),
	PROT_READ|PROT_WRITE,
	MAP_SHARED,
	fb_fd,
	0);

    if(fb_mem == -1){
	printf("framebuffer mapping error\n");
	return(false);
    }

    return(true);
}

bool fb_getTopLeftBuff_for_OLED(unsigned char* buff,int size_x,int size_y)
{
    int y;
    unsigned char tmp_buff[OLED_SIZE_X*OLED_SIZE_Y*2];
    unsigned char* tmp_buffp;

    tmp_buffp = tmp_buff;

    for(y=0;y<size_y;y++){
	memcpy(
	    tmp_buffp,
	    (void*)(fb_mem + y * screen_line_length),
	    (Size_x * screen_bpp) >> 3);
	tmp_buffp += size_x * screen_bpp >> 3;
    }


    fb_memconvert(buff,tmp_buff,sizeof(tmp_buff));

    return(true);
}

static void fb_memconvert(unsigned char* dest,unsigned char* src,int size)
{
    int i;
    unsigned char tmpr,tmpg,tmpb;
    unsigned short s,sd;

    for(i=0;i<size;i+=2){
	s = src[i+1] << 8 | src[i];
	tmpr = (s >> 11) & 0x1f;
	tmpg = (s >>  6) & 0x1f;
	tmpb = (s      ) & 0x1f;

	// OLED format : BBBBBGGGGGGRRRRR
	sd = (tmpr) | (tmpg << 6) | (tmpb << 11);
	dest[i  ] = (sd >> 8) & 0xff;
	dest[i+1] = (sd     ) & 0xff;
    }
}
