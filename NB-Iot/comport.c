/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年06月09日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年06月09日 21时36分09秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <errno.h>
#include "comport.h"

#define CONFIG_PRINT_STDOUT

#if ( defined CONFIG_PRINT_STDOUT )
#define dbg_print(format,args...) printf(format, ##args)

#else
#define dbg_print(format,args...) do{} while(0);
#endif

int comport_open(comport_t *comport, char *devname, long baudrate, const char *conf)
{
	struct termios oldtio;
	struct termios newtio;
	speed_t speed;

    if( !comport || !devname )
    {
        dbg_print("invalid input arugments\n");
        return -1;
    }

	/* 初始化串口结构体*/
	memset(comport, 0, sizeof(*comport));
	strncpy(comport->devname, devname, sizeof(comport->devname));
	comport->baudrate = baudrate;
	comport->fragsize = CONFIG_DEF_FRAGSIZE;
	comport->dbit = conf[0];
	comport->parity = conf[1];
	comport->sbit = conf[2];

#if 0
	dbg_print("%d\n",comport->baudrate);
	dbg_print("%c\n",comport->dbit);
	dbg_print("%c\n",comport->parity);
	dbg_print("%c\n",comport->sbit);
	dbg_print("%s\n",comport->devname);
#endif

    if( !strstr(comport->devname, "tty") )
    {
        dbg_print("comport device \"%s\" is not tty device\n", comport->devname);
        return -2;
    }
    comport->fd = open(devname, O_RDWR | O_NOCTTY | O_NONBLOCK);    
	if (comport->fd < 0)
	{
		dbg_print("comport open \"%s\" failed:%s\n", comport->devname, strerror(errno));
	}
    /* 获取串口当前的配置参数 */
    if (0 > tcgetattr(comport->fd, &oldtio)) 
    {   
        dbg_print("fail to get old attribution of terminal\n");
        close(comport->fd);
        return -2; 
    }


	/* 配置串口参数 */
	/* 设置为原始模式 */
	memset(&newtio, 0x0, sizeof(struct termios));
    cfmakeraw(&newtio);

    /* CREAD 使能接受 */
    newtio.c_cflag |= CREAD;

    /* 设置波特率 */
    switch (comport->baudrate)
    {
        case 1200:
            speed = B1200;
            break;
        case 1800:
            speed = B1800;
            break;
        case 2400:
            speed = B2400;
            break;
        case 4800:
            speed = B4800;
            break;
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400: 
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        case 230400:
            speed = B230400;
            break;
        case 460800:
            speed = B460800;
            break;
        case 500000:
            speed = B500000;
            break;
        default:
            speed = B115200;
            dbg_print("default baud rate is 115200\n");
            break;
    }

    /* cfsetspeed 函数，设置波特率 */
    if (0 > cfsetspeed(&newtio, speed))
    {
        dbg_print("fail to set baud rate of uart\n");
        return -3;
    }

    /* 设置数据位大小 
     * CSIZE 是数据位的位掩码，与上掩码的反，就是将数据位相关的比特位清零
     * CSX (X=5,6,7,8) 表示数据位位数
     */
    newtio.c_cflag &= ~CSIZE;
    switch (comport->dbit)
    {
        case '5':
            newtio.c_cflag |= CS5;
            break;
        case '6':
            newtio.c_cflag |= CS6;
            break;
        case '7':
            newtio.c_cflag |= CS7;
            break;
        case '8':
            newtio.c_cflag |= CS8;
            break;
        default:
            newtio.c_cflag |= CS8;
            dbg_print("default data bit size is 8\n");
            break;
    }

    /* 设置奇偶校验 
     * PARENB 用于使能校验
     * INPCK 用于对接受的数据执行校验
     * PARODD 指的是奇校验
     */
    switch (comport->parity)
    {
        case 'N':   //无校验
		case 'n':
            newtio.c_cflag &= ~PARENB;
            newtio.c_iflag &= ~INPCK;
            break;
        case 'O':   //奇校验
		case 'o':
            newtio.c_cflag |= (PARODD | PARENB);
            newtio.c_iflag |= INPCK;
            break;
        case 'E':   //偶校验
		case 'e':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            newtio.c_iflag |= INPCK;
            break;
        default:    //默认配置为无校验
            newtio.c_cflag &= ~PARENB;
            newtio.c_iflag &= ~INPCK;
            dbg_print("default parity is N (no check)\n");
            break;
    }

    /* 设置停止位 
     * CSTOPB 表示设置两个停止位
     */
    switch (comport->sbit)
    {
        case '1':     //1个停止位
            newtio.c_cflag &= ~CSTOPB;
            break;
        case '2':     //2个停止位
            newtio.c_cflag |= CSTOPB;
            break;
        default:    //默认配置为1个停止位
            newtio.c_cflag &= ~CSTOPB;
            dbg_print("default stop bit size is 1\n");
            break;
    }

    /* 将 MIN 和 TIME 设置为 0，通过对 MIN 和 TIME 的设置有四种 read 模式
     * read 调用总是会立即返回，若有可读数据，则读数据并返回被读取的字节数，否则读取不到数据返回 0
     */
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    /* 清空输入输出缓冲区 */
    if (0 > tcflush(comport->fd, TCIOFLUSH))
    {
        dbg_print("fail to flush the buffer\n");
        return -4;
    }

    /* 写入配置，使配置生效 */
    if (0 > tcsetattr(comport->fd, TCSANOW, &newtio))
    {
        dbg_print("fail to flush the buffer\n");
        return -5;
    }

    /* 写入配置，使配置生效 */
    if (0 > tcsetattr(comport->fd, TCSANOW, &newtio))
    {
        dbg_print("fail to set new attribution of terminal\n");
        return -6;
    }

    return 0;
}


int comport_close(comport_t *comport)
{
    if( !comport )
    {
        dbg_print("invalid input arugments\n");
        return -1;
    }

	struct termios oldtio;

	tcsetattr(comport->fd, TCSANOW, &oldtio);
	if (comport->fd >= 0)
	{
		close(comport->fd);
	}
	comport->fd = -1;

	return 0;
}

int comport_send(comport_t *comport, char *data, int data_bytes)
{
	int    rv = 0;
	int	   left;
	int    bytes = 0;
	char  *ptr;

    if( !comport || !data || data_bytes<=0 )
    {
        dbg_print("invalid input arugments\n");
        return -1;
    }

    if( comport->fd < 0 )
    {
        dbg_print("Serail port not opened\n");
        return -2;
    }

	ptr = data;
	left = data_bytes;

	while(left > 0)
	{
		bytes = left>comport->fragsize ? comport->fragsize : left;

		rv = write(comport->fd, ptr, bytes);
		if (rv < 0)
		{
			dbg_print("write to comport failure:%s\n", strerror(errno));
			rv = -3;
			break;
		}

		left -= rv;
		ptr += rv;
	}

	return rv;
}

int comport_recv(comport_t *comport, char *buf, int size, int timeout)
{
	int            rv = 0;
	int            ret;
	int            bytes = 0;
	fd_set         rfds,exfds;
	struct timeval tv, *tv_ptr=NULL;

    if ( !comport || !buf || size<=0 )
    {
        dbg_print("invalid input arugments\n");
        return -1;
    }

    if ( comport->fd < 0 )
    {
        dbg_print("Serail port not opened\n");
        return -2;
    }

	dbg_print("into comport_recv\n");

	FD_ZERO(&rfds);
	FD_ZERO(&exfds);
	FD_SET(comport->fd, &rfds);
	FD_SET(comport->fd, &exfds);

	if( TIMEOUT_NONE != timeout )
	{
		tv.tv_sec = (time_t) (timeout / 1000);
		tv.tv_usec = (long)(1000 * (timeout % 1000));
		tv_ptr = &tv;
	}

	memset(buf, 0, size);

	while(1)
	{
		ret = select(comport->fd+1, &rfds, NULL, &exfds, tv_ptr);
		if (ret < 0)
		{
			/* EINTR means catch interrupt signal ,not real error*/
			dbg_print("select failure :%s\n", strerror(errno));
			rv = EINTR==errno ? 0 : -3;
			break;
		}
		else if (ret == 0)/* timeout */
		{
			break;
		}
			
		ret = read(comport->fd, buf+bytes, size-bytes);
		if(ret <= 0)
		{
			dbg_print("read from comport failure:%s\n", strerror(errno));
			break;
		}

		bytes += ret;
		if (bytes >= size )
			break;

		/* try to read data in 10ms again, if no data arrive it will break */
		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		tv_ptr = &tv;
	}

	if ( !rv )
		rv = bytes;
	
	dbg_print("read:%s\n", buf);

	return rv;

}


