/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sht20.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年07月10日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月10日 19时23分51秒"
 *                 
 ********************************************************************************/

#ifndef _SHT20_H_
#define _SHT20_H_

#define SOFTRESET						0xFE
#define TRIGGER_TEMPERATURE_NO_HOLD		0xF3
#define TRIGGER_HUMIDITY_NO_HOLD		0xF5

static inline void msleep(unsigned long ms);

static inline void dump_buf(const char *prompt, uint8_t *buf, int size);

int sht2x_init(char *i2c_dev);

int sht2x_get_serialnumber(int fd, uint8_t *serialnumber, int size);

int sht2x_get_temp_humidity(int fd, float *temp, float *rh);

#endif
