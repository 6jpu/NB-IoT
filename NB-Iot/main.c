/*********************************************************************************
 *      Copyright:  (C) 2023 Nbiot<lingyun@gail.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/06/23)
 *         Author:  Nbiot <lingyun@gail.com>
 *      ChangeLog:  1, Release initial version on "10/06/23 15:42:24"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nbiot.h"
#include "bc28.h"

#if ( defined CONFIG_PRINT_STDOUT )
#define dbg_print(format,args...) printf(format, ##args)

#else
#define dbg_print(format,args...) do{} while(0);
#endif

int main (int argc, char **argv)
{
    int         n;
    int         rv = 0;
    char        dev[20] = "/dev/ttyUSB0";
    long        baudrate = 9600;
    char        conf[] = "8N1";
    char        data[] = "9,02000F000400000010";
    char        buf[1024];
	char        ip[] = "221.229.214.202";
	char        port[] = "5683";
    comport_t   com;

    if (comport_open(&com, dev, baudrate, conf) < 0)
    {
        dbg_print ("comport open failure!\n");
        return -1;
    }
    dbg_print ("comport open successfully!\n");

    for ( ; ; )
    {
		rv = atcmd_nmstatus(&com);
		if ( rv < 0 )
		{	
			rv = nbiot_attach_check(&com);
			if ( rv < 0 )
			{
				dbg_print ("nbiot_attach_check error!\n");
				continue;
			}

			rv = nbiot_connect_cloud(&com, ip, port);
			if ( rv < 0 )
			{
				dbg_print ("nbiot_connect_cloud error!\n");
				continue;
			}
		}

		rv = atcmd_qlwuldataex(&com, data);
		if ( rv < 0 )
		{
			dbg_print ("atcmd_qlwuldataex error!\n");
		}
	
		sleep(10);
    }

CleanUp:
    comport_close(&com);
    dbg_print ("comport closed!\n");
    return rv;
}

