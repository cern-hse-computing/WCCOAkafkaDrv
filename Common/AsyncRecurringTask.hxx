/** © Copyright 2019 CERN
 *
 * This software is distributed under the terms of the
 * GNU Lesser General Public Licence version 3 (LGPL Version 3),
 * copied verbatim in the file “LICENSE”
 *
 * In applying this licence, CERN does not waive the privileges
 * and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 *
 * Author: Alexandru Savulescu (HSE-CEN-CO)
 *
 **/

#ifndef AsyncRecurringTask_HXX
#define AsyncRecurringTask_HXX

#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <future>


/**
 *  @brief An asynchrounous recurring task wrapper
 */


namespace Common
{
    template <class Task>
    class AsyncRecurringTask
    {
    public:
        AsyncRecurringTask(Task&& action, const uint32_t& timeoutMs)
            : _action(std::move(action)), _timeoutMs(timeoutMs)
        {
            _future = std::async(std::launch::async, [this]()
                {
                    while(!this->_done)
                    {
                        this->_action();
                        this->sleep();
                    }
                });
        }

        AsyncRecurringTask(const AsyncRecurringTask& ) = delete;
        AsyncRecurringTask& operator=(const AsyncRecurringTask& ) = delete;

        ~AsyncRecurringTask()
        {
            // This will break the sleep cycle if the task is destroyed
            _done = true;
            _cv.notify_one();
        }


    private:
        void sleep()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait_for(lock, std::chrono::milliseconds(_timeoutMs));
        }

        Task _action;
        std::atomic<bool> _done{false};
        std::future<void> _future;
        std::condition_variable _cv;
        std::mutex _mutex;
        const uint32_t& _timeoutMs;
    };
}
#endif // AsyncRecurringTask_HXX
