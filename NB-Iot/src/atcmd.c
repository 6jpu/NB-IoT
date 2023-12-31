/*********************************************************************************
 *      Copyright:  (C) 2023 Nbiot<lingyun@gail.com>
 *                  All rights reserved.
 *
 *       Filename:  atcmd.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(12/06/23)
 *         Author:  Nbiot <lingyun@gail.com>
 *      ChangeLog:  1, Release initial version on "12/06/23 19:37:15"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "atcmd.h"
#include <termios.h>
#include <unistd.h>

/*  发送AT命令
  *  comport:打开的串口，atcmd:发送的AT命令，
  *  expect:期望的返回值，error:出错返回值
  *  reply:存放AT命令返回内容，size：reply大小，
  *  timeout:超时时间
  *  */
int send_atcmd(comport_t *comport, char *atcmd, char *expect, char *error, char *reply, int size, int timeout)
{
	 int          i;
     int          bytes = 0;
     int          rv = 0;
	 int          res;
	 char         buf[ATCMD_REPLY_LEN] = {'\0'};

	 if( !comport || !atcmd )
	 {
		 PARSE_LOG_ERROR ("Invalid input arguments\n");
		 return -1;
	 }
	 
	 if( comport->fd <= 0 )
	 {
		 PARSE_LOG_ERROR ("comport[%s] not opened\n", comport->devname);
		 return -2;
	 }


	 rv = comport_send(comport, atcmd, strlen(atcmd));
     if (rv < 0)
	 {
		 PARSE_LOG_ERROR ("comport_send error!\n");
		 return -3;
	 }

	 res = ATRES_TIMEOUT;
	 memset(buf, 0, sizeof(buf));

	 for(i=0; i<timeout/10; i++)
	 {
		   if( bytes >= sizeof(buf) )
			   break;

		   rv = comport_recv(comport, buf+bytes, sizeof(buf)-bytes, 10);
           if( rv < 0 )
           {
			   PARSE_LOG_ERROR ("comport_recv error!\n");
			   return -4;
           }

		   bytes += rv;
		   PARSE_LOG_DEBUG ("send_atcmd buf:%s\n", buf);

		   if (expect && strstr(buf, expect))
		   {
			   res = ATRES_EXPECT;
			   break;
		   }
		   
		   if (error && strstr(buf, error))
		   {
			   res = ATRES_ERROR;
			   break;
		   }

	 }
	 
	 if (bytes > 0)
	 {
		 PARSE_LOG_INFO ("comport_recv buf:%s\n", buf);
	 }

	 if (reply && size>0)
	 {
		 bytes = strlen(buf)>size ? size : strlen(buf);
		 memset(reply, 0, size);
		 strncpy(reply, buf, bytes);
	 }

	 if (res == ATRES_TIMEOUT)
	 {
		 PARSE_LOG_INFO ("comport receive timeout!\n");
	 }

	 return res;
}


/* 获取AT命令回复的信息，通过输入首尾字符或字符串来获取需要的内容
 * 成功返回0，出错返回负数
 * */
int str_fetch(char *fetched, int size, const char *src, const char *start, const char *end) 
{
    int    length;
    char  *start_p = NULL;
    char  *end_p = NULL;

    /* 参数检验 */
    if ( fetched==NULL || src==NULL || start==NULL || end==NULL)
    {   
        PARSE_LOG_ERROR ("str_fetch parameter error!\n");
        return -1; 
    }   

    /* 查找首字符 */
    start_p = strstr(src, start);
    if (start_p==NULL)
    {   
        PARSE_LOG_ERROR ("未找到首字符！\n");
        return -2; 
    }   
    start_p += strlen(start);

    /* 查找尾字符 */
    end_p = strstr(start_p, end);
    if ( end_p==NULL )
    {   
        PARSE_LOG_ERROR ("未找到尾字符！\n");
        return -3; 
    }   

    length = end_p - start_p;
	length = (length>size) ? size : length;

    strncpy(fetched, start_p, length);
    fetched[length] = '\0';

    return 0;
}

/*
 *  Description: Send AT command which will only reply by "OK" or "ERROR", such as AT:
 *                 Reply:   \r\nOK\r\n
 *  Return Value: 0: OK     -X: Failure
 */
int send_atcmd_check_ok(comport_t *comport, char *at, int timeout)
{
    int     rv;

    if( !comport || !at )
    {
        PARSE_LOG_ERROR ("Invalid input arguments\n");
        return -1;
    }

    rv=send_atcmd(comport, at, AT_OKSTR, AT_ERRSTR, NULL, 0, timeout);
    if( ATRES_EXPECT == rv )
    {
        return 0;
    }
    else
    {
		PARSE_LOG_DEBUG("send_atcmd rv = %d\n", rv);
        return -2;
    }
}


