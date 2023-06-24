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

#include "nbiot.h"

/* 检查NB模块的网络附着情况 */
int nbiot_attach_check(comport_t *comport, nbiot_info_t *nbiot)
{
	int    rv;

	rv = atcmd_at(comport)
	if ( rv < 0 )
	{
		printf ("atcmd_at error!\n");
		return -1;
	}

	/* 关闭回显 */
	rv = atcmd_ate(comport, 0 );
	if ( rv < 0 )
	{
		printf ("atcmd_ate error!\n");
		return -2;
	}
    
	rv = atcmd_csq(comport)
    if ( rv < 0 ) 
    {   
        printf ("atcmd_csq error!\n");
		return -3;
    }

    rv = atcmd_cgatt1(comport)
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cgatt1 error!\n");
		return -4;
    }

    rv = atcmd_cgreg(comport)
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cgreg error!\n");
		return -5;
    }

    rv = atcmd_cgatt(comport)
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cgatt error!\n");
		return -6;
    }

    rv = atcmd_cgpaddr(comport)
    if ( rv < 0 ) 
    {   
        printf ("atcmd_cgpaddr error!\n");
		return -7;
    }

    return 0;
}

