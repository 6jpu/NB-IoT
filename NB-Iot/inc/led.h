/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  led.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年07月15日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月15日 10时11分21秒"
 *                 
 ********************************************************************************/
#ifndef _LED_H_
#define _LED_H_

#include "logger.h"

/* RGB LED index */
enum
{
	LED_RED,
	LED_GREEN,
	LED_BLUE,
	LED_MAX,
};	

/* common anode RGB LED status */
enum
{
	ON=0,
	OFF,
};	

/* RGB LED struct */
typedef struct gpiod_led_s
{
	struct gpiod_chip	*chip;
	struct gpiod_line 	*line;
}gpiod_led_t;

extern int led_init(gpiod_led_t *gpiod_led, unsigned int gpio_chip_num, unsigned int gpio_off_num);

extern int led_control(gpiod_led_t *gpiod_led, int status);

extern void blink_led(gpiod_led_t *gpiod_led, unsigned int interval);

extern int led_release(gpiod_led_t *gpiod_led);

#endif

