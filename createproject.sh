#!/bin/sh

me=$0

shortopts="lhm::n:"
longopts="help,platforms,name:,mcu:"
opts=$(getopt -o "$shortopts" -l "$longopts" -n "$(basename $0)" -- "$@")
eval set -- "$opts"

MCU=''
NAME=''

function print_help
{
	echo "Usage $me [param(s)]";
	echo -e "\tParams:";
	echo -e "\t-h|--help\tPrints this help screen";
	echo -e "\t-l|--platforms\tList supported platforms(mcu)";
	echo -e "\t-n|--name\tNew project name (manditory)";
	echo -e "\t-m|--mcu\tTarget MCU (manditory). See --platforms";
	exit;	
}

function print_platforms
{
	echo "Supported achitectures and MCUs";
	echo ""
	cat scripts/platforms.dat
	echo ""
	echo "To define target MCU use parameter -m . Example: $0 --mcu=stm8s"

	exit;
}

while true; do	
	case "$1" in
		-h | --help) print_help ;;
		-l | --platforms) print_platforms ;;
		-m | --mcu ) MCU=$2; shift 2;;
		-n | --name ) NAME=$2; shift 2;;
    		-- ) shift; break ;;
    		* ) break ;;
	esac
done

if [ -z $MCU  ] || [ -z $NAME ];then	
	print_help
fi

ARCH=$(cat scripts/platforms.dat | grep $MCU | awk -F ':' '{print $1}' )
PROJECTNAME=`echo firmware-$NAME-$ARCH | tr '[:upper:]' '[:lower:]'`
WORKDIR="devices/$PROJECTNAME"

mkdir -p $WORKDIR
. scripts/create-makefile-$ARCH.sh

create_makefile 

echo -e "Project created !";
echo -e "\nNext steps:\n"
echo -e "1. cd $WORKDIR"
echo -e "2. Edit Makefile and define WIREBUS_UUID_MAJOR and WIREBUS_UUID_MINOR of your new device"
echo -e "3. Edit file $PROJECTNAME.c to insert your custom code"
echo -e "4. make"
echo ""


