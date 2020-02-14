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
	//�ڴ˴�����������
	while (true)
	{
		if (!m_canuse)
		{
			//pt.print_string_num("    ���߳�ͣ������  threadid: ", this_thread::get_id());
			working--;
			m_endcon.notify_one();
			return;
		}

		unique_lock<mutex> lock(m_synchr);
		while (isEmpty())
		{
			working--;
			waiting++;//���ĳ���߳��ڴ˴� Ȼ�����ֹͣ�̳߳صĲ��� 
					  //��ô�̳߳ص�spy����Զ���ᱻֹͣ  �ν�<�����޽�> 
			m_convec.wait(lock);
			working++;
			waiting--;
			if (isEmpty())//����Ϊ��
			{
				working--;
#ifndef _SpyTest1_ 
				pt.print_string_num("        ����������е�������ٶ������߳� ", this_thread::get_id());
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
	//��ȡ����	 
	if (!getCanUse())
		return;
	unique_lock<mutex> lock(m_synchr);
	while (isOverFlow())
	{
		m_provec.wait(lock);
	}

	m_taskvec.push_back(arg);
	lock.unlock();
	//֪ͨ�������߳�
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
	//֪ͨ�������߳�
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
			//û������ڵ���Ϣ
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
	//֪ͨ�������߳�
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
	//�����������߳�
	while (true)
	{

		this_thread::sleep_for(1s);
#ifndef _SpyTest1_ 
		pt.print_string_num("           ��ǰ�߳��еĹ����߳���Ŀ  ", working);
		pt.print_string_num("           ��ǰ�߳���wait�����߳���Ŀ", waiting);
		pt.print_string_num("           ��ǰ��������е�������Ŀ  ", m_taskvec.size());
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
			if (isMinnerMinWaiting())          //��wait���еȴ��߳���Ŀ ��[0, 1]��������
			{
			}
			else if (isMinnerIsZero())         //��wait���еȴ��߳���Ŀ   Ϊ 0 ˵���������̲߳������� 
			{
				if (isLargerToMaxWorking())	   //�������������е��߳���Ŀ
				{
				}
				else if (isInBewteenWorking()) //������֮��
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
			else if (isLargerToMaxWaiting())  //��wait���еȴ��߳���Ŀ   Ϊ [3,+] ˵����ǰҵ��Ƚ��� 
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

//����ֹͣ
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

//����������е�����ִ�н���֮��ֹͣ
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
			//����߳����� �����ػ��Ǵ��ڹ�����״̬	ִ�д���	�̵߳Ĺ���״̬	�̵߳ĵ�������	���µ�ִ��ʱ��
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