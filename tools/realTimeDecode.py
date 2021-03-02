import codecs
import paho.mqtt.subscribe as subscribe
from message_handler import decode_mesg





def on_message(client, userdata, msg):

	rawMsg=msg.payload.decode("utf-8").replace(" ", "")
	print( rawMsg)

	decode_mesg(rawMsg)


subscribe.callback(on_message, "petfeeder/70b3d5f9c0022a6e/message", hostname="10.0.0.12")



