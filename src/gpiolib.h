#ifndef __GPIOLIB_H__
#define __GPIOLIB_H__

extern bool gpiolib_init(void);
extern bool gpiolib_setDirectionGpio17(bool in_xout);
extern bool gpiolib_setDataGpio17(bool level);

#endif /* __GPIOLIB_H__ */
