/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  control.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年07月16日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月16日 11时11分52秒"
 *                 
 ********************************************************************************/

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "led.h"
#include "pwm_beep.h"

/*  云平台服务ID */
extern char        LED_ID[];
extern char        BEEP_ID[];
extern gpiod_led_t led[LED_MAX];

int parse_ctrl(char *value, int size);

#endif
