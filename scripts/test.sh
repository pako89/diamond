#!/bin/bash

TEMP=temp.bstr
OUT=out.yuv
VIDEO=
WIDTH=
HEIGHT=
OPENCL="-C"

if [ "$#" -eq 1 ]
then
	case $1 in
	"c")
		OPENCL="-C"
		;;
	"n")
		OPENCL=""
		;;
	*)
		VIDEO=$1

		;;
	esac
fi

RES=$(ls video)
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

VID=$(find video/$RES -name "*.yuv")
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
time ./diamond encode $OPENCL -tYUV420 -W $WIDTH -H $HEIGHT -o $TEMP $VIDEO
time ./diamond decode -o $OUT $TEMP
mplayer $OUT -loop 0 -demuxer rawvideo -rawvideo w=$WIDTH:h=$HEIGHT
