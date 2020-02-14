#include"thread_pool_spy.h"

ThreadPoolSpy::ThreadPoolSpy()
{
	this->spy_time = 1;
}

ThreadPoolSpy::~ThreadPoolSpy()
{}

void ThreadPoolSpy::stopOneThreadRecode(thread::id id)
{
	lock_guard<mutex> lock(m_spyinfo);
	auto i = spy_info.find(id);
	for (auto it = (*i).second.begin(); it != (*i).second.end(); it++)
	{
		(*it).overThread();
	}
}

void ThreadPoolSpy::stopAllThreadRecode()
{
	lock_guard<mutex> lock(m_spyinfo);
	for (auto i = spy_info.begin(); i != spy_info.end(); i++)
	{
		for (auto it = (*i).second.begin(); it != (*i).second.end(); it++)
		{
			(*it).overThread();
		}
	}
	return;
}

void ThreadPoolSpy::consum()
{
	//在此处添加任务监视
	while (true)
	{
		if (!m_canuse)
		{
			//pt.print_string_num("    因线程停用销毁  threadid: ", this_thread::get_id());
			working--;
			m_endcon.notify_one();
			return;
		}

		unique_lock<mutex> lock(m_synchr);
		while (isEmpty())
		{
			working--;
			waiting++;//如果某个线程在此处 然后进行停止线程池的操作 
					  //那么线程池的spy将永远不会被停止  何解<或许无解> 
			m_convec.wait(lock);
			working++;
			waiting--;
			if (isEmpty())//队列为空
			{
				working--;
#ifndef _SpyTest1_ 
				pt.print_string_num("        因任务队列中的任务过少而销毁线程 ", this_thread::get_id());
#endif 

				stopOneThreadRecode(this_thread::get_id());
				return;
			}
		}
		lock.unlock();
		shared_ptr<TaskPack> dt = get();
		if (dt != nullptr)
		{
			if (dynamic_cast<solveTaskEd*>(dt.get()))
			{
				//cout << "end" << endl;
				m_allsolvcon.notify_one();
			}
			else
			{
				m_provec.notify_one();
				dt->doit();
				//this_thread::get_id();
				insertSpyArg(this_thread::get_id(), SpyInfoPack(0, "working"), dt->task);
			}
		}
	}
}

void ThreadPoolSpy::put(shared_ptr<TaskPack> arg)
{
	//获取数据	 
	if (!getCanUse())
		return;
	unique_lock<mutex> lock(m_synchr);
	while (isOverFlow())
	{
		m_provec.wait(lock);
	}

	m_taskvec.push_back(arg);
	lock.unlock();
	//通知消费者线程
	m_convec.notify_one();
}
 
std::string ThreadPoolSpy::getNowTime()
{
	time_t setTime;
	time(&setTime);
	tm* ptm = localtime(&setTime);
	std::string time = std::to_string(ptm->tm_year + 1900)
		+ "/"
		+ std::to_string(ptm->tm_mon + 1)
		+ "/"
		+ std::to_string(ptm->tm_mday)
		+ " "
		+ std::to_string(ptm->tm_hour) + ":"
		+ std::to_string(ptm->tm_min) + ":"
		+ std::to_string(ptm->tm_sec);
	return time;
 
}


void ThreadPoolSpy::putWithoutTaskLimit(shared_ptr<TaskPack> arg)
{
	try_put(arg);
}


bool ThreadPoolSpy::try_put(shared_ptr<TaskPack> arg)
{
	if (!getCanUse())
		return false;
	{
		lock_guard<mutex> lock(m_synchr);
		m_taskvec.push_back(arg);
	}
	//通知消费者线程
	m_convec.notify_one();
	return true;
}

bool ThreadPoolSpy::isEmpty()
{
	return m_taskvec.size() == this->min_num_vec;
}

bool ThreadPoolSpy::isOverFlow()
{
	return m_taskvec.size() == this->max_num_vec;
}

void ThreadPoolSpy::insertSpyArg(thread::id id, SpyInfoPack arg, string task)
{
	lock_guard<mutex> lock(m_spyinfo);
	arg.now_act = task;
	arg.now_time = getNowTime();
	map<thread::id, vector<SpyInfoPack>>::iterator it = spy_info.find(id);
	if (it == spy_info.end())
	{
		vector<SpyInfoPack> vf_tem;
		vf_tem.push_back(arg);
		spy_info.insert(pair<thread::id, vector<SpyInfoPack>>(id, vf_tem));
	}
	else
	{
		vector<SpyInfoPack>::iterator  it_fd = find((*it).second.begin(), (*it).second.end(), arg);
		if (it_fd == (*it).second.end())
		{
			//没有这个节点信息
			(*it).second.push_back(arg);
		}
		else
		{
			(*it_fd).now_act = task;
			(*it_fd).now_time = getNowTime();

			(*it_fd).upperThreadTm();
		}
	}
}

shared_ptr<TaskPack> ThreadPoolSpy::get()
{
	unique_lock<mutex> lock(m_synchr);
	while (isEmpty())
	{
		m_convec.wait(lock);
	}
	auto ret = *m_taskvec.begin();
	m_taskvec.pop_front();
	lock.unlock();
	//通知生产者线程
	m_provec.notify_one();
	return ret;
}

shared_ptr<TaskPack> ThreadPoolSpy::getDataToVec()
{
	lock_guard<mutex> lock(m_task);
	auto ret = *m_taskvec.begin();
	m_taskvec.pop_front();
	return ret;
}

bool ThreadPoolSpy::isLargerToMaxWorking()
{
	if (working >= max_num_thread)
		return true;
	return false;
}

bool ThreadPoolSpy::isMinnerToMinWorking()
{
	if (working >= min_num_thread)
		return true;
	return false;
}

bool ThreadPoolSpy::isInBewteenWorking()
{
	if (working >= min_num_thread && working < max_num_thread)
		return true;
	return false;
}

bool ThreadPoolSpy::isLargerToMaxWaiting()
{
	if (waiting >= 3)
		return true;
	return false;
}

bool ThreadPoolSpy::isMinnerMinWaiting()
{
	if (waiting <= 2 && waiting > 0)
		return true;
	return false;
}

bool ThreadPoolSpy::isMinnerIsZero()
{
	if (waiting == 0)
		return true;
	return false;
}

bool ThreadPoolSpy::isInBewteenWaiting()
{
	if (waiting > 2 && working < 3)
		return true;
	return false;
}

void ThreadPoolSpy::spyControl()
{
	//监视消费者线程
	while (true)
	{

		this_thread::sleep_for(1s);
#ifndef _SpyTest1_ 
		pt.print_string_num("           当前线程中的工作线程数目  ", working);
		pt.print_string_num("           当前线程中wait工作线程数目", waiting);
		pt.print_string_num("           当前任务队列中的任务数目  ", m_taskvec.size());
#endif 

#ifndef _SpyTest2_ 
		reportTask();
		pt.print_next_line();
#endif 
		{
			insertSpyArg(this_thread::get_id(), SpyInfoPack(0, "spy"), "spy");
			lock_guard<mutex> lock(m_synchr);
			if ((working + waiting) == 0)
			{
				//this_thread::sleep_for(0.01s);
				m_endcon.notify_one();
				break;
			}
			if (isMinnerMinWaiting())          //在wait集中等待线程数目 在[0, 1]则不做处理
			{
			}
			else if (isMinnerIsZero())         //在wait集中等待线程数目   为 0 说明消费者线程不够用了 
			{
				if (isLargerToMaxWorking())	   //超过了允许运行的线程数目
				{
				}
				else if (isInBewteenWorking()) //在两者之间
				{
					thread th = std::move(thread(&ThreadPoolSpy::consum, this));
#ifndef _SpyTest1_ 
					pt.print_string_num("           Create Thread Id:", th.get_id());
#endif 

					insertSpyArg(th.get_id(), SpyInfoPack(0, "working"), "working");
					th.detach();
					working++;

					int* p = ((int*)&th.get_id());
					vec.push_back(*p);
					//pair<thread::id, SpyInfoPack> arg();
					//spy_info.insert(arg);
				}
			}
			else if (isLargerToMaxWaiting())  //在wait集中等待线程数目   为 [3,+] 说明当前业务比较少 
			{
				m_convec.notify_one();
			}
		}
	}
}

void ThreadPoolSpy::start()
{
	if (m_canuse)
	{
		thread th_spy = std::move(thread(&ThreadPoolSpy::spyControl, this));
		insertSpyArg(th_spy.get_id(), SpyInfoPack(0, "spy"), "spying");
#ifndef _SpyTest1_ 
		pt.print_string_num("           Create Spy Thread Id:", th_spy.get_id());
#endif

		th_spy.detach();
		for (int i = 0; i < 3; i++)
		{
			thread th = std::move(thread(&ThreadPoolSpy::consum, this));
			insertSpyArg(th.get_id(), SpyInfoPack(0, "working"), "working");
#ifndef _SpyTest1_ 
			pt.print_string_num("           Create     Thread Id:", th.get_id());
#endif
			th.detach();
		}
	}
}

void ThreadPoolSpy::setCanUse()
{
	m_canuse = true;
}

void ThreadPoolSpy::setSpyTime(int _time)
{
	spy_time = _time;
}

bool ThreadPoolSpy::getCanUse()
{
	return m_canuse;
}

//立即停止
void ThreadPoolSpy::stopPool()
{
	m_canuse = false;
	try_put(shared_ptr<TaskPack>(new solveTaskEd()));
	 
	m_convec.notify_all();
	unique_lock<mutex> lock(m_end);
	m_endcon.wait(lock, [this]()
	{
		int ret = working + waiting;
		return (ret == 0);
	});
	this->stopAllThreadRecode();
}

//将任务队列中的任务执行结束之后停止
void ThreadPoolSpy::stopUnilTaskOver()
{
	 
	try_put(shared_ptr<TaskPack>(new solveTaskEd()));
	 
	unique_lock<mutex> lock(m_allsolv);
	m_allsolvcon.wait(lock);

	m_canuse = false;
	m_convec.notify_all();

	m_endcon.wait(lock, [this]()
	{
		int ret = working + waiting;
		return (ret == 0);
	});
	this->stopAllThreadRecode();
}

void ThreadPoolSpy::reportTask()
{
	lock_guard<mutex> lock(m_spyinfo);
	for (auto report = spy_info.begin(); report != spy_info.end(); report++)
	{
		cout << "thread id:" << setw(6) << report->first << "  ";
		for (auto it = (report->second).begin(); it != (report->second).end(); it++)
		{
			//输出线程属性 例如监控还是处于工作的状态	执行次数	线程的工作状态	线程的当下任务	当下的执行时间
			cout << setw(8) << (*it).m_info << "      act times:"
				<< setw(4) << (*it).m_num << "     status:"
				<< (*it).working << "  action task: "
				<< setw(13) << (*it).now_act << " get task time: "
				<< setw(10) << (*it).now_time << endl;
		}

	}
}

void ThreadPoolSpy::destoryTask()
{
	lock_guard<mutex> lock(m_spyinfo);
	spy_info.clear();
}