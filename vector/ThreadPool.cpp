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
	cout << "�������" << endl;
	is_over.sem_V();
	pt.print_string("�̳߳��е��߳�ȫ���˳�����");
}

shared_ptr<TubeImple> ThreadPool::get()
{
	return tube.get();
}

void ThreadPool::Comsumer()
{
	//�������߳�
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
	//����߳�
	//while (start)
	while(start == MAGIC)
	{
		/*
			��Ϊ���������
			��һ�����߳�̫�� ����̫���� ����Ҫ���������̵߳���Ŀ
					�߳�̫�� ����̫��   ���������̵߳���Ŀ
			�ڶ����߳�̫��    ����̫��  �����߳���Ŀ�Ѿ����ӵ�����ֵ
		*/
		this_thread::sleep_for(1s);
		pt.print_string_num("           ���ڹ������߳���Ŀ", working);
		pt.print_string_num("           ���ڵȴ����߳���Ŀ", waiting);
		//if (start == MAGIC)
		{
			int all_workingthread = working + waiting;
			int waitingthread = waiting;
			//����߳����������Ļ� ���������̵߳���Ŀ

			if (all_workingthread >= MINTHREAD && all_workingthread < MAXTHREAD)
			{
				//������������������Ļ� �Ǿͼ����߳�
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
				//����������̺߳͵ȴ����߳���Ŀ������MAXTHREAD �ǾͲ���һ��������־
				if (tube.size() == 0)
				{
					//������������û������ �Ǿ�
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
	//����߳�
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
		pt.print_string("judeover �̳߳��е��߳�ȫ���˳�����");
	}
}