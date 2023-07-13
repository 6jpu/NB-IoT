/*********************************************************************************
 *      Copyright:  (C) 2023 Nbiot<lingyun@gail.com>
 *                  All rights reserved.
 *
 *       Filename:  nbiot.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(24/06/23)
 *         Author:  Nbiot <lingyun@gail.com>
 *      ChangeLog:  1, Release initial version on "24/06/23 16:44:11"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "nbiot.h"

/* 检查NB模块的网络附着情况 */
int nbiot_attach_check(comport_t *comport)
{
	int    rv;

	rv = atcmd_at(comport);
	if ( rv < 0 )
	{
		printf ("atcmd_at error!\n");
		return -1;
	}

	/* 关闭回显 */
 	rv = atcmd_ate(comport, DISABLE );
	if ( rv < 0 )
	{
		printf ("atcmd_ate error!\n");
		return -2;
	}

	/* 设置手动联网 */
    rv = atcmd_nconfig(comport, DISABLE);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_nconfig_0 error!\n");
        return -3; 
    }   

	/* 设置手动连接云平台 */
    rv = atcmd_qregswt(comport, DISABLE);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_qregswt_0 error!\n");
        return -3; 
    }

	/* 重启模块 */
    rv = atcmd_nrb(comport);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_nrb error!\n");
        return -4; 
    }

	/* 关闭新消息指示 */
	rv = atcmd_nnmi0(comport);
	if (rv < 0)
	{
		printf ("atcmd_nnmi0 error!\n");
		return -5;
	}

	/* 关闭射频 */
    rv = atcmd_cfun(comport, DISABLE);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cfun_0 error!\n");
        return -6; 
    }   

	/* 设置相应频段 */
    rv = atcmd_nband(comport, "5,8");
    if ( rv < 0 ) 
    {   
		printf ("atcmd_nband error!\n");
        return -7; 
    } 
  
	/*  打开射频 */ 
	rv = atcmd_cfun(comport, ENABLE);
	if ( rv < 0 )
	{
		printf ("atcmd_cfun_1 error!\n");
		return -8;
	}

	/* 查询信号强度 */
	rv = atcmd_csq(comport);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_csq error!\n");
		return -9;
    }

	/* 附着网络 */
    rv = atcmd_cgatt1(comport);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cgatt1 error!\n");
		return -10;
    }

	/* 查询网络注册状态 */
    rv = atcmd_cereg(comport);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cereg error!\n");
		return -11;
    }

	/* 查询模块附着网络状态 */
    rv = atcmd_cgatt(comport);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cgatt error!\n");
		return -12;
    }

	/* 查询模块获取到的 IP 地址 */
    rv = atcmd_cgpaddr(comport);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cgpaddr error!\n");
		return -13;
    }

    return 0;
}


/* 连接云平台 */
int nbiot_connect_cloud(comport_t *comport, char *ip , char *port)
{
    int       rv;
 
    rv = atcmd_ncdp(comport, ip, port);
    if( rv < 0 )
    {
        printf("atcmd_ncdp error!\n");
        return -1;
    }

	/* 开始注册 */
    rv = atcmd_qlwsregind(comport, DISABLE);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_qlwsregind_0 error!\n");
        return -3; 
    }   

	sleep(10);
    rv = atcmd_nmstatus(comport);
    if ( rv < 0 ) 
    {   
        printf ("atcmd_nmstatus error!\n");
        return -3; 
    }   
    return 0;
}
