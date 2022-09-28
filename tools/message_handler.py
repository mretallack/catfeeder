
import codecs

org_key = [0x00,0x00,0x00,0x58,0x9e,0x6c,0x5a,0x71,0xba,0x96,0x33,0xf8,0xc7,0xfc,
		0x4e,0xaf,0xce,0x9e,0xe2,0x03,0xc3,0xa8,0x9e,0xe4,0x98,0x82,0x2b,0xa0,0x0d,0x9b,
		0xc7,0xbd,0xe0,0x54,0xd5,0xdd,0x4a,0xb0,0x2b,0xa6,0x1a,0x01,0xfa,0x47,0x7a,0xec,0x12,
		0x48,0x11,0x27,0x3f,0x59,0xee,0x84,0x8b,0x93,0x03,0x90,0x3b,0x3a,0xcd,0x74,0x67,0x8f,
		0x83,0x05,0xd5,0xef,0x33,0xdf,0x79,0xd5,0xd5,0x6e,0x00 ]

new_key = [0x00,0x00,0x00,0x00,
			# 4 - event data length
			# encoded b1, when key is 0x9e, so value is 2f. 
			# so 2f ^ aa = 85
			0x85,
			0x00,
			# 6 - msgLen, 
			# for type 0x18, the length should be 0x29
			# we get 0xC1, so C1 ^ 29 = 0xe8
			0xe8,
			# 7- msgSubType
			# we find this because the 0x16 message type is shorter, and the 0x18 is longer
			0x39,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			# 15 - first ID for chipID
			# for manual opening, the chipID is "01 02 03 04 05 06 07"

			# chipID 0: 5 ^ 1 = 4
			# chipID 1: 243 ^ 2 = 0xf1
			# chipID 2: 188 ^ 3 = 0xbf
			# chipID 3: 196 ^ 4 = 0xc0
			# chipID 4: 152 ^ 5 = 0x9d
			# chipID 5: 70 ^ 6 = 0x40
			# chipID 6: 105 ^ 7 = 0x6e

			0x04,0xf1,0xbf,
			0xc0,0x9d,0x40,			
			0x6e,
			# 22 - lidState, when opening, we got 250, so 250 ^ 04 = 0xfe
			0xfe,
			# 23 - opentime low: 117
			0x75,
			# 24 - opentime high: 244
			0xf4,

			0x00,
			# 26 - Left hand open
			# weight low2: 123
			# weight high1: 130
			# weight high2: 106
			# second byte: 0x7b, 7b, 0x7b

			0x00,0x7b,130,106,
			# 30 - Left hand close
			169,99,211,139,
			#0x00,0x00,0x00,0x00,
			# 34 - Right hand open
			# weight low2: 48, 42, 45, 4d, so first seems to be 0x40
			# weight high1: 67
			# weight high2: 142
			# the second byte seems to have 0x4x, but cannot seem to zero to be able to find the others
			# c, 0x9, 0x09, 0x0e
			# setting second to 4, not sure
			00,0x44,67,142,
			# 38 - Right hand close
			# TODO: the LSB is wrong, possible values: 
			0,154,229,156,
			0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00]


print("org_key len: "+str(len(org_key)))
print("new_key len: "+str(len(new_key)))




def getWeight(packetStart, weightEntry):
	


		# // get the weight
		# long rawWeight =
		#	(((long)rxPayload[26-6 + (weightEntry*4)])) |
		#	(((long)rxPayload[27-6 + (weightEntry*4)])<<8) |
		#	(((long)rxPayload[28-6 + (weightEntry*4)])<<16) |
		#	(((long)rxPayload[29-6 + (weightEntry*4)])<<24);

		#float weight=((float)rawWeight)/100;

		print("weight low1: "+str(hex(packetStart[26-6 + (weightEntry*4)])))
		print("weight low2: "+str(hex(packetStart[27-6 + (weightEntry*4)])))
		print("weight high1: "+str(hex(packetStart[28-6 + (weightEntry*4)])))
		print("weight high2: "+str(hex(packetStart[29-6 + (weightEntry*4)])))

		# // get the weight
		#rawWeight =	int(packetStart[26-6 + (weightEntry*4)]) | \
		#			(int(packetStart[27-6 + (weightEntry*4)])<<8) | \
		#			(int(packetStart[28-6 + (weightEntry*4)])<<16) | \
		#			(int(packetStart[29-6 + (weightEntry*4)])<<24)

		extract=[packetStart[26-6 + (weightEntry*4)],
				 packetStart[27-6 + (weightEntry*4)],
				 packetStart[28-6 + (weightEntry*4)],
				 packetStart[29-6 + (weightEntry*4)]]

		
		rawWeight = int.from_bytes( extract, "little", signed=True )


		weight=float(rawWeight)/100;


		print("weight: "+str(weight))

		# so left close is the following on an open
		# weight low1: 169
		# weight low2: 99
		# weight high1: 211
		# weight high2: 139
		#
		# add a weight and its now:
		# weight low1: 169
		# weight low2: 99
		# weight high1: 211
		# weight high2: 139

		# and for the right hand close, when opened
		# weight low1: 39
		# weight low2: 104
		# weight high1: 26
		# weight high2: 99

			
		# to get last@
		# open
		# zero
		# close
		# record number
		# 114
		# 237 (1)
		# 



def decodePacket(msgBytes, key):


	decoded=b''
	o=0

	msgBytes=msgBytes[:len(key)]

	for i in msgBytes:
		decoded+=(bytes([i ^ key[o]]))
		o=o+1

	msgEventDataLength=decoded[4];

	print("msgEventDataLength: "+str(msgEventDataLength))

	# strip off the start
	packetStart=decoded[6:]

	# subtract the header
	msgEventDataLength-=6;
	
	# check there is at least 2 bytes in the frame
	while len(packetStart) >= 2:


		# for type 0x18, the length should be 0x29
		msgSubTypeLength=packetStart[0]+1
		msgSubType=packetStart[1]

		#if msgSubTypeLength<2:
		#
		#	print("Malformed packet, Not enougth data in packet, is "+str(msgSubTypeLength))
		#	break

		print(decoded.hex())

		print("type: "+str(hex(msgSubType))+", len: "+str(msgSubTypeLength))

		# decode the type
		if msgSubType == 0x18:

			print("Message is a Feeder open / close action")
			
			# get chip ID
			#sprintf(chipID, "%02x%02x%02x%02x%02x%02x%02x",
			#			packetStart[15-6],
			#			packetStart[16-6],
			#			packetStart[17-6],
			#			packetStart[18-6],
			#			packetStart[19-6],
			#			packetStart[20-6],
			#			packetStart[21-6],
			#			packetStart[22-6]);

			# for the chip ID, manual is "01 02 03 04 05 06 07 06"

			for i in range(0,7):
				print("chipID "+str(i)+": "+str(hex(packetStart[15-6 + i])))


			#BYTE lidState = packetStart[22-6];
			lidState = packetStart[22-6]

			# Feed state - 
			# 00 Animal closed to Open 
			# 01 Animal open to closed
			# 04 Manual open
			# 05 Manual closed, 
			# 06 scales zeroed while manually opened and zero button pressed on the back of feeder
			# when opening, we got 250, so 250 ^ 04 = 0xfe
			print("lidState: "+str(lidState))
			if lidState==0:
				print("Animal closed to Open")
			elif lidState==1:
				print("Animal open to closed")		
			elif lidState==4:
				print("Manual open")	
			elif lidState==5:
				print("Manual closed")	
			elif lidState==6:
				print("Scales zeroed while manually opened and zero button pressed on the back of feeder")	

			#// and get the amount of time the lid is open
			#unsigned short openTime =
			#		((unsigned short)packetStart[23-6]) |
			#		((unsigned short)packetStart[24-6]<<8);

			print("opentime low: "+str(packetStart[23-6]))
			print("opentime high: "+str(packetStart[24-6]))

			# for a quick close and then open:
			# opentime low: 117
			# opentime high: 244
			# assuming that its zero when going from closed to open (its time open), then
			# the xor key will be 117 and 244

			openTime= packetStart[23-6] | (packetStart[24-6]<<8)

			print("opentime: "+str(openTime))
		
			for weightEntry in range(0,4):

				# for weight, there are 4 entries, 
				# Left hand open 
				# Left hand close
				# Right hand open
				# Right hand close
				# the closing weights are only available on close
				# so this may leak the key

				# on opening, the close entry is 

				getWeight(packetStart, weightEntry)


		# move to next packet
		msgEventDataLength-=msgSubTypeLength;
		packetStart=packetStart[msgSubTypeLength:];



def decode_mesg(rawMsg):
	
	msgBytes = codecs.decode(rawMsg, 'hex')

	#print(msgBytes)


	msgType = msgBytes[0]
	seqId = msgBytes[1]

	if msgType ==0x08:

		moreData=msgBytes[2]
		battery=msgBytes[3]

		# assume battery is -> 6?

		print("beacon, moredata: "+str(moreData), ", battery: "+str(battery))


	elif msgType ==0x01:

		print("----------------------------")
		print("Event payload")

		decodePacket(msgBytes, org_key)

		decodePacket(msgBytes, new_key)


	elif msgType == 0x0a:

		print("ACK msg: "+str(seqId))

	else:
		print("unknown message type: "+str(msgType))

