/*
 * serialTest.c:
 *	Very simple program to test the serial port. Expects
 *	the port to be looped back to itself
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int main ()
{
    int fd ;
    int count ;
    unsigned int nextTime ;

    if ((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0)
    {
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1 ;
    }

    if (wiringPiSetup () == -1)
    {
        fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
        return 1 ;
    }
    char s1[41] ={ 0xdd, 0xdd, 0xdd, 0xdd, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                   0x01, 0x01, 0x00, 0x3d, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01,
                   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xee, 0xee, 0xee, 0xee,
                   '\0'};
    char s2[41] ={ 0xdd, 0xdd, 0xdd, 0xdd, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                   0x02, 0x02, 0x00, 0x3e, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02,
                   0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                   0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xee, 0xee, 0xee, 0xee,
                   '\0'};

    nextTime = millis () + 3000 ;

    int flag = 0;
    
    while(1)
    {
        if (millis () > nextTime)
        {
            flag = (flag + 1) % 2;
            nextTime += 30000 ;
            if(flag == 0)
            {
                int x = 0;
                for(x = 0; x < 41; x++)
                {
                  serialPutchar(fd, s1[x]);
                  printf("%2x ",s1[x]);
                }
                printf("\n");
            }else
            {
                int x = 0;
                for(x = 0; x < 41; x++)
                {
                    serialPutchar(fd, s2[x]);
                    printf("%2x ", s2[x]);
                }
                printf("\n");
            }
      //s1[10] = s1[10] + 1;
      //s1[22] = s1[22] + 1;
        }
        fflush (stdout) ;
  }

  return 0 ;
}
