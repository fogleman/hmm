#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(int numThreads = std::thread::hardware_concurrency()) :
        m_Stop(false)
    {
        for (int i = 0; i < numThreads; i++) {
            m_Threads.emplace_back([this]() {
                while (1) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(m_Mutex);
                        m_Condition.wait(lock, [this]() {
                            return m_Stop || !m_Queue.empty();
                        });
                        if (m_Stop && m_Queue.empty()) {
                            return;
                        }
                        task = std::move(m_Queue.front());
                        m_Queue.pop();
                    }

                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> guard(m_Mutex);
            m_Stop = true;
        }
        m_Condition.notify_all();
        for (auto &thread: m_Threads) {
            thread.join();
        }
    }

    int NumThreads() const {
        return m_Threads.size();
    }

    template<class F, class... Args>
    auto Add(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using ReturnType = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<ReturnType> result = task->get_future();

        {
            std::lock_guard<std::mutex> guard(m_Mutex);
            if (!m_Stop) {
                m_Queue.emplace([task]() {
                    (*task)();
                });
            }
        }

        m_Condition.notify_one();
        return result;
    }

private:
    std::vector<std::thread> m_Threads;
    std::queue<std::function<void()>> m_Queue;
    std::mutex m_Mutex;
    std::condition_variable m_Condition;
    bool m_Stop;
};
