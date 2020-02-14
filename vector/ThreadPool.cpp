#include "ThreadPool.h"



ThreadPool::ThreadPool(int tubesize):tube(tubesize),is_over(0)
{
	this->start = 0;
	this->waiting = 0;
	this->working = 0;
	if (tubesize < 2 * MAXTHREAD)
		tube.changeMaxSize(2 * MAXTHREAD);
	is_over.set_CanUse();
}

ThreadPool::~ThreadPool()
{
	this->stopAtonce();
	//this->stopUntilTask();
}

void ThreadPool::put(shared_ptr<ThreadPoolImpl> sh)
{
	tube.put(sh);
}

void ThreadPool::stopAtonce()
{
	start = 0;
	for (int i = 0; i < MAXTHREAD; i++)
	{
		tube.put(nullptr);
	}
	cout << "插入结束" << endl;
	is_over.sem_V();
	pt.print_string("线程池中的线程全部退出结束");
}

shared_ptr<TubeImple> ThreadPool::get()
{
	return tube.get();
}

void ThreadPool::Comsumer()
{
	//消费者线程
	while (start== MAGIC)
	{
		working--;
		waiting++;
		auto ret = tube.get();
		working++;
		waiting--;
		if (dynamic_cast<ThreadPoolEnd*> (ret.get()))
		{
			break;
		}
		if (ret != nullptr)
		{
			ret->doit();
		}
	}
	working--;
	jude_over();
}

 

void ThreadPool::spy()
{
	//监控线程
	//while (start)
	while(start == MAGIC)
	{
		/*
			分为两种情况：
			第一个是线程太多 任务太少了 所以要削减工作线程的数目
					线程太少 任务太多   所以增加线程的数目
			第二种线程太少    任务太多  但是线程数目已经增加到极限值
		*/
		this_thread::sleep_for(1s);
		pt.print_string_num("           正在工作的线程数目", working);
		pt.print_string_num("           正在等待的线程数目", waiting);
		//if (start == MAGIC)
		{
			int all_workingthread = working + waiting;
			int waitingthread = waiting;
			//如果线程再这个区间的话 可以增加线程的数目

			if (all_workingthread >= MINTHREAD && all_workingthread < MAXTHREAD)
			{
				//如果任务队列中有任务的话 那就加入线程
				if (tube.size() == 0 && all_workingthread > MINTHREAD)
				{
					tube.put(shared_ptr<ThreadPoolEnd>(new ThreadPoolEnd()));
				}
				else if (tube.size() > 0)
				{
					working++;
					thread th(&ThreadPool::Comsumer, this);
					th.detach();
				}
			}
			else if (all_workingthread >= MAXTHREAD)
			{
				//如果工作的线程和等待的线程数目超过了MAXTHREAD 那就插入一个结束标志
				if (tube.size() == 0)
				{
					//如果任务队列中没有任务 那就
					tube.put(shared_ptr<ThreadPoolEnd>(new ThreadPoolEnd()));
				}
			}
		}
	}
}

void ThreadPool::startPool()
{
	start = MAGIC;
	for (int i = 0; i < MINTHREAD; i++)
	{
		thread th(&ThreadPool::Comsumer, this);
		th.detach();
		working++;
	}
	//监控线程
	thread th_spy(&ThreadPool::spy, this);
	th_spy.detach();
}

void ThreadPool::jude_over()
{
	lock_guard<mutex> lock(mu);
	int all_thread = working + waiting;
	if (all_thread == 0)
	{
		is_over.sem_P();
		pt.print_string("judeover 线程池中的线程全部退出结束");
	}
}