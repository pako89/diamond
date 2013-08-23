#!/bin/bash

TEMP=temp.bstr
OUT=out.yuv
VIDEO=
WIDTH=
HEIGHT=
VARIANT=0
HUFFMAN="static"
PROGRESS_BAR="--progress-bar yes"
PRINT_TIMERS="--print-timers yes"
VERBOSE="-vv"
GOP="3"
Q="2"
DEBUG=1
DIR="videos_test"

if [ $DEBUG == 1 ]
then
	COMMON="--progress-bar no $PRINT_TIMERS $VERBOSE"
else
	COMMON="$PROGRESS_BAR $PRINT_TIMERS $VERBOSE"
fi

RES=$(ls $DIR)
select R in $RES
do
	if [ -d $DIR/$R ]
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
ARGS="encode $COMMON -I 2 -V$VARIANT -g$GOP -q$Q -tYUV420 -W $WIDTH -H $HEIGHT -e $HUFFMAN -o $TEMP $VIDEO"

if [ $DEBUG == 1 ]
then
cat > .gdbinit << EOF
b main
run $ARGS
EOF
cgdb ./irena
else
	CMD="./irena $ARGS"
	echo "Encoding...[$CMD]"
	$CMD
	if [ $? == 0 ];
	then
		echo "Encoding Done"
	fi
fi

if [ $? == 0 ]
then
	./irena decode $COMMON -o $OUT $TEMP
fi

if [ $? == 0 ]
then
	mplayer $OUT -loop 0 -demuxer y4m
fi
