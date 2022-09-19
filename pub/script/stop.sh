#/bin/sh
#start or stop the im-server


curpath=$(pwd)

LD_LIBRARY_PATH=$curpath/lib
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH"

function stop() {
    cd $1
    if [ ! -e *.conf  ]
    then
        echo "no config file"
        return
    fi

    if [ -e server.pid  ]; then
        pid=`cat server.pid`
        echo "kill $1,pid=$pid"
        kill $pid
        while true
        do
            oldpid=`ps -ef|grep $1|grep $pid`;
            if [ "$oldpid" == "$pid" ]; then
                echo "wait $1[oldPid:$oldpid,newPid:$pid] exit"
                sleep 10
            else
                break
            fi
        done
    fi
    cd ..
}

case $1 in
    all)
		stop websocket_server
		stop msg_server
		stop login_server
		stop push_server
		stop http_msg_server
		stop msfs
        stop route_server
        stop file_server
		stop db_proxy_server
        ;;
    login_server)
        stop $1
        ;;
    msg_server)
        stop $1
        ;;
    route_server)
        stop $1
        ;;
    http_msg_server)
        stop $1
        ;;
    file_server)
        stop $1
        ;;
    push_server)
        stop $1
        ;;
    db_proxy_server)
        stop $1
        ;;
    msfs)
        stop $1
        ;;
    websocket_server)
        stop $1
        ;;
    *)
        echo "Usage: "
        echo "  ./stop.sh (all|login_server|msg_server|route_server|http_msg_server|file_server|push_server|websocket_server)"
        ;;
esac
