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

#define  MAX					32
#define  TIMEOUT				10000	
#define  ATCMD_REPLY_LEN		1024 /*  Max AT command reply message length */

#include "comport.h"


/* send_atcmd() return value status */
enum
{
    ATRES_EXPECT,   /* AT command reply got expect string, such as "OK\r\n" */
    ATRES_ERROR,    /* AT command reply got error string, such as "ERROR\r\n" */
    ATRES_TIMEOUT,  /* AT command not get error/expect string, receive util timeout */
};

#define AT_OKSTR                 "OK"     /*  expect string always be OK */
#define AT_ERRSTR                "ERROR"  /*  error string always be ERROR */

/* 发送AT命令 */
extern int send_atcmd(comport_t *comport, char *atcmd, char *expect, char *error, char *reply, int size, int timeout);

/* 获取AT命令回复的信息 */
extern int str_fetch(char *fetched, int size, const char *src, const char *start, const char *end);


extern int send_atcmd_check_ok(comport_t *comport, char *at, int timeout);

#endif
