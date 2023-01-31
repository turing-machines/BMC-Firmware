#!/bin/sh

date=`date +%F`

version=$1

if [ 1 != $# ]; then
	echo please input version
fi
echo $1 |grep "^[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}$" > /dev/null
if [ $? = 1 ];then
	echo please version 0~999 and must 3 version eg:1.1.1
	exit 1
fi

if [ ! -d "build/${date}" ];then
    echo "mkdir build/${date}"
    mkdir -p "build/${date}"
fi

echo "----- make fw -----" 
echo "Version: ${version}" 
echo "Date: ${date}"

echo "build fw"
make -C buildroot V=1
echo "cp -rf buildroot/output/images/buildroot_linux_nand_uart3.img ./build/${date}/turingpi-${version}.img"
cp -rf buildroot/output/images/buildroot_linux_nand_uart3.img ./build/${date}/turingpi-${version}.img

cd buildroot/output/images/
./genSWU.sh
cd -

cp -rf ./buildroot/output/images/turingpi_.swu ./build/${date}/turingpi-${version}.swu

echo "build turing pi firmware over"
