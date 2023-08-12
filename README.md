
### 编译方式

开启wsl root模式
ubuntu config --default-user root

or
创建.wslconfig 文件
```angular2html
[wsl2]
#networkingMode=bridged
#vmSwitch=WSLCore
dhcp=false
memory=10GB
swap=4GB
macAddress=5c:bb:f6:9e:ee:55
[boot]
systemd=true
[user]
default=root

```


```bash
mkdir build
cd build && cmake ..
make && make install 
```