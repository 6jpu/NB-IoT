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
#include "atcmd.h"

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

