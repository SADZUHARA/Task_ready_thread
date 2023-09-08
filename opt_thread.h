#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include<functional>
#include <condition_variable>
#include <future>

using namespace std;

typedef function<void()> task_type; 
typedef void (*FuncType) (vector<int>&, long, long); 



template<class T> class BlockedQueue
{
public:
	void push(T& item)
	{
		lock_guard<mutex> l(m_locker); //безопасно блокируем мьютекс
		m_task_queue.push(item); // помещаем в очередь

		m_event_holder.notify_one(); //оповещаем поток о том что мы поместили задачу и её можно выполнить
	}

	void pop(T& item)
	{
		unique_lock<mutex> l(m_locker);
		if (m_task_queue.empty())
			m_event_holder.wait(l, [this] {return !m_task_queue.empty(); });

		item = m_task_queue.front();
		m_task_queue.pop();
	}

	bool fast_pop(T& item)
	{
		unique_lock<mutex> l(m_locker);
		if (m_task_queue.empty())
			return false;
		item = m_task_queue.front();
		m_task_queue.pop();
		return true;
	}
private:
	queue<T> m_task_queue; //очередь хранит функции которые нужно вызвать
	mutex m_locker;
	condition_variable m_event_holder;
};

class OptimizeThreadPool
{
public:
	OptimizeThreadPool();
	void start(); // запуск потоков в m_threads
	void stop(); // остановка
	void push_task(FuncType f, vector<int>& vect, long arg1, long arg2); //метод проброса задачи(функции) в пулл потоков
	void threadFunc(int qindex); //входящая функция для потока
private:
	int m_thread_count; // счетчик запущеных потоков
	vector<thread> m_threads; //вектор потоков
	vector<BlockedQueue<task_type>> m_thred_queues;
	unsigned m_qindex; //счетчик того в какую из очередей добавлена задача
};


class RequestHandler_2 // класс обертка над классом ThreadPool
{
public:
	RequestHandler_2();
	~RequestHandler_2();
	void push_task(FuncType f, vector<int>& vect, long arg1, long arg2);
private:
	OptimizeThreadPool m_tpool;
};

