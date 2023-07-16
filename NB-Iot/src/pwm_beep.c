/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  pwm_music.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年07月08日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月08日 11时37分40秒"
 *                 
 ********************************************************************************/

#include "pwm_beep.h"

static char pwm_path[100];

/* pwm 配置函数 
 * attr：属性文件名字 
 * val：属性的值(字符串)
 */
int pwm_config(const char *attr, const char *val)
{
    char		file_path[100];
    int			len;
    int			fd =  -1;

    if(attr == NULL || val == NULL)
    {
        printf("[%s] argument error\n", __FUNCTION__);
        return -1;
    }

    memset(file_path, 0, sizeof(file_path));
    snprintf(file_path, sizeof(file_path), "%s%s", pwm_path, attr);
    if (0 > (fd = open(file_path, O_WRONLY))) {
        printf("[%s] open %s error\n", __FUNCTION__, file_path);
        return fd;
    }

    len = strlen(val);
    if (len != write(fd, val, len)) {
        printf("[%s] write %s to %s error\n", __FUNCTION__, val, file_path);
        close(fd);
        return -2;
    }

    close(fd);  //关闭文件
    return 0;
}

/* pwm 初始化函数
 * pwm_dev:pwm 对应芯片, e.g. pwm_dev = "/pwmchip1" */
int pwm_init(char *pwm_dev)
{
	char      temp[100];
	int       fd = -1;
	int       i = 0;

	if ( !pwm_dev )
	{
		printf ("[%s]:input argument is invalid\n", __FUNCTION__);
		return -1;
	}

    /* 导出pwm 首先确定最终导出的文件夹路径*/
    memset(pwm_path, 0, sizeof(pwm_path));
    snprintf(pwm_path, sizeof(pwm_path), "/sys/class/pwm%s/pwm0/", pwm_dev);
    
    //如果pwm0目录不存在, 则导出,access()函数若文件存在则返回0
    memset(temp, 0, sizeof(temp));
    if (access(pwm_path, F_OK)) 
	{
        snprintf(temp, sizeof(temp) , "/sys/class/pwm%s/export", pwm_dev);
        if (0 > (fd = open(temp, O_WRONLY))) {
            printf("open %s error\n", pwm_dev);
            return -2;
        }
        //导出pwm0文件夹
        if (1 != write(fd, "0", 1)) {
            printf("write '0' to  %s/export error\n", pwm_dev);
            close(fd);
            return -3;
        }
        close(fd); 
    }

	/*  配置默认PWM 周期 */
	if (pwm_config("period", "10000"))
		return -4;

	/*  配置默认占空比 */
	if (pwm_config("duty_cycle", "5000"))
		return -5;

	return 0;
} 



