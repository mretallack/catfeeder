
#include <stdio.h>
#include <stdlib.h>

#include "WirelessProtocols/MCHP_API.h"

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

#if 0

    MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);

#else

    MiApp_StartConnection(START_CONN_DIRECT, 10, 0xFFFFFFFF);


    printf("Wait for packet....\n");


    int chann=0x0f;
	MiApp_SetChannel(chann);
    while(1)
    {
    	sleep(1);

    	printf("PRocessing\n");
    	//printf("Sending on %d\n", chann);

    	//MiApp_BroadcastPacket(FALSE);


		if( MiApp_MessageAvailable())
		{
			printf("Message available\n");

			MiApp_DiscardMessage();

			sleep(60);
			printf("Starting...\n");


		}
    }

    //if(MiMAC_ReceivedPacket())
   // {
   // 	printf("Got packet\n");
   //     MiApp_DiscardMessage();
   // }
#endif

	return 0;
}
