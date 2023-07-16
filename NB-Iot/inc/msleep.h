/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  msleep.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年07月15日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月15日 11时35分08秒"
 *                 
 ********************************************************************************/

#ifndef _MSLEEP_H_
#define _MSLEEP_H_

#include <time.h>

/* ms 级休眠函数 */
static inline void msleep(unsigned long ms)
{
    struct timespec cSleep;
    unsigned long ulTmp;

    cSleep.tv_sec = ms / 1000;
    if (cSleep.tv_sec == 0)
    {
        ulTmp = ms * 10000;
        cSleep.tv_nsec = ulTmp * 100;
    }
    else
    {
        cSleep.tv_nsec = 0;
    }

    nanosleep(&cSleep, 0);
}
#endif

