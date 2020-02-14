#pragma once
#include<iostream>
#include<mutex>
#include<thread>
#include<condition_variable>
#include<atomic>
#include<string>
#include<atomic>
#include"print_to_buf.h"
#include"RwLock.h"
using namespace std;

/*
	此文件的功能是测试读写锁的一下功能
		1:测试读写锁的多个线程读取  :可以同时读取
		2:测试读写锁的多个线程写入  :不能同时写入
		3:测试读写的时候的优先级    : 写锁的优先级更高
		4:测试该锁的互斥性
		运行成功

	关于读写锁的异常测试
		1:单个线程 连续两次获取读锁  两次释放读锁  
		2:单个线程 连续两次获取写锁  两次释放写锁  
		3:单个线程 获取写锁之后 解除读锁
		4:单个线程 获取写锁之后 解除读锁

		结果是程序挂掉
*/


RW_Lock rwlock;

void testMoreThreadRead()
{
	//1:测试读写锁的多个线程读取:可以同时读取
	rwlock.rd_lock();
	pt.print_string_num("read ",this_thread::get_id());
	this_thread::sleep_for(3s);
	rwlock.rd_unlock();
}
 
void testMoreThreadWrite()
{
	//1:测试读写锁的多个线程读取:可以同时读取
	rwlock.wr_lock();
	pt.print_string_num("write ", this_thread::get_id());
	this_thread::sleep_for(3s);
	rwlock.wr_unlock();
}


int actFuncMR()//测试多线程同时获取读锁
{
	thread th[2];
	for (int i = 0; i < 2; i++)
	{
		th[i] = std::move(thread(testMoreThreadRead));
	}
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}
int actFuncMW()//测试多线程同时获取写锁
{
	thread th[2];
	for (int i = 0; i < 2; i++)
	{
		th[i] = std::move(thread(testMoreThreadWrite));
	}
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}
int actFuncMRW()//测试 读锁于写锁之间互斥
{
	thread th[2];
	//测试先写入 后读取
	th[0] = std::move(thread(testMoreThreadWrite));
	th[1] = std::move(thread(testMoreThreadRead));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	//测试先读取后写入
	th[0] = std::move(thread(testMoreThreadRead));
	th[1] = std::move(thread(testMoreThreadWrite));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}

int actFuncMRW_()//测试 读锁于写锁之间互斥
{
	thread th[2];
	//测试先写入 后读取
	th[0] = std::move(thread(testMoreThreadWrite));
	th[1] = std::move(thread(testMoreThreadRead));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	//测试先读取后写入
	th[0] = std::move(thread(testMoreThreadRead));
	th[1] = std::move(thread(testMoreThreadWrite));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}

int actWriLockFrist()//测试 优先写锁
{
	/*
		测试方案:
			先让写锁锁住
			在锁住的时候  加入两个读线程 以及一个写线程 观察第一个写锁开锁之后 执行的内容 是读线程还是写线程
	*/
	thread th[3];
	th[0] = std::move(thread(testMoreThreadWrite));
	th[1] = std::move(thread(testMoreThreadRead));  
	th[2] = std::move(thread(testMoreThreadWrite));
	for (int i = 0; i < 3; i++)
	{
		th[i].join();
	}
	return 1;
}


/*
	异常测试：
		1：测试一个线程 在未加 读锁的时候 解除读锁  加锁的时候加了两次读锁
		2：测试一个线程 在未加 写锁的时候 解除写锁  加锁的时候加了两次写锁
		3: 测试一个线程 在 加  读锁的时候 解除写锁
		   测试一个线程 在 加  写锁的时候 解除读锁
*/

void ExpTest1()
{
	//1.6测试成功
	//加锁的时候加了两次读锁
	rwlock.rd_lock();
	rwlock.rd_lock();
	
	//测试一个线程 在未加 读锁的时候 解除读锁
	rwlock.rd_unlock();
}

void ExpTest2()
{
	//加锁的时候加了两次读锁
	rwlock.wr_lock();
	rwlock.wr_lock();

	//测试一个线程 在未加 读锁的时候 解除读锁
	rwlock.wr_unlock();
}

void ExpTest3()
{
	//测试一个线程 在 加  写锁的时候 解除读锁
	rwlock.wr_lock();
	rwlock.rd_unlock();

	//测试一个线程 在 加  读锁的时候 解除写锁
	rwlock.rd_lock();
	rwlock.wr_unlock();
}
int main1234()
{
	actFuncMR();//测试两个线程同时读取
	actFuncMW();//测试两个线程同时写入
	actFuncMRW_();//测试读写锁的互斥行为
	actWriLockFrist();//测试写锁的优先级

	/*异常测试  一下所有函数测试均达到预期*/
	ExpTest1();
	ExpTest2();
	ExpTest3();
	system("pause");
	return 1;
}