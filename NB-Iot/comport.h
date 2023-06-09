/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年06月09日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年06月09日 21时36分44秒"
 *                 
 ********************************************************************************/

int comport_open(comport_t *comport, char *devname, long baudrate, char *conf);

int comport_close(comport_t *comport);

int comport_send(comport_t *comport, char *data, int bytes);

int comport_recv(comport_t *comport, char *buf, int size, int timeout);

