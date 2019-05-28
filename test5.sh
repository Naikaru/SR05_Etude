#!/bin/bash

#si il y a deux paramètre (bas et net)
if [ $# -eq 2 ]
then
	\rm /tmp/in* /tmp/out*
	BAS=$1
	NET=$2
	mkfifo /tmp/in1a /tmp/out1a
	mkfifo /tmp/in1b /tmp/out1b
	mkfifo /tmp/in2a /tmp/out2a
	mkfifo /tmp/in2b /tmp/out2b
	mkfifo /tmp/in3a /tmp/out3a
	mkfifo /tmp/in3b /tmp/out3b
	mkfifo /tmp/in4a /tmp/out4a
	mkfifo /tmp/in4b /tmp/out4b
	mkfifo /tmp/in5a /tmp/out5a
	mkfifo /tmp/in5b /tmp/out5b
	 
	./$BAS --ident=0 < /tmp/in1a > /tmp/out1a &
	./$NET --ident=0 --nbNode=5 < /tmp/in1b > /tmp/out1b &

	./$BAS --ident=1 < /tmp/in2a > /tmp/out2a &
	./$NET --ident=1 --nbNode=5 < /tmp/in2b > /tmp/out2b &

	./$BAS --ident=2 < /tmp/in3a > /tmp/out3a &
	./$NET --ident=2 --nbNode=5 < /tmp/in3b > /tmp/out3b &
	 
	./$BAS --ident=3 < /tmp/in4a > /tmp/out4a &
	./$NET --ident=3 --nbNode=5 < /tmp/in4b > /tmp/out4b &

	./$BAS --ident=4 < /tmp/in5a > /tmp/out5a &
	./$NET --ident=4 --nbNode=5 < /tmp/in5b > /tmp/out5b &
	 

	# BAS1 -> NET1
	cat /tmp/out1a > /tmp/in1b &
	# NET1 -> BAS1 and NET2 and NET3
	cat /tmp/out1b | tee /tmp/in1a /tmp/in2b /tmp/in3b & 
	# BAS2 -> NET2
	cat /tmp/out2a > /tmp/in2b &
	# NET2 -> BAS2 and NET1 and NET4 and NET5
	cat /tmp/out2b | tee /tmp/in2a /tmp/in1b /tmp/in4b /tmp/in5b &
	# BAS3 -> NET3
	cat /tmp/out3a > /tmp/in3b &
	# NET3 -> BAS3 and NET1 and NET5
	cat /tmp/out3b | tee /tmp/in3a /tmp/in1b /tmp/in5b &  
	# BAS4 -> NET4
	cat /tmp/out4a > /tmp/in4b &
	# NET4 -> BAS4 and NET2 and NET5
	cat /tmp/out4b | tee /tmp/in4a /tmp/in2b /tmp/in5b &
	# BAS5 -> NET5
	cat /tmp/out5a > /tmp/in5b &
	# NET5 -> BAS5 and NET2 and NET3 and NET4
	cat /tmp/out5b | tee /tmp/in5a /tmp/in2b /tmp/in3b /tmp/in4b &  
fi


