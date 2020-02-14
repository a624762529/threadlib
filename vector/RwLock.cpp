#include "RwLock.h"

RW_Lock::RW_Lock()
{
	waiting_read = 0;
	waiting_write = 0;
	writing = 0;
	reading = 0;
}

RW_Lock::~RW_Lock() 
{
}
 
void RW_Lock::rd_lock()
{
	/*
		�������Ա���ס��ǰ����   û�߳�����дд ����û���߳������Ŷӵȴ�д��
	*/
	unique_lock<mutex> lock(mu);
	//��ǰ���߳�����д���� ���ڵȴ�д
	while (waiting_write != 0 || writing != 0)
	{
		waiting_read++;
		con_read.wait(lock, [this]() {
					return (waiting_write == 0 && writing == 0); 
			    });
		waiting_read--;
	}

	auto ret=getNode(this_thread::get_id());
	//��ʱ ��������Ӧ��û�нڵ�
	if (ret == 'f')
	{
		addNode(Map_Node(Map_Node::read));
	}
	else
	{
		cout << "����ڵ��쳣" << endl;
		throw bad_alloc();
	}

	reading++;				
	//��ȡ����֮�� �ԼӶ�Ӧ�ı�־λ
	lock.unlock();
}

void RW_Lock::rd_unlock()
{
	//�򿪶�Ӧ�Ķ���
	unique_lock<mutex> lock(mu);
	reading--;
	//��������߳�������д���״̬ ����û���̴߳��ڶ���״̬
	//֪ͨд�߳̿�ʼд
	if ((waiting_write != 0)&&(reading==0))
	{
		con_write.notify_one();
	}

	auto ret = getNode(this_thread::get_id());
	//��ʱ ��������Ӧ��û�нڵ�
	if (ret == Map_Node::read)
	{
		rmNode( this_thread::get_id() );
	}
	else
	{
		cout << "�Ƴ��ڵ��쳣" << endl;
		throw bad_alloc();
	}

	lock.unlock();
}


void RW_Lock::wr_lock()
{
	unique_lock<mutex> lock(mu);
	auto ret = getNode(this_thread::get_id());
	//��ʱ ��������Ӧ��û�нڵ�
	if (ret == 'f')
	{
		addNode(Map_Node(Map_Node::write));
	}
	else
	{
		cout << "����ڵ��쳣" << endl;
		throw bad_alloc();
	}

	while (writing != 0 || reading != 0)
	{
		waiting_write++;
		con_write.wait(lock,
			[this]()
			{
				return (writing == 0 && reading == 0);
			});
		waiting_write--;
	}
	writing++;
	lock.unlock();
}

void RW_Lock::wr_unlock()
{
	unique_lock<mutex> lock(mu);
	writing--;
	if (waiting_write == 0)
	{
		con_read.notify_all();
		//��ǰд��д����� �ͷŶ���
	}
	else
	{
		con_write.notify_one();
	}
	auto ret = getNode(this_thread::get_id());
	//��ʱ ��������Ӧ��û�нڵ�
	if (ret == Map_Node::write)
	{
		rmNode(this_thread::get_id());
	}
	else
	{
		cout << "����ڵ��쳣" << endl;
		throw bad_alloc();
	}
	lock.unlock();
}


char RW_Lock::getNode(thread::id id)
{
	lock_guard<mutex> lock(vec_lock);
	auto i=find(vec_node.begin(), vec_node.end(), id);
	if (i != vec_node.end())
	{
		return (*i).stata;
	}
	return 'f';
}

bool RW_Lock::changeNode(thread::id id, char val)
{
	lock_guard<mutex> lock(vec_lock);
	auto i = find(vec_node.begin(), vec_node.end(), id);
	(*i).stata = val;
	return true;
}

void RW_Lock::addNode(const Map_Node nd)
{
	lock_guard<mutex> lock(vec_lock);
	auto i = find(vec_node.begin(), vec_node.end(), nd);
	if (i != vec_node.end())
	{
		cout << "rd-lock addnode exep" << endl;
		throw bad_alloc();
	}
	vec_node.push_back(nd);
	return;
}

void RW_Lock::rmNode(thread::id id)
{
	lock_guard<mutex> lock(vec_lock);
	auto i = find(vec_node.begin(), vec_node.end(), id);
	if (i == vec_node.end())
	{
		cout << "rd-lock rm node exep" << endl;
		throw bad_alloc();
	}
	vec_node.erase(i);
	return;
}