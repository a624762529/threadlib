#pragma once
#pragma once
#include<iostream>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<list>
#include<functional>
#include<iomanip>
#include<map>
#include<atomic>

#include"print_to_buf.h"

using namespace std;
 
class ThreadPoolSpy;
class TaskPack
{
public:
	virtual void doit() = 0;
	string task;			//任务名称
};


class solveTaskEd :public TaskPack
{
public:
	solveTaskEd();
	void doit();
};


class SpyInfoPack
{
public:

	SpyInfoPack();
	SpyInfoPack(int tm, string info, int work = true);
	SpyInfoPack(const SpyInfoPack& arg);
	bool operator==(const SpyInfoPack& pr);
	void overThread();
	void upperThreadTm();
public:
	int	   m_num;  //线程执行的次数
	string m_info; //附加信息
	bool   working;//是否处于工作的状态
	string now_act;//当下的动作
	string now_time;
};