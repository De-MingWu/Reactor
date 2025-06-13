#include <sys/syscall.h>     // for SYS_gettid
#include <unistd.h>          // for syscall()

#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadNum, const std::string &threadType):stop_(false), threadType_(threadType)
{
    //启动threadNum个线程，每个线程将阻塞在条件变量上
    for(size_t i = 0; i < threadNum; i++)
    {
        //用lambda函数创建线程
        threads_.emplace_back([this]
                              {
                                  printf("create %s thread(%ld).\n", threadType_.c_str(), syscall(SYS_gettid));
                                  while(!stop_)
                                  {
                                      std::function<void()> task;
                                      //锁作用域开始
                                      {
                                          std::unique_lock<std::mutex> lock(this->mutex_);

                                          //等待生产者的条件变量
                                          this->condition_.wait(lock, [this]
                                          {
                                              return ((this->stop_) || (!this->taskQueue_.empty()));
                                          });
                                          //在线程池停止之前， 如果队列中还有任务，执行完再退出
                                          if((this->stop_) && (this->taskQueue_.empty()))
                                              return;

                                          //出队一个任务
                                          task = std::move(this->taskQueue_.front());
                                          this->taskQueue_.pop();
                                      }
                                      //锁作用域结束
                                      //printf("%s (%ld) execute task.\n", threadType_.c_str(), syscall(SYS_gettid));
                                      //执行任务
                                      task();
                                  }
                              });
    }
}

/**
 * 停止线程
 */
void ThreadPool::StopThread()
{
    if(stop_) return;

    stop_ = true;
    condition_.notify_all();//唤醒全部线程
    //等待全部线程执行完后任务退出
    for(std::thread &th : threads_)
        th.join();
}

/**
 * 将任务添加到队列中
 * @param task
 */
// void ThreadPool::AddTasks(std::function<void()> task)
// {
//     {
//         std::lock_guard<std::mutex> lock(mutex_);
//         if(stop)
//             throw std::runtime_error("ThreadPool already stop, can't add task any more");
//         taskQueue_.push(task);
//     }
//     condition_.notify_one();//唤醒一个线程
// }

/**
 * 线程池大小
 * @return
 */
size_t ThreadPool::GetSize()
{
    return threads_.size();
}

/**
 * 析构函数中停止线程
 */
ThreadPool::~ThreadPool()
{
    StopThread();
}