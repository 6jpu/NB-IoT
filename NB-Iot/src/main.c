/*********************************************************************************
 *      Copyright:  (C) 2023 Nbiot<lingyun@gail.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/06/23)
 *         Author:  Nbiot <lingyun@gail.com>
 *      ChangeLog:  1, Release initial version on "10/06/23 15:42:24"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include "nbiot.h"
#include "bc28.h"
#include "sht20.h"
#include "adc_mq2.h"
#include "msleep.h"
#include "control.h"
#include "logger.h"
#include "mysqlite.h"


#define I2C_DEV         "/dev/i2c-0"
#define BEEP_DEV		"/pwmchip1"

/* 信号处理函数 */
void sig_handler(int signum);
/* 打印帮助信息 */
void print_usage(char *proname);

int			g_stop = 0;
gpiod_led_t led[LED_MAX];
/*  云平台服务ID */
char		LED_ID[] = "1F42";
char        BEEP_ID[] = "1F43";


int main (int argc, char **argv)
{
    int         n;
    int         rv = 0;
	int			ret = 0;
    char        dev[20] = "/dev/ttymxc7";	//BC28 默认串口设备
    long        baudrate = 9600;
    char        conf[] = "8N1";
    char        data[64] = "17,02001B000C0000001C0000001C0000001C";
    char        buf[1024];
	char        ip[] = "221.229.214.202";
	char        port[] = "5683";
	char		ctrl_cmd[32];
    comport_t   com;
	int			sht20_fd;
	char		temp_str[10];
	char		rh_str[10];
	uint8_t		serialnumber[8];
	adc_dev_t	mq2;
	char		smoke_str[10];
	time_t		current_time;      //当前时间戳
	time_t		pretime = 0;       //上次采样时间戳
	int			set_time = 5;      //设置上报时间间隔，默认为5秒
	int			loglevel = LOG_LEVEL_DEBUG;   //可设置LOG_LEVEL_DEBUG,LOG_LEVEL_INFO,
											  //LOG_LEVEL_WARN,LOG_LEVEL_ERROR四个级别
	int			daemon_run = 0;
	int			opt;
	struct option opts[] = {
								{"device", required_argument, NULL, 'd'},
								{"daemon", no_argument, NULL, 'b'},
								{"time_interval", required_argument, NULL, 't'},
								{"help", no_argument, NULL, 'h'},
								{NULL, 0, NULL, 0}
                   	       };
    
    while((opt = getopt_long(argc, argv, "d:t:bh", opts, NULL)) != -1)
	{
        switch(opt)
        {
            case 'd':
					 memset(dev, 0, sizeof(dev));
                     strcpy(dev, optarg);
                     break;
            case 't':
                     set_time = atoi(optarg);
                     break;
            case 'b':
                     daemon_run = 1;
                     break;
            case 'h':
                     print_usage(argv[0]);
                     return 0;
        }
    }
	
	/* 后台运行程序 */
    if(daemon_run)
	{
		if(daemon(1, 0))
		{
			printf("Running daemon failure:%s\n", strerror(errno));
		}
	}

	/* 安装信号 */
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
   
	/* 日志系统初始化 */
	logger_init(loglevel);

	if (comport_open(&com, dev, baudrate, conf) < 0)
    {
        PARSE_LOG_ERROR ("comport open failure!\n");
        return -1;
    }
    dbg_print ("comport open successfully!\n");

	//sht20初始化
	sht20_fd = sht2x_init(I2C_DEV);
	if (sht20_fd < 0)
	{
			PARSE_LOG_ERROR ("SHT2x initialize failure\n");
			return -2;
	}

	if ( sht2x_get_serialnumber(sht20_fd, serialnumber, 8) < 0 )
	{
			PARSE_LOG_ERROR ("SHT2x get serial number failure\n");
			return -3;
	}

	//LED 初始化
	if (led_init(&led[LED_RED], 1, 11) < 0)
	{
		PARSE_LOG_ERROR ("LED RED initialize error.\n");
		return -4;
	}
	
	//蜂鸣器初始化
	if ( pwm_init(BEEP_DEV) < 0)
	{   
		PARSE_LOG_ERROR ("PWM BEEP initialize failure\n");
		return -5; 
	} 

	// 数据库初始化
	if(sqlite_create() < 0)
	{
		PARSE_LOG_ERROR ("Creat database failure\n");
		return -6;
	}
	
	dbg_print ("Program initialization complete!\n");

    while ( !g_stop )
    {
		rv = atcmd_nmstatus(&com);
		if ( rv < 0 )
		{	
			rv = nbiot_attach_check(&com);
			if ( rv < 0 )
			{
				PARSE_LOG_WARN ("nbiot_attach_check error!\n");
				continue;
			}

			rv = nbiot_connect_cloud(&com, ip, port);
			if ( rv < 0 )
			{
				PARSE_LOG_WARN ("nbiot_connect_cloud error!\n");
				continue;
			}
			printf ("Sampling!\n");
		}

		/* 判断是否到了采样时间 */
		current_time = time(NULL);
		if ( current_time-pretime >= set_time )
		{
			//温湿度采样
			if ( sht2x_sample(sht20_fd, temp_str, rh_str, 20) < 0 )
			{
					PARSE_LOG_ERROR ("sht20 sample failure\n");
					ret = -4;
					goto CleanUp;
			}
			PARSE_LOG_DEBUG ("temp_str:%s\n", temp_str);
			PARSE_LOG_DEBUG ("rh_str:%s\n", rh_str);
			
			//烟雾浓度采样并自检
			if ( mq2_sample(&mq2, sizeof(mq2), smoke_str, sizeof(smoke_str)) < 0 )
			{
				PARSE_LOG_ERROR ("smoke_sample error\n");
				ret = -5;
				goto CleanUp;
			}
			PARSE_LOG_DEBUG ("smoke_str:%s\n", smoke_str);
			
			//上报烟雾浓度和温湿度
			//data[64] = "17,02001B000C0000001C0000001C0000001C";
			memset(data, 0, sizeof(data));
			snprintf(data, sizeof(data), "17,02001B000C%s%s%s", smoke_str, temp_str, rh_str);
			rv = atcmd_qlwuldataex(&com, data);
			if ( rv < 0 )
			{
				PARSE_LOG_ERROR ("atcmd_qlwuldataex data error!\n");
				break;
			}
			pretime = current_time;
			sleep(3);
        }
		
		rv = atcmd_ctrl_recv(&com, ctrl_cmd, sizeof(ctrl_cmd));
		if ( rv < 0 )
		{
			PARSE_LOG_WARN ("atcmd_ctrl_recv error!\n");
		}
		else if ( ATRES_EXPECT==rv )
		{
			PARSE_LOG_DEBUG ("Value:%s\n", ctrl_cmd);
			rv = parse_ctrl(ctrl_cmd, strlen(ctrl_cmd));
			if ( rv < 0 )
			{
				PARSE_LOG_WARN ("parse_ctrl error!\n");
			}
		}
	}

CleanUp:
    comport_close(&com);
    dbg_print ("comport closed!\n");
	close(sht20_fd);
	led_release(&led[LED_RED]);
    return ret;
}

/* 使程序可以被Ctrl+C 终止*/
void sig_handler(int signum)
{
    switch(signum)
    {
        case SIGINT:
        case SIGTERM:
            g_stop = 1;

        default:
            break;
    }

    return ;
}

void print_usage(char *proname)
{
    printf("%s usage :\n",proname );
    printf("-d(--device): specify BC28 comport device\n");
    printf("-b[daemon]: start daemon\n");
    printf("-t(--time_interval): set data reporting time interval\n");
    printf("-h(--help): print this help information.\n");

    return ;

}
