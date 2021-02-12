/*
 * ConfigApp.h
 *
 *  Created on: 4 Feb 2021
 *      Author: markretallack
 */

#ifndef INC_CONFIGAPP_H_
#define INC_CONFIGAPP_H_


#define PROTOCOL_MIWI

#define NWK_ROLE_COORDINATOR

#define ADDITIONAL_NODE_ID_SIZE 0

#define MY_ADDRESS_LENGTH 8


//#define TARGET_SMALL


#define TX_BUFFER_SIZE 200

#define CONNECTION_SIZE 0x7d

#define __CATFEEDER__


#define EUI_0 0x33
#define EUI_1 0x22
#define EUI_2 0x12
#define EUI_3 0xff
#define EUI_4 0xfe
#define EUI_5 0xe8
#define EUI_6 0x8d
#define EUI_7 0xd9


static void Nop()
{

}

// TODO: timer
#define TMRL 33


#define MY_PAN_ID 0x3421


extern int RFIF;
extern int RFIE;

#define MRF24J40

#define USE_DEFAULT


#endif /* INC_CONFIGAPP_H_ */
