# Reactor
参考 muduo 网络库 和 c++11 标准实现的reactor主从网络库，可以根据需求在server制定功能，目前实现建议http服务和echo服务

运行环境 ubuntu24.04 腾讯云服务器

注意cmake版本

下载方式
```
git clone https://github.com/De-MingWu/Reactor.git
```

reactor目录下编译
```
mkdir build && cd build
```
```
cmake ..
```

```
make
```

运行命令

运行前需要先创建日志文件夹
```
mkdir LogFile
```
运行命令，可在CmakeLists.txt文件夹修改生成的可执行程序名称
```
./httptest 127.0.0.1 8080
```
