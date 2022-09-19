# TeamTalk

## 环境准备

1.ubuntu gcc  cmake 

## 搭建IM

### 安装基础环境
```bash
mkdir build
cd build && cmake ..
make && make install 
```

### 安装数据库

1.安装数据库：mariadb

```basj
> yum install -y mariadb-devel mariadb-server
```

2.安装redis

```bash
> yum install -y redis
> 或者
> cd /etc/yum.repos.d/ && wget http://mirrors.aliyun.com/repo/epel-6.repo && sudo yum install -y redis && sudo service redis start
```

### 编译安装其他依赖项


### 运行

#### 配置并导入数据库

```bash
> cd /home/repo/github/TeamTalk/auto_setup/mariadb/ && chmod +x *.sh && ./setup.sh install 

#在这里会提示修改 MariaDB 的 root 密码
#请确保设置为 12345（如果不是，进入mysql执行：set password for root@localhost = password('12345');）
#即确保和 dbproxyserver.conf 和 /home/repo/github/TeamTalk/auto_setup/mariadb/setup.sh 文件里的保持一致。剩下的 MariaDB 的设置可以都按照默认的来（一路回车）。

Enter current password for root (enter for none): 【直接回车】
Set root password? [Y/n] y
New password: 12345
Re-enter new password: 12345
Remove anonymous users? [Y/n] 【直接回车】
Disallow root login remotely? [Y/n] 【直接回车】
Remove test database and access to it? [Y/n] 【直接回车】
Reload privilege tables now? [Y/n]【直接回车】

# 输出如下结果代表成功
ttopen.sql existed, begin to run ttopen.sql
run sql successed.
create database successed.
```

#### 防火墙设置

以下操作二选其一（如果不熟悉，请关闭防火墙）：
1.关闭防火墙

```bash
> systemctl disable firewalld.service # 防火墙默认开机不启动  
> systemctl stop firewalld.service    # 防火墙关闭
```

2.开放防火墙端口

| 名称 | 类型 | 端口 |
| ---- | ---- | ---- |
| login_server | http | 8080 |
| msg_server | tcp | 8000  |
| msfs_server | http | 8700 |
| file_server | tcp | 8600 |
| push_server | tcp | 8500 |

#### 开机启动mariadb和redis

```bash
> systemctl enable redis      # 开机启动redis  
> systemctl start redis  
> systemctl enable mariadb    # 开机启动mariadb  
> systemctl start mariadb
```

#### 手动添加2个测试账号

ps：密码是123456，用户名是gaozz和hanmm。

``` sql
mysql -u root -p12345  
show databases;  
use teamtalk;  
INSERT INTO IMDepart (`id`, `departName`, `priority`, `parentId`, `status`, `created`, `updated`) VALUES ('1', '测试', '0', '0', '0', '1546410063', '1546410063');  
INSERT INTO `teamtalk`.`IMUser` (`id`, `sex`, `name`, `domain`, `nick`, `password`, `salt`, `phone`, `email`, `avatar`, `departId`, `status`, `created`, `updated`, `push_shield_status`, `sign_info`) VALUES ('7', '0', 'gaozz', '0', '高真真', 'abe84781319bd8222792f124245429f8', '1952', '', '', 'http://127.0.0.1:8700/', '1', '0', '1548215851', '1548215942', '0', '');  
INSERT INTO `teamtalk`.`IMUser` (`id`, `sex`, `name`, `domain`, `nick`, `password`, `salt`, `phone`, `email`, `avatar`, `departId`, `status`, `created`, `updated`, `push_shield_status`, `sign_info`) VALUES ('8', '0', 'hanmm', '0', '韩梅梅', 'd4e546771dd30d75076b928326947df1', '4650', '', '', 'http://127.0.0.1:8700/', '1', '0', '1548215935', '1548215948', '0', '');
```

如果需要测试群组：
```sql
INSERT INTO `teamtalk`.`IMGroup` (`id`, `name`, `avatar`, `creator`, `type`, `userCnt`, `status`, `version`, `lastChated`, `updated`, `created`) VALUES ('1', '测试群', '', '7', '2', '2', '0', '1', '0', '0', '0');
INSERT INTO `teamtalk`.`IMGroupMember` (`id`, `groupId`, `userId`, `status`, `created`, `updated`) VALUES ('1', '1', '7', '0', '0', '0');
INSERT INTO `teamtalk`.`IMGroupMember` (`id`, `groupId`, `userId`, `status`, `created`, `updated`) VALUES ('2', '1', '8', '0', '0', '0');
```

#### 配置并运行IM服务

1. 解压到/data目录
```bash
$ cp /home/repo/github/TeamTalk/server/im-server-test.tar.gz /data
$ cd /data
$ tar -zxvf im-server-test.tar.gz # 解压
```

2. dbproxyserver，确保数据库和redis配置正确
``` bash
$ cd /data/im-server-test
$ vim db_proxy_server/dbproxyserver.conf

#确认：teamtalk_master_username=root, teamtalk_master_password=12345
#确认：teamtalk_slave_username=root, teamtalk_slave_password=12345
```

3. msg_server，修改监听IP
```bash
$ vim msg_server/msgserver.conf
# 修改：IpAdr1 为 运行机器的 IP 地址, ifconfig，如果是公网，则配置公网IP
# 修改：IpAdr2 为 运行机器的 IP 地址, ifconfig，如果是公网，则配置公网IP
```

4. 同步依赖库
```bash
$ chmod 777 *.sh
$ ./sync_lib_for_zip.sh
# 手动拷贝一下，db_proxy_server依赖，直接放到系统目录，省事
$ cp /home/repo/github/TeamTalk/server/src/hiredis/hiredis-master/libhiredis.so /usr/lib64/libhiredis.so.0.13
```

5. 启动服务
```bash
$ restart.sh all
```

6. 如果成功，则显示：

```bash
$ ps -ef|grep _server

root      7471     1  0 16:38 ?        00:00:00 ./route_server
root      7474     1  0 16:38 ?        00:00:00 ./file_server
root      7516     1  0 16:38 ?        00:00:00 ./http_msg_server
root      7520     1  0 16:38 ?        00:00:00 ./push_server
root      7529     1  0 16:38 ?        00:00:00 ./login_server
root      7530     1  0 16:38 ?        00:00:00 ./msg_server
root      7544     1  0 16:38 ?        00:00:00 ./websocket_server
```

否则：查看log，排查错误

```bash
> cat msg_server/log/default.log  
> 或者  
> tail -f msg_server/log/default.log   # 实时查看
```

#### 测试客户端

1. 下载客户端，这里已经编译好了windows版本：[百度网盘](https://pan.baidu.com/s/1tjlN_BF1H9QcEEykGbwDbw) 提取码：kt6e
2. 解压，双击bin/teamtalk.exe
3. 点击“设置”按钮，把默认的“http://access.teamtalk.im:8080/msg_server”更改为“http://IpAdr1:8080/msg_server”（loginServer部署的机器IP，不是MsgServer）
4. 输入账号和密码（上面插入了gaozz和hanmm共2个测试用户，密码为123456）
5. 点击登录，如果失败，去服务端查看log。主要看login_server,msg_server,db_proxy_server。（tail -f login_server/log/default.log）

## 公网映射

公网只需要映射如下服务的端口：
| 名称 | 类型 | 端口 |
| ---- | ---- | ---- |
| login_server | http | 8080 |
| msg_server | tcp | 8000  |
| msfs_server | http | 8700 |
| file_server | tcp | 8600 |

具体参考腾讯云和阿里云的安全组配置。

## 常见错误

### db_proxy_server没启动

1. 查看log，如果没有log。直接./db_proxy_server执行一下，看是否缺少依赖。PS：如果缺少libhiredis.so.0.13，手动拷贝一下
```bash
$ cp /home/repo/github/TeamTalk/server/src/hiredis/hiredis-master/libhiredis.so /data/im-server-test/db_proxy_server/libhiredis.so.0.13
$ cd /data/im-server-test && ./restart.sh db_proxy_server # 重启启动db_proxy_server
$ ps -ef|grep server # 查看是否存在db_proxy_server
```

2. 检测redis是否启动，如果没启动。vim /etc/redis.conf，查看监听的端口是否正确。
3. 检测mysql是否启动，用户名密码是否配置正确。

找到问题后，可以单独启动服务：
```bash
./restart.sh db_proxy_server
```

### 客户端报“服务端异常”

1. 查看login_server的日志和msg_server的日志，以及db_proxy_server的日志。

## 搭建WEB

1. 安装nginx
```bash
# 安装nginx
$ yum install nginx 
$ systemctl restart nginx
$ netstat -anp|grep nginx
tcp        0      0 0.0.0.0:80            0.0.0.0:*               LISTEN      16843/nginx: master
tcp6       0      0 :::80                 :::*                    LISTEN      16843/nginx: master
# 此时浏览器输入：http://ip:80，如果出现页面代表成功
```

2. 安装php
推荐：
```bash
$ yum remove php*   # 先卸载旧的包
# webtatic 源
$ rpm -Uvh https://mirror.webtatic.com/yum/el7/webtatic-release.rpm
# 安装php5.6 和 php-fpm 插件
$ yum install php56w php56w-fpm
$ cp /home/repo/github/TeamTalk/auto_setup/nginx_php/php/conf/php-fpm.conf /etc/php-fpm.conf
$ cp /home/repo/github/TeamTalk/auto_setup/nginx_php/php/conf/php.ini /etc/php.ini
$ chmod 755 /etc/php-fpm.conf
$ chmod 755 /etc/php.ini
```

验证php: 
```bash
# 创建目录，存放web文件
$ mkdir -p /var/www/html
# 创建验证nginx配置文件
$ vi /etc/nginx/conf.d/default.conf
server {
    listen       8080;
    server_name  localhost;

    location / {
        root   /var/www/html;
        # 默认页增加index.php
        index  index.php index.html index.htm;
    }

    #error_page  404              /404.html;

    # redirect server error pages to the static page /50x.html
    #
    error_page   500 502 503 504  /50x.html;
    location = /50x.html {
        root   /var/www/html;
    }

    #去掉location ~ \.php$配置节前面的#
    location ~ \.php$ {
        root           html;
        #php-fpm默认的监听端口为9000
        fastcgi_pass   127.0.0.1:9000;
        fastcgi_index  index.php;
        # 文件位置修改为/usr/share/nginx/html
        fastcgi_param  SCRIPT_FILENAME  /var/www/html$fastcgi_script_name;
        include        fastcgi_params;
    }
}

# 创建测试php页面
$ vim /var/www/html/index.php
<?php
 phpinfo() 
?>
$ nginx -s reload #重新载入nginx
$ systemctl enable php-fpm #将php-fpm设置为开启启动
$ systemctl start php-fpm #启动php-fpm服务
$ #浏览器输入：http://ip:8080，如果出现 https://www.cnblogs.com/cnxkey/articles/10685489.html 页面的图片代表成功
```

废弃: 
``` bash
> cd /home/repo/github/TeamTalk/auto_setup
> chmod 777 nginx_php/nginx/setup.sh && chmod 777 nginx_php/php/setup.sh # 脚本执行权限
> ./nginx_php/nginx/setup.sh install    # 安装nginx
> ./nginx_php/php/setup.sh install      # 安装php5.6.6
```

2. 部署发布tt管理web后台

```bash
$ yum install -y zip                                        # 安装压缩软件
$ cd /home/repo/github/TeamTalk && cp -rf php/ tt && zip -r tt.zip tt # 打tt.zip的web源码包
$ mv tt.zip auto_setup/im_web/                              # 放入im_web下，以便脚本能解压安装
$ cd auto_setup/im_web/
$ chmod 777 setup.sh
$ ./setup.sh install                                        # 发布web包到 nginx /var/www/html/目录下
$ cd /etc/nginx/conf.d/                                     # 进入nginx配置文件目录
$ vim im.com.conf # 把80改为如8080                           # 更改web访问端口
```

3. 临时关闭selinux[https://blog.csdn.net/lzm198707/article/details/50130615](永久关闭selinux参考)

```bash
> getenforce  
> setenforce 0      ##设置SELinux 成为permissive模式
```

4. 如果启动nginx

```bash
$ systemctl start nginx  
$ systemctl status nginx    # 查看nginx启动状态
$ tail -f /var/log/nginx/error.log # 查看nginx错误log
```

5. 如果要启动php-fmp [https://www.cnblogs.com/hnhycnlc888/p/9434309.html](参考)

```bash
$ systemctl start php-fpm
```

6.打开浏览器，输入：http://ip:8080即可，默认用户admin\admin
