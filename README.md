# 基于 C++的高性能网络库
这是一个基于非阻塞IO和Reactor模式的现代C++网络库。

该网络库主要通过事件循环EventLoop响应定时器 Timer和 IO事件，支持 one loop per thread的 IO模型，从而实现多线程并发。

1. 对多线程相关的类进行了封装，实现了线程池；

2. 日志系统：支持不同的日志记录级别、支持日志滚动、由单独线程写入日志文件

3. 对socket及相关的底层系统调用进行了封装

4. 设计实现应用层缓冲区 Buffer

5. 实现了基于红黑树的定时器

6. one loop per thread + thread pool的多线程并发模型实现

技术要点：Linux网络编程、服务器性能优化、TCP协议栈、C++智能指针、设计模式