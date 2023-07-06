/*********************************************************************************
 *      Copyright:  (C) 2023 Nbiot<lingyun@gail.com>
 *                  All rights reserved.
 *
 *       Filename:  bc28.c
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
#include "bc28.h"
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

#define CONFIG_PRINT_STDOUT

#if ( defined CONFIG_PRINT_STDOUT )
#define dbg_print(format,args...) printf(format, ##args)

#else
#define dbg_print(format,args...) do{} while(0);
#endif


/* 接受云平台控制命令
 * value 存放接受的数部分，size 为value 大小
 * 成功返回0，出错返回负数
 * */
int atcmd_ctrl_recv(comport_t *comport, char *value, int size, int timeout)
{
	int        rv = 0;
	int        res = 1;
	char      *ptr = NULL;
	int        i = 0;
	int        j = 0;
	int        bytes = 0;
	char       buf[1024];

	if ( !comport || !value)
	{
		dbg_print("invalid input arugments\n");
		return -1;
	}

	memset(value, 0, size);

	for(i=0; i<timeout/10; i++)
	{
		if( bytes >= sizeof(buf) )
		    break;

		rv = comport_recv(comport, buf+bytes, sizeof(buf)-bytes, 10);
		if( rv < 0 )
	    {
	   	    dbg_print("comport_recv error!\n");
		    return -2;
	    }

	    bytes += rv;
//		   dbg_print("send_atcmd buf:%s\n", buf);

		ptr = strstr(buf, "+NNMI:");
	    if ( ptr )
	    {
			ptr = strchr(ptr, ',');  /* found ',' before the data */
			if( !ptr )
			{
				dbg_print("Not found ',' before value\n");
				continue;
			}
			ptr++;   /* skip ','  */

			while(*ptr!='\r' && j<size-1)
			{
				if( !isspace(*ptr) && *ptr!='\"') /* skip space,\r,\n ...  */
					value[j++] = *ptr;
				ptr ++;
			}

			ptr++; /* skip  */

		    res = ATRES_EXPECT;
		    break;
	    }

		/* Clear non-control message */
		bytes -= rv;
		memset(buf, 0, sizeof(buf));
	}

	if (bytes > 0)
	{
		dbg_print("comport_recv ctrl buf:%s\n", buf);
	}

	return res;
}

/* 解析控制命令
 * value 为接受的数据，size 为value 大小，ID 为服务ID
 * 成功返回0，出错返回负数
 * */
int atcmd_ctrl_parse(char *value, int size, char *ID)
{
	if ( !value || !ID)
	{
		dbg_print("invalid input arugments\n");
		return -1;
	}

	// 判断是否为下发控制指令
    if (value[0] != '0' || value[1] != '6') 
	{
        dbg_print("not control message!\n");
        return -2;
    }

    // 判断是否为相应ID
    if (strncmp(value + 2, ID, 4) != 0)
	{
        dbg_print("ID does't match!\n");
        return -3;
    }

    // 控制指令
    if (value[size - 2] == '0' && value[size - 1] == '1')
    {
        dbg_print("开灯\n");
    }
	else if (value[size - 2] == '0' && value[size - 1] == '0') 
    {
        dbg_print("关灯\n");
    }
	else
    {
        dbg_print("control message error!\n");
		return -4;
    }

    return 0;
}

/*  查看 AT 命令通信是否正常 
 *  成功 返回0，出错返回负数
 * */
int atcmd_at(comport_t *comport)
{
	int   rv;
	char  at[] = "AT\r\n"; 

    if( !comport )
    {
        dbg_print("invalid input arugments\n");
        return -1;
    }

	rv = send_atcmd_check_ok(comport, at, 500);
	if ( rv < 0)
	{
		dbg_print ("NB-IoT not connected...\n");
		return -2;
	}

	return 0;
}

/*  重启模块 */
int atcmd_nrb(comport_t *comport)
{	
	int   rv;
	char  buf[1024];

	if ( !comport )
	{
		dbg_print ("atcmd_nrb parameter error!\n");
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	rv = send_atcmd(comport, "AT+NRB\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);

	dbg_print ("atcmd_nrb buf:%s\n", buf);

	if (rv < 0)
	{
		dbg_print ("send_atcmd:AT+NRB error!\n");
		return -2;
	}

	if ( rv != ATRES_EXPECT  && !strstr(buf, "REBOOTING"))
	{
		dbg_print ("Reboot error!\n");
		return -3;
	}

	dbg_print ("AT+NRB %s\n", buf);

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
		dbg_print ("atcmd_csq parameter error!\n");
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	rv = send_atcmd(comport, "AT+CSQ\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
	if (rv < 0)
	{
		dbg_print ("send_atcmd:AT+CSQ error!\n");
		return -2;
	}

	if ( !strstr(buf, "OK") )
	{
		dbg_print ("Get signal strength error!\n");
		return -3;
	}

	/* 获取信号质量 */
	rv = str_fetch(csq_str, sizeof(csq_str), buf, ":", ",");
	if (rv < 0)
	{
		dbg_print ("fetch csq error!\n");
		return -4;
	}
    /* 检测信号质量 */
	csq = atoi(csq_str);
	if (csq==99 || csq < 10)
	{
		dbg_print ("signal strength poor or unknown\n");
	//	return -4;
	}

	dbg_print ("AT+CSQ %s\n", buf);

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
        dbg_print ("atcmd_cereg parameter error!\n");
        return -1; 
    }

	/* wait to register */
	sleep(5);

	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "AT+CEREG?\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:AT+CEREG? error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("Network registration status error!\n");
        return -3; 
    }

	/* 判断网络是否注册 */
	if ( !strstr(buf, "0,1") )
	{
		dbg_print ("Network not registered!\n");
		return -4;
	}

	dbg_print ("AT+CEREG? %s\n", buf);

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
        dbg_print ("atcmd_cgatt parameter error!\n");
        return -1; 
    }   

	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "AT+CGATT?\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:AT+CGATT? error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("query attach status error!\n");
        return -3; 
    }

	/* Detached */
	if ( strstr(buf, "0") )
	{
		dbg_print ("Detached\n");
		return -4;
	}

	dbg_print ("AT+CGATT? %s\n", buf);
	return 0;

}   

/*  设置模块开始附着网络 
 *  成功返回0，出错返回负数
 *  */
int atcmd_cgatt1(comport_t *comport)
{
    int   rv; 
    char  at[] = "AT+CGATT=1\r\n";

    if ( !comport )
    {   
        dbg_print ("atcmd_cgatt1 parameter error!\n");
        return -1; 
    }   

	rv = send_atcmd_check_ok(comport, at, 1000);
    if ( rv < 0 )
    {   
        dbg_print ("ps attach error!\n");
        return -3; 
    }

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
        dbg_print ("atcmd_cgpaddr parameter error!\n");
        return -1; 
    }   

	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "AT+CGPADDR\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:AT+CGPADDR error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("show PDP addresses error!\n");
        return -3; 
    }

	dbg_print ("AT+CGPADDR %s\n", buf);

	return 0;
}

/* 查询在云平台注册情况 */
int atcmd_nmstatus(comport_t *comport)
{
    int   rv;
	char  buf[64];

	if ( !comport )
	{
		dbg_print ("atcmd_nmstatus parameter error!\n");
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	rv = send_atcmd(comport, "AT+NMSTATUS?\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
	if (rv < 0)
	{
		dbg_print ("send_atcmd:AT+NMSTATUS? error!\n");
		return -2;
	}

	if ( !strstr(buf, "OK") )
	{
		dbg_print ("Message registration status error!\n");
		return -3;
	}

	if ( !strstr(buf, "MO_DATA_ENABLED"))
	{
		dbg_print ("Register failed!\n");
		return -4;
	}

	dbg_print ("AT+NMSTATUS? %s\n", buf);

	return 0;
}

/*  设置云平台IP和端口 
 *  成功返回0，出错返回负数
 *  */
int atcmd_ncdp(comport_t *comport, char *ip, char *port)
{
    int   rv; 
	char  at[32];

    if ( !comport && !ip && !port )
    {   
        dbg_print ("atcmd_csq parameter error!\n");
        return -1; 
    }   

	memset(at, 0, sizeof(at));
	snprintf(at, sizeof(at), "AT+NCDP=%s,%s\r\n", ip, port);

	rv = send_atcmd_check_ok(comport, at, 1000);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}

/*  查询设备唯一标识码IMEI */
int atcmd_cgsn(comport_t *comport, char *imei, int size)
{
    int   rv; 
    char  buf[64];
    
	if ( !comport && !imei && !size )
    {   
        dbg_print ("atcmd_cgsn parameter error!\n");
        return -1; 
    }   

	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "AT+CGSN=1\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:AT+CGSN=1 error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("Request product serial number error!\n");
        return -3;
    }

	rv = str_fetch(imei, size, buf, ":", "\r\n");
	if (rv < 0)
	{
		dbg_print ("fetch imei error!\n");
		return -4;
	}

	dbg_print ("AT+CGSN=1 %s\n", buf);
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
        dbg_print ("atcmd_cimi parameter error!\n");
        return -1; 
    }   

	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "AT+CIMI\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:AT+CIMI error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("Request imsi error!\n");
        return -3; 
    }

	rv = str_fetch(imsi_str, sizeof(imsi_str), buf, "460" ,"\r\n");
	if (rv < 0)
	{
		dbg_print ("fetch imsi error!\n");
		return -4;
	}
	sprintf(imsi, "460%s", imsi_str);

	dbg_print ("AT+CIMI %s\n",buf);

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
        dbg_print ("atcmd_cgmm parameter error!\n");
        return -1; 
    }   

	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "AT+CGMM\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:AT+CGMM error!\n");
        return -2; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("Request manufacturer model error!\n");
        return -3; 
    }
	
    rv = str_fetch(name_str, sizeof(name_str), buf, "BC" ,"\r\n");
    if (rv < 0)
    {   
        dbg_print ("fetch manufacturer model error!\n");
        return -4; 
    }   
	snprintf(name, size, "BC%s", name_str); 

    dbg_print ("AT+CGMM %s\n",buf);

    return 0;

}

/*  查看模块的版本信息 */
int atcmd_cgmr(comport_t *comport, char *version, int size)
{
    int   rv; 
    char  buf[256];

    if ( !comport && !version && !size)
    {   
        dbg_print ("atcmd_cgmr parameter error!\n");
        return -1; 
    }   

	/* 打开回显便于进行信息解析 */
	if ( atcmd_ate(comport, 1) < 0 )
	{
		dbg_print ("atcmd_ate error!\n");
		return -2;
	}

	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "AT+CGMR\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:AT+CGMR error!\n");
        return -3; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("Request manufacturer revision error!\n");
        return -4; 
    }

    rv = str_fetch(version, size, buf, "\r\n" ,"OK");
    if (rv < 0)
    {   
        dbg_print ("fetch manufacturer revision error!\n");
        return -5; 
    }   

    dbg_print ("AT+CGMR %s\n",buf);

	/* 关闭回显节约资源 */
	if ( atcmd_ate(comport, 0) < 0)
	{
		dbg_print ("atcmd_ate error!\n");
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
        dbg_print ("atcmd_csq parameter error!\n");
        return -1; 
    }
   
	memset(buf, 0, sizeof(buf));
    rv = send_atcmd(comport, "ATI\r\n", AT_OKSTR, AT_ERRSTR, buf, sizeof(buf),  TIMEOUT);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:ATI error!\n");
        return -3; 
    }   

    if ( !strstr(buf, "OK") )
    {   
        dbg_print ("Display product identification information error!\n");
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

    dbg_print ("ATI %s\n", buf);

	return 0;
}

/*  上报数据 */
int atcmd_qlwuldataex(comport_t *comport, char *data)
{
    int   rv; 
	char  at[64];

    if ( !comport && !data )
    {   
        dbg_print ("atcmd_qlwuldataex parameter error!\n");
        return -1; 
    }   

	memset(at, 0, sizeof(at));
	snprintf(at, sizeof(at), "AT+QLWULDATAEX=%s,0x0100\r\n", data);

	dbg_print ("%s\n", at);		//debug
    rv = send_atcmd_check_ok(comport, at, 500); 
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}

/*  设置回显,enabled = ENABLE 开启回显,DISABLE关闭回显*/
int atcmd_ate(comport_t *comport, int enabled)
{
    int   rv; 
	char *at = enabled ? "ATE1\r\n" : "ATE0\r\n";

    if ( !comport )
    {   
        dbg_print ("atcmd_ate parameter error!\n");
        return -1; 
    }  

    rv = send_atcmd_check_ok(comport, at, 500);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}

/*  设置模块联网模式 
 *  enabled=ENABLE 设置手动联网
 *  enabled=DISABLE 设置自动联网
 *  成功返回0，出错返回负数
 *  */
int atcmd_nconfig(comport_t *comport, int enabled)
{
    int     rv; 
	char   *at = enabled ? "AT+NCONFIG=AUTOCONNECT,TRUE\r\n" : 
		"AT+NCONFIG=AUTOCONNECT,FALSE\r\n";

    if ( !comport )
    {   
        dbg_print ("atcmd_nconfig parameter error!\n");
        return -1; 
    }

    rv = send_atcmd_check_ok(comport, at, 500); 
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}

/*  模块射频开关 
 *  enabled=DISABLE 关闭射频模块
 *  enabled=ENABLE 打开射频模块
 *  成功返回0，出错返回负数
 *  */
int atcmd_cfun(comport_t *comport, int enabled)
{
    int     rv; 
	char   *at = enabled ? "AT+CFUN=1\r\n" : "AT+CFUN=0\r\n";

    if ( !comport )
    {   
        dbg_print ("atcmd_cfun parameter error!\n");
        return -1; 
    }  

    rv = send_atcmd_check_ok(comport, at, 7000); 
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}

/* 设置模块连接云平台模式 
 * enabled=DISABLE,通过 AT+QLWSREGIND 命令手动触发连接云平台
 * enabled=ENABLE,自动连接云平台
 * 成功返回0，出错返回负数
 * */
int atcmd_qregswt(comport_t *comport, int enabled)
{
    int    rv; 
	char  *at = enabled ? "AT+QREGSWT=1\r\n" : "AT+QREGSWT=0\r\n";

    if ( !comport )
    {   
        dbg_print ("atcmd_qregswt parameter error!\n");
        return -1; 
    }  

    rv = send_atcmd_check_ok(comport, at, 500); 
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}

/* 注册云平台 
 * enabled=DISABLE,开始注册云平台
 * enabled=ENABLE,撤销注册
 * 成功返回0，出错返回负数
 * */
int atcmd_qlwsregind(comport_t *comport, int enabled)
{
    int     rv; 
	char   *at = enabled ? "AT+QLWSREGIND=1\r\n" : "AT+QLWSREGIND=0\r\n";

    if ( !comport )
    {   
        dbg_print ("atcmd_qlwsregind parameter error!\n");
        return -1; 
    }  

    rv = send_atcmd_check_ok(comport, at, 500);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}

/*  设置模块的工作频段 
 *  例：atcmd_nband(&comport, "5,8");
 *  成功返回0，出错返回负数
 *  */
int atcmd_nband(comport_t *comport, char *band)
{
    int   rv; 
	char  at[16];

    if ( !comport && !band )
    {   
        dbg_print ("atcmd_nband parameter error!\n");
        return -1; 
    }   

	memset(at, 0, sizeof(at));
	snprintf(at, sizeof(at), "AT+NBAND=%s\r\n", band);

    rv = send_atcmd_check_ok(comport, at, 1000);
    if (rv < 0)
    {   
        dbg_print ("send_atcmd:%s error!\n", at);
        return -2; 
    }   

	return 0;
}
