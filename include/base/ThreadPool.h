#ifndef LEARN_THREADPOOL_H
#define LEARN_THREADPOOL_H

#include <vector>
#include <future>
#include <queue>
#include <memory>
#include <functional>


class ThreadPool
{
private:
    std::vector<std::thread> threads_;            //线程池中的线程
    std::queue<std::function<void()>> taskQueue_; //任务队列
    std::mutex mutex_;                            //任务队列同步的互斥锁
    std::condition_variable condition_;           //任务队列同步的条件变量
    std::atomic_bool stop_;                       //在析构函数中，把stop_的值设置为true，全部的线程将退出
    std::string threadType_;                      //线程种类，IO， Work

public:
    //构造函数中启动threadNum个线程
    ThreadPool(size_t threadNum, const std::string &threadType);
    //析构函数中停止线程
    ~ThreadPool();

    //获取线程池大小
    size_t GetSize();

    //停止线程
    void StopThread();

    //将任务添加到队列中
    // void AddTasks(std::function<void()> task);
    template<class F, class... Args>
    auto AddTasks(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> 
    {
        using return_type = typename std::result_of<F(Args...)>::type;  //返回值类型

        auto task = std::make_shared<std::packaged_task<return_type()>>(  //使用智能指针
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)  //完美转发参数
            );  
            
        std::future<return_type> res = task->get_future();  // 使用期约，用它在主线程等待/获取任务执行结果
        {   //队列锁作用域
            std::unique_lock<std::mutex> lock(mutex_);   //加锁

            if(stop_)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            taskQueue_.emplace([task](){ (*task)(); });  //将任务添加到任务队列
        }
        condition_.notify_one();    //通知一次条件变量
        return res;     //返回一个期约
    } 
};


#endif //LEARN_THREADPOOL_H
