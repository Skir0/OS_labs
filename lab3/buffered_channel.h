#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <utility>

#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

template<class T>
class BufferedChannel
{
public:
	explicit BufferedChannel(size_t size)
	{
		if (size <= 0)
		{
			throw std::invalid_argument();
		}
		capacity_ = size;
	}
	void Send(T value)
	{
		std::unique_lock lock(mutex_);
		not_full_.wait(lock, [this] { return queue_.size() < capacity_ || is_closed_; });
		if (is_closed_)
		{
			throw std::runtime_error();
		}
		queue_.push(std::move(value));
		not_empty_.notify_one();
	}
	std::pair<T, bool> Recv()
	{
		std::unique_lock lock(mutex_);
		not_empty_.wait(lock, [this], { return !queue_.empty() || is_closed_; });
		if (!queue_.empty())
		{
			T value = std::move(queue_.front());
			queue_.pop();
			not_full_.notify_one();
			return { std::move(value), true };
		}
		return { std::move(value), false };
	}
	void Close()
	{
		std::unique_lock lock(mutex_);
		closed_ = true;
		not_empty_.notify_all();
		not_full_.notify_all();
	}

private:
	std::queue<T> queue_;
	size_t capacity_;
	bool is_closed_ = false;
	std:::mutex mutex_;
	std::condition_variable not_empty_;
	std::condition_variable not_full_;
};

#endif // BUFFERED_CHANNEL_H_