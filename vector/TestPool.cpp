#pragma once
#include<iostream>
#include<atomic>
#include"thread_pool_spy.h"



using namespace std;
/*
	2020.1.5
	线程池的任务说明:
		已经做到的内容 根据任务的多寡 自动的增加 或者减少线程的数目
		并且监控在多线程的情况的对应变量的改变 例如工作线程 等待（即阻塞的线程）
												   线程池的任务队列中任务的多寡
												   监控线程执行循环的次数 以
												   及对应的线程是否处于工作的状态
		添加任务的时候 可以设置上线 也可以不设置上线
		关闭线程池的时候可以等待线程池中的任务队列执行结束
		也可以不等待任务队列的执行结束

		监控对应线程正在做的内容<即任务队列中任务的名称>

	下一版本更新的内容:
		以及获取任务的时间<要是超过100s>		视之为废弃线程<C++11不能结束一个线程>头疼
		所以用户在使用这个线程池组件的时候记着不要往其中加入阻塞任务

	使用说明：
		任何想加入到线程的任务 必须继承TaskPack这个对象 实现其中的doit(对应的任务在doit中执行)接口
		加入任务的时候使用share_ptr进行加入/逃
*/
class TestRealize :public TaskPack
{
public:
	TestRealize()
	{
		task = "TestRealize";
	}

	TestRealize(int i)
	{
		this->i = i;
		task = "TestRealize";
	}

	void doit() override
	{
		action();
	}

	void action()
	{
		pt.print_string_num("doit  ", i, "  threadid:  ", this_thread::get_id());
		this_thread::sleep_for(2s);
	}
private:
	int i;
};
int main123213()
{
	int pre = clock();
	ThreadPoolSpy pool;
	pool.setCanUse();
	pool.start();

	/*
		测试方案
			在加入 1-40个任务的时候 直接插入
			在插入 40-70个任务的时候 20s插入一次

	*/


	while (true)
	{
		static int i = 0;
		//pool.putWithoutTaskLimit(shared_ptr<TaskPack>(new TestRealize(i)));
		pool.put(shared_ptr<TaskPack>(new TestRealize(i)));
		if (i > 40)
		{
			this_thread::sleep_for(2s);
		} 
	    if (i > 70)
		{
			break;
		}
		i++;
	}

	//停止pool中相关的内容
	pt.print_string("start: task_over");
	//测试执行结束之后结束线程
	//pool.stopUnilTaskOver();
	pool.stopPool();
	cout << "end: task over" << endl;
	int now = clock();
	cout << "消耗的时间为" << now - pre << endl;

	//cout << "start: report" << endl;
	//pool.reportTask();
	while (true)
	{
		this_thread::sleep_for(10s);
	}
	system("pause");
	return 0;
}