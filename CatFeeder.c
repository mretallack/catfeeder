
#include <stdio.h>
#include <stdlib.h>

#include "WirelessProtocols/MCHP_API.h"

void send_msg(BYTE *buff, int buffSize)
{
	extern MAC_TRANS_PARAM MTP;
	TxData = 0;

	for (int i=0; i<buffSize; i++)
	{
		MiApp_WriteData(buff[i]);
	}

	//MiApp_WriteData(0x14);
	//MiApp_WriteData(0xa8);
	//MiApp_WriteData(0x70);
	//MiApp_WriteData(0x00);

    MTP.flags.Val = 0;
    MTP.flags.bits.ackReq = 0;
    MTP.flags.bits.secEn = FALSE;

	MTP.altDestAddr = FALSE;
	MTP.altSrcAddr = FALSE;
	MTP.DestAddress = rxMessage.SourceAddress;
	MTP.DestPANID.Val = myPANID.Val;
	MiMAC_SendPacket(MTP, TxBuffer, TxData);


}


int main(int argc, char **argv) {

	setup_spi();

	printf("Setup\n");

	// and setup the protocol
	MiApp_ProtocolInit(FALSE);

	printf("Set Channel\n");

    if(MiApp_SetChannel(0x0f) == FALSE)
    {
    	printf("Set channel failed\n");
        return(1);
    }

    printf("Set connection mode\n");

    MiApp_ConnectionMode(ENABLE_ALL_CONN);

    // start the connection state
    MiApp_StartConnection(START_CONN_DIRECT, 10, 0xFFFFFFFF);


#if 0

    MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);

#else


    printf("Wait for packet....\n");


    int chann=0x0f;
	MiApp_SetChannel(chann);

	BYTE sequence=0xa8;
	BYTE beaconAckSequence=0x11;

	int first = 0;

    while(1)
    {
    	usleep(10 * 1000);

    	//printf("Processing\n");

		if( MiApp_MessageAvailable())
		{
		#if 0 
			printf("Src Address: ");
			for (int i=0; i<8; i++)
			{
				printf("%02x ", rxMessage.SourceAddress[i]);
			}
			printf("\n");
		#endif 
			// ok, read the RxMessage
			printf("RX: ");
			for (int i=0; i<rxMessage.PayloadSize; i++) {
				printf("%02x ",rxMessage.Payload[i]);
			}
			printf("\n");


			int msgType = rxMessage.Payload[0];
			int seqId = rxMessage.Payload[1];
			int msgSubType = rxMessage.Payload[2];


			printf("msg type: %x, %x\n", msgType, msgSubType);
			
			MiApp_DiscardMessage();

			int loop=1;
			
			while (loop==1)
			{
				loop=0;
			
			switch (first)
			{

				case 0:
					// got 13 69 4 3
					if (msgType==0x13)
					{
						// this seems to be some sort of ack?

						//14 a8 70 00 10 ea


						MiApp_DiscardMessage();


						BYTE payload[] = {0x14, 0xa8, 0x70, 0x00};
						payload[1]=sequence;
						// need the sequence from the previous for this one
						payload[2]=rxMessage.Payload[1];
						// I think 00 is ok, it is else where in the stack
						
						send_msg(payload, sizeof(payload));

						sequence++;
						first=1;
					}
					
					else if (msgType==0x08)
					{
						// already started...
						// resend...
						first=1;
						msgType=0x08;
						loop=1;
					}
					
					break;
				#if 0 
				else if (msgType==0x08 && msgSubType==0x01)
				{
					// 01b32d588e6c5a713696b15b51a858afce4e
					
					BYTE payload[] = {0x01,0xb3,0x2d,0x58,0x8e,0x6c,0x5a,0x71,0x36,0x96,0xb1,0x5b,0x51,0xa8,0x58,0xaf,0xce,0x4e};
					payload[1]=sequence;


					send_msg(payload, sizeof(payload));

					sequence++;
					
				}
				
				else if (msgType==0x08 && msgSubType==0x00)
				{
					BYTE payload[] = {0x0a,0xb9,0x6d};
					payload[1]=sequence;


					send_msg(payload, sizeof(payload));

					sequence++;
					
				}
				#endif 
				
				case 1:
				if (msgType==0x08)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x0e,0xab,0x0c,0x07,0x3a,0x0c,0x3c,0x05,0x30,0x45,0x07,0x10,0x37,0x0b,0x30,0x1b,0x2d,0x1d};


					//0010   2d 1d
					payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Sending first reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					first=2;
				}
				break;
				
				case 2:
				if (msgType==0x03)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x01,0xac,0x2d,0x58,0x91,0x6c,0x5b,0x71,0xbb,0x96,0x5c,0x5b,0x51,0xa8,0x45,0xaf,0x7d};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					//0010   2d 1d
					payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Sending 2 reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					
					first=3;
				}
				else if (msgType==0x08)
				{
					// resend...
					first=1;
					msgType=0x08;
					loop=1;
				}
				break;
				
				case 3:
				if (msgType==0x02)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x01,0xac,0x2d,0x58,0x91,0x6c,0x5b,0x71,0xbb,0x96,0x5c,0x5b,0x51,0xa8,0x45,0xaf,0x7d};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					//0010   2d 1d
					payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Sending 3 reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					
					first=4;
				}
				else if (msgType==0x08)
				{
					// resend...
					first=2;
					msgType=0x08;
					loop=1;
				}
				break;
				
				case 4:
				
				if (msgType==0x02)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x01,0xad,0x2d,0x58,0x8e,0x6c,0x5b,0x71,0xb8,0x96,0x5c,0x5b,0x51,0xa8,0x59,0xaf,0xce,0x2d};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					//0010   2d 1d
					payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Sending 4 reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					
					first=5;
				}
				else if (msgType==0x08)
				{
					// resend...
					printf("Got 0x08 resendin...");
					first=3;
					msgType=0x02;
					loop=1;
				}
				break;		
				
				case 5:		
				if (msgType==0x02)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x01,0xae,0x2d,0x58,0x91,0x6c,0x5b,0x71,0xb9,0x96,0x5c,0x5b,0x51,0xa8,0x5e,0xaf,0x64};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					//0010   2d 1d
					payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Sending 5 reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					
					first=6;
				}
				else if (msgType==0x08)
				{
					printf("Resending /n");
					// resend...
					first=4;
					msgType=0x02;
					loop=1;
				}
				break;
				
				case 6:
				if (msgType==0x02)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x01,0xaf,0x2d,0x58,0x91,0x6c,0x5b,0x71,0xbe,0x96,0x5c,0x5b,0x51,0xa8,0x42,0xaf,0x7f};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					//0010   2d 1d
					payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Sending 6 reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					
					first=7;
				}
				else if (msgType==0x08)
				{
					printf("Resending /n");
					// resend...
					first=5;
					msgType=0x02;
					loop=1;
				}
				break;
				
				case 7:
				if (msgType==0x02)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x01,0xb0,0x2d,0x58,0x8c,0x6c,0x5d,0x71,0xbf,0x96,0x5c,0x5b,0x51,0xa8,0x4e,0xaf,0xce,0x9e,0xe7,0xa1};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					//0010   2d 1d
					payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Sending 7 reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					
					first=8;
				}
				else if (msgType==0x08)
				{
					printf("Resending /n");
					// resend...
					first=6;
					msgType=0x02;
					loop=1;
				}
				break;
				
				case 8:
				
				if (msgType==0x02)
				{

					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x09,0xb1,0x10};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d

					beaconAckSequence=0x10;
					//0010   2d 1d
					payload[1]=sequence;
					payload[2]=beaconAckSequence;
					
					printf("Sending 8 reply\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					beaconAckSequence++;
					
					first=9;
				}
				else if (msgType==0x08)
				{
					printf("Resending /n");
					// resend...
					first=7;
					msgType=0x02;
					loop=1;
				}
				break;
			
			
			case 9:
			
				if (msgType==0x0a)
				{
				
					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x0a,0xb8,0x6c};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d

					//beaconAckSequence=0x10;
					///0010   2d 1d
					//payload[1]=sequence;
					//payload[2]=beaconAckSequence;
					
					printf("Finished\n");
					//send_msg(payload, sizeof(payload));

					//sequence++;
					
					first=10;
				}
				else if (msgType==0x08)
				{
					printf("Resending /n");
					// resend...
					first=8;
					msgType=0x02;
					loop=1;
				}
				break;
				
				
			case 10:
			
				if (msgType==0x08)
				{
				
					//RX: 08 74 01 a5 00 18 00 2d 93 03 05 00 00 00
					//TX: 01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d


					BYTE payload[] = {0x0a,0xb8,0x6c};
					
					//01 ac 2d 58 91 6c 5b 71 bb 96 5c 5b 51 a8 45 af 7d

					//beaconAckSequence=0x10;
					///0010   2d 1d
					payload[1]=sequence;
					payload[2]=beaconAckSequence;
					
					if (beaconAckSequence % 10 == 0)
					{
						printf("sending other\n");
						payload[0]=0x09;
					}
					
					printf("Sending beacon ACK\n");
					send_msg(payload, sizeof(payload));

					sequence++;
					beaconAckSequence++;
					first=10;
				}
				
			}
			}
			
			#if 0
			else if (msgType==0x08)
			{

				// send a beacon ACK
				BYTE payload[] = {0x0a,0xa9,0x6b}; //0a1270

				//0010   2d 1d
				payload[1]=sequence;
				payload[2]=beaconAckSequence;
				
				if (beaconAckSequence % 10 == 0)
				{
					printf("sending other\n");
					payload[0]=0x09;
				}

				send_msg(payload, sizeof(payload));

				sequence++;
				beaconAckSequence++;
			}
			// messgae 3 and two are some sort of ACK or NAC
			// when we get one of these messages, the 3nd byte
			// is the sequence number from the previous one
			else if (msgType==0x03)
			{
				//0000
				//0010   7d
#if 0 
				MiApp_DiscardMessage();


				BYTE payload[] = {0x01, 0xac, 0x2d, 0x58, 0x91, 0x6c, 0x5b, 0x71, 0xbb, 0x96, 0x5c, 0x5b, 0x51, 0xa8, 0x45, 0xaf, 0x7d};
				//0010   2d 1d
				payload[1]=sequence;


				send_msg(payload, sizeof(payload));

				sequence++;
#endif 
			}
#endif

			//MiApp_DiscardMessage();
		}
    }

#endif

	return 0;
}
