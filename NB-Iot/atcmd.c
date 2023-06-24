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
#include "atcmd.h"

 /*  发送AT命令
  *  comport:打开的串口，atcmd:发送的AT命令，buf:存放AT命令返回内容，
  *  size：buf大小，timeout:超时时间
  *  */
int send_atcmd(comport_t *comport, char *atcmd, char *buf, int size, int timeout)
{
     int bytes = 0;
     int rv = 0;

	 rv = comport_send(comport, atcmd, strlen(atcmd));
     if (rv < 0)
	 {
		 printf ("comport_send error!\n");
		 return rv;
	 }


     while (timeout > 0) 
	 {	 

           rv = comport_recv(comport, &buf[bytes], size-bytes, 10);

           if( rv>0 )
           {
			   
                bytes+=rv;

                if (strstr(buf, "OK\r\n"))
				{
                      return 0;
				}
                else if (strstr(buf, "ERROR\r\n"))
				{
					return 1;
				}
				else
				{
					continue;
				}
				
           }
		   else
		   {
			   printf ("comport_recv error!\n");
			   return rv;
		   }

		   timeout -=10;
	 }
	 printf ("comport receive timeout!\n");
	 return -1;
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
        printf ("str_fetch parameter error!\n");
        return -1; 
    }   

    /* 查找首字符 */
    start_p = strstr(src, start);
    if (start_p==NULL)
    {   
        printf ("未找到首字符！\n");
        return -2; 
    }   
    start_p += strlen(start);

    /* 查找尾字符 */
    end_p = strstr(start_p, end);
    if ( end_p==NULL )
    {   
        printf ("未找到尾字符！\n");
        return -3; 
    }   

    length = end_p - start_p;
	length = (length>size) ? size : length;

    strncpy(fetched, start_p, length);
    fetched[length] = '\0';

    return 0;
}


/*  查看 AT 命令通信是否正常 
 *  成功 返回0，出错返回负数
 * */
int atcmd_at(comport_t *comport)
{
	int   rv;
	char  buf[8];

	if ( !comport )
	{
		printf ("atcmd_at parameter error!\n");
		return -1;
	}

	rv = send_atcmd(comport, "AT\r\n", buf, sizeof(buf),  TIMEOUT);
	if (rv < 0)
	{
		printf ("send_atcmd:AT error!\n");
		return -2;
	}

	if ( !strstr(buf, "OK") )
	{
		printf ("NB-IoT not connected...\n");
		return -3;
	}

	printf ("AT:%s\n", buf);

	return 0;
}

/*  重启模块 */
int atcmd_nrb(comport_t *comport)
{	
	int   rv;
	char  buf[128];

	if ( !comport )
	{
		printf ("atcmd_nrb parameter error!\n");
		return -1;
	}

	rv = send_atcmd(comport, "AT+NRB\r\n", buf, sizeof(buf),  TIMEOUT);
	if (rv < 0)
	{
		printf ("send_atcmd:AT+NRB error!\n");
		return -2;
	}

	if ( !strstr(buf, "OK") )
	{
		printf ("Reboot error!\n");
		return -3;
	}

	printf ("AT:%s\n", buf);

	return 0;
}

/*  查询信号强度
 *  信号正常返回0,出错返回负数
 *  */
int atcmd_csq(comport_t *comport)
{
	int   rv;
	int   csq;
	char  buf[32];
	char  csq_str[2];

	if ( !comport )
	{
		printf ("atcmd_csq parameter error!\n");
		return -1;
	}

	rv = send_atcmd(comport, "AT+CSQ\r\n", buf, sizeof(buf),  TIMEOUT);
	if (rv < 0)
	{
		printf ("send_atcmd:AT+CSQ error!\n");
		return -2;
	}

	if ( !strstr(buf, "OK") )
	{
		printf ("Get signal strength error!\n");
		return -3;
	}

	/* 获取信号质量 */
	rv = str_fetch(csq_str, sizeof(csq_str), buf, ":", ",");
	if (rv < 0)
	{
		printf ("fetch csq error!\n");
		return -4;
	}
    /* 检测信号质量 */
	csq = atoi(csq_str);
	if (csq==99 || csq < 10)
	{
		printf ("signal strength poor or unknown\n");
		return -4;
	}

	printf ("AT+CSQ:%s\n", buf);

	return 0;
}

/*  查询网络注册状态
 *  注册返回0，未注册返回1，出错返回负数
 *  */
int atcmd_cereg(comport_t *comport)
{
    int   rv; 
    char  buf[32];

    if ( !comport )
    {   
        printf ("atcmd_cereg parameter error!\n");
        return -1; 
    }   

    rv = send_atcmd(comport, "AT+CEREG?\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CEREG? error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Network registration status error!\n");
        return -3; 
    }

	/* 判断网络是否注册 */
	if ( !strstr(buf, "0,1") )
	{
		printf ("Network not registered!\n");
		return -4;
	}

	printf ("AT+CEREG?:%s\n", buf);

	return 0;
}          
                                              
/*  查询模块附着网络状态 
 *  附着返回0，未附着返回1，出错返回负数
 *  */                    
int atcmd_cgatt(comport_t *comport)
{
    int   rv; 
    char  buf[32];

    if ( !comport )
    {   
        printf ("atcmd_cgatt parameter error!\n");
        return -1; 
    }   

    rv = send_atcmd(comport, "AT+CGATT?\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CGATT? error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("query attach status error!\n");
        return -3; 
    }

	/* Detached */
	if ( strstr(buf, "0") )
	{
		printf ("Detached\n");
		return -4;
	}

	printf ("AT+CGATT?:%s\n", buf);
	return 0;

}   

/*  设置模块开始附着网络 
 *  成功返回0，出错返回负数
 *  */
int atcmd_cgatt1(comport_t *comport)
{
    int   rv; 
    char  buf[24];

    if ( !comport )
    {   
        printf ("atcmd_cgatt1 parameter error!\n");
        return -1; 
    }   

    rv = send_atcmd(comport, "AT+CGATT=1\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CGATT=1 error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("ps attach error!\n");
        return -3; 
    }

	printf ("AT+CGATT1=1:%s\n", buf);

	return 0;
}

/*  查询模块IP地址 
 *  成功返回0，出错返回负数
 *  */
int atcmd_cgpaddr(comport_t *comport)
{
    int   rv; 
    char  buf[64];

    if ( !comport )
    {   
        printf ("atcmd_cgpaddr parameter error!\n");
        return -1; 
    }   

    rv = send_atcmd(comport, "AT+CGPADDR\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CGPADDR error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("show PDP addresses error!\n");
        return -3; 
    }

	printf ("AT+CGPADDR:%s\n", buf);

	return 0;
}

/*  设置云平台IP和端口 
 *  成功返回0，出错返回负数
 *  */
int atcmd_ncdp(comport_t *comport, char *ip, char *port)
{
    int   rv; 
    char  buf[64];
	char  cmd[32];

    if ( !comport && !ip && !port )
    {   
        printf ("atcmd_csq parameter error!\n");
        return -1; 
    }   

	snprintf(cmd, sizeof(cmd), "AT+NCDP=%s,%s", ip, port);
    rv = send_atcmd(comport, cmd, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:%s error!\n", cmd);
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("configure server IP address and port error!\n");
        return -3; 
    }

	printf ("%s:%s\n", cmd, buf);
	return 0;
}

/*  查询设备唯一标识码IMEI */
int atcmd_cgsn(comport_t *comport, char *imei, int size)
{
    int   rv; 
    char  buf[64];
    
	if ( !comport && !imei && !size )
    {   
        printf ("atcmd_cgsn parameter error!\n");
        return -1; 
    }   

    rv = send_atcmd(comport, "AT+CGSN=1\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CGSN=1 error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Request product serial number error!\n");
        return -3;
    }

	rv = str_fetch(imei, size, buf, ":", "\r\n");
	if (rv < 0)
	{
		printf ("fetch imei error!\n");
		return -4;
	}

	printf ("AT+CGSN=1:%s\n", buf);
	return 0;
}

/*  查询国际移动用户标识码IMSI */
int atcmd_cimi(comport_t *comport, char *imsi, int size)
{
    int   rv; 
    char  buf[32];
	char  imsi_str[32];

    if ( !comport && !imsi && !size)
    {   
        printf ("atcmd_cimi parameter error!\n");
        return -1; 
    }   

    rv = send_atcmd(comport, "AT+CIMI\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CIMI error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Request imsi error!\n");
        return -3; 
    }

	rv = str_fetch(imsi_str, sizeof(imsi_str), buf, "460" ,"\r\n");
	if (rv < 0)
	{
		printf ("fetch imsi error!\n");
		return -4;
	}
	sprintf(imsi, "460%s", imsi_str);

	printf ("AT+CIMI:%s\n",buf);

	return 0;
}

/*  查看模块型号 */
int atcmd_cgmm(comport_t *comport, char *name, int size)
{
    int   rv; 
    char  buf[32];
	char  name_str[32];

    if ( !comport && !name && !size)
    {   
        printf ("atcmd_cgmm parameter error!\n");
        return -1; 
    }   

    rv = send_atcmd(comport, "AT+CGMM\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CGMM error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Request manufacturer model error!\n");
        return -3; 
    }
	
    rv = str_fetch(name_str, sizeof(name_str), buf, "BC" ,"\r\n");
    if (rv < 0)
    {   
        printf ("fetch manufacturer model error!\n");
        return -4; 
    }   
	snprintf(name, size, "BC%s", name_str); 

    printf ("AT+CIMI:%s\n",buf);

    return 0;

}

/*  查看模块的版本信息 */
int atcmd_cgmr(comport_t *comport, char *version, int size)
{
    int   rv; 
    char  buf[256];

    if ( !comport && !version && !size)
    {   
        printf ("atcmd_cgmr parameter error!\n");
        return -1; 
    }   

	/* 打开回显便于进行信息解析 */
	if ( atcmd_ate(comport, 1) < 0 )
	{
		printf ("atcmd_ate error!\n");
		return -2;
	}

    rv = send_atcmd(comport, "AT+CGMR\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:AT+CGMR error!\n");
        return -3; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Request manufacturer revision error!\n");
        return -4; 
    }

    rv = str_fetch(version, size, buf, "\r\n" ,"OK");
    if (rv < 0)
    {   
        printf ("fetch manufacturer revision error!\n");
        return -5; 
    }   

    printf ("AT+CGMR:%s\n",buf);

	/* 关闭回显节约资源 */
	if ( atcmd_ate(comport, 0) < 0)
	{
		printf ("atcmd_ate error!\n");
		return -6;
	}

    return 0;
}

/*  模块产品信息 */
int atcmd_ati(comport_t *comport, nbiot_info_t *info)
{
    int   rv;
	int   t;
	char  buf[64];
	char *p;
	char *token[5];//定义指针数组存放解析出来的数据

    if ( !comport && !info )
    {   
        printf ("atcmd_csq parameter error!\n");
        return -1; 
    }
   
    rv = send_atcmd(comport, "ATI\r\n", buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:ATI error!\n");
        return -3; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Display product identification information error!\n");
        return -4; 
    }

	/* 解析数据 */
    p = strtok(buf, "\r\n");
    while (p != NULL) {
         token[t] = p;
         t +=1;
         p = strtok(NULL, "\r\n");
    }
	strcpy(info->manufacturer, token[1]);
	strcpy(info->module_name, token[2]);
	strcpy(info->version, token[3]);

    printf ("AT+CGMR:%s\n", buf);

	return 0;
}

/*  上报数据 */
int atcmd_qlwuldataex(comport_t *comport, char *data)
{
    int   rv; 
    char  buf[64];
	char  cmd[64];

    if ( !comport && !data )
    {   
        printf ("atcmd_qlwuldataex parameter error!\n");
        return -1; 
    }   

	snprintf(cmd, sizeof(cmd), "AT+QLWULDATEX=%s,0x0100\r\n", data);
    rv = send_atcmd(comport, cmd, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:%s error!\n", cmd);
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Report data error!\n");
        return -3; 
    }

	printf ("%s:%s\n", cmd, buf);

	return 0;
}

/*  设置回显 */
int atcmd_ate(comport_t *comport, int flag)
{
    int   rv; 
    char  buf[32];
	char  cmd[16];

    if ( !comport )
    {   
        printf ("atcmd_ate parameter error!\n");
        return -1; 
    }  
	if (flag == 0)
	{
		strcpy(cmd, "AT+ATE0\r\n");
	}
	else 
	{
		strcpy(cmd, "AT+ATE1\r\n");
	}


    rv = send_atcmd(comport, cmd, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:%s error!\n", cmd);
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Set echo error!\n");
        return -3; 
    }

	printf ("%s:%s\n", cmd, buf);

	return 0;
}

/*  设置模块联网模式 
 *  flag=0 设置手动联网
 *  flag=0 设置自动联网
 *  成功返回0，出错返回负数
 *  */
int atcmd_nconfig(comport_t *comport, int flag)
{
    int   rv; 
    char  buf[40];
	char  cmd[33];

    if ( !comport )
    {   
        printf ("atcmd_nconfig parameter error!\n");
        return -1; 
    }  
	if (flag == 0)
	{
		strcpy(cmd, "AT+NCONFIG=AUTOCONNECT,FALSE\r\n");
	}
	else 
	{
		strcpy(cmd, "AT+NCONFIG=AUTOCONNECT,TRUE\r\n");
	}


    rv = send_atcmd(comport, cmd, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:%s error!\n", cmd);
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Configure UE behaviour error!\n");
        return -3; 
    }

	printf ("%s:%s\n", cmd, buf);

	return 0;
}

/*  模块射频开关 
 *  flag=0 关闭射频模块
 *  flag=0 打开射频模块
 *  成功返回0，出错返回负数
 *  */
int atcmd_cfun(comport_t *comport, int flag)
{
    int   rv; 
    char  buf[16];
	char  cmd[16];

    if ( !comport )
    {   
        printf ("atcmd_cfun parameter error!\n");
        return -1; 
    }  
	if (flag == 0)
	{
		strcpy(cmd, "AT+CFUN=0\r\n");
	}
	else 
	{
		strcpy(cmd, "AT+CFUN=1\r\n");
	}


    rv = send_atcmd(comport, cmd, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:%s error!\n", cmd);
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Set UE functionality error!\n");
        return -3; 
    }

	printf ("%s:%s\n", cmd, buf);

	return 0;
}
/*  设置模块的工作频段 
 *  例：atcmd_nband(&comport, "5,8");
 *  成功返回0，出错返回负数
 *  */
int atcmd_nband(comport_t *comport, char *band)
{
    int   rv; 
    char  buf[32];
	char  cmd[16];

    if ( !comport && !band )
    {   
        printf ("atcmd_nband parameter error!\n");
        return -1; 
    }   

	snprintf(cmd, sizeof(cmd), "AT+NBAND=%s\r\n", band);
    rv = send_atcmd(comport, cmd, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        printf ("send_atcmd:%s error!\n", cmd);
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        printf ("Set supported bands error!\n");
        return -3; 
    }

	printf ("%s:%s\n", cmd, buf);

	return 0;
}
