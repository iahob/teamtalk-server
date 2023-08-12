#!/bin/bash

# 拷贝配置文件
cfg_path=/home/github/TeamTalk/server/cfg/

echo "$cfg_path"

if [ ! -n "$1" ] ;then
	echo "usage:./setcfg.sh dest_dir"
	exit 1
fi

cp $cfg_path/dbproxyserver.conf $1/db_proxy_server/
cp $cfg_path/fileserver.conf $1/file_server/
cp $cfg_path/httpmsgserver.conf $1/http_msg_server/
cp $cfg_path/loginserver.conf $1/login_server/
cp $cfg_path/msfs.conf $1/msfs/
cp $cfg_path/msgserver.conf $1/msg_server/
cp $cfg_path/pushserver.conf $1/push_server/
cp $cfg_path/routeserver.conf $1/route_server/
cp $cfg_path/websocket_server.conf $1/websocket_server/

# 同步进程所需动态库
cd $1
./sync_lib_for_zip.sh
