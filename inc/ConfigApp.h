/*
 * MIT License
 *
 * Copyright (c) 2021 Mark Retalack
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
