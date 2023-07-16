/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  pwm_beep.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年07月15日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月15日 20时31分12秒"
 *                 
 ********************************************************************************/

#ifndef _PWM_BEEP_H_
#define _PWM_BEEP_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int pwm_init(char *pwm_dev);

int pwm_config(const char *attr, const char *val);

#endif

