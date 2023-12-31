/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  adc_mq2.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年07月10日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月10日 19时20分05秒"
 *                 
 ********************************************************************************/
#ifndef _ADC_MQ2_H_
#define _ADC_MQ2_H_

#include "led.h"
#include "pwm_beep.h"

typedef struct adc_dev_s
{
	int    raw; //原始数据，数字值
	float  scale; //精度值
	float  act; //实际值
	float  conc; //浓度值
}adc_dev_t;

enum path_index
{
	IN_VOLTAGE_SCALE = 0,
	IN_VOLTAGE1_RAW,
};	

extern gpiod_led_t led[LED_MAX];

static int file_data_read(char *filename, char *buf, size_t buf_size);

int mq2_sample(adc_dev_t *mq2, int size, char *smoke_str, size_t len);

#endif


