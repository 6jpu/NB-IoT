/********************************************************************************
 *      Copyright:  (C) 2023 Nbiot<lingyun@gail.com>
 *                  All rights reserved.
 *
 *       Filename:  nbiot.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(24/06/23)
 *         Author:  Nbiot <lingyun@gail.com>
 *      ChangeLog:  1, Release initial version on "24/06/23 16:44:42"
 *                 
 ********************************************************************************/

#ifndef  _NBIOT_H_
#define  _NBIOT_H_

#include "bc28.h"
#include "logger.h"

/* 检查NB模块的网络附着情况 */
extern int nbiot_attach_check(comport_t *comport);

/* 连接云平台 */
extern int nbiot_connect_cloud(comport_t *comport, char *ip , char *port);


#endif
