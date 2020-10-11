/***************************************************************************
 * Copyright (C) 2003 Walter Pirri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 ***************************************************************************/

/*  UPS EXATRON */
/***************************************************************************
 *            a PC (DB-25 Hembra)                 a UPS (DB-25 Macho)
 *
 *      (GND)     (GND)  1 o-----------------------------o 9
 *      (UPS)     (CTS)  5 o-----------------------------o 3
 *      ()        (DSR)  6 o-----------------------------o 6
 *      (GND)     (GND)  7 o-----------------------------o 4
 *      (Bat)     (DCD)  8 o-----------------------------o 5
 *      (Apagado) (STR) 20 o-----------------------------o 1
 *
 ***************************************************************************/

/*  UPS SENDON */
/***************************************************************************
*              a PC (DB-9 Hembra)                 a UPS (DB-9 Macho)
*
*       (GND)       TXD 3 o---------------------+--------o 4
*                                               |
*                                               +--------o 7
*       (Apagado)   DTR 4 o----------< 10K >-------------o 6
*       (UPS)       CTS 8 o-----+------------------------o 2
*                               |
*                              10K
*                               |
*       (+)         RTS 7 o-----+
*                               |
*                              10K
*                               |
*        (Bat)      DCD 1 o-----+------------------------o 5
*
 ***************************************************************************/

/***************************************************************************
 *  |       |        |        |       |         |          |                                |
 *  |DB-25  |  DB-9  |  Name  |  EIA  |  CCITT  |  DTE-DCE |  Description                   |
 *  |Pin #  |  Pin # |        |       |         |          |                                |
 *  |_______|________|________|_______|_________|__________|______________                  |
 *  |1      |        |  FG    |  AA   |  101    |  ---     | Frame Ground/Chassis GND       |
 *  |2      |  3     |  TD    |  BA   |  103    |  --->    | Transmitted Data, TxD          |
 *  |3      |  2     |  RD    |  BB   |  104    |  <---    |  Received Data, RxD            |
 *  |4      |  7     |  RTS   |  CA   |  105    |  --->    | Request To Send                |
 *  |5      |  8     |  CTS   |  CB   |  106    |  <---    |  Clear To Send                 |
 *  |6      |  6     |  DSR   |  CC   |  107    |  <---    |  Data Set Ready                |
 *  |7      |  5     |  SG    |  AB   |  102    |  ----    | Signal Ground, GND             |
 *  |8      |  1     |  DCD   |  CF   |  109    |  <---    |  Data Carrier Detect           |
 *  |9      |        |  --    |  --   |  -      |  -       | Positive DC test voltage       |
 *  |10     |        |  --    |  --   |  -      |  -       | Negative DC test voltage       |
 *  |11     |        |  QM    |  --   |  -      |  <---    |  Equalizer mode                |
 *  |12     |        |  SDCD  |  SCF  |  122    |  <---    |  Secondary Data Carrier Detect |
 *  |13     |        |  SCTS  |  SCB  |  121    |  <---    |  Secondary Clear To Send       |
 *  |14     |        |  STD   |  SBA  |  118    |  --->    | Secondary Transmitted Data     |
 *  |15     |        |  TC    |  DB   |  114    |  <---    |  Transmitter (signal) Clock    |
 *  |16     |        |  SRD   |  SBB  |  119    |  <---    |  Secondary Receiver Clock      |
 *  |17     |        |  RC    |  DD   |  115    |  --->    | Receiver (signal) Clock        |
 *  |18     |        |  DCR   |  --   |  -      |  <---    |  Divided Clock Receiver        |
 *  |19     |        |  SRTS  |  SCA  |  120    |  --->    | Secondary Request To Send      |
 *  |20     |  4     |  DTR   |  CD   |  108.2  |  --->    | Data Terminal Ready            |
 *  |21     |        |  SQ    |  CG   |  110    |  <---    |  Signal Quality Detect         |
 *  |22     |  9     |  RI    |  CE   |  125    |  <---    |  Ring Indicator                |
 *  |23     |        |  --    |  CH   |  111    |  --->    | Data rate selector             |
 *  |24     |        |  --    |  CI   |  112    |  <---    |  Data rate selector            |
 *  |25     |        |  TC    |  DA   |  113    |  <---    |  Transmitted Clock             |
 *
 *      Pin Assignment for the Serial Port (RS-232C), 25-pin and 9-pin
 *          1                         13         1         5
 *        _______________________________      _______________
 *        \  . . . . . . . . . . . . .  /      \  . . . . .  /    RS232-connectors
 *         \  . . . . . . . . . . . .  /        \  . . . .  /     seen from outside
 *          ---------------------------          -----------      of computer.
 *          14                      25            6       9
 *
 *     DTE : Data Terminal Equipment (i.e. computer)
 *     DCE : Data Communications Equipment (i.e. modem)
 *     RxD : Data received; 1 is transmitted "low", 0 as "high"
 *     TxD : Data sent; 1 is transmitted "low", 0 as "high"
 *     DTR : DTE announces that it is powered up and ready to communicate
 *     DSR : DCE announces that it is ready to communicate; low=modem hangup
 *     RTS : DTE asks DCE for permission to send data
 *     CTS : DCE agrees on RTS
 *     RI  : DCE signals the DTE that an establishment of a connection is attempted
 *     DCD : DCE announces that a connection is established
 *
 *  --> Ioctl to RS232 correspondence
 *  --> modem lines
 *
 *  #define TIOCM_LE        0x001
 *  #define TIOCM_DTR       0x002
 *  #define TIOCM_RTS       0x004
 *  #define TIOCM_ST        0x008
 *  #define TIOCM_SR        0x010
 *  #define TIOCM_CTS       0x020
 *  #define TIOCM_CAR       0x040
 *  #define TIOCM_RNG       0x080
 *  #define TIOCM_DSR       0x100
 *  #define TIOCM_CD        TIOCM_CAR
 *  #define TIOCM_RI        TIOCM_RNG
 *
 ***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define PWRS_NORMAL	0
#define PWRS_UPS	1
#define PWRS_BAT	2
#define PWRS_UNKNOW	3

int fdups;
char upsport[256];
int powerstatus;
int oldstatus;

void CheckUPS(int);

int wpower_init()
{
	int flags;
	int DCD, CTS;
	/* cargo los defaults */
	strcpy(upsport, "/dev/ttyS0");
	powerstatus = PWRS_UNKNOW;
	oldstatus = -1;
	fdups = -1;

	/* abro el port */
	if((fdups = open(upsport, O_RDWR | O_NDELAY)) < 0)
	{
		// no se pudo abrir el port de la ups
		return -1;
	}
	sleep(1);
        flags = TIOCM_RTS | TIOCM_ST;
	ioctl(fdups, TIOCMSET, &flags);
	sleep(1);
	/* control para prevenir si la ups no está conectada */
	ioctl(fdups, TIOCMGET, &flags);
	DCD = ((flags & TIOCM_CAR) > 0);
	CTS = ((flags & TIOCM_CTS) > 0);
	if((DCD == 0) || (CTS == 0))
	{
		/* puede ser que la UPS no esté conectada*/
    		close(fdups);
		fdups = -1;
		return -1;
	}
	return 0;
}

void wpower_check_ups()
{
	int flags;
	int DCD, CTS;

	if(fdups == -1)
	{
		if(wpower_init() != 0)
		{
			return;
		}
	}

	ioctl(fdups, TIOCMGET, &flags);
	/* flag de batería baja */
	DCD = ((flags & TIOCM_CAR) > 0);
	/* flag de corte de luz */
	CTS = ((flags & TIOCM_CTS) > 0);
	/* analisis de las entradas */
	if(!DCD)
	{
		powerstatus = PWRS_BAT;
	}
	else if(!CTS)
	{
		powerstatus = PWRS_UPS;
	}
	else
	{
		powerstatus = PWRS_NORMAL;
	}
	/* Logeo los cambios de estado */
	if(oldstatus != powerstatus)
	{
		oldstatus = powerstatus;
		switch(powerstatus)
		{
		case PWRS_NORMAL:
			syslog(LOG_WARNING, "MONITOR-UPS: Sistema normalizado");
			break;
		case PWRS_UPS:
			syslog(LOG_WARNING, "MONITOR-UPS: Corte de energia");
			break;
		case PWRS_BAT:
			syslog(LOG_CRIT, "MONITOR-UPS: Limite de la bateria");
			break;
		}
	}
}

int wpower_status()
{
	return powerstatus;
}

