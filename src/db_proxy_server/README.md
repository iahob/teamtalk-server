
# 说明

~~~
prepare before compile the server
1. install MySQL client library in /usr/local/mysql/
        add /usr/local/mysql/bin to $PATH
2. install Redis client library
        git clone https://github.com/redis/hiredis.git
        cd hiredis
        make
        sudo make install

~~~


# 一些问题：

## 1.多个db_proxy_server同时启动 会导致recentsession增加
主要是因为默认CSyncCenter会启动doSyncGroupChat 用于同步群聊会话的状态 多个db_proxy_server操作会存在并发操作的可能 所以会导致重复添加recentsession
