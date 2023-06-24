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

#ifndef  _ATCMD_H_
#define  _ATCMD_H_

#define  MAX   32
#define  TIMEOUT 10

#include "comport.h"

typedef struct nbiot_info_s
{
    char     manufacturer[MAX];
    char     module_name[MAX];
	char     sn[MAX];
    char     imei[MAX];
    char     ismi[MAX];
    char     version[MAX];
}nbiot_info_t;

/* 发送AT命令 */
int send_atcmd(comport_t *comport, char *atcmd, char *buf, int size, int timeout);

/* 获取AT命令回复的信息 */
int str_fetch(char *fetched, int size, const char *src, const char *start, const char *end);

/* 查看 AT 命令通信是否正常 */
int atcmd_at(comport_t *comport);

/* 查询信号强度 */
int atcmd_csq(comport_t *comport);

/* 查询网络注册状态 */
int atcmd_cereg(comport_t *comport);

/* 查询模块附着网络状态 */
int atcmd_cgatt(comport_t *comport);

/* 设置模块开始附着网络 */
int atcmd_cgatt1(comport_t *comport);

/* 查询模块IP地址 */
int atcmd_cgpaddr(comport_t *comport);

/* 设置云平台IP和端口 */
int atcmd_ncdp(comport_t *comport, char *ip, char *port);

/* 查询设备唯一标识码IMEI */
int atcmd_cgsn(comport_t *comport, char *imei, int size);

/* 查询国际移动用户标识码ISMI */
int atcmd_cimi(comport_t *comport, char *imsi, int size);

/* 查看模块型号 */
int atcmd_cgmm(comport_t *comport, char *name, int size);

/* 查看模块的版本信息 */                                
int atcmd_cgmr(comport_t *comport, char *version, int size);

/* 模块产品信息 */                                
int atcmd_ati(comport_t *comport, nbiot_info_t *info);

/* 上报数据 */
int atcmd_qlwuldataex(comport_t *comport, char *data);

/* 设置回显 */
int atcmd_ate(comport_t *comport, int flag);

#endif
