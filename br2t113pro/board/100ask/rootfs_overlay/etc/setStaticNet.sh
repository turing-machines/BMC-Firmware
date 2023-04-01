#!/bin/bash

if [ -f /etc/tpi.cfg ]; then
    ip=$(cat /etc/tpi.cfg |grep ip|awk -F '=' '{print $2}')
    mac=$(cat /etc/tpi.cfg |grep mac|awk -F '=' '{print $2}')
fi
if [ -f /mnt/sdcard/tpi.ini ]; then
    inip=$(cat /mnt/sdcard/tpi.ini |grep ip|awk -F '=' '{print $2}')
    inmac=$(cat /mnt/sdcard/tpi.ini |grep mac|awk -F '=' '{print $2}')
    echo input ip:$inip 
    echo input mac:$inmac
fi

validate_ip() {
  local  ip=$1
  local  stat=1

  if [[ $ip =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    OIFS=$IFS
    IFS='.'
    ip=($ip)
    IFS=$OIFS
    [[ ${ip[0]} -le 255 && ${ip[1]} -le 255 && ${ip[2]} -le 255 && ${ip[3]} -le 255 ]]
    stat=$?
  fi
  return $stat
}

validate_mac() {
  local mac=$1
  local stat=1

  if [[ $mac =~ ^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$ ]]; then
    stat=0
  fi

  return $stat
}

# 调用函数来验证MAC地址
if [ $inmac ]; then
    if validate_mac $inmac; then
    if [ "$mac" != "$inmac" ]; then
        mac=$inmac;
        if [ $ip ]; then
            echo "ip=$ip" > /etc/tpi.cfg
        fi
        echo "mac=$inmac" >> /etc/tpi.cfg
    fi
    else
    echo "MAC $inmac error" > /mnt/sdcard/tpi_ini_err.log
    fi
fi

# 调用函数来验证IP地址
if [ $inip ]; then
    if validate_ip $inip; then
    if [ "$ip" != "$inip" ]; then
        ip=$inip;
        echo "ip=$inip" > /etc/tpi.cfg
        if [ $mac ]; then
            echo "mac=$mac" >> /etc/tpi.cfg
        fi
    fi
    else
    echo "IP $inip error" >> /mnt/sdcard/tpi_ini_err.log
    fi
fi

# 如果不为空则设置mac
if [ $mac ]; then
	ifconfig eth0 down
	echo set mac: $mac
	ifconfig eth0  hw ether $mac
	ifconfig eth0 up
fi
# 如果不为空则设置IP
if [ $ip ]; then
	echo set ip: $ip
    udhcpc -r $ip -n
    if [ $? -eq 0 ]; then
        curip=$(ifconfig | grep 'inet addr:' | grep -v '127.0.0.1' | awk '{print $2}' | cut -d ':' -f 2)
        if [ "$ip" != "$curip" ]; then
            ifconfig eth0 $ip up
        fi
    else
        ifconfig eth0 $ip up
    fi
fi

