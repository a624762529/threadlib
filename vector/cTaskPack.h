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
	string task;			//��������
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
	int	   m_num;  //�߳�ִ�еĴ���
	string m_info; //������Ϣ
	bool   working;//�Ƿ��ڹ�����״̬
	string now_act;//���µĶ���
	string now_time;
};