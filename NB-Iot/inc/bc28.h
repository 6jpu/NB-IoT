/********************************************************************************
 *      Copyright:  (C) 2023 Nbiot<lingyun@gail.com>
 *                  All rights reserved.
 *
 *       Filename:  atcmd.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(12/06/23)
 *         Author:  Nbiot <lingyun@gail.com>
 *      ChangeLog:  1, Release initial version on "12/06/23 19:40:53"
 *                 
 ********************************************************************************/

#ifndef  _BC28_H_
#define  _BC28_H_

#include "led.h"
#include "pwm_beep.h"
#include "atcmd.h"

enum
{
	DISABLE = 0,    /*  common disable */
	ENABLE,         /*  common enable  */
};

typedef struct nbiot_info_s
{
    char     manufacturer[MAX];
    char     module_name[MAX];
	char     sn[MAX];
    char     imei[MAX];
    char     ismi[MAX];
    char     version[MAX];
}nbiot_info_t;


extern gpiod_led_t led[LED_MAX];

/* 接收云平台的控制指令 */
extern int atcmd_ctrl_recv(comport_t *comport, char *value, int size);

/* 解析控制命令 */
int atcmd_ctrl_parse(char *value, int size, char *ID);

/* 查看 AT 命令通信是否正常 */
extern int atcmd_at(comport_t *comport);

/* 重启模块 */
extern int atcmd_nrb(comport_t *comport);

/*  关闭新消息指示，使用AT+NMGR接收指令 */
int atcmd_nnmi0(comport_t *comport);

/* 查询信号强度 */
extern int atcmd_csq(comport_t *comport);

/* 查询网络注册状态 */
extern int atcmd_cereg(comport_t *comport);

/* 查询模块附着网络状态 */
extern int atcmd_cgatt(comport_t *comport);

/* 设置模块开始附着网络 */
extern int atcmd_cgatt1(comport_t *comport);

/* 查询模块IP地址 */
extern int atcmd_cgpaddr(comport_t *comport);

/* 查询在云平台注册情况 */
extern int atcmd_nmstatus(comport_t *comport);

/* 设置云平台IP和端口 */
extern int atcmd_ncdp(comport_t *comport, char *ip, char *port);

/* 查询设备唯一标识码IMEI */
extern int atcmd_cgsn(comport_t *comport, char *imei, int size);

/* 查询国际移动用户标识码ISMI */
extern int atcmd_cimi(comport_t *comport, char *imsi, int size);

/* 查看模块型号 */
extern int atcmd_cgmm(comport_t *comport, char *name, int size);

/* 查看模块的版本信息 */                                
extern int atcmd_cgmr(comport_t *comport, char *version, int size);

/* 模块产品信息 */                                
extern int atcmd_ati(comport_t *comport, nbiot_info_t *info);

/* 上报数据 */
extern int atcmd_qlwuldataex(comport_t *comport, char *data);

/* 设置回显 */
extern int atcmd_ate(comport_t *comport, int enabled);

/* 设置模块联网模式 */
extern int atcmd_nconfig(comport_t *comport, int enabled);

/* 模块射频开关 */
extern int atcmd_cfun(comport_t *comport, int enabled);

/* 设置模块连接云平台模式 */
extern int atcmd_qregswt(comport_t *comport, int enabled);

/* 注册云平台 */
extern int atcmd_qlwsregind(comport_t *comport, int enabled);

/* 设置模块的工作频段 */
extern int atcmd_nband(comport_t *comport, char *band);


#endif
