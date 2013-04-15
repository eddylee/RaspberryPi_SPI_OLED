/*
  OLED Control sample for SPI bus
  This code is designed to work with Raspberry Pi and
  OLED by aitendo(http://www.aitendo.com/product/2099)

  Coded by Yasuhiro ISHII,4/15/2013

  This software is distributed under Apache 2.0 license
*/


#ifndef __fb_h__
#define __fb_h__

extern bool fb_init(void);
extern bool fb_getTopLeftBuff_for_OLED(unsigned char* buff,int size_x,int size_y);

#endif
