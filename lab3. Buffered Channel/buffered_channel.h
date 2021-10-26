#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(size_t size): max_size(size), is_closed(false) {}

    void Send(T value) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            // to avoid spurious wakeup
            while (isFull()) {
                cv_full.wait(lock);
            }
            if(is_closed) {
                throw std::runtime_error("An attempt to send data through closed channel.");
            }
            q.push(value);
            cv_empty.notify_one();
        }
    }

    std::pair<T, bool> Recv() {
        std::pair<T, bool> p{};
        {
            std::unique_lock<std::mutex> lock(mutex);
            // to avoid spurious wakeup
            while (!is_closed && isEmpty()) {
                cv_empty.wait(lock);
            }
            if(!isEmpty()) {
                p.first = q.front();
                p.second = true;
                q.pop();
                cv_full.notify_one();
            }
        }
        return p;
    }

    void Close() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            is_closed = true;
            // To avoid waiting for elements while channel's closed.
            cv_empty.notify_all();
        }
    }

private:

    size_t max_size;
    bool is_closed;
    std::queue<T> q;
    std::mutex mutex;
    // for notifications about adding elements to queue
    std::condition_variable cv_empty;
    // for notifications about getting elements from queue
    std::condition_variable cv_full;

    bool isFull(){
        return q.size() == max_size;
    }

    bool isEmpty(){
        return q.empty();
    }
};

#endif // BUFFERED_CHANNEL_H_