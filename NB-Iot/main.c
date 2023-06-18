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
#include "atcmd.h"
#include "comport.h"

int main (int argc, char **argv)
{
    int         n;
    int         rv = 0;
    char        dev[20] = "/dev/ttyUSB0";
    long        baudrate = 9600;
    char        conf[8] = "8N1";
    char        data[16] = "AT\r\n";
    char        buf[1024];
    comport_t   com;

    if (comport_open(&com, dev, baudrate, conf) < 0)
    {
        printf ("comport open failure!\n");
        return -1;
    }
    printf ("comport open successfully!\n");

    for ( ; ; )
    {
        memset(data, 0, sizeof(data));
        printf ("Data to comport:");
        scanf ("%s", &data);
        strcat (data,"\r\n");

		memset(buf, 0, sizeof(buf));
		rv = send_atcmd(&com, data, buf, sizeof(buf), 10);
		if (rv < 0)
		{
			printf ("send_atcmd error!\n");
			goto CleanUp;
		}


		printf ("Data from comport:%d\n", strlen(buf));
		printf ("%s\n", buf);

/* 		
        if (comport_send(&com, data, strlen(data)) < 0)
        {
            printf ("send data failure!\n");
            rv =  -1;
            goto CleanUp;
        }
        printf ("send data successfully!\n");

        memset (buf, 0, sizeof(buf));
        if (comport_recv(&com, buf, sizeof(buf), 5) < 0)
		{
            printf ("receive data failure!\n");
            rv =  -2;
            goto CleanUp;
        }

        printf ("Data from comport:\n");
        printf ("%s", buf);
*/
    }

CleanUp:
    comport_close(&com);
    printf ("comport closed!\n");
    return rv;
}

