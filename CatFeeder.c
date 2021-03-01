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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/queue.h>

#include <cjson/cJSON.h>

#include <mosquitto.h>

#include "WirelessProtocols/MCHP_API.h"

#include "interface.h"


/**
 * debug flag
 */
int debug = 0;


/**
 * Define the pet_entry
 */
struct pet_entry {

	// define the MAC
	char *chipID;
	// and the total feeding time for the day
	int totalDailyFeedingTime;
	// and the totol g eaten
	float totalDailyEaten;

	// list management
    TAILQ_ENTRY(pet_entry) entries;
};

/** 
 * The list of detected pets
 */
static TAILQ_HEAD(, pet_entry) petlist_head;

/**
 * Define the key used to encode the 0x01 messages
 */
static BYTE key[] = { 0x00,0x00,0x00,0x58,0x00,0x6c,0x5a,0x71,0xba,0x96,0x33,0xf8,0xc7,0xfc,
		0x4e,0xaf,0xce,0x9e,0xe2,0x03,0xc3,0xa8,0x9e,0xe4,0x98,0x82,0x2b,0xa0,0x0d,0x9b,
		0xc7,0xbd,0xe0,0x54,0xd5,0xdd,0x4a,0xb0,0x2b,0xa6,0x1a,0x01,0xfa,0x47,0x7a,0xec,0x12,
		0x48,0x11,0x27,0x3f,0x59,0xee,0x84,0x8b,0x93,0x03,0x90,0x3b,0x3a,0xcd,0x74,0x67,0x8f,
		0x83,0x05,0xd5,0xef,0x33,0xdf,0x79,0xd5,0xd5,0x6e,0x00};


/**
 * Function to send a frame
 */
static void send_msg(BYTE *buff, int buffSize) {
	extern MAC_TRANS_PARAM MTP;
	TxData = 0;

	if (debug)
	{
		printf("TX: ");
		for (int i = 0; i < buffSize; i++) {
			printf("%02x ", buff[i]);
		}
		printf("\n");
	}

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
static void dexor(BYTE *buff, int buffSize)
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
static time_t getMSTime()
{
	struct timespec monotime;
	clock_gettime(CLOCK_MONOTONIC, &monotime);

	return((monotime.tv_sec*1000) + (monotime.tv_nsec/1000000));
}


/**
 * Helper to get the weight
 */
static float getWeight(BYTE rxPayload[], int weightEntry)
{
	// get the weight
	long rawWeight =
		(((long)rxPayload[26 + (weightEntry*4)])) |
		(((long)rxPayload[27 + (weightEntry*4)])<<8) |
		(((long)rxPayload[28 + (weightEntry*4)])<<16) |
		(((long)rxPayload[29 + (weightEntry*4)])<<24);

	float weight=((float)rawWeight)/100;

	return(weight);
}


/**
 * Function to save state
 */
static void save_state(const char *stateFile)
{
	FILE *fp;
	cJSON *root, *pets;
	struct pet_entry *item;
	char * tmpName = (char *)malloc(strlen(stateFile)+10);
	
	root  = cJSON_CreateObject();
	
	pets = cJSON_CreateArray();
	
	cJSON_AddItemToObject(root, "pets", pets);

	// add items to the pets array
	TAILQ_FOREACH(item, &petlist_head, entries)
	{
		cJSON *pet;
		cJSON_AddItemToArray(pets, pet  = cJSON_CreateObject());
		
		cJSON_AddItemToObject(pet, "chipID", cJSON_CreateString(item->chipID));
		cJSON_AddItemToObject(pet, "totalDailyFeedingTime", cJSON_CreateNumber(item->totalDailyFeedingTime));
		cJSON_AddItemToObject(pet, "totalDailyEaten", cJSON_CreateNumber(item->totalDailyEaten));
		
	}

	// create new tmp name
	sprintf(tmpName, "%s.new", stateFile);
	
	fp = fopen(tmpName, "w+");

	if (fp == NULL)
	{
		fprintf( stderr, "Failed to open state file, %s\n",
				strerror(errno));
	}
	else 
	{
		int success;

		char *out;
		out = cJSON_Print(root); 
		success = fputs(out, fp);

		if (success<=0)
		{
			fprintf( stderr, "Failed to write to state file, %s\n",
					strerror(errno));
		}

		free(out);
  
		fclose(fp);	

		if (success>0)
		{
			// and we have created file file ok, so rename it
			rename(tmpName,stateFile);
		}
	}
	cJSON_Delete(root);
	free(tmpName);
}


/**
 * Function to save state
 */
static void load_state(const char *stateFile)
{
	FILE *fp;
	
	fp = fopen(stateFile, "r");
	if (fp==NULL)
	{
		printf("Cannot open state file %s, %s\n", stateFile, strerror(errno));
	}
	else
	{
		long lSize;
		char *buffer;
		cJSON *root;
		
		// find the size of the file
		fseek(fp, 0, SEEK_END);
		lSize = ftell(fp);
		rewind(fp);
	
		buffer = (char *)malloc(sizeof(char)*lSize + 1);
		
		memset(buffer, 0, sizeof(char)*lSize + 1);
		fread(buffer, sizeof(char), lSize, fp);
		
		root = cJSON_Parse(buffer);
		
		if (root==NULL)
		{
			printf("Failed to Parse state file\n");
		}
		else
		{
			// look for the pets array
			struct pet_entry *petEntry = NULL;
			int i;

			cJSON *pets = cJSON_GetObjectItem(root,"pets");
			for (i = 0 ; i < cJSON_GetArraySize(pets) ; i++)
			{
				cJSON *pet = cJSON_GetArrayItem(pets, i);
				
				petEntry = (struct pet_entry *)malloc(sizeof(struct pet_entry));
				memset(petEntry,0x0, sizeof(struct pet_entry));
				
				petEntry->chipID= strdup(cJSON_GetObjectItem(pet, "chipID")->valuestring);
				
				petEntry->totalDailyFeedingTime=(int)cJSON_GetObjectItem(pet, "totalDailyFeedingTime")->valueint;
				petEntry->totalDailyEaten=(float)cJSON_GetObjectItem(pet, "totalDailyEaten")->valuedouble;

				if (debug)
				{
					printf("Loading Pet %s, totalDailyFeedingTime=%d, totalDailyEaten=%f\n",
							petEntry->chipID,
							petEntry->totalDailyFeedingTime,
							petEntry->totalDailyEaten );
				}

				TAILQ_INSERT_TAIL(&petlist_head, petEntry, entries);
			}
		
			cJSON_Delete(root);
		}
		
		fclose(fp);	

	}
	
}


/**
 * Main entry point
 */
int main(int argc, char **argv) {

	//const char *clientID = "catfeeder";
	const char *brokerName ="127.0.0.1";
	const char *stateFile ="/var/run/catfeeder/state.json";
	int brokerPort = 1883;
	int keepAlive = 60;
	int verbose = 0;

	int disconnected = 0;

	BYTE sequence = 0xa8;
	BYTE beaconAckSequence = 0x0;

	int currentState = STATE_WAITING_FOR_STARTUP_ACK;

	int chann = 0x0f;
	struct mosquitto *m;

	int c;

	opterr = 0;

	while ((c = getopt (argc, argv, "dvh:p:k:s:")) != -1)
	{
		switch (c)
		{

			case 'd':
				debug = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				brokerName = optarg;
				break;
			case 's':
				stateFile = optarg;
				break;
			case 'p':
				brokerPort = atoi(optarg);
				break;
			case 'k':
				keepAlive = atoi(optarg);
				break;
			default:
				break;
		}
	}

	// setup the pet list

	TAILQ_INIT(&petlist_head);

	// load in the state
	load_state(stateFile);

	setup_spi();

	// and setup the protocol
	MiApp_ProtocolInit(FALSE);


	if (MiApp_SetChannel(chann) == FALSE)
	{
		printf("Set channel failed\n");
	}
	else
	{
		// setup miwi
		MiApp_ConnectionMode(ENABLE_ALL_CONN);

		// start the connection state
		MiApp_StartConnection(START_CONN_DIRECT, 10, 0xFFFFFFFF);

		MiApp_SetChannel(chann);

		// create a new client, using clean session
		// auto allocate name
		m = mosquitto_new(NULL, true, &disconnected);

		// set the reconnect settings
		mosquitto_reconnect_delay_set(m, 2, 30, true);

		int res = mosquitto_connect(m, brokerName, brokerPort, keepAlive);

		if (res != MOSQ_ERR_SUCCESS)
		{
			printf("Failed to connect to MQTT broker: %d, %s\n", res, strerror(errno));
		}
		else if (mosquitto_loop_start(m) != MOSQ_ERR_SUCCESS)
		{
			printf("Failed to start mosquitto\n");
		}
		else
		{
			int lastHour = -1;

			// The main loop
			while (1)
			{
				int stateUpdated = 0;
				struct tm localTime;

				char topicName[200];

				// perform periodic events
				time_t currentTime = time(NULL);

				localtime_r(&currentTime, &localTime);

				// once a hour, do the reports
				if (localTime.tm_hour!=lastHour)
				{
					struct pet_entry *item;

					// find the entry
					TAILQ_FOREACH(item, &petlist_head, entries)
					{
						// and reset daily stats
						// we do this at the end so that the midnight reading
						// is the last. The next reading will be around 1'ish
						// and will be the new value
						if (localTime.tm_hour==0)
						{
							char value[30];

							// reset these stats
							item->totalDailyFeedingTime=0;
							item->totalDailyEaten=0;
							stateUpdated=1;

							// the totals have also changed, so re-publish
							// start with the pet total feeding time
							sprintf(topicName, "pet/%s/petTotalDailyFeedingTime", item->chipID);
							sprintf(value, "%d", item->totalDailyFeedingTime);

							mosquitto_publish(m, NULL, topicName,
											strlen(value), value, 0, false);

							// and total grams eaten
							sprintf(topicName, "pet/%s/petTotalDailyEaten", item->chipID);
							sprintf(value, "%f", item->totalDailyEaten);

							mosquitto_publish(m, NULL, topicName,
											strlen(value), value, 0, false);
						}
					}

					// record that we have reported this hour
					lastHour=localTime.tm_hour;
				}


				// check if a message is available, and if so process it
				if (MiApp_MessageAvailable())
				{

					char srcAddr[20];
					BYTE rxPayload[200];

					// ok, read the RxMessage

					// take a copy of the payload
					int rxPayloadSize = rxMessage.PayloadSize;
					memcpy(rxPayload, rxMessage.Payload, rxPayloadSize);


					sprintf(srcAddr, "%02x%02x%02x%02x%02x%02x%02x%02x",
							rxMessage.SourceAddress[7],
							rxMessage.SourceAddress[6],
							rxMessage.SourceAddress[5],
							rxMessage.SourceAddress[4],
							rxMessage.SourceAddress[3],
							rxMessage.SourceAddress[2],
							rxMessage.SourceAddress[1],
							rxMessage.SourceAddress[0]);



					// reset the state ready for the next message
					MiApp_DiscardMessage();

					// see if we want to dump the payload
					if (debug) {
						printf("RX: ");
						for (int i = 0; i < rxPayloadSize; i++) {
							printf("%02x ", rxPayload[i]);
						}
						printf("\n");
					}

					// get the message type and sequence ID
					int msgType = rxPayload[0];
					int seqId = rxPayload[1];


					// in debug mode, publish the petfeeder messages
					if (debug)
					{
						sprintf(topicName, "petfeeder/%s/message", srcAddr);

						char payloadTxt[400];
						payloadTxt[0]=0;
						for (int i = 0; i < rxPayloadSize; i++) {
							char tmp[30];
							sprintf(tmp, "%02x ", rxPayload[i]);
							strcat(payloadTxt, tmp);
						}

						res = mosquitto_publish(m, NULL, topicName,
										strlen(payloadTxt), payloadTxt, 0, false);
					}
											
					switch (currentState) {

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

							currentState = STATE_WAITING_FOR_BEACON;
						}
						else if (msgType == 0x08) {

							// go straight to waiting for beacon
							currentState = STATE_WAITING_FOR_BEACON;
						}

						break;


					/**
					 * Wait for a beacon frame from the device
					 */
					case STATE_WAITING_FOR_BEACON:

						// check if this is a beacon
						if (msgType == 0x08 ) {

							char value[30];
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
								printf("Event available, requesting...\n");
								// ok, somthing is waiting, so send 0x09
								payload[0] = 0x09;

								// and we need to wait for event data
								currentState = STATE_WAITING_FOR_EVENTDATA;
							}
							else
							{
								// no event data, so just stay in this state
								currentState = STATE_WAITING_FOR_BEACON;
							}

							// currently this is a guess
							sprintf(topicName, "petfeeder/%s/battery", srcAddr);
							sprintf(value, "%d", rxPayload[3]);

							mosquitto_publish(m, NULL, topicName,
											strlen(value), value, 0, false);

							// and send...
							send_msg(payload, sizeof(payload));

						}
						else if (msgType == 0x13) {

							// need to go back to waiting for startup ack
							currentState = STATE_WAITING_FOR_STARTUP_ACK;
						}

						break;


					/**
					 * Wait for Event data
					 */
					case STATE_WAITING_FOR_EVENTDATA:

						// see if this is event data
						if (msgType == 0x01) {

							// "decrypt" the payload
							for (int i = 0; i < rxPayloadSize; i++)
							{
								rxPayload[i] = rxPayload[i] ^ key[i];
							}

							if (debug)
							{
								printf("RXDECODE: ");
								for (int i = 0; i < rxPayloadSize; i++) {
									printf("%02x ", rxPayload[i]);
								}
								printf("\n");
							}

							int msgLength=rxPayload[6];
							int msgSubType=rxPayload[7];

							if (verbose)
							{
								printf("Message Type %x\n", msgSubType);
							}

							// ok, decode
							if (msgSubType==0x18)
							{

								// cat tag add
								// 01 bb 2a 00 b1 00 29 18 00 b4
								// 00 b4 56 98 54 01 13 c5 b7 7f
								// 00 03 00 00 00 02 5f fd ff ff
								// 00 00 00 00 fe ff ff ff 00 00
								// 00 00 12 00 23 01 00 00 b7


								// cat tag remove
								// 01 bf 2a 00 dc 00 29 18 00 b5
								// 00 c3 56 98 54 01 13 c5 b7 7f
								// 00 03 01 0a 00 02 5f fd ff ff
								// 78 fd ff ff fe ff ff ff 02 00
								// 00 00 13 00 23 01 00 00 12 16
								// 00 b6 00 c5 56 98 54 00 01 af
								// 8a 6e 04 7e be f6 03 b7


								// manual???
								// 01 c8 2a 00 b1 00 29 18 00 bd
								// 00 cc 56 98 54 01 02 03 04 05
								// 06 07 06 00 00 02 00 00 00 00
								// 7a fd ff ff 00 00 00 00 03 00
								// 00 00 13 00 23 01 00 00 fa

								// cat - opening
								// 01 ea 2a 00 b1 00 29 18 00 c1
								// 00 cf 57 98 54 3d 05 1f db 63
								// f6 01 00 00 00 02 6d 16 00 00
								// 00 00 00 00 fe ff ff ff 00 00
								// 00 00 14 00 23 01 00 00 13

								// get the chip ID
								BYTE chipID[20];


								sprintf(chipID, "%02x%02x%02x%02x%02x%02x%02x",
										rxPayload[15],
										rxPayload[16],
										rxPayload[17],
										rxPayload[18],
										rxPayload[19],
										rxPayload[20],
										rxPayload[21],
										rxPayload[22]);

								if (verbose)
								{
									printf("ChipID: %s\n", chipID);
								}

								struct pet_entry *petEntry = NULL;
								struct pet_entry *item;

								// find the entry
								TAILQ_FOREACH(item, &petlist_head, entries)
								{
									if (strcmp(item->chipID, chipID)==0)
									{
										petEntry=item;
									}
								}

								// if we did not find it
								if (petEntry==NULL)
								{
									if (debug)
									{
										printf("Adding new pet %s\n", chipID);
									}
									petEntry = (struct pet_entry *)malloc(sizeof(struct pet_entry));
									memset(petEntry,0x0, sizeof(struct pet_entry));
									petEntry->chipID= strdup(chipID);
									petEntry->totalDailyFeedingTime=0;
									petEntry->totalDailyEaten=0;

									TAILQ_INSERT_TAIL(&petlist_head, petEntry, entries);
									
								}


								// get the lid state
								BYTE lidState = rxPayload[22];

								// and get the amount of time the lid is open
								unsigned short openTime =
										((unsigned short)rxPayload[23]) |
										((unsigned short)rxPayload[24]<<8);



								// 00 Tag triggered - closed to Open
								// 01 Tag triggered - open to closed
								// 04 User triggered - Open
								// 05 User triggered - Close
								// 06 zeroed when user opened

								const char *lidStateString ="false";
								const char *petFeedingStateString ="false";
								const char *userOpenStateString ="false";
								int petClosing=0;
								int closing=0;


								switch(lidState)
								{
									// 00 Tag triggered - closed to Open
									case 0x00:
										lidStateString="true";
										petFeedingStateString="true";
										break;

									// 01 Tag triggered - open to closed
									case 0x01:
										petClosing=1;
										closing=1;
										break;

									// 04 User triggered - Open
									case 0x04:
										lidStateString="true";
										userOpenStateString="true";
										break;

									// 05 User triggered - Close
									case 0x05:
										closing=1;
										break;

									// 06 zeroed when user opened
									// this gets sent while its open once
									// the user presses the close button
									// it also then sends a 0x05 to state that
									// its now closed.
									case 0x06:
										// the lid is still considered open because user
										// opened it
										lidStateString="true";
										userOpenStateString="true";
										// ok, it has been zero'ed,
										// this this time we dont need to do anything here
										// but if we start using the leftOpen and rightOpen for something
										// it will need to be reset here. currently this is only used
										// for animal open, so not currently an issue
										break;
								}

								if (verbose)
								{
									printf("LidState %x, %s\n",lidState, lidStateString);
								}

								// send the lid state
								sprintf(topicName, "petfeeder/%s/lidOpen", srcAddr);

								res = mosquitto_publish(m, NULL, topicName,
												strlen(lidStateString), lidStateString, 0, false);

								// send the user open state
								sprintf(topicName, "petfeeder/%s/userOpen", srcAddr);

								res = mosquitto_publish(m, NULL, topicName,
												strlen(userOpenStateString), userOpenStateString, 0, false);


								// send the pet feeding state
								sprintf(topicName, "pet/%s/petFeeding", chipID);

								res = mosquitto_publish(m, NULL, topicName,
												strlen(petFeedingStateString), petFeedingStateString, 0, false);


								// now do the food weight
								enum weights
								{
									weight_leftOpen,
									weight_leftClose,
									weight_rightOpen,
									weight_rightClose
								};

								const char *weightStrings[] = {"leftOpen", "leftClose", "rightOpen", "rightClose"};
								float weights[4];

								for (int weightEntry=0; weightEntry<4; weightEntry++)
								{
									char value[30];

									float weight = getWeight(rxPayload, weightEntry);

									// only publish the weights if closing because of user or pet.
									// this is because the closing weight is only available on the
									// closing message
									if (closing)
									{
										sprintf(topicName, "petfeeder/%s/%s_weight", srcAddr, weightStrings[weightEntry]);
										sprintf(value, "%f", weight);

										mosquitto_publish(m, NULL, topicName,
														strlen(value), value, 0, false);
									}

									weights[weightEntry]=weight;
								}

								// if closing because of user or pet,
								// make sure we publish a total amount left
								if (closing)
								{
									char value[30];

									// calculate the total left
									float totalLeft = weights[weight_rightClose] + weights[weight_leftClose];

									sprintf(topicName, "petfeeder/%s/weight", srcAddr);
									sprintf(value, "%f", totalLeft);

									mosquitto_publish(m, NULL, topicName,
													strlen(value), value, 0, false);
								}

								// if closing because pet closing...
								if (petClosing)
								{
									char value[30];

									// also publish the amount of time this pet was feeding
									sprintf(topicName, "pet/%s/petFeedingTime", chipID);
									sprintf(value, "%d", openTime);

									res = mosquitto_publish(m, NULL, topicName,
													strlen(value), value, 0, false);

									// add the additional total feeding time..
									petEntry->totalDailyFeedingTime+=openTime;

									// we want to calculate the total eaten by this pet
									float totalEaten = (weights[weight_leftOpen] - weights[weight_leftClose]) +
											(weights[weight_rightOpen] - weights[weight_rightClose]);

									// and publish it
									sprintf(topicName, "pet/%s/eaten", chipID);
									sprintf(value, "%f", totalEaten);

									mosquitto_publish(m, NULL, topicName,
													strlen(value), value, 0, false);

									// and keep the running total daily
									petEntry->totalDailyEaten+=totalEaten;

									// the totals have also changed, so re-publish
									// start with the pet total feeding time
									sprintf(topicName, "pet/%s/petTotalDailyFeedingTime", item->chipID);
									sprintf(value, "%d", item->totalDailyFeedingTime);

									mosquitto_publish(m, NULL, topicName,
													strlen(value), value, 0, false);

									// and total grams eaten
									sprintf(topicName, "pet/%s/petTotalDailyEaten", item->chipID);
									sprintf(value, "%f", item->totalDailyEaten);

									mosquitto_publish(m, NULL, topicName,
													strlen(value), value, 0, false);

								}
								
								stateUpdated=1;
							}

							// send the ACK
							BYTE payload[] = { 0x02,0x5a,0x5e,0x00 };

							// set the sequence ID
							payload[1] = sequence++;
							// add in the sequence number of the 0x01 message we have received
							payload[2] = rxPayload[1];

							// and send it...
							send_msg(payload, sizeof(payload));

							currentState = STATE_WAITING_FOR_EVENTACK;
						}
						else
						{
							// if we get something we dont expect, go back to beacon
							currentState = STATE_WAITING_FOR_BEACON;
						}
						break;

					/**
					 * Wait for an ACK from the EVENT message
					 */
					case STATE_WAITING_FOR_EVENTACK:

						if (msgType == 0x0a) {

							// ok, dont need to do anythin here, just go
							// back to beacon
							currentState = STATE_WAITING_FOR_BEACON;
						}
						else
						{
							// unknown payload, so go back to beacon for now
							// we should really re-transmit
							currentState = STATE_WAITING_FOR_BEACON;
						}

						break;

					}
				}
				
				
				if (stateUpdated)
				{
					if (verbose)
					{
						printf("Saving state data to %s\n", stateFile);
					}
					save_state(stateFile);
				}
			}
		}
	}

	return 0;
}
