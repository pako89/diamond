#!/bin/bash

BASENAME=$(basename $0)

function usage()
{
	echo "$BASENAME [OPTIONS] FILE|DIR [DEST DIR]"
	exit 1
}

function error()
{
	echo $1 >&2
	exit 1
}

function convert_file()
{
	echo "Convertin file $1 to $2"
	NAME=$(basename $1)
	yuv4mpeg -w $WIDTH -h $HEIGHT -r $RATE -x $CHROMA < $1 > "$2/$NAME"
}

function convert_dir()
{
	echo "Convering dir $1 to $2"
	for f in $(find $1 -name "*.yuv");
	do
		convert_file $f $2
	done
}

function print_args()
{
	echo "Width    : $WIDTH"
	echo "Height   : $HEIGHT"
	echo "Chroma   : $CHROMA"
	echo "Rate     : $RATE"
	echo "Input    : $FILE_OR_DIR"
	echo "Dest     : $DEST_DIR"
}

echo "YUV to YUV4MPEG Converter"

which yuv4mpeg 1>/dev/null || error "yuv4mpeg not found"

case $# in
	0)
		usage
		;;
esac

WIDTH="720"
HEIGHT="576"
CHROMA="420jpeg"
RATE="24:1"

while getopts "w:h:x:r:" opt;
do
	case $opt in
		w)
			WIDTH=$OPTARG
			;;
		h)
			HEIGHT=$OPTARG
			;;
		x)
			CHROMA=$OPTARG
			;;
		r)
			RATE=$OPTARG
			;;
		\?)
			echo "Invalid option: $OPTARG" > $2
			exit 1
			;;
	esac
done

shift "$((OPTIND-1))"

case $# in
	1)
		FILE_OR_DIR=$1
		DEST_DIR=""
		;;
	2)
		FILE_OR_DIR=$1
		DEST_DIR=$2
		;;
	*)
		usage
esac

if [ -f $FILE_OR_DIR ]
then
	IS_FILE=1
else
	if [ -d $FILE_OR_DIR ]
	then
		IS_FILE=0
		if [ "$DEST_DIR" = "" ]
		then
			DEST_DIR=$FILE_OR_DIR
		fi
	else
		usage
	fi
fi

if [ "$DEST_DIR" = "" ]
then
	DEST_DIR="."
fi

if [ ! -d $DEST_DIR ]
then
	mkdir $DEST_DIR
fi

print_args

if [ $IS_FILE = "1" ]
then
	convert_file $FILE_OR_DIR $DEST_DIR
else
	convert_dir $FILE_OR_DIR $DEST_DIR
fi
