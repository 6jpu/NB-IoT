/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sht20.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年07月08日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月08日 22时22分11秒"
 *                 
 ********************************************************************************/

/*
 *   Hardware connection:
 *  
 *   Sht20 Module          RaspberryPi/IGKBoard
 *   VCC      <----->      3.3V
 *   SDA      <----->      SDA
 *   SCL      <----->      SCL
 *   GND      <----->      GND
 *         
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "sht20.h"

//#define CONFIG_PRINT_STDOUT

#if ( defined CONFIG_PRINT_STDOUT )
#define dbg_print(format,args...) printf(format, ##args)

#else
#define dbg_print(format,args...) do{} while(0);
#endif

/* ms级休眠函数 */
static inline void msleep(unsigned long ms)
{
    struct timespec cSleep;
    unsigned long ulTmp;

    cSleep.tv_sec = ms / 1000;
    if (cSleep.tv_sec == 0)
    {
        ulTmp = ms * 10000;
        cSleep.tv_nsec = ulTmp * 100;
    }
    else
    {
        cSleep.tv_nsec = 0;
    }

    nanosleep(&cSleep, 0);
}

static inline void dump_buf(const char *prompt, uint8_t *buf, int size)
{
	int		i;

	if (!buf)
	{
		return;
	}

	if (prompt)
	{
		dbg_print ("%s ", prompt);
	}

	for (i=0; i<size; i++)
	{
		dbg_print ("%02x ", buf[i]);
	}
	dbg_print ("\n");

	return;
}

/* sht20命令复位 */
int sht2x_softreset(int fd)
{
	uint8_t		buf[4];

	if (fd < 0)
	{
		printf ("%s line [%d] %s() get invalid input argument\n", __FILE__, __LINE__, __func__);
		return -1;
	}

	/* software reset  SHT2x*/
	memset(buf, 0, sizeof(buf));

	/* 根据手册发送命令并休眠需要时间 */
	buf[0] = SOFTRESET;
	write(fd, buf, 1);

	msleep(50);

	return 0;
}

/*  初始化sht20 设备，形参为i2c总线节点 */
int sht2x_init(char *i2c_dev)
{
	int		fd;

	if ( (fd=open(i2c_dev, O_RDWR)) < 0 )
	{
		printf ("i2c device open fialed:%s\n", strerror(errno));
		return -1;
	}

	/* set I2C mode and SHT2x slave address */
	ioctl(fd, I2C_TENBIT, 0); //NOT 10-bit but 7-bit mode
	ioctl(fd, I2C_SLAVE, 0x40); //set SHT2x slave address 0x40
	
	if ( sht2x_softreset(fd) < 0 )
	{
		printf ("SHT2x softreset failure\n");
		return -2;
	}

	return fd;
}

int sht2x_get_serialnumber(int fd, uint8_t *serialnumber, int size)
{
	uint8_t		buf[4];

	if (fd < 0 || !serialnumber || size!=8)
	{
		printf ("%s line [%d] %s() get invalid input argument\n", __FILE__, __LINE__, __func__);
		return -1;
	}

	/* Read serialnumber from location 1 */
	memset(buf, 0, sizeof(buf));
	buf[0] = 0xfa; //command for readout on-chip
    buf[1] = 0x0f; //on-chip memory address
	write(fd, buf, 2);

	memset(buf, 0, sizeof(buf));
	read(fd, buf, 4);

	serialnumber[1]=buf[0]; /*  Read SNC_1 */
	serialnumber[0]=buf[1]; /*  Read SNC_0 */
	serialnumber[7]=buf[2]; /*  Read SNA_1 */
	serialnumber[6]=buf[3]; /*  Read SNA_0 */

	dump_buf("SHT2x serial number:", serialnumber, 8);

	return 0;
}

/* 获取sht20的温湿度值 */
int sht2x_get_temp_humidity(int fd,float *temp, float *rh)
{
	uint8_t		buf[4];

	if (fd<0 || !temp || !rh)
	{
		printf ("%s line [%d] %s() get invalid input arguments\n", __FILE__, __LINE__, __func__);
		return -1;
	}

	/* send trigger temperatue measure command and read the data */
	memset(buf, 0, sizeof(buf));
	//发送非主机保持的温度测量命令
	buf[0] = TRIGGER_TEMPERATURE_NO_HOLD;
	write(fd, buf, 1);

	/* 等待需要时间,数据手册可知 */
	msleep(85);

	memset(buf, 0, sizeof(buf));
	/* 读取三个字节数据，最后一个字节位crc校验 */
	read(fd, buf, 3);
	dump_buf("Temperature sample data:", buf, 3);
	/* 计算实际温度值 */
	*temp = 175.72 * (((((int) buf[0]) << 8) + (buf[1] & 0xfc)) / 65536.0) -46.85;

	/* send trigger humidity measure command and read the data */
	memset(buf, 0, sizeof(buf));
	/* 发送非主机保持的湿度测量命令 */
	buf[0] = TRIGGER_HUMIDITY_NO_HOLD;
	write(fd, buf, 1);

	msleep(29);
	memset(buf, 0, sizeof(buf));

	read(fd, buf, 3);
	dump_buf("Relative humidity sample data: ", buf, 3);
	/* 计算实际湿度值 */
	*rh = 125 * (((((int) buf[0]) << 8) + (buf[1] & 0xfc)) / 65535.0) -6;

	return 0;
}
