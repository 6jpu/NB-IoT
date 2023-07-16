/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test_Raspberry.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年07月06日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月06日 21时34分21秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "led.h"
#include "msleep.h"


int led_init(gpiod_led_t *gpiod_led, unsigned char gpio_chip_num, unsigned char gpio_off_num)
{
	char	dev_name[16];

	if (gpio_chip_num == 0 || gpio_chip_num > 5 || gpio_off_num == 0 || gpio_off_num > 32 || !gpiod_led)
	{
		printf ("[INFO] %s argument error.\n", __FUNCTION__);
		return -1;
	}

	memset(dev_name, 0, sizeof(dev_name));
	snprintf(dev_name, sizeof(dev_name), "dev/gpiochip%d", gpio_chip_num-1);

	if (!(gpiod_led->chip = gpiod_chip_open(dev_name)))
	{
		printf ("Open gpiochip failure\n");
		return -2;
	}

	if (!(gpiod_led->line = gpiod_chip_get_line(gpiod_led->chip, gpio_off_num)))
	{
		printf ("Get gpio line_led failure\n");
		return -3;
	}

	/* 设置初始值为灭 */
	if (gpiod_line_request_output(gpiod_led->line, "led_out", OFF) < 0)
	{
		printf ("Request line_led\n");
	}

	return 0;
}

int led_control(gpiod_led_t *gpiod_led, int status)
{
	int		level;

	if ( !gpiod_led )
	{
		printf ("[INFO] %s argument error.\n", __FUNCTION__);
		return -1;
	}

	level = (status==ON) ? ON : OFF;
	if (gpiod_line_set_value(gpiod_led->line, level) < 0)
	{
		printf ("Set line_led value failure\n");
		return -2;
	}

	return 0;
}

void blink_led(gpiod_led_t *gpiod_led, unsigned int interval)
{
	led_control(gpiod_led, ON);
	msleep(interval);

	led_control(gpiod_led, OFF);
	msleep(interval);
}

int led_release(gpiod_led_t *gpiod_led)
{
	if (!gpiod_led)
	{
		printf ("[INFO] %s argument error.\n", __FUNCTION__);
		return -1;
	}

	gpiod_line_release(gpiod_led->line);
	gpiod_chip_close(gpiod_led->chip);

	return 0;
}

