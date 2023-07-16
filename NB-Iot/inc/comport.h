/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年06月09日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年06月09日 21时36分44秒"
 *                 
 ********************************************************************************/
#ifndef _COMPORT_H_
#define _COMPORT_H_

#include "logger.h"

#define CONFIG_DEF_FRAGSIZE    128
typedef struct comport_s {
	int             fd;
	int             fragsize;       // 大数据传输时fragment大小 
	char            devname[32];    // 串口设备
    long            baudrate;       // 波特率 
    char            dbit;           // 数据位 
    char            parity;         // 奇偶校验 
    char            sbit;           // 停止位 
} comport_t;

extern int comport_open(comport_t *comport, char *devname, long baudrate, const char *conf);

extern int comport_close(comport_t *comport);

extern int comport_send(comport_t *comport, char *data, int data_bytes);

/* read data from $comport in $timeout <ms> to $buf no more than $size bytes */
#define TIMEOUT_NONE           0
extern int comport_recv(comport_t *comport, char *buf, int size, int timeout);

#endif
