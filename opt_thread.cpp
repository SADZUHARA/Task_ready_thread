#include "opt_thread.h"
OptimizeThreadPool::OptimizeThreadPool()
	: m_thread_count(thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 4),
	m_thred_queues(m_thread_count)
{}

void OptimizeThreadPool::start()
{
	//Создаем несколько потоков и помещаем их в  m_threads
	for (int i = 0; i < m_thread_count; i++)
	{
		m_threads.emplace_back(&OptimizeThreadPool::threadFunc, this, i);
	}
}

void OptimizeThreadPool::stop()
{
	for (int i = 0; i < m_thread_count; i++)
	{
		task_type empty_task;
		m_thred_queues[i].push(empty_task);
	}

	for (auto& t : m_threads)
		t.join();
}

void OptimizeThreadPool::push_task(FuncType f, vector<int>& vect, long arg1, long arg2)
{
	int queue_to_push = m_qindex++ % m_thread_count;//определяем индекс очереди в которую нужно положить задачу
	task_type new_task = task_type([&vect, arg1, arg2, f] {f(vect, arg1, arg2); });

	m_thred_queues[queue_to_push].push(new_task); // помещаем в очередь
}


void OptimizeThreadPool::threadFunc(int qindex)
{
	//бесконечный цикл читает задачи из очереди
	while (true)
	{
		task_type task_to_do;
		bool res; // для того чтобы определить удолось ли получить элемент из очереди
		int i = 0;
		for (; i < m_thread_count; i++) // пытаемся быстро получить задачу из очереди
		{
			if (res = m_thred_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do))
				break;
		}
		if (!res)
		{
			m_thred_queues[qindex].pop(task_to_do);
		}
		else if (!task_to_do)
		{
			m_thred_queues[(qindex + i) % m_thread_count].push(task_to_do);
		}
		// если кто-то запушил задачу проверяем
		if (!task_to_do) // если задача в очереди
		{
			return;
		}

		task_to_do();
	}
}

RequestHandler_2::RequestHandler_2()
{
	this->m_tpool.start();
}

RequestHandler_2::~RequestHandler_2()
{
	this->m_tpool.stop();
}

void RequestHandler_2::push_task(FuncType f, vector<int>& vect, long arg1, long arg2)
{
	this->m_tpool.push_task(f, vect, arg1, arg2);
}