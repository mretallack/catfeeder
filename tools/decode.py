import codecs
import paho.mqtt.subscribe as subscribe

key = [0x00,0x00,0x00,0x58,0x00,0x6c,0x5a,0x71,0xba,0x96,0x33,0xf8,0xc7,0xfc,
		0x4e,0xaf,0xce,0x9e,0xe2,0x03,0xc3,0xa8,0x9e,0xe4,0x98,0x82,0x2b,0xa0,0x0d,0x9b,
		0xc7,0xbd,0xe0,0x54,0xd5,0xdd,0x4a,0xb0,0x2b,0xa6,0x1a,0x01,0xfa,0x47,0x7a,0xec,0x12,
		0x48,0x11,0x27,0x3f,0x59,0xee,0x84,0x8b,0x93,0x03,0x90,0x3b,0x3a,0xcd,0x74,0x67,0x8f,
		0x83,0x05,0xd5,0xef,0x33,0xdf,0x79,0xd5,0xd5,0x6e,0x00 ]


rawMsg="01c02d588e6c5a71088e64d66aa856afce05"

msgBytes = codecs.decode(rawMsg, 'hex')


decoded=b''
o=0
for i in msgBytes:
	decoded+=(bytes([i ^ key[o]]))
	o=o+1

print(decoded.hex())


