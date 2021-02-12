
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

    while(1)
    {
    	usleep(10 * 1000);

    	//printf("Processing\n");

		if( MiApp_MessageAvailable())
		{
			printf("Message available\n");


			printf("Src Address: ");
			for (int i=0; i<8; i++)
			{
				printf("%x ", rxMessage.SourceAddress[i]);
			}
			printf("\n");

			// ok, read the RxMessage
			for (int i=0; i<rxMessage.PayloadSize; i++) {
				printf("%x ",rxMessage.Payload[i]);
			}
			printf("\n");


			int msgType = rxMessage.Payload[0];


			printf("msg type: %x\n", msgType);

			// got 13 69 4 3
			if (msgType==0x13)
			{


				//14 a8 70 00 10 ea


				MiApp_DiscardMessage();


				BYTE payload[] = {0x14, 0xa8, 0x70, 0x00};
				payload[1]=sequence;
				send_msg(payload, sizeof(payload));

				sequence++;
			}
			else if (msgType==0x08)
			{
				MiApp_DiscardMessage();


				BYTE payload[] = {0x0e, 0xab, 0x0c, 0x07, 0x3a, 0x0c, 0x3c, 0x05, 0x30, 0x45, 0x07, 0x10, 0x37, 0x0b, 0x30, 0x1b};
				//0010   2d 1d
				payload[1]=sequence;


				send_msg(payload, sizeof(payload));

				sequence++;
			}
			else if (msgType==0x03)
			{
				//0000
				//0010   7d

				MiApp_DiscardMessage();


				BYTE payload[] = {0x01, 0xac, 0x2d, 0x58, 0x91, 0x6c, 0x5b, 0x71, 0xbb, 0x96, 0x5c, 0x5b, 0x51, 0xa8, 0x45, 0xaf, 0x7d};
				//0010   2d 1d
				payload[1]=sequence;


				send_msg(payload, sizeof(payload));

				sequence++;
			}

			MiApp_DiscardMessage();
		}
    }

#endif

	return 0;
}
