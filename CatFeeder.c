#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "WirelessProtocols/MCHP_API.h"

#include "interface.h"

/**
 * Define the key used to encode the 0x01 messsages
 */
BYTE key[] = { 0x00,0x00,0x00,0x58,0x00,0x6c,0x5a,0x71,0xba,0x96,0x33,0xf8,0xc7,0xfc,
		0x4e,0xaf,0xce,0x9e,0xe2,0x03,0xc3,0xa8,0x9e,0xe4,0x98,0x82,0x2b,0xa0,0x0d,0x9b,
		0xc7,0xbd,0xe0,0x54,0xd5,0xdd,0x4a,0xb0,0x2b,0xa6,0x1a,0x01,0xfa,0x47,0x7a,0xec,0x12,
		0x48,0x11,0x27,0x3f,0x59,0xee,0x84,0x8b,0x93,0x03,0x90,0x3b,0x3a,0xcd,0x74,0x67,0x8f,
		0x83,0x05,0xd5,0xef,0x33,0xdf,0x79,0xd5,0xd5,0x6e,0x00};


/**
 * Function to send a frame
 */
void send_msg(BYTE *buff, int buffSize) {
	extern MAC_TRANS_PARAM MTP;
	TxData = 0;

	printf("TX: ");
	for (int i = 0; i < buffSize; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");

	for (int i = 0; i < buffSize; i++) {
		MiApp_WriteData(buff[i]);
	}

	MTP.flags.Val = 0;
	MTP.flags.bits.ackReq = 0;
	MTP.flags.bits.secEn = FALSE;

	MTP.altDestAddr = FALSE;
	MTP.altSrcAddr = FALSE;
	MTP.DestAddress = rxMessage.SourceAddress;
	MTP.DestPANID.Val = myPANID.Val;
	MiMAC_SendPacket(MTP, TxBuffer, TxData);

	//usleep(10* 1000);
}

/**
 * Helper funtion to decode a 0x01 event frame
 */
void dexor(BYTE *buff, int buffSize)
{
	printf("XOR: ");
	for (int i = 0; i < buffSize; i++) {

		//if (i >= 6) {
		BYTE tmp = buff[i] ^ key[i];
		//}
		printf("%02x ",tmp);

	}
	printf("\n");

}


/**
 * Define the different possible states
 */
enum
{
	STATE_WAITING_FOR_STARTUP_ACK,
	STATE_WAITING_FOR_BEACON,
	STATE_WAITING_FOR_EVENTDATA,
	STATE_WAITING_FOR_EVENTACK,
};

/**
 * Helper function to get the millisecond counter
 */
time_t getMSTime()
{
	struct timespec monotime;
	clock_gettime(CLOCK_MONOTONIC, &monotime);

	return((monotime.tv_sec*1000) + (monotime.tv_nsec/1000000));
}


/**
 * Main entry point
 */
int main(int argc, char **argv) {

	setup_spi();

	printf("Setup\n");

	// and setup the protocol
	MiApp_ProtocolInit(FALSE);

	printf("Set Channel\n");

	if (MiApp_SetChannel(0x0f) == FALSE) {
		printf("Set channel failed\n");
		return (1);
	}

	printf("Set connection mode\n");

	MiApp_ConnectionMode(ENABLE_ALL_CONN);

	// start the connection state
	MiApp_StartConnection(START_CONN_DIRECT, 10, 0xFFFFFFFF);


	printf("Wait for packet....\n");

	int chann = 0x0f;
	MiApp_SetChannel(chann);

	BYTE sequence = 0xa8;
	BYTE beaconAckSequence = 0x0;

	int currnetState = STATE_WAITING_FOR_STARTUP_ACK;
	int dump = 1;

	time_t lastEvent = 0;


	// The main loop
	while (1) {

		// check if a message is available, and if so process it
		if (MiApp_MessageAvailable()) {

			// ok, read the RxMessage
			printf("Message ready\n");

			// get the last event timestamp
			lastEvent = getMSTime();

			// take a copy of the payload
			BYTE rxPayload[200];
			int rxPayloadSize = rxMessage.PayloadSize;
			memcpy(rxPayload, rxMessage.Payload, rxPayloadSize);

			// reset the state ready for the next message
			MiApp_DiscardMessage();

			// see if we want to dump the payload
			if (dump) {
				printf("RX: ");
				for (int i = 0; i < rxPayloadSize; i++) {
					printf("%02x ", rxPayload[i]);
				}
				printf("\n");
			}

			// get the message type and sequence ID
			int msgType = rxPayload[0];
			int seqId = rxPayload[1];

			printf("State: %d\n", currnetState);

			switch (currnetState) {

			/**
			 * Wait for a startup packet, or a beacon
			 */
			case STATE_WAITING_FOR_STARTUP_ACK:
				// got 13 69 4 3
				if (msgType == 0x13) {
					// this seems to be some sort of ack?

					BYTE payload[] = { 0x14, 0xa8, 0x70, 0x00 };
					// add the squence ID
					payload[1] = sequence++;
					// need the sequence from the previous for this one
					payload[2] = rxPayload[1];
					// I think 00 is "ok", it is else where in the stack

					// and send it
					send_msg(payload, sizeof(payload));

					currnetState = STATE_WAITING_FOR_BEACON;
				}
				else if (msgType == 0x08) {

					// go straight to waiting for beacon
					currnetState = STATE_WAITING_FOR_BEACON;
				}

				break;


			/**
			 * Wait for a beacon frame from the device
			 */
			case STATE_WAITING_FOR_BEACON:

				// check if this is a beacon
				if (msgType == 0x08 ) {

					BYTE payload[] = { 0x0a, 0xb8, 0x6c };

					// set the sequence ID
					payload[1] = sequence++;
					// and each device has a seperate beacon ACK id
					payload[2] = beaconAckSequence++;

					// ok, see if the end device has something waiting
					// if offset 2 contains a "1", its means the device
					// has something to tell us
					if (rxPayload[2] != 0x00)
					{
						printf("Message available, requesting...\n");
						// ok, somthing is waiting, so send 0x09
						payload[0] = 0x09;

						// and we need to wait for event data
						currnetState = STATE_WAITING_FOR_EVENTDATA;
					}
					else
					{
						// no event data, so just stay in this state
						currnetState = STATE_WAITING_FOR_BEACON;
					}

					// and send...
					send_msg(payload, sizeof(payload));

				}
				else if (msgType == 0x13) {

					// need to go back to waiting for startup ack
					currnetState = STATE_WAITING_FOR_STARTUP_ACK;
				}

				break;


			/**
			 * Wait for Event data
			 */
			case STATE_WAITING_FOR_EVENTDATA:

				// see if this is event data
				if (msgType == 0x01) {

					printf("RX: ");
					for (int i = 0; i < rxPayloadSize; i++) {

						//if (i >= 6) {
						rxPayload[i] = rxPayload[i] ^ key[i];
						//}
						printf("%02x ", rxPayload[i]);

					}
					printf("\n");


					int msgLength=rxPayload[6];
					int msgSubType=rxPayload[7];

					printf("Message Length %x, Type %x\n",msgLength, msgSubType);

					// ok, decode
					if (msgSubType==0x18)
					{
						BYTE lidState = rxPayload[21];

						printf("LidState %x, %x\n",lidState);

					}

					// send the ACK
					BYTE payload[] = { 0x02,0x5a,0x5e,0x00 };

					// set the sequence ID
					payload[1] = sequence++;
					// add in the sequence number of the 0x01 message we have received
					payload[2] = rxPayload[1];

					// and send it...
					send_msg(payload, sizeof(payload));

					currnetState = STATE_WAITING_FOR_EVENTACK;
				}
				else
				{
					// if we get something we dont expect, go back to beacon
					currnetState = STATE_WAITING_FOR_BEACON;
				}
				break;

			/**
			 * Wait for an ACK from the EVENT message
			 */
			case STATE_WAITING_FOR_EVENTACK:

				if (msgType == 0x0a) {

					// ok, dont need to do anythin here, just go
					// back to beacon
					currnetState = STATE_WAITING_FOR_BEACON;
				}
				else
				{
					// unknown payload, so go back to beacon for now
					// we should really re-transmit
					currnetState = STATE_WAITING_FOR_BEACON;
				}

				break;

			}
		}
	}

	return 0;
}
