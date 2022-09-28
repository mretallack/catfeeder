
2.3 seconds:

standard beacon, and ack


-----

4.05 seconds:

standard beacon, and ack


-----

7.4 seconds:

standard beacon, and ack


-----

9.06 seconds:

standard beacon, and ack


-----

12.41 seconds:

The Feeder sends a beacon with the "I want comms" bit set

08be01aa12380024fc0010e0fc01

The hub asks for more

09ac63

The feeder sends data

01bf2a58b56c4867ba3b2bf7e9511aafcfc18e242186900198903da0a383c8934d00d5ddaedc0c449f01fa4716

The Hub ack's the data

02adbf00

The feeder ends comms

0ac0

-----

14.07 seconds:

standard beacon, and ack




-----

17.39 seconds:

The feeder sends a Beacon

08c101aa12380021fd0013e0fc01

The Hub seems to send:

01af2d588e6c5a71178e08d66aa858afce78

then it gets acked by the feeder and the following gets sent from the HUB:

01b02d588e6c5a71148e08d66aa858afce7b

And again gets acked by the Feeder

02c3b000

The Hub then asks for any data from the feeder

09b164

The feeder then sends another beacon

087a00a41238000f3b02d3c34800

The hub has nothing else to do

0ab229

-----

22.37 seconds:


The feeder sends a Beacon

08c401aa1238001efe0016e0fc01

Hub resuest more...

09b365

Feeder sends data

01c52a58b16c7369ba392be8e9511aaecc9de606c5af9ae49880b759f264c7bde054d5dd4ab02ba61a01fa475ded124884

Hub acks data

02b4c500

Feeder finishes 

0ac6


-----

24.09 seconds:

standard beacon, and ack


-----

27.40 seconds:

feeder sends beacon

08c701aa1238001eff0119e0fc01

Hub sends data

01b62d588e6c5a71158e7bd66aa856afce07

Feeder ACKS

02c8b600

Hub asks for more?

09b766

Feeder sends data

01c92a58b56c4867ba262be1e9511aafcf567724214cb6e498903da0bc83de934d00d5dd01260c449901fa4720

Hub acks data

02b8c900

Feeder ends comms

0aca


-----

29.11 seconds:

standard beacon, and ack



-----
32.44 seconds:

Feeder sends beacon

08cb01aa1238002100011de0fc01

Hub sends data

01ba2d588e6c5a710a8e7ed66aa858afce13

Feeder acks

02ccba00

HUB sends data

01bb2d588e6c5a710b8e7ed66aa858afce12

Comms seems to break down, would expect an ack from feeder??


-----
34.11 seconds:

The feeder sends another beacon

087d00a41238000c3e02d6c34800

No reply from hub

-----
37.47 seconds:

Feeder sends beacon

08cd01aa1238002001011fe0fc01

Hub sends data

01bb2d588e6c5a710b8e7ed66aa858afce12

Feeder acks

02cebb00

Hub Asks for more info

09bc67

Feeder sends more info

01cf2a58b16c7369ba242be1e9511aaecc9de606c5af9bed9880b759f2644a441fabd5dd4ab02ba61a01fb475ded1248ed

Hub acks

02bdcf00

Feeder ends comms

0ad0

NOTE: Hub sends this???? 


0abe2c


-----
39.12 seconds:


standard beacon, and ack

-----
42.51 seconds:

Feeder send:

08d100aa12380021020123e0fc01

Hub sends:

01c02d588e6c5a71088e64d66aa856afce05

Feeder acks

02d2c000

Hub acks beacon

0ac168


-----
44.17 seconds:


standard beacon, and ack






































