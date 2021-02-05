/*
 * interface.c
 *
 *  Created on: 4 Feb 2021
 *      Author: markretallack
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

typedef unsigned char           BYTE;                           /* 8-bit unsigned  */
typedef unsigned short int      WORD;                           /* 16-bit unsigned */


int RFIF;
int RFIE;

int PHY_CS;
int PHY_RESETn;
int RF_INT_PIN;

#define CHANNEL 0


// see https://pinout.xyz/pinout/pin31_gpio6#
#define RADIO_INT 		22 //5
#define RADIO_RESET 	21  //6


void setup_spi()
{
	wiringPiSetup();

	int fd = wiringPiSPISetup(CHANNEL, 500000);

	// set GPIO 5 to output (Reset line)
	pinMode(RADIO_RESET, OUTPUT);

	// set GPIO 76 as input for INT line
	pinMode(RADIO_INT, INPUT);
}


void PHY_RESETn_LOW()
{
	//printf("PHY_RESETn_LOW\n");
	digitalWrite (RADIO_RESET, LOW);

}

void PHY_RESETn_HIGH()
{
	//printf("PHY_RESETn_HIGH\n");
	digitalWrite (RADIO_RESET, HIGH);
}



void RFIF_SET()
{

}

void RFIF_CLEAR()
{

}

int RFIF_PIN()
{

	int res = digitalRead(RADIO_INT);

	//printf("%d\n",res);

	if (res==0)
	{
		RFIE=1;
		RFIF=1;

    	_INT1Interrupt();

		printf("RFIF_PIN %d\n",res);
	}

	return(res);
}


void PHYSetLongRAMAddr(WORD address, BYTE value)
{
	unsigned char buffer[4];

	//printf("Writing (LONG) %x to %x\n",value, address);

	buffer[0]=((((BYTE)(address>>3))&0x7F)|0x80);
	buffer[1]=((((BYTE)(address<<5))&0xE0)|0x10);
	buffer[2]=(value);

	wiringPiSPIDataRW(CHANNEL, buffer, 3);
}

void PHYSetShortRAMAddr(BYTE address, BYTE value)
{
	unsigned char buffer[4];
	buffer[0]=(address);
	buffer[1]=(value);

	//printf("Writing (SHORT) %x to %x\n",value, address);

	wiringPiSPIDataRW(CHANNEL, buffer, 2);
}


BYTE PHYGetShortRAMAddr(BYTE address)
{
	unsigned char buffer[4];

	buffer[0]=(address);
	buffer[1]=0;

	wiringPiSPIDataRW(CHANNEL, buffer, 2);

	//printf("Reading (SHORT) %x from %x\n",buffer[1],address );

	return(buffer[1]);
}

BYTE PHYGetLongRAMAddr(WORD address)
{
	unsigned char buffer[4];

	buffer[0]=(((address>>3)&0x7F)|0x80);
	buffer[1]=(((address<<5)&0xE0));
	buffer[2]=0;

	wiringPiSPIDataRW(CHANNEL, buffer, 3);

	//printf("Reading (LONG) %x from %x\n",buffer[2],address );

	return(buffer[2]);
}


void SYMBOL_TIMER_INIT()
{

}

void ConsoleInit(void)
{
	printf("setup\n");




}

void ConsolePutROMString(char* str)
{
	printf("%s\n",str);
}

void ConsolePut(unsigned char c)
{
	printf("%c",c);
}


unsigned char ConsoleGet()
{
	return(0x0);
}

void PrintChar(unsigned char c)
{
	printf("%c",c);
}
void PrintDec(unsigned char c)
{
	printf("%d",c);
}

