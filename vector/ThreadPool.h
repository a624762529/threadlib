#pragma once
#include"thread_pool_spy.h"
#include"Tube.h"
#include<iostream>
using namespace std;
#define MAXTHREAD thread::hardware_concurrency()*2+2
#define MINTHREAD 3
#define MAGIC 11  //ʹ��11��Ϊ�߳̿�ʼ��tag

class ThreadPoolImpl:public TubeImple
{
};
class ThreadPoolEnd :public TubeImple
{
public:
	void doit() override {}
};
class ThreadPool
{
public:
	ThreadPool(int tube_size);
	~ThreadPool();
	
	void put(shared_ptr<ThreadPoolImpl> sh);
	void startPool();
	void stopAtonce();
private:
	void jude_over();
	shared_ptr<TubeImple> get();
	void Comsumer();
	void spy();//���working�ı仯
	atomic<int> working;
	atomic<int> waiting;
private:
	int start;//�����Ƿ�ʼ
	Tube tube;
	Sem_Pv is_over;
	mutex mu;
};

