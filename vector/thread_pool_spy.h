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
#include<time.h>
#include"cTaskPack.h"
#include"print_to_buf.h"
#include"thread_pool_spy.h"
using namespace std;
#define _SpyTest1_
/*
	第一种监控模式 监控线程的状态 执行任务 获取任务的时间 就是上次截图的那个
*/
#define _SpyTest2_
/*
	第二种监控模式 监控线程池中线程的执行任务线程的个数 监控的属性 执行任务或者监控 任务队列中任务的数目
				   正在工作的线程个数 正在休息的线程个数
				   监控线程的创建
				   以及销毁
*/
//线程池子的升级版本 :升级的内容 可以根据正在等待的线程数目动态的扩展线程池中线程的数
class ThreadPoolSpy
{
public:
	ThreadPoolSpy();
	~ThreadPoolSpy();
	void put(shared_ptr<TaskPack> arg);
	void putWithoutTaskLimit(shared_ptr<TaskPack> arg);
	void insertSpyArg(thread::id id, SpyInfoPack arg,string task);//将任务加入到队列中
	void start();
	void setSpyTime(int time);
	void setCanUse();		   //设置它可以被使用
	bool getCanUse();          //决策是否可被使用
	void stopPool();
	void stopUnilTaskOver();
	void reportTask();
	void destoryTask();
private:
	void consum();
private:
	//void reportTask();
	//void destoryTask();
	string getNowTime();
private:
	bool try_put(shared_ptr<TaskPack> arg);
	//void insertSpyArg(thread::id id, SpyInfoPack arg);
	bool isEmpty();            //数据包比较相关的函数
	bool isOverFlow();
	bool isLargerToMaxWorking();//线程数比较相关的函数
	bool isMinnerToMinWorking();
	bool isInBewteenWorking();
	bool isLargerToMaxWaiting();//线程数比较相关的函数
	bool isMinnerMinWaiting();
	bool isMinnerIsZero();
	bool isInBewteenWaiting();
	void stopAllThreadRecode();
	void stopOneThreadRecode(thread::id id);
private:
	shared_ptr<TaskPack> get();
	void spyControl();
	shared_ptr<TaskPack> getDataToVec();
private:
	bool m_canuse = false;
	int spy_time;
	//vector<thread::id> m_threadvec;
	mutex m_synchr;				//同步锁
	mutex m_task;				//队列锁
	list<shared_ptr<TaskPack>> m_taskvec;
	enum {
		max_num_vec = 7,
		min_num_vec = 0
		/*
			任务队列中最大任务以及最小任务的数目
		*/
	};
	enum {
		max_num_thread = 6,
		min_num_thread = 2
		/*
			线程队列中最大线程和最小线程的数目 工作线程
		*/
	};
	condition_variable m_convec;
	condition_variable m_provec;
	atomic<int> waiting = 0;
	atomic<int> working = 3;
	mutex m_end;				//线程池结束的时候使用
	condition_variable m_endcon;//条件变量
	vector<int> vec;

	mutex m_allsolv;				//线程池结束的时候使用
	condition_variable m_allsolvcon;//条件变量
	
	mutex m_spyinfo;
	map<thread::id, vector<SpyInfoPack>> spy_info;
};
 