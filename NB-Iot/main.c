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
#include "sht20.h"

#define I2C_DEV         "/dev/i2c-1"

int main (int argc, char **argv)
{
    int         n;
    int         rv = 0;
	int			ret = 0;
    char        dev[20] = "/dev/ttyUSB0";
    long        baudrate = 9600;
    char        conf[] = "8N1";
    char        data[] = "9,02000F000400000010";
    char        buf[1024];
	char        ip[] = "221.229.214.202";
	char        port[] = "5683";
	char		LED_ID[] = "1F42";
	char		ctrl_cmd[32];
    comport_t   com;
	int			sht20_fd;
	char		temp_str[20];
	char		rh_str[20];
	uint8_t		serialnumber[8];

    if (comport_open(&com, dev, baudrate, conf) < 0)
    {
        dbg_print ("comport open failure!\n");
        return -1;
    }
    dbg_print ("comport open successfully!\n");

	sht20_fd = sht2x_init(I2C_DEV);
	if (sht20_fd < 0)
	{
			printf ("SHT2x initialize failure\n");
			return -2;
	}

	if ( sht2x_get_serialnumber(sht20_fd, serialnumber, 8) < 0 )
	{
			printf ("SHT2x get serial number failure\n");
			ret = -3;
			goto CleanUp;
	}

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

		if ( sht2x_sample(sht20_fd, temp_str, rh_str, 20) < 0 )
		{
				printf ("sht20 sample failure\n");
				ret = -4;
				goto CleanUp;
		}
		dbg_print ("temp_str:%s\n", temp_str);
		dbg_print ("rh_str:%s\n", rh_str);
	
		//data[] = "9,02000F000400000010";
		//上报温度
		memset(data, 0, sizeof(data));
		snprintf(data, 22, "9,%s", temp_str);
		rv = atcmd_qlwuldataex(&com, data);
		if ( rv < 0 )
		{
			dbg_print ("atcmd_qlwuldataex error!\n");
		}

		//上报相对湿度
		memset(data, 0, sizeof(data));
		snprintf(data, 22, "9,%s", rh_str);
		rv = atcmd_qlwuldataex(&com, data);
		if ( rv < 0 )
		{
			dbg_print ("atcmd_qlwuldataex error!\n");
		}

		sleep(1);
		rv = atcmd_ctrl_recv(&com, ctrl_cmd, sizeof(ctrl_cmd), 10000);
		if ( rv < 0 )
		{
			dbg_print ("atcmd_ctrl_recv error!\n");
		}
		else if ( 0==rv )
		{
			printf ("Value:%s\n", ctrl_cmd);
			rv = atcmd_ctrl_parse(ctrl_cmd, strlen(ctrl_cmd), LED_ID);
			if ( rv < 0 )
			{
				dbg_print ("atcmd_ctrl_parse error1\n");
			}
		}
    }

CleanUp:
    comport_close(&com);
    dbg_print ("comport closed!\n");
	close(sht20_fd);
    return ret;
}

