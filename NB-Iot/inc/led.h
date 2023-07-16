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


/* RGB LED index */
enum
{
	LED_RED,
	LED_GREEN,
	LED_BLUE,
	LED_MAX,
};	

/* RGB LED status */
enum
{
	OFF=0,
	ON,
};	

/* RGB LED struct */
typedef struct gpiod_led_s
{
	struct gpiod_chip	*chip;
	struct gpiod_line 	*line;
}gpiod_led_t;

int led_init(gpiod_led_t *gpiod_led, unsigned char gpio_chip_num, unsigned char gpio_off_num);

int led_control(gpiod_led_t *gpiod_led, int status);

void blink_led(gpiod_led_t *gpiod_led, unsigned int interval);

int led_release(gpiod_led_t *gpiod_led);

#endif

