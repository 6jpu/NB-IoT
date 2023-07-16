/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  control.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年07月16日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月16日 11时11分26秒"
 *                 
 ********************************************************************************/

#include "control.h"

/* 打印颜色控制 */
#define RED_FONT        "\033[1;31m"
#define GREEN_FONT      "\033[1;32m"
#define DEFAULT_FONT    "\033[0m"

/* 解析命令控制硬件
 * value 为接受的数据，size 为value 大小
 * 成功返回0，出错返回负数
 * */
int parse_ctrl(char *value, int size)
{
    if ( !value || !size)
    {
        PARSE_LOG_ERROR("invalid input arugments\n");
        return -1;
    }

    // 判断是否为下发控制指令
    if (value[0] != '0' || value[1] != '6')
    {
        PARSE_LOG_ERROR("not control message!\n");
        return -2;
    }

    // 判断是否为相应LED_ID
    if (strncmp(value + 2, LED_ID, 4) == 0)
    {
		// 控制指令
		if (value[size - 2] == '0' && value[size - 1] == '1')
		{
			dbg_print("%s开灯%s\n", RED_FONT, DEFAULT_FONT);
			led_control(&led[LED_RED], ON);
			sleep(2);
		}
		else if (value[size - 2] == '0' && value[size - 1] == '0')
		{
			dbg_print("%s关灯%s\n", GREEN_FONT, DEFAULT_FONT);
			led_control(&led[LED_RED], OFF);
			sleep(2);
		}
	}
    // 判断是否为相应BEEP_ID
    else if (strncmp(value + 2, BEEP_ID, 4) == 0)
    {
		// 控制指令
		if (value[size - 2] == '0' && value[size - 1] == '1')
		{
			dbg_print("%s开蜂鸣器%s\n", RED_FONT, DEFAULT_FONT);
			pwm_config("enable", "1");
			sleep(2);
		}
		else if (value[size - 2] == '0' && value[size - 1] == '0')
		{
			dbg_print("%s关蜂鸣器%s\n", GREEN_FONT, DEFAULT_FONT);
			pwm_config("enable", "0");
			sleep(2);
		}
    }
	else
	{
        PARSE_LOG_ERROR("control message error!\n");
        return -3;
	}

    return 0;
}

