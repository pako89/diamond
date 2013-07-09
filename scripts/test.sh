#!/bin/bash

TEMP=temp.bstr
OUT=out.yuv
VIDEO=
WIDTH=
HEIGHT=
VARIANT=2
HUFFMAN="static"
PROGRESS_BAR="--progress-bar yes"
GOP="3"
COMMON=$PROGRESS_BAR
DEBUG=0
DIR="yuvvideo"

RES=$(ls $DIR)
select R in $RES
do
	if [ -d video/$R ]
	then
		echo "Resolution: ${R}"
		RES=$R
		break
	else
		echo "Error: wrong resolution"
		exit 1
	fi
done

VID=$(find $DIR/$RES -name "*.yuv")
select v in $VID
do
	if [ -f $v ]
	then
		echo "Video: ${v}"
		VIDEO=$v
		break
	else
		echo "Error: wrong video file"
		exit 1
	fi
done
WIDTH=$(echo $RES | sed 's/\([0-9]\+\)x\([0-9]\+\)/\1/')
HEIGHT=$(echo $RES | sed 's/\([0-9]\+\)x\([0-9]\+\)/\2/')

if [ $DEBUG == 1 ]
then
cat > .gdbinit << EOF
b main
run encode $COMMON -V$VARIANT -g$GOP -tYUV420 -W $WIDTH -H $HEIGHT -e $HUFFMAN -o $TEMP $VIDEO
EOF
cgdb ./diamond
else
	./diamond encode $COMMON -V$VARIANT -g$GOP -tYUV420 -W $WIDTH -H $HEIGHT -e $HUFFMAN -o $TEMP $VIDEO
fi

if [ $? == 0 ]
then
	./diamond decode $COMMON -o $OUT $TEMP
fi

if [ $? == 0 ]
then
	mplayer $OUT -loop 0 -demuxer y4m
fi
