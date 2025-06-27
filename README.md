# Reactor网络库

## 1 介绍

基于C++11标准，重写moduo网络库核心部分

1. 底层使用 **Epoll + LT** 模式的I/O复用模型,并结合非阻塞I/ O实现**主从****Reacto**r模型；

2. 采用**[one loop per thread]**线程模型，并向上封装线程池避免线程创建和销毁带来的性能开销；

3. 遵循**RAII** 手法使用智能拍针管理内存，减小内存泄露风险，同时通过引用计数的方式管理或延长某些资源的生命周期；

4. 采用**eventfd** 作为事件通知描述符，方便高效派发事件到其他线程执行异步任务；
   
5. 实现异步日志和http协议解析和传输

6. 在单线程环境下支持5万并发连接，**QPS达4.24 W/S**，相较传统epoll架构（QPS约**0.01W/S**）提升两个数量级；

## 2 目录结构

```
Reactor/
├── include     # 头文件部分
│   ├── base    # 一些公共的头文件
│   ├── http    # http请求 解析部分
│   ├── log     # 同步、异步日志实现
│   ├── net     # 网络核心
│   └── service # 主要功能实现部分
└── src 	    # 函数实现
    ├── base
    ├── fileapp # 主文件入口部分
    ├── http
    ├── log
    ├── net
    └── service
```

## 3 环境

WSL2024.04 

json库，安装命令如下

```
sudo apt update
sudo apt install nlohmann-json3-dev
```

## 4 编译和运行

```
git clone https://github.com/De-MingWu/Reactor.git

cd Reactor

mkdir build && cd build

cmake ..

make

./test 127.0.0.1 8888
```

在浏览器输入127.0.0.1:8888 可以测试 get请求，运行client可以测试简单post请求
