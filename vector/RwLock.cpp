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
		读锁可以被锁住的前提是   没线程正在写写 并且没有线程正在排队等待写锁
	*/
	unique_lock<mutex> lock(mu);
	//当前有线程正在写或者 正在等待写
	while (waiting_write != 0 || writing != 0)
	{
		waiting_read++;
		con_read.wait(lock, [this]() {
					return (waiting_write == 0 && writing == 0); 
			    });
		waiting_read--;
	}

	auto ret=getNode(this_thread::get_id());
	//此时 在容器中应该没有节点
	if (ret == 'f')
	{
		addNode(Map_Node(Map_Node::read));
	}
	else
	{
		cout << "加入节点异常" << endl;
		throw bad_alloc();
	}

	reading++;				
	//获取读锁之后 自加对应的标志位
	lock.unlock();
}

void RW_Lock::rd_unlock()
{
	//打开对应的读锁
	unique_lock<mutex> lock(mu);
	reading--;
	//如果有有线程正处于写入的状态 并且没有线程处于读的状态
	//通知写线程开始写
	if ((waiting_write != 0)&&(reading==0))
	{
		con_write.notify_one();
	}

	auto ret = getNode(this_thread::get_id());
	//此时 在容器中应该没有节点
	if (ret == Map_Node::read)
	{
		rmNode( this_thread::get_id() );
	}
	else
	{
		cout << "移除节点异常" << endl;
		throw bad_alloc();
	}

	lock.unlock();
}


void RW_Lock::wr_lock()
{
	unique_lock<mutex> lock(mu);
	auto ret = getNode(this_thread::get_id());
	//此时 在容器中应该没有节点
	if (ret == 'f')
	{
		addNode(Map_Node(Map_Node::write));
	}
	else
	{
		cout << "加入节点异常" << endl;
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
		//当前写锁写入结束 释放读锁
	}
	else
	{
		con_write.notify_one();
	}
	auto ret = getNode(this_thread::get_id());
	//此时 在容器中应该没有节点
	if (ret == Map_Node::write)
	{
		rmNode(this_thread::get_id());
	}
	else
	{
		cout << "加入节点异常" << endl;
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