/*
 * interface.c
 *
 *  Created on: 4 Feb 2021
 *      Author: markretallack
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

typedef unsigned char           BYTE;                           /* 8-bit unsigned  */
typedef unsigned short int      WORD;                           /* 16-bit unsigned */

extern void _INT1Interrupt();

int RFIF;
int RFIE;

int PHY_CS;
int PHY_RESETn;
int RF_INT_PIN;

#define CHANNEL 0


// see https://pinout.xyz/pinout/pin31_gpio6#
#define RADIO_INT 		22 //5
#define RADIO_RESET 	21  //6


#define USR_ISR

#ifdef USR_ISR

/**
 * The event semaphore
 */
static sem_t sem;


void radio_interrupt(void)
{
	printf("radio_interrupt\n");
	
	sem_post(&sem);
}


/**
 * Wait for the interrupt or a timeout
 * \param timeout timeout to wait before exiting
 * \return 0=timeout, 1=int
 */
int wait_for_interupt(int timeout)
{
	int res=0;
	int s;
	struct timespec ts;


	int bob = digitalRead(RADIO_INT);

	if (bob==0)
	{
		printf("Int low\n");

	}

	// get the current time
	clock_gettime(CLOCK_REALTIME, &ts);

	// add in the timeout
	ts.tv_sec+=timeout;

	// ok, wait for the semaphore event
	s = sem_timedwait(&sem, &ts);

	printf("Wake up\n");

	if (s==0)
	{
		printf("Processing interrupt\n");

		_INT1Interrupt();

		res=1;
	}

	return(res);
}

#elif defined( USE_POLL)

int wait_for_interupt(int timeout)
{
	int ret=0;
	int loop=1;

	printf("wait_for_interupt\n");

	time_t start = time(NULL);

	while(loop==1)
	{
		usleep(1*100);


		// check it is low
		int res = digitalRead(RADIO_INT);

		if (res==0)
		{
			printf("Calling _INT1Interrupt\n");

			// and run the ISR
			_INT1Interrupt();

			loop=0;
			ret=1;
		}

		time_t current = time(NULL);

		if ((current - start)> timeout)
		{
			loop=0;
		}
	}

	return(ret);
}
#else
int wait_for_interupt(int timeout)
{
	usleep(1*100);
	return(0);
}
#endif

/**
 * Function to setup SPI
 */
void setup_spi()
{

	// setup the wiring lib
	wiringPiSetup();

	// and the SPI channel
	int fd = wiringPiSPISetup(CHANNEL, 500000);

	// set GPIO 5 to output (Reset line)
	pinMode(RADIO_RESET, OUTPUT);

	// set GPIO 76 as input for INT line
	pinMode(RADIO_INT, INPUT);

#ifdef USR_ISR
	// setup the semaphore
	sem_init(&sem, 0, 0);

	// and add an ISR to the RADIO_INT
	wiringPiISR(RADIO_INT, INT_EDGE_FALLING, radio_interrupt);
#endif


}

/**
 * Function to set the reset to the Radio Low
 */
void PHY_RESETn_LOW()
{
	//printf("PHY_RESETn_LOW\n");
	digitalWrite (RADIO_RESET, LOW);

}

/**
 * Function to set the reset to the Radio high
 */
void PHY_RESETn_HIGH()
{
	//printf("PHY_RESETn_HIGH\n");
	digitalWrite (RADIO_RESET, HIGH);
}


/**
 * Stub functions
 */
void RFIF_SET()
{

}

void RFIF_CLEAR()
{

}

int RFIF_PIN()
{


	wait_for_interupt(100);


#if 0
	int res;



	res = digitalRead(RADIO_INT);

	if (res==0)
	{
		//RFIE=1;
		//RFIF=1;
		
		printf("RADIO_INT=  0\n");

    	_INT1Interrupt();

	}
#endif
	return(0);
}


/**
 * Function to set A long address value
 */
void PHYSetLongRAMAddr(WORD address, BYTE value)
{
	unsigned char buffer[4];

	buffer[0]=((((BYTE)(address>>3))&0x7F)|0x80);
	buffer[1]=((((BYTE)(address<<5))&0xE0)|0x10);
	buffer[2]=(value);

	wiringPiSPIDataRW(CHANNEL, buffer, 3);
}

/**
 * Function to set A short address value
 */
void PHYSetShortRAMAddr(BYTE address, BYTE value)
{
	unsigned char buffer[4];
	buffer[0]=(address);
	buffer[1]=(value);

	wiringPiSPIDataRW(CHANNEL, buffer, 2);
}

/**
 * Function to get A short address value
 */
BYTE PHYGetShortRAMAddr(BYTE address)
{
	unsigned char buffer[4];

	buffer[0]=(address);
	buffer[1]=0;

	wiringPiSPIDataRW(CHANNEL, buffer, 2);

	return(buffer[1]);
}

/**
 * Function to get A long address value
 */
BYTE PHYGetLongRAMAddr(WORD address)
{
	unsigned char buffer[4];

	buffer[0]=(((address>>3)&0x7F)|0x80);
	buffer[1]=(((address<<5)&0xE0));
	buffer[2]=0;

	wiringPiSPIDataRW(CHANNEL, buffer, 3);

	return(buffer[2]);
}

/**
 * Stub function
 */
void SYMBOL_TIMER_INIT()
{

}

/**
 * Stub function to setup console
 */
void ConsoleInit(void)
{
	printf("setup\n");
}

/**
 * wrapper to print to console
 */
void ConsolePutROMString(char* str)
{
	printf("%s\n",str);
}

/**
 * wrapper to print to console
 */
void ConsolePut(unsigned char c)
{
	printf("%c",c);
}


/**
 * wrapper to get from the console
 */
unsigned char ConsoleGet()
{
	return(0x0);
}

/**
 * wrapper to put a char to the console
 */
void PrintChar(unsigned char c)
{
	printf("%c",c);
}

/**
 * wrapper to put a value to the console
 */
void PrintDec(unsigned char c)
{
	printf("%d",c);
}

